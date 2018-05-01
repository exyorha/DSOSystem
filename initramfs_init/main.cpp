#include <sys/mount.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#define private private_renamed
#include <linux/keyctl.h>
#undef private
#include <linux/loop.h>
#include <asm/unistd.h>
#include <ctype.h>

const char *rootdev = nullptr;
const char *rootfstype = "squashfs";
const char *init = "/sbin/init";
const char *sdcard = nullptr;
const char *sdcard_rootfs = nullptr;

static const struct {
    const char *option;
    const char **value;
} options[] = {
    { "root=", &rootdev },
    { "rootfstype=", &rootfstype },
    { "init=", &init },
    { "sdcard=", &sdcard },
    { "sdcard_rootfs=", &sdcard_rootfs }
};

typedef int32_t key_serial_t;

static key_serial_t add_key(const char *type,
                            const char *description,
                            const void *payload,
                            size_t plen,
                            key_serial_t ringid) {
    return syscall(__NR_add_key, type, description, payload, plen, ringid);
}

static int purge_filesystem(const char *path, dev_t rootdev) {
    struct stat statbuf;
    if(lstat(path, &statbuf) < 0)
        return -1;

    if(statbuf.st_dev != rootdev)
        return 0;

    if(S_ISDIR(statbuf.st_mode)) {
        DIR *dir = opendir(path);
        if(dir == nullptr)
            return -1;

        struct dirent *entry;

        while((entry = readdir(dir))) {
            if(strcmp(entry->d_name, ".") == 0 ||
               strcmp(entry->d_name, "..") == 0)
               continue;

            char *subpath = static_cast<char *>(malloc(strlen(path) + 1 + strlen(entry->d_name) + 1));
            strcpy(subpath, path);
            if(strcmp(path, "/") != 0)
                strcat(subpath, "/");
            strcat(subpath, entry->d_name);

            int result = purge_filesystem(subpath, rootdev);
            free(subpath);

            if(result < 0) {
                closedir(dir);
                return result;
            }
        }

        closedir(dir);

        rmdir(path);
    } else {
        unlink(path);
    }

    return 0;
}

int main(int argc, char **argv) {
    for(int arg = 1; arg < argc; arg++) {
        auto argstr = argv[arg];

        auto delim = strchr(argstr, '=');
        if(delim == nullptr)
            continue;

        for(const auto &option: options) {
            if(strncmp(argstr, option.option, delim - argstr + 1) == 0) {
                *option.value = delim + 1;
                break;
            }
        }
    }

    if(rootdev == nullptr && (sdcard_rootfs == nullptr || sdcard == nullptr)) {
        fprintf(stderr, "root device must be specified\n");
        return 1;
    }

    if(mount("devtmpfs", "/dev", "devtmpfs", MS_NOSUID | MS_NOEXEC, nullptr) < 0) {
        perror("mount(devtmpfs)");
        return 1;
    }

    if(sdcard) {
        fputs("Waiting for sdcard to come up: ", stdout);
        fflush(stdout);
        while(access(sdcard, R_OK) < 0)
            usleep(10000);
        fputs("done\n", stdout);
        fflush(stdout);

        if(mount(sdcard, "/sdcard", "vfat", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_RDONLY, nullptr) < 0) {
            perror("mount(sdcard)");
            return 1;
        }
    }

    if(sdcard_rootfs) {
        int controlfd = open("/dev/loop-control", O_RDWR);
        if(controlfd < 0) {
            perror("open(loop-control)");
            return 1;
        }

        int device = ioctl(controlfd, LOOP_CTL_GET_FREE);
        if(device < 0) {
            perror("ioctl(LOOP_CTL_GET_FREE)");
            return 1;
        }

        static char buf[32];
        snprintf(buf, sizeof(buf), "/dev/loop%d", device);

        int loopfd = open(buf, O_RDWR | O_CLOEXEC);
        if(loopfd < 0) {
            perror("open(/dev/loopN)");
            return 1;
        }

        rootdev = buf;

        close(controlfd);

        int imagefd = open(sdcard_rootfs, O_RDONLY);
        if(imagefd < 0) {
            perror("open(sdcard rootfs)");
            return 1;
        }

        if(ioctl(loopfd, LOOP_SET_FD, imagefd) < 0) {
            perror("ioctl(LOOP_SET_FD)");
            return 1;
        }

        close(imagefd);

        printf("Root device: %s\n", rootdev);
    }

    if(mount(rootdev, "/rootfs", rootfstype, MS_NOSUID | MS_NODEV | MS_RDONLY, nullptr) < 0) {
        perror("mount(rootfs)");
        return 1;
    }
    
    if(mount("/dev", "/rootfs/dev", nullptr, MS_MOVE, nullptr) < 0) {
        perror("mount(dev move)");
        return 1;
    }

    if(sdcard) {
        if(mount("/sdcard", "/rootfs/sdcard", nullptr, MS_MOVE, nullptr) < 0) {
            perror("mount(sdcard move)");
            return 1;
        }
    }

    auto ima_keyring = add_key("keyring", "_ima", nullptr, 0, KEY_SPEC_USER_KEYRING);
    if(ima_keyring < 0) {
        perror("add_key(_ima)");
        return 1;
    }

    if(chdir("/ima-keys") < 0) {
        perror("chdir(/ima-keys)");
        return 1;
    }

    DIR *dir = opendir(".");
    if(dir == nullptr) {
        perror("opendir(/ima-keys)");
        return 1;
    }

    struct dirent *entry;

    while((entry = readdir(dir)) != nullptr) {
        if(entry->d_name[0] != '.') {
            int fd = open(entry->d_name, O_RDONLY);
            if(fd < 0) {
                perror("open(ima key)");
                return 1;
            }

            struct stat statbuf;
            if(fstat(fd, &statbuf) < 0) {
                perror("fstat(ima key)");
                return 1;
            }

            char *buf = static_cast<char *>(malloc(statbuf.st_size));
            auto result = read(fd, buf, statbuf.st_size);
            if(result < 0) {
                perror("read(ima key)");
                return 1;
            }

            close(fd);

            auto key = add_key("user", entry->d_name, buf, result, ima_keyring);
            if(key < 0) {
                perror("add_key(ima key)");
                return 1;
            }

            free(buf);
        }
    }

    closedir(dir);

    chdir("/");

    if(mount("sysfs", "/sys", "sysfs", MS_NOEXEC | MS_NODEV | MS_NOSUID, nullptr) < 0) {
        perror("mount(sysfs)");
        return 1;
    }

    if(mount("securityfs", "/sys/kernel/security", "securityfs", MS_NOEXEC | MS_NODEV | MS_NOSUID, nullptr) < 0) {
        perror("mount(securityfs)");
        return 1;
    }

    struct stat statbuf;
    int fd = open("/sys/kernel/security/ima/policy", O_WRONLY);
    if(fd < 0) {
        perror("open(kernel ima policy)");
        return 1;
    }

    FILE *polfile = fopen("/ima-policy", "r");
    if(polfile == nullptr) {
        perror("fopen(/ima-policy)");
        return 1;
    }

    static char line[256];

    while(fgets(line, sizeof(line), polfile)) {
        char *end = line + strlen(line) - 1;
        while(end >= line && isspace(*end)) {
            *end-- = 0;
        }

        if(*line == 0 || line[0] == '#')
            continue;
        
        int result = write(fd, line, strlen(line));
        if(result < 0) {
            perror("write(ima policy)");
            return 1;
        }
    }

    fclose(polfile);

    close(fd);

    umount("/sys/kernel/security");
    umount("/sys");

    if(chdir("/rootfs") < 0) {
        perror("chdir(rootfs)");
        return 1;
    }

    fd = open("dev/console", O_RDWR);
    if(fd < 0) {
        perror("open(/dev/console)");
        return 1;
    }

    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    
    close(fd);

    stat("/", &statbuf);
    if(purge_filesystem("/", statbuf.st_dev) < 0) {
        perror("purge_filesystem");
        return 1;
    }

    if(mount(".", "/", nullptr, MS_MOVE, nullptr) < 0) {
        perror("mount(rootfs move)");
        return 1;
    }

    if(chroot(".") < 0) {
        perror("chroot(rootfs)");
        return 1;
    }

    execv(init, argv);

    perror("execv");

    return 1;
}

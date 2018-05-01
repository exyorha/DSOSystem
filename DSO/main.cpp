#include <DSO/ApplicationInstance.h>
#include <LinuxPlatform/LinuxDSOPlatform.h>

int main(int argc, char **argv) {
    ApplicationInstance instance(std::unique_ptr<LinuxDSOPlatform>(new LinuxDSOPlatform()));

    instance.exec();
}

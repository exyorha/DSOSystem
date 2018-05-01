#include <DSO/ApplicationInstance.h>

#if defined(__linux__)
#include <LinuxPlatform/LinuxDSOPlatform.h>
#endif

#if defined(_WIN32)
#include <Win32/WindowsDSOPlatform.h>
#endif

int main(int argc, char **argv) {
#if defined(__linux__)
	ApplicationInstance instance(std::unique_ptr<LinuxDSOPlatform>(new LinuxDSOPlatform()));

    instance.exec();
#endif

#if defined(_WIN32)
	ApplicationInstance instance(std::make_unique<WindowsDSOPlatform>());

	instance.exec();
#endif
}

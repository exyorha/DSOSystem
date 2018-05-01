#include <LinuxPlatform/LinuxAcquisition.h>
#include <DSO/DSOAccumulator.h>

LinuxAcquisition::LinuxAcquisition() {

}

LinuxAcquisition::~LinuxAcquisition() {

}

size_t LinuxAcquisition::channelCount() const {
    return 0;
}

std::unique_ptr<DSOAccumulator> LinuxAcquisition::allocateChannelAccumulator(size_t channel, size_t width, size_t height) {
    return nullptr;
}

void LinuxAcquisition::internalStart() {

}

void LinuxAcquisition::internalStop() {

}

void LinuxAcquisition::internalSetTriggerMode(TriggerMode mode) {

}

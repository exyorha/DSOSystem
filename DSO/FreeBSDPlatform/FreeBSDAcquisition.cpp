#include <FreeBSDPlatform/FreeBSDAcquisition.h>
#include <DSO/DSOAccumulator.h>

FreeBSDAcquisition::FreeBSDAcquisition() {

}

FreeBSDAcquisition::~FreeBSDAcquisition() {

}

size_t FreeBSDAcquisition::channelCount() const {
    return 0;
}

std::unique_ptr<DSOAccumulator> FreeBSDAcquisition::allocateChannelAccumulator(size_t channel, size_t width, size_t height) {
    return nullptr;
}

void FreeBSDAcquisition::internalStart() {

}

void FreeBSDAcquisition::internalStop() {

}

void FreeBSDAcquisition::internalSetTriggerMode(TriggerMode mode) {

}

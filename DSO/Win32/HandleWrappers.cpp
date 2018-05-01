#include <Win32/HandleWrappers.h>

void DeviceContextDeleter::operator()(HDC dc) {
    DeleteDC(dc);
}

void GdiObjectDeleter::operator()(HGDIOBJ obj) {
    DeleteObject(obj);
}

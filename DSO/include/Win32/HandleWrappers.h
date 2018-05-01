#ifndef WIN32__HANDLE_WRAPPERS__H
#define WIN32__HANDLE_WRAPPERS__H

#include <memory>

#include <Windows.h>

struct DeviceContextDeleter {
    void operator()(HDC dc);
};

struct GdiObjectDeleter {
    void operator()(HGDIOBJ obj);
};

typedef std::unique_ptr<HDC__, DeviceContextDeleter> DeviceContextHandle;
typedef std::unique_ptr<HBITMAP__, GdiObjectDeleter> BitmapHandle;

#endif

#include <Win32/WindowsApplicationPlatform.h>

WindowsApplicationPlatform::WindowsApplicationPlatform() {
    RECT defaultRect{ CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT };
    if (!m_hostWindow.Create(nullptr, &defaultRect, L"DSO-100 UI", WS_OVERLAPPEDWINDOW, WS_EX_OVERLAPPEDWINDOW))
        AtlThrow(HRESULT_FROM_WIN32(GetLastError()));
    m_hostWindow.ShowWindow(SW_SHOW);
}

WindowsApplicationPlatform::~WindowsApplicationPlatform() {

}

DisplayInformation WindowsApplicationPlatform::displayInformation() const {
    DisplayInformation disp;
    disp.displaySize = SkISize::Make(800, 480);

    return disp;
}

void WindowsApplicationPlatform::exec() {
    MSG message;
    while (GetMessage(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

void WindowsApplicationPlatform::setApplicationPlatformInterface(ApplicationPlatformInterface *iface) {
    m_hostWindow.setApplicationPlatformInterface(iface);
}

void WindowsApplicationPlatform::requestFrame() {
    m_hostWindow.Invalidate();
}

void WindowsApplicationPlatform::keepPreviousFrame() {

}



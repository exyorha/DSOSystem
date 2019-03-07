#ifndef WIN32__WINDOWS_HOST_WINDOW__H
#define WIN32__WINDOWS_HOST_WINDOW__H

#include <atlbase.h>
#include <atlwin.h>
#include <atlimage.h>

#include <Win32/HandleWrappers.h>
#include <GUI/PixmanUtils.h>

class ApplicationPlatformInterface;

class WindowsHostWindow final : public CWindowImpl<WindowsHostWindow> {
public:
	DECLARE_WND_CLASS_EX(nullptr, CS_HREDRAW | CS_VREDRAW, (HBRUSH)(COLOR_BACKGROUND + 1))

	enum {
		WM_DEFERRED = WM_USER
	};

	enum : unsigned int {
		ScreenWidth = 800,
		ScreenHeight = 480
	};

    WindowsHostWindow();
    virtual ~WindowsHostWindow();

    BEGIN_MSG_MAP(WindowsHostWindow)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, handleGetMinMaxInfo)
        MESSAGE_HANDLER(WM_CLOSE, handleClose)
        MESSAGE_HANDLER(WM_PAINT, handlePaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, onLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, onLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, onMouseMove)
		MESSAGE_HANDLER(WM_DEFERRED, onDeferred)
    END_MSG_MAP()

    inline ApplicationPlatformInterface *applicationPlatformInterface() const { return m_platformInterface; }
    inline void setApplicationPlatformInterface(ApplicationPlatformInterface *applicationPlatformInterface) {
        m_platformInterface = applicationPlatformInterface;
    }

private:
    LRESULT handleGetMinMaxInfo(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT handleClose(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT handlePaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onLButtonUp(UINT message, WPARAM wParam, LPARAM lParam, BOOL &handled);
	LRESULT onLButtonDown(UINT message, WPARAM wParam, LPARAM lParam, BOOL &handled);
	LRESULT onMouseMove(UINT message, WPARAM wParam, LPARAM lParam, BOOL &handled);
	LRESULT onDeferred(UINT message, WPARAM wParam, LPARAM lParam, BOOL &handled);

	int getButtonIndex(int x, int y);
	void setCurrentButton(int button);
		
    DeviceContextHandle m_screenDC;
    BitmapHandle m_screenBitmap;
    void *m_bitmapBits;
    PixmanImage m_screenSurface;
    ApplicationPlatformInterface *m_platformInterface;
	ATL::CImage m_panelImage;
	ATL::CImage m_buttonMapImage;
	int m_currentButton;
};

#endif

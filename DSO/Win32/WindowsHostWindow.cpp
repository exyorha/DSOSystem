#include <GUI/ApplicationPlatformInterface.h>

#include <Win32/WindowsHostWindow.h>

#include <memory>

#include <assert.h>

WindowsHostWindow::WindowsHostWindow() : m_platformInterface(nullptr), m_currentButton(-1) {
	{
		auto res = FindResource(nullptr, MAKEINTRESOURCE(1), RT_RCDATA);

		CComPtr<IStream> stream(SHCreateMemStream(static_cast<const BYTE *>(LockResource(LoadResource(nullptr, res))), SizeofResource(nullptr, res)));
		m_panelImage.Load(stream);
	}

	{
		auto res = FindResource(nullptr, MAKEINTRESOURCE(2), RT_RCDATA);

		CComPtr<IStream> stream(SHCreateMemStream(static_cast<const BYTE *>(LockResource(LoadResource(nullptr, res))), SizeofResource(nullptr, res)));
		m_buttonMapImage.Load(stream);
	}

    auto rawDC = CreateCompatibleDC(nullptr);
    if (!rawDC)
        AtlThrow(HRESULT_FROM_WIN32(GetLastError()));

    m_screenDC.reset(rawDC);

    BITMAPINFO bitmapInfo;
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = ScreenWidth;
    bitmapInfo.bmiHeader.biHeight = -ScreenHeight;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage = 0;
    bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    bitmapInfo.bmiHeader.biClrImportant = 0;
        
    auto rawBitmap = CreateDIBSection(m_screenDC.get(), &bitmapInfo, DIB_RGB_COLORS, &m_bitmapBits, nullptr, 0);
    if(!rawBitmap)
        AtlThrow(HRESULT_FROM_WIN32(GetLastError()));
        
    m_screenBitmap.reset(rawBitmap);

    SelectObject(m_screenDC.get(), m_screenBitmap.get());

	auto image = pixman_image_create_bits(PIXMAN_a8b8g8r8, ScreenWidth, ScreenHeight, static_cast<uint32_t *>(m_bitmapBits), ScreenWidth * sizeof(uint32_t));
	assert(image);
	m_screenSurface.reset(image);
}

WindowsHostWindow::~WindowsHostWindow() {

}

LRESULT WindowsHostWindow::handleGetMinMaxInfo(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
    (void)nMsg;
    (void)wParam;

	RECT clientRectangle = {
		0,
		0,
		m_panelImage.GetWidth(),
		m_panelImage.GetHeight()
	};

	AdjustWindowRectEx(&clientRectangle,
		GetWindowLong(GWL_STYLE),
		FALSE,
		GetWindowLong(GWL_EXSTYLE));


    auto info = reinterpret_cast<MINMAXINFO *>(lParam);
	info->ptMaxSize.x = clientRectangle.right - clientRectangle.left;
    info->ptMaxSize.y = clientRectangle.bottom - clientRectangle.top;
    info->ptMinTrackSize.x = clientRectangle.right - clientRectangle.left;
    info->ptMinTrackSize.y = clientRectangle.bottom - clientRectangle.top;
    info->ptMaxTrackSize.x = clientRectangle.right - clientRectangle.left;
    info->ptMaxTrackSize.y = clientRectangle.bottom - clientRectangle.top;

    bHandled = TRUE;
    return 0;
}

LRESULT WindowsHostWindow::handleClose(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
    (void)nMsg;
    (void)wParam;
    (void)lParam;

    PostQuitMessage(0);

    bHandled = FALSE;

    return 0;
}
    
LRESULT WindowsHostWindow::handlePaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
    PAINTSTRUCT paint;

    m_platformInterface->drawFrame(m_screenSurface.get());

    auto dc = BeginPaint(&paint);

	m_panelImage.Draw(dc, 0, 0);

    if (!BitBlt(dc, 3, 60, ScreenWidth, ScreenHeight, m_screenDC.get(), 0, 0, SRCCOPY))
        AtlThrow(HRESULT_FROM_WIN32(GetLastError()));

    EndPaint(&paint);

    bHandled = TRUE;

    return 0;
}


LRESULT WindowsHostWindow::onLButtonUp(UINT message, WPARAM wParam, LPARAM lParam, BOOL &handled) {
	setCurrentButton(-1);

	return 0;
}

LRESULT WindowsHostWindow::onLButtonDown(UINT message, WPARAM wParam, LPARAM lParam, BOOL &handled) {
	auto buttonIndex = getButtonIndex(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	setCurrentButton(buttonIndex);

	return 0;
}

LRESULT WindowsHostWindow::onMouseMove(UINT message, WPARAM wParam, LPARAM lParam, BOOL &handled) {
	auto buttonIndex = getButtonIndex(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	if ((wParam & MK_LBUTTON) || buttonIndex < 0) {
		setCurrentButton(buttonIndex);
	}

	return 0;
}

int WindowsHostWindow::getButtonIndex(int x, int y) {
	auto pixel = m_buttonMapImage.GetPixel(x, y);

	auto value = pixel & 0xFF;
	if (value >= 128 && value < 255) {
		return value - 128;
	}
	else {
		return -1;
	}
}

void WindowsHostWindow::setCurrentButton(int button) {
	if (m_currentButton != button) {
		if (button >= 0) {
			if (m_platformInterface) {
				m_platformInterface->keyPressed(button);
			}

		}
		else if (button < 0) {
			if (m_platformInterface) {
				m_platformInterface->keyReleased(m_currentButton);
			}
		}

		m_currentButton = button;
	}
}

LRESULT WindowsHostWindow::onDeferred(UINT message, WPARAM wParam, LPARAM lParam, BOOL &handled) {
	if (m_platformInterface) {
		m_platformInterface->dispatchDeferred();
	}

	return 0;
}

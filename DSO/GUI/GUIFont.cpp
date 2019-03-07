#include <GUI/GUIFont.h>

GUIFont::GUIFont() {
	m_font.userdata.ptr = this;
	m_font.width = widthProxy;
}

GUIFont::~GUIFont() {

}

float GUIFont::widthProxy(nk_handle handle, float h, const char *text, int len) {
	return byHandle(handle)->width(h, text, len);
}

void GUIFont::initializeFont(float height) {
	m_font.height = height;
}

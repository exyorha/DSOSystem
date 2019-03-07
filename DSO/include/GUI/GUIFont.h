#ifndef GUI_GUI_FONT_H
#define GUI_GUI_FONT_H

#include <GUI/NuklearPlatform.h>
#include <pixman.h>

class GUIFont {
protected:
	GUIFont();
	~GUIFont();

public:
	GUIFont(const GUIFont &other) = delete;
	GUIFont &operator =(const GUIFont &other) = delete;

	virtual float width(float height, const char *text, int length) const = 0;
	virtual void drawText(int x, int y, const char *string, int length, pixman_image_t *source, pixman_image_t *destination) const = 0;

	static inline GUIFont *byHandle(nk_handle handle) {
		return static_cast<GUIFont *>(handle.ptr);
	}

	inline const nk_user_font *font() const { return &m_font; }

protected:
	void initializeFont(float height);

private:
	static float widthProxy(nk_handle handle, float h, const char *text, int len);

	nk_user_font m_font;
};

#endif

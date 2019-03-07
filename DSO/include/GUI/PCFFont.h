#ifndef GUI_PCF_FONT_H
#define GUI_PCF_FONT_H

#include <GUI/GUIFont.h>

struct PCFAcceleratorTable;
struct PCFEncodingTable;
struct PCFBitmapTable;
struct PCFCompressedMetrics;
struct PCFUncompressedMetrics;

class PCFFont final : public GUIFont {
public:
	PCFFont(const void *data, size_t dataLength);
	~PCFFont();

	virtual float width(float height, const char *text, int length) const override;
	virtual void drawText(int x, int y, const char *string, int length, pixman_image_t *source, pixman_image_t *destination) const override;

private:
	unsigned int lookupCharacter(unsigned int codepoint) const;

	PCFUncompressedMetrics retrieveMetrics(unsigned int codepoint) const;
	pixman_image_t *retrieveBitmapForCharacter(unsigned int codepoint, PCFUncompressedMetrics &metrics) const;

	const PCFAcceleratorTable *m_accelerators;
	const PCFEncodingTable *m_encoding;
	const PCFBitmapTable *m_bitmaps;
	const PCFCompressedMetrics *m_compressedMetrics;
	const PCFUncompressedMetrics *m_metrics;
};

#endif

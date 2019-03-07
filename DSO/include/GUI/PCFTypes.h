#ifndef GUI_PCF_TYPES_H
#define GUI_PCF_TYPES_H

#include <stdint.h>

enum : uint32_t {
	PCFMagic = 0x70636601, // 'pcf\1'

	PCFProperties = 1 << 0,
	PCFAccelerators = 1 << 1,
	PCFMetrics = 1 << 2,
	PCFBitmaps = 1 << 3,
	PCFInkMetrics = 1 << 4,
	PCFBdfEncodings = 1 << 5,
	PCFSwidths = 1 << 6,
	PCFGlyphNames = 1 << 7,
	PCFBdfAccelerators = 1 << 8,

	PCFFormatGlyphPadMask = 3 << 0,
	PCFFormatMostSignificantByteFirst = 1 << 2,
	PCFFormatMostSignificantBitFirst = 1 << 3,
	PCFFormatCompressedMetrics = 1 << 8
};

struct PCFHeader {
	uint32_t magic;
	uint32_t tableCount;
};

struct PCFTableHeader {
	uint32_t type;
	uint32_t format;
	uint32_t size;
	uint32_t offset;
};

struct PCFTablePrefix {
	uint32_t format;
};

struct PCFCompressedMetrics {
	uint8_t leftBearing;
	uint8_t rightBearing;
	uint8_t width;
	uint8_t ascent;
	uint8_t descent;
};

struct PCFUncompressedMetrics {
	int16_t leftBearing;
	int16_t rightBearing;
	int16_t width;
	int16_t ascent;
	int16_t descent;
	uint16_t attributes;
};

struct PCFAcceleratorTable {
	uint8_t noOverlap;
	uint8_t constantMetrics;
	uint8_t terminalFont;
	uint8_t constantWidth;
	uint8_t inkInside;
	uint8_t inkMetrics;
	uint8_t drawDirection;
	uint8_t padding;
	int32_t fontAscent;
	int32_t fontDescent;
	int32_t maxOverlap;
	PCFUncompressedMetrics minbounds;
	PCFUncompressedMetrics maxbounds;
};

struct PCFEncodingTable {
	uint16_t minByte2;
	uint16_t maxByte2;
	uint16_t minByte1;
	uint16_t maxByte1;
	uint16_t defaultChar;
	/*uint16_t glyphIndices[];*/
};

struct PCFBitmapTable {
	uint32_t glyphCount;
	/*uint32_t offsets[glyphCount];*/
};

struct PCFBitmapTableRest {
	uint32_t bitmapSizes[4];
};

#endif

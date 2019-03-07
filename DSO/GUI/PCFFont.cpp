#include <GUI/PCFFont.h>
#include <GUI/PCFTypes.h>

#include <assert.h>
#include <stdio.h>

PCFFont::PCFFont(const void *data, size_t dataLength) : m_accelerators(nullptr), m_encoding(nullptr), m_bitmaps(nullptr), m_compressedMetrics(nullptr), m_metrics(nullptr) {
	auto header = static_cast<const PCFHeader *>(data);

	assert(header->magic == PCFMagic);

	auto tables = reinterpret_cast<const PCFTableHeader *>(header + 1);

	for (size_t index = 0; index < header->tableCount; index++) {
		auto &header = tables[index];

		auto &prefix = *reinterpret_cast<const PCFTablePrefix *>(static_cast<const unsigned char *>(data) + header.offset);

		assert((header.format & (PCFFormatMostSignificantByteFirst | PCFFormatMostSignificantBitFirst | PCFFormatGlyphPadMask)) == 2);
		assert(header.format == prefix.format);

		printf("PCF Table %zu: type %u, format %u (prefix: %u), size %u, offset %u\n",
			index, header.type, header.format, prefix.format, header.size, header.offset);

		if (header.type == PCFAccelerators || header.type == PCFBdfAccelerators) {
			m_accelerators = reinterpret_cast<const PCFAcceleratorTable *>(&prefix + 1);
		}
		else if (header.type == PCFBitmaps) {
			m_bitmaps = reinterpret_cast<const PCFBitmapTable *>(&prefix + 1);
		}
		else if (header.type == PCFMetrics) {
			if (header.format & PCFFormatCompressedMetrics) {
				m_compressedMetrics = reinterpret_cast<const PCFCompressedMetrics *>(reinterpret_cast<const unsigned char *>(&prefix + 1) + sizeof(uint16_t));
			}
			else {
				m_metrics = reinterpret_cast<const PCFUncompressedMetrics *>(reinterpret_cast<const unsigned char *>(&prefix + 1) + sizeof(uint32_t));
			}

		}
		else if (header.type == PCFBdfEncodings) {
			m_encoding = reinterpret_cast<const PCFEncodingTable *>(&prefix + 1);
		}
	}

	assert(m_accelerators && m_encoding && m_bitmaps && (m_compressedMetrics || m_metrics));

	initializeFont(static_cast<float>(m_accelerators->fontAscent + m_accelerators->fontDescent));
}

PCFFont::~PCFFont() {

}

float PCFFont::width(float height, const char *text, int len) const {
	float width = 0.0f;

	nk_rune codepoint;
	int clen;
	int cpos = 0;

	float pos = 0.0f;

	float xpos = 0.0f;

	while (cpos < len) {
		clen = nk_utf_decode(text + cpos, &codepoint, len - cpos);

		if (clen == 0)
			break;

		cpos += clen;

		PCFUncompressedMetrics metrics = retrieveMetrics(codepoint);

		if (cpos == clen) {
			xpos = metrics.leftBearing;
		}

		width = pos + metrics.width;
		pos += metrics.width;
	}

	return xpos + width + 16.0;
}

void PCFFont::drawText(int x, int y, const char *string, int length, pixman_image_t *source, pixman_image_t *destination) const {
	nk_rune codepoint;
	int clen;
	int cpos = 0;

	while (cpos < length) {
		clen = nk_utf_decode(string + cpos, &codepoint, length - cpos);

		if (clen == 0)
			break;

		cpos += clen;

		PCFUncompressedMetrics metrics;
		auto image = retrieveBitmapForCharacter(codepoint, metrics);

		pixman_image_composite(
			PIXMAN_OP_OVER,
			source,
			image,
			destination,
			0, 0, 0, 0,
			x + metrics.leftBearing,
			y + m_accelerators->fontAscent - metrics.ascent,
			metrics.rightBearing - metrics.leftBearing,
			metrics.ascent + metrics.descent
		);

		x += metrics.width;

		pixman_image_unref(image);
	}
}


unsigned int PCFFont::lookupCharacter(unsigned int codepoint) const {
	if (codepoint >= 0x10000)
		return m_encoding->defaultChar;

	auto byte1 = (codepoint >> 8) & 0xFF;
	auto byte2 = codepoint & 0xFF;

	if (byte1 < m_encoding->minByte1 || byte1 > m_encoding->maxByte1 ||
		byte2 < m_encoding->minByte2 || byte2 > m_encoding->maxByte2)
		return m_encoding->defaultChar;

	auto index = (byte1 - m_encoding->minByte1) * (m_encoding->maxByte2 - m_encoding->minByte2 + 1) + byte2 - m_encoding->minByte2;

	auto val = reinterpret_cast<const uint16_t *>(m_encoding + 1)[index];

	if (val == 0xFFFF)
		return m_encoding->defaultChar;
	else
		return val;
}

pixman_image_t *PCFFont::retrieveBitmapForCharacter(unsigned int codepoint, PCFUncompressedMetrics &metrics) const {
	auto index = lookupCharacter(codepoint);

	assert(index < m_bitmaps->glyphCount);

	auto offsets = reinterpret_cast<const uint32_t *>(m_bitmaps + 1);

	auto bitmapRest = reinterpret_cast<const PCFBitmapTableRest *>(&offsets[m_bitmaps->glyphCount]);
	auto bitmapData = reinterpret_cast<const unsigned char *>(bitmapRest + 1);

	auto offset = offsets[index];

	if (m_compressedMetrics) {
		const auto &metric = m_compressedMetrics[index];

		metrics.leftBearing = static_cast<int>(metric.leftBearing) - 0x80;
		metrics.rightBearing = static_cast<int>(metric.rightBearing) - 0x80;
		metrics.width = static_cast<int>(metric.width) - 0x80;
		metrics.ascent = static_cast<int>(metric.ascent) - 0x80;
		metrics.descent = static_cast<int>(metric.descent) - 0x80;
		metrics.attributes = 0;
	}
	else {
		assert(m_metrics);
		metrics = m_metrics[index];
	}

	assert((offset & 3) == 0);

	auto image = pixman_image_create_bits_no_clear(
		PIXMAN_a1,
		metrics.rightBearing - metrics.leftBearing,
		metrics.ascent + metrics.descent,
		const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(&bitmapData[offset])),
		(metrics.rightBearing - metrics.leftBearing + 31) / 32 * 4
	);

	assert(image);

	return image;
}

PCFUncompressedMetrics PCFFont::retrieveMetrics(unsigned int index) const {
	if (m_compressedMetrics) {
		PCFUncompressedMetrics metrics;

		const auto &metric = m_compressedMetrics[index];
		metrics.leftBearing = static_cast<int>(metric.leftBearing) - 0x80;
		metrics.rightBearing = static_cast<int>(metric.rightBearing) - 0x80;
		metrics.width = static_cast<int>(metric.width) - 0x80;
		metrics.ascent = static_cast<int>(metric.ascent) - 0x80;
		metrics.descent = static_cast<int>(metric.descent) - 0x80;
		metrics.attributes = 0;
		return metrics;
	}
	else {
		assert(m_metrics);
		return m_metrics[index];
	}
}


#ifndef NUKLEAR_RENDERER_H
#define NUKLEAR_RENDERER_H

#include <pixman.h>
#include <GUI/NuklearPlatform.h>

class NuklearRenderer {
public:
	NuklearRenderer(pixman_image_t *image);
	~NuklearRenderer();

	NuklearRenderer(const NuklearRenderer &other) = delete;
	NuklearRenderer &operator =(const NuklearRenderer &other) = delete;

	void renderContext(nk_context *ctx);

private:
	void renderCommand(const nk_command *cmd);
	void renderScissor(const nk_command_scissor *scissor);
	void renderLine(const nk_command_line *line);
	void renderRect(const nk_command_rect *rect);
	void renderRectFilled(const nk_command_rect_filled *rect);
	void renderRectMultiColor(const nk_command_rect_multi_color *rect);
	void renderCircleFilled(const nk_command_circle_filled *arc);
	void renderArc(const nk_command_arc *arc);
	void renderArcFilled(const nk_command_arc_filled *arc);
	void renderTriangleFilled(const nk_command_triangle_filled *arc);
	void renderText(const nk_command_text *text);

	void renderLineImpl(int x0, int y0, int x1, int y1, pixman_color_t &color, unsigned int thickness, void (NuklearRenderer::*drawPointFunc)(int x, int y, unsigned int thickness, pixman_color_t &color));
	void renderArcImpl(int cx, int cy, unsigned int r, pixman_color_t &color, unsigned int thickness, float startAngle, float endAngle, void (NuklearRenderer::*drawPointFunc)(int x, int y, unsigned int thickness, pixman_color_t &color));

	void drawPixelUnclipped(int x, int y, unsigned int thickness, pixman_color_t &color);
	void drawPixelClipped(int x, int y, unsigned int thickness, pixman_color_t &color);
	void drawPoint(int x, int y, unsigned int thickness, pixman_color_t &color);

	template<typename Functor>
	void bresenhamCircle(int w, int h, Functor &&processPoint);

	pixman_color_t translateColor(const nk_color &color);

	pixman_image_t *m_image;
	pixman_region16_t m_clip;
};

#endif

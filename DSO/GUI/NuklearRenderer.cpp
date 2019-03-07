#include <GUI/NuklearRenderer.h>
#include <GUI/GUIFont.h>

#include <algorithm>

#include <stdio.h>

#define PrintRenderTrace(...)

NuklearRenderer::NuklearRenderer(pixman_image_t *image) : m_image(image) {
	pixman_image_ref(m_image);
	pixman_region_init_rect(&m_clip, 0, 0, pixman_image_get_width(m_image), pixman_image_get_height(m_image));
	pixman_image_set_clip_region(m_image, &m_clip);
}

NuklearRenderer::~NuklearRenderer() {
	pixman_region_fini(&m_clip);
	pixman_image_unref(m_image);
}

void NuklearRenderer::renderContext(nk_context *ctx) {
	const nk_command *cmd = nullptr;
	nk_foreach(cmd, ctx) {
		renderCommand(cmd);
	}
}

void NuklearRenderer::renderScissor(const nk_command_scissor *scissor) {
	PrintRenderTrace("NK_COMMAND_SCISSOR(x=%d,y=%d,w=%u,h=%u)\n", scissor->x, scissor->y, scissor->w, scissor->h);

	pixman_region_fini(&m_clip);
	pixman_region_init_rect(&m_clip, scissor->x, scissor->y, scissor->w, scissor->h);
	pixman_region_intersect_rect(&m_clip, &m_clip, 0, 0, pixman_image_get_width(m_image), pixman_image_get_height(m_image));
	pixman_image_set_clip_region(m_image, &m_clip);
}

pixman_color_t NuklearRenderer::translateColor(const nk_color &color) {
	pixman_color_t ocolor;
	ocolor.red = color.r << 8;
	ocolor.green = color.g << 8;
	ocolor.blue = color.b << 8;
	ocolor.alpha = color.a << 8;
	return ocolor;
}

void NuklearRenderer::renderLine(const nk_command_line *line) {
	PrintRenderTrace("NK_COMMAND_LINE(line_thickness=%u,begin=(%d,%d),end=(%d,%d),color=%02X|%02X|%02X|%02X)\n",
		line->line_thickness,
		line->begin.x,
		line->begin.y,
		line->end.x,
		line->end.y,
		line->color.r,
		line->color.g,
		line->color.b,
		line->color.a);

	auto x0 = line->begin.x;
	auto y0 = line->begin.y;
	auto x1 = line->end.x;
	auto y1 = line->end.y;

	if (x1 < x0)
		std::swap(x0, x1);

	if (y1 < y0)
		std::swap(y0, y1);

	pixman_color_t color = translateColor(line->color);

	if (x0 == x1) {
		// Vertical line
		pixman_box32_t rect;
		rect.x1 = x0 - line->line_thickness / 2;
		rect.y1 = y0;
		rect.x2 = rect.x1 + line->line_thickness;
		rect.y2 = y1 + 1;

		pixman_image_fill_boxes(PIXMAN_OP_SRC, m_image, &color, 1, &rect);
	}
	else if (line->begin.y == line->begin.y) {
		// Horizontal line
		pixman_box32_t rect;
		rect.x1 = x0;
		rect.y1 = y0 - line->line_thickness / 2;
		rect.x2 = x1 + 1;
		rect.y2 = rect.y1 + line->line_thickness;

		pixman_image_fill_boxes(PIXMAN_OP_SRC, m_image, &color, 1, &rect);
	}
	else {
		// General line

		pixman_box16_t lineBox;
		lineBox.x1 = x0 - (line->line_thickness + 1) / 2;
		lineBox.y1 = y0 - (line->line_thickness + 1) / 2;
		lineBox.x2 = x1 + (line->line_thickness + 1) / 2 + 1;
		lineBox.y2 = y1 + (line->line_thickness + 1) / 2 + 1;
		auto queryResult = pixman_region_contains_rectangle(&m_clip, &lineBox);

		if (queryResult != PIXMAN_REGION_OUT) {
			auto func = &NuklearRenderer::drawPoint;

			if (line->line_thickness == 1) {
				if (queryResult == PIXMAN_REGION_IN) {
					func = &NuklearRenderer::drawPixelUnclipped;
				}
				else if (queryResult == PIXMAN_REGION_PART) {
					func = &NuklearRenderer::drawPixelClipped;
				}
			}

			renderLineImpl(x0, y0, x1, y1, color, line->line_thickness, func);
		}
	}
}

void NuklearRenderer::renderRect(const nk_command_rect *rect) {
	PrintRenderTrace("NK_COMMAND_RECT(rounding=%u,line_thickness=%u,x=%d,y=%d,w=%u,h=%u,color=%02X|%02X|%02X|%02X)\n",
		rect->rounding,
		rect->line_thickness,
		rect->x,
		rect->y,
		rect->w,
		rect->h,
		rect->color.r,
		rect->color.g,
		rect->color.b,
		rect->color.a);

	if (rect->rounding == 0) {
		nk_command_line line;
		line.line_thickness = rect->line_thickness;
		line.color = rect->color;
		line.color.r = 0xFF;
		line.color.g = 0xFF;
		line.color.b = 0xFF;
		line.color.a = 0xFF;

		line.begin.x = rect->x;
		line.begin.y = rect->y;
		line.end.x = rect->x + rect->w - 1;
		line.end.y = rect->y;
		renderLine(&line);

		line.begin.x = rect->x + rect->w - 1;
		line.begin.y = rect->y;
		line.end.x = rect->x + rect->w - 1;
		line.end.y = rect->y + rect->h - 1;
		renderLine(&line);

		line.end.x = rect->x + rect->w - 1;
		line.end.y = rect->y + rect->h - 1;
		line.begin.x = rect->x;
		line.begin.y = rect->y + rect->h - 1;
		renderLine(&line);

		line.end.x = rect->x;
		line.end.y = rect->y + rect->h - 1;
		line.begin.x = rect->x;
		line.begin.y = rect->y;
		renderLine(&line);
	}
	else {
		nk_command_line line;
		line.line_thickness = rect->line_thickness;
		line.color = rect->color;

		line.begin.x = rect->x + rect->rounding;
		line.begin.y = rect->y;
		line.end.x = rect->x + rect->w - 1 - rect->rounding;
		line.end.y = rect->y;
		renderLine(&line);

		line.begin.x = rect->x + rect->w - 1;
		line.begin.y = rect->y + rect->rounding;
		line.end.x = rect->x + rect->w - 1;
		line.end.y = rect->y + rect->h - 1 - rect->rounding;
		renderLine(&line);

		line.end.x = rect->x + rect->w - 1 - rect->rounding;
		line.end.y = rect->y + rect->h - 1;
		line.begin.x = rect->x + rect->rounding;
		line.begin.y = rect->y + rect->h - 1;
		renderLine(&line);

		line.end.x = rect->x;
		line.end.y = rect->y + rect->h - 1 - rect->rounding;
		line.begin.x = rect->x;
		line.begin.y = rect->y + rect->rounding;
		renderLine(&line);

		nk_command_arc arc;
		arc.line_thickness = rect->line_thickness;
		auto r = rect->rounding;
		arc.r = r;
		arc.color = rect->color;

		auto xc = rect->x + r;
		auto yc = rect->y + r;
		auto wc = rect->w - 2 * r;
		auto hc = rect->h - 2 * r;

		arc.cx = xc + wc - 1;
		arc.cy = yc;
		arc.a[0] = 0.0f;
		arc.a[1] = 90.0f;
		renderArc(&arc);

		arc.cx = xc;
		arc.cy = yc;
		arc.a[0] = 90.0f;
		arc.a[1] = 180.0f;
		renderArc(&arc);

		arc.cx = xc;
		arc.cy = yc + hc - 1;
		arc.a[0] = 270.0f;
		arc.a[1] = 0.0f;
		renderArc(&arc);

		arc.cx = xc + wc - 1;
		arc.cy = yc + hc - 1;
		arc.a[0] = 180.0f;
		arc.a[1] = 270.0f;
		renderArc(&arc);
	}
}

void NuklearRenderer::renderRectFilled(const nk_command_rect_filled *filled) {
	PrintRenderTrace("NK_COMMAND_RECT_FILLED(rounding=%u,x=%d,y=%d,w=%u,h=%u,color=%02X|%02X|%02X|%02X)\n",
		filled->rounding,
		filled->x,
		filled->y,
		filled->w,
		filled->h,
		filled->color.r,
		filled->color.g,
		filled->color.b,
		filled->color.a);

	if (filled->rounding == 0) {

		pixman_box32_t rect;
		rect.x1 = filled->x;
		rect.y1 = filled->y;
		rect.x2 = rect.x1 + filled->w;
		rect.y2 = rect.y1 + filled->h;

		pixman_color_t color = translateColor(filled->color);
		pixman_image_fill_boxes(PIXMAN_OP_SRC, m_image, &color, 1, &rect);
	}
	else {
		pixman_box32_t rects[3];

		// Main rectangle
		rects[0].x1 = filled->x + filled->rounding;
		rects[0].y1 = filled->y;
		rects[0].x2 = rects[0].x1 + filled->w - filled->rounding * 2;
		rects[0].y2 = rects[0].y1 + filled->h;

		// Left band
		rects[1].x1 = filled->x;
		rects[1].y1 = filled->y + filled->rounding;
		rects[1].x2 = rects[1].x1 + filled->rounding;
		rects[1].y2 = rects[1].y1 + filled->h - filled->rounding * 2;

		// Right band
		rects[2].x1 = filled->x + filled->w - filled->rounding;
		rects[2].y1 = filled->y + filled->rounding;
		rects[2].x2 = rects[2].x1 + filled->rounding;
		rects[2].y2 = rects[2].y1 + filled->h - filled->rounding * 2;

		pixman_color_t color = translateColor(filled->color);
		pixman_image_fill_boxes(PIXMAN_OP_SRC, m_image, &color, 3, rects);

		nk_command_arc_filled arc;

		auto r = filled->rounding;
		arc.r = r;
		arc.color = filled->color;

		auto xc = filled->x + r;
		auto yc = filled->y + r;
		auto wc = filled->w - 2 * r;
		auto hc = filled->h - 2 * r;

		arc.cx = xc + wc;
		arc.cy = yc;
		arc.a[0] = 0.0f;
		arc.a[1] = 90.0f;
		renderArcFilled(&arc);

		arc.cx = xc;
		arc.cy = yc;
		arc.a[0] = 90.0f;
		arc.a[1] = 180.0f;
		renderArcFilled(&arc);

		arc.cx = xc;
		arc.cy = yc + hc;
		arc.a[0] = 270.0f;
		arc.a[1] = 0.0f;
		renderArcFilled(&arc);

		arc.cx = xc + wc;
		arc.cy = yc + hc;
		arc.a[0] = 180.0f;
		arc.a[1] = 270.0f;
		renderArcFilled(&arc);
	}
}

void NuklearRenderer::renderRectMultiColor(const nk_command_rect_multi_color *rect) {
	PrintRenderTrace("NK_COMMAND_RECT_MULTI_COLOR(x=%d,y=%d,w=%u,h=%u,left=%02X|%02X|%02X|%02X,top=%02X|%02X|%02X|%02X,right=%02X|%02X|%02X|%02X,bottom=%02X|%02X|%02X|%02X)\n",
		rect->x,
		rect->y,
		rect->w,
		rect->h,
		rect->left.r,
		rect->left.g,
		rect->left.b,
		rect->left.a,
		rect->top.r,
		rect->top.g,
		rect->top.b,
		rect->top.a,
		rect->right.r,
		rect->right.g,
		rect->right.b,
		rect->right.a,
		rect->bottom.r,
		rect->bottom.g,
		rect->bottom.b,
		rect->bottom.a
	);

}

void NuklearRenderer::renderCircleFilled(const nk_command_circle_filled *circle) {
	PrintRenderTrace("NK_COMMAND_CIRCLE(x=%d,y=%d,w=%u,h=%u,color=%02X|%02X|%02X|%02X)\n",
		circle->x, circle->y, circle->w, circle->h, circle->color.r, circle->color.g, circle->color.b, circle->color.a);

	auto cx = circle->x + (circle->w + 1) / 2;
	auto cy = circle->y + (circle->h + 1) / 2;

	auto color = translateColor(circle->color);
	pixman_box32_t boxes[2];

	bresenhamCircle(circle->w, circle->h, [&](int x, int y) {
		boxes[0].x1 = cx - x;
		boxes[0].y1 = cy + y;
		boxes[0].x2 = cx + x + 1;
		boxes[0].y2 = cy + y + 1;

		boxes[1].x1 = cx - x;
		boxes[1].y1 = cy - y;
		boxes[1].x2 = cx + x + 1;
		boxes[1].y2 = cy - y + 1;

		pixman_image_fill_boxes(PIXMAN_OP_SRC, m_image, &color, 2, boxes);
	});
}

void NuklearRenderer::renderArc(const nk_command_arc *arc) {
	PrintRenderTrace("NK_COMMAND_ARC(cx=%d,cy=%d,r=%u,thickness=%u,a=(%f,%f),color=%02X|%02X|%02X|%02X)\n",
		arc->cx, arc->cy, arc->r, arc->line_thickness, arc->a[0], arc->a[1], arc->color.r, arc->color.g, arc->color.b, arc->color.a);

	pixman_box16_t arcBox;
	arcBox.x1 = arc->cx - arc->r - (arc->line_thickness + 1) / 2;
	arcBox.y1 = arc->cy - arc->r - (arc->line_thickness + 1) / 2;
	arcBox.x2 = arc->cx + arc->r + (arc->line_thickness + 1) / 2;
	arcBox.y2 = arc->cy + arc->r + (arc->line_thickness + 1) / 2;

	auto queryResult = pixman_region_contains_rectangle(&m_clip, &arcBox);

	if (queryResult != PIXMAN_REGION_OUT) {
		pixman_color_t color = translateColor(arc->color);

		auto func = &NuklearRenderer::drawPoint;

		if (arc->line_thickness == 1) {
			if (queryResult == PIXMAN_REGION_IN) {
				func = &NuklearRenderer::drawPixelUnclipped;
			}
			else {
				func = &NuklearRenderer::drawPixelClipped;
			}
		}

		renderArcImpl(arc->cx, arc->cy, arc->r, color, arc->line_thickness, arc->a[0], arc->a[1], func);
	}
}

template<typename Functor>
void NuklearRenderer::bresenhamCircle(int w, int h, Functor &&processPoint) {
	auto a2 = (w * w) / 4;
	auto b2 = (h * h) / 4;
	auto fa2 = 4 * a2;
	auto fb2 = 4 * b2;
	int x, y, sigma;

	if (w < 1 || h < 1)
		return;

	h = (h + 1) / 2;
	w = (w + 1) / 2;

	/* First half */
	for (x = 0, y = h, sigma = 2 * b2 + a2 * (1 - 2 * h); b2*x <= a2 * y; x++) {
		processPoint(x, y);

		if (sigma >= 0) {
			sigma += fa2 * (1 - y);
			y--;
		} sigma += b2 * ((4 * x) + 6);
	}

	/* Second half */
	for (x = w, y = 0, sigma = 2 * a2 + b2 * (1 - 2 * w); a2*y <= b2 * x; y++) {
		processPoint(x, y);

		if (sigma >= 0) {
			sigma += fb2 * (1 - x);
			x--;
		} sigma += a2 * ((4 * y) + 6);
	}


}

void NuklearRenderer::renderArcFilled(const nk_command_arc_filled *arc) {
	PrintRenderTrace("NK_COMMAND_ARC_FILLED(cx=%d,cy=%d,r=%u,a=(%f,%f),color=%02X|%02X|%02X|%02X)\n",
		arc->cx, arc->cy, arc->r, arc->a[0], arc->a[1], arc->color.r, arc->color.g, arc->color.b, arc->color.a);

	auto s = arc->a[0];

	if (s != 0 && s != 90 && s != 180 && s != 270)
		return;

	auto x0 = arc->cx;
	auto y0 = arc->cy;
	auto r = arc->r;

	auto xl = x0 - r;
	auto yt = y0 - r;

	// struct nk_vec2i pnts[3];
	pixman_triangle_t tri;

	tri.p1.x = pixman_int_to_fixed(arc->cx);
	tri.p1.y = pixman_int_to_fixed(arc->cy);
	tri.p3.x = pixman_int_to_fixed(arc->cx);
	tri.p3.y = pixman_int_to_fixed(arc->cy);

	auto mask = pixman_image_create_bits(PIXMAN_a8, arc->r * 2, arc->r * 2, nullptr, -1);

	bresenhamCircle(2 * r, 2 * r, [&](int x, int y) {
		if (s == 180) {
			tri.p2.x = pixman_int_to_fixed(x0 + x);
			tri.p2.y = pixman_int_to_fixed(y0 + y);
		}
		else if (s == 270) {
			tri.p2.x = pixman_int_to_fixed(x0 - x);
			tri.p2.y = pixman_int_to_fixed(y0 + y);
		}
		else if (s == 0) {
			tri.p2.x = pixman_int_to_fixed(x0 + x);
			tri.p2.y = pixman_int_to_fixed(y0 - y);
		}
		else if (s == 90) {
			tri.p2.x = pixman_int_to_fixed(x0 - x);
			tri.p2.y = pixman_int_to_fixed(y0 - y);
		}

		pixman_add_triangles(mask, -xl, -yt, 1, &tri);

		tri.p3 = tri.p2;
	});

	auto color = translateColor(arc->color);
	auto fill = pixman_image_create_solid_fill(&color);
	pixman_image_composite(PIXMAN_OP_OVER, fill, mask, m_image, 0, 0, 0, 0, xl, yt, r * 2, r * 2);
	pixman_image_unref(fill);
	pixman_image_unref(mask);
}

void NuklearRenderer::renderLineImpl(int x0, int y0, int x1, int y1, pixman_color_t &color, unsigned int thickness, void (NuklearRenderer::*drawPointFunc)(int x, int y, unsigned int thickness, pixman_color_t &color)) {
	auto dy = y1 - y0;
	auto dx = x1 - x0;

	int stepx, stepy;
	if (dy < 0) {
		dy = -dy;
		stepy = -1;
	}
	else
		stepy = 1;

	if (dx < 0) {
		dx = -dx;
		stepx = -1;
	}
	else
		stepx = 1;

	dy <<= 1;
	dx <<= 1;

	(this->*drawPointFunc)(x0, y0, thickness, color);
	if (dx > dy) {
		int fraction = dy - (dx >> 1);
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			(this->*drawPointFunc)(x0, y0, thickness, color);
		}
	}
	else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			(this->*drawPointFunc)(x0, y0, thickness, color);
		}
	}
}

void NuklearRenderer::renderArcImpl(int x0, int y0, unsigned int r, pixman_color_t &color, unsigned int thickness, float s, float endAngle, void (NuklearRenderer::*drawPointFunc)(int x, int y, unsigned int thickness, pixman_color_t &color)) {
	bresenhamCircle(r * 2, r * 2, [&](int x, int y) {
		if (s == 180)
			(this->*drawPointFunc)(x0 + x, y0 + y, thickness, color);
		else if (s == 270)
			(this->*drawPointFunc)(x0 - x, y0 + y, thickness, color);
		else if (s == 0)
			(this->*drawPointFunc)(x0 + x, y0 - y, thickness, color);
		else if (s == 90)
			(this->*drawPointFunc)(x0 - x, y0 - y, thickness, color);
	});
}

void NuklearRenderer::drawPixelUnclipped(int x, int y, unsigned int thickness, pixman_color_t &color) {
	auto pixel = reinterpret_cast<uint32_t *>(reinterpret_cast<unsigned char *>(pixman_image_get_data(m_image)) + pixman_image_get_stride(m_image) * y) + x;
	*pixel = ((color.red >> 8) & 0xFF) | (((color.green >> 8) & 0xFF) << 8) | (((color.blue >> 8) & 0xFF) << 16) | (((color.alpha >> 8) & 0xFF) << 24);
}

void NuklearRenderer::drawPixelClipped(int x, int y, unsigned int thickness, pixman_color_t &color) {
	if (pixman_region_contains_point(&m_clip, x, y, nullptr)) {
		drawPixelUnclipped(x, y, thickness, color);
	}
}

void NuklearRenderer::drawPoint(int x, int y, unsigned int thickness, pixman_color_t &color) {
	pixman_box32_t rect;
	rect.x1 = x - thickness / 2;
	rect.y1 = y - thickness / 2;
	rect.x2 = rect.x1 + thickness;
	rect.x2 = rect.x2 + thickness;

	pixman_image_fill_boxes(PIXMAN_OP_SRC, m_image, &color, 1, &rect);
}

void NuklearRenderer::renderTriangleFilled(const nk_command_triangle_filled *triangle) {
	PrintRenderTrace("NK_COMMAND_TRIANGLE_FILLED(a=(%d,%d),b=(%d,%d),c=(%d,%d),color=%02X|%02X|%02X|%02X)\n",
		triangle->a.x, triangle->a.y, triangle->b.x, triangle->b.y, triangle->c.x, triangle->c.y,
		triangle->color.r, triangle->color.g, triangle->color.b, triangle->color.a);

	auto color = translateColor(triangle->color);
	auto fill = pixman_image_create_solid_fill(&color);

	pixman_triangle_t tri;
	tri.p1.x = pixman_int_to_fixed(triangle->a.x);
	tri.p1.y = pixman_int_to_fixed(triangle->a.y);
	tri.p2.x = pixman_int_to_fixed(triangle->b.x);
	tri.p2.y = pixman_int_to_fixed(triangle->b.y);
	tri.p3.x = pixman_int_to_fixed(triangle->c.x);
	tri.p3.y = pixman_int_to_fixed(triangle->c.y);

	pixman_composite_triangles(
		PIXMAN_OP_OVER,
		fill,
		m_image,
		PIXMAN_a8,
		0, 0, 0, 0,
		1,
		&tri
	);

	pixman_image_unref(fill);
}

void NuklearRenderer::renderText(const nk_command_text *text) {
	printf("NK_COMMAND_TEXT(font=%p,background=%02X|%02X|%02X|%02X,foreground=%02X|%02X|%02X|%02X,x=%d,y=%d,w=%u,h=%u,height=%f,string=%.*s)\n",
		text->font,
		text->background.r, text->background.g, text->background.b, text->background.a,
		text->foreground.r, text->foreground.g, text->foreground.b, text->foreground.a,
		text->x, text->y, text->w, text->h, text->height, text->length, text->string);

	auto font = GUIFont::byHandle(text->font->userdata);

	auto color = translateColor(text->foreground);
	auto fill = pixman_image_create_solid_fill(&color);

	pixman_region16_t tempClip;
	pixman_region_init(&tempClip);
	pixman_region_intersect_rect(&tempClip, &m_clip, text->x, text->y, text->w, text->h);
	pixman_image_set_clip_region(m_image, &tempClip);
	pixman_region_fini(&tempClip);

	font->drawText(text->x, text->y, text->string, text->length, fill, m_image);

	pixman_image_set_clip_region(m_image, &m_clip);

	pixman_image_unref(fill);
}

void NuklearRenderer::renderCommand(const nk_command *cmd) {
	switch (cmd->type) {
	case NK_COMMAND_NOP:
		break;

	case NK_COMMAND_SCISSOR:
		renderScissor(reinterpret_cast<const nk_command_scissor *>(cmd));
		break;

	case NK_COMMAND_LINE:
		renderLine(reinterpret_cast<const nk_command_line *>(cmd));
		break;

	case NK_COMMAND_RECT:
		renderRect(reinterpret_cast<const nk_command_rect *>(cmd));
		break;

	case NK_COMMAND_RECT_FILLED:
		renderRectFilled(reinterpret_cast<const nk_command_rect_filled *>(cmd));
		break;

	case NK_COMMAND_RECT_MULTI_COLOR:
		renderRectMultiColor(reinterpret_cast<const nk_command_rect_multi_color *>(cmd));
		break;

	case NK_COMMAND_CIRCLE_FILLED:
		renderCircleFilled(reinterpret_cast<const nk_command_circle_filled *>(cmd));
		break;

	case NK_COMMAND_ARC:
		renderArc(reinterpret_cast<const nk_command_arc *>(cmd));
		break;

	case NK_COMMAND_ARC_FILLED:
		renderArcFilled(reinterpret_cast<const nk_command_arc_filled *>(cmd));
		break;

	case NK_COMMAND_TRIANGLE_FILLED:
		renderTriangleFilled(reinterpret_cast<const nk_command_triangle_filled *>(cmd));
		break;

	case NK_COMMAND_TEXT:
		renderText(reinterpret_cast<const nk_command_text *>(cmd));
		break;

	default:
		printf("Command of type %d\n", cmd->type);
		break;

	}
}

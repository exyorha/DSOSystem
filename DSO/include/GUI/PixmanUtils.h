#ifndef GUI_PIXMAN_UTILS_H
#define GUI_PIXMAN_UTILS_H

#include <pixman.h>

#include <memory>

struct PixmanImageDeleter {
	void operator()(pixman_image_t *image) const;
};

using PixmanImage = std::unique_ptr<pixman_image_t, PixmanImageDeleter>;

#endif

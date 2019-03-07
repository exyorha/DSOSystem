#include <GUI/PixmanUtils.h>

void PixmanImageDeleter::operator()(pixman_image_t *image) const {
	pixman_image_unref(image);
}

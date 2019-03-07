#include <GUI/Window.h>

Window::Window(const std::string &title, const struct nk_rect &bounds, nk_flags flags) : m_title(title), m_bounds(bounds), m_flags(flags) {
	sprintf(m_name, "%p", this);
}

Window::~Window() {

}

void Window::execute(nk_context *nk) {

}

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <GUI/NuklearPlatform.h>
#include <string>

class Window {
public:
	Window(const std::string &title, const struct nk_rect &bounds, nk_flags flags);
	virtual ~Window();

	Window(const Window &other) = delete;
	Window &operator =(const Window &other) = delete;

	inline const char *name() const {
		return m_name;
	}
	
	inline const std::string &title() const {
		return m_title;
	} 

	inline void setTitle(const std::string &title) {
		m_title = title;
	}

	inline void setTitle(std::string &&title) {
		m_title = std::move(title);
	}

	inline const struct nk_rect &bounds() const {
		return m_bounds;
	}

	inline void setBounds(const struct nk_rect &bounds) {
		m_bounds = bounds;
	}

	inline nk_flags flags() const {
		return m_flags;
	}

	inline void setFlags(nk_flags flags) {
		m_flags = flags;
	}

	virtual void execute(nk_context *nk);
	
private:
	char m_name[17];
	std::string m_title;
	struct nk_rect m_bounds;
	nk_flags m_flags;
};

#endif

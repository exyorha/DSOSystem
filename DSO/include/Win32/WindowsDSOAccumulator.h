#ifndef WIN32__WINDOWS_DSO_ACCUMULATOR__H
#define WIN32__WINDOWS_DSO_ACCUMULATOR__H

#include <DSO/DSOAccumulator.h>

class WindowsDSOAccumulator final : public DSOAccumulator {
public:
	WindowsDSOAccumulator(size_t width, size_t height);
	virtual ~WindowsDSOAccumulator();

	virtual const void *base() const override;
	virtual size_t pitch() const override;

private:
	void *m_base;
	size_t m_pitch;
};

#endif

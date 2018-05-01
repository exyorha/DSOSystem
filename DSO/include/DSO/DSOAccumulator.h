#ifndef DSO__DSO_ACCUMULATOR__H
#define DSO__DSO_ACCUMULATOR__H

#include <string.h>

class DSOAccumulator {
protected:
	DSOAccumulator();

public:
	virtual ~DSOAccumulator();

	DSOAccumulator(const DSOAccumulator &other) = delete;
	DSOAccumulator &operator =(const DSOAccumulator &other) = delete;

	virtual const void *base() const = 0;
	virtual size_t pitch() const = 0;
};

#endif

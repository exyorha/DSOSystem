#define NK_IMPLEMENTATION
#include <GUI/NuklearPlatform.h>

#include <stdio.h>

double nkplat_strtod(const char *str, const char **endptr) {
	return strtod(str, const_cast<char **>(endptr));
}

char *nkplat_dtoa(char *buffer, double value) {
	sprintf(buffer, "%f", value);

	return buffer;
}

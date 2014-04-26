
#if !defined(_DEBUG) && !defined(DEBUG)
#include <crtdefs.h>
#include <Windows.h>
int _snprintf(char* buf, size_t n, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	n = wvsprintfA(buf, format, ap);
	va_end(ap);
	return (int)(n);
}
#endif

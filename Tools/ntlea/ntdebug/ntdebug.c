
#include "ntdebug.h"
#include <stdarg.h>
#include <malloc.h>
#include <assert.h>
#include <Shlwapi.h>

#pragma auto_inline(off)
int ntprintfA(int cchlen, int level, char const* fmtstr, ...)
{
	UNREFERENCED_PARAMETER(level);
	va_list ap; va_start(ap, fmtstr);
	void* buffer = _alloca(cchlen * sizeof(char));
	int ret = wvnsprintfA((LPSTR)buffer, cchlen, fmtstr, ap); *((char*)buffer + min(ret, cchlen - 1)) = '\0';
	OutputDebugStringA((char const*)buffer);
	return (0);
}
int ntprintfW(int cchlen, int level, wchar_t const* fmtstr, ...)
{
	UNREFERENCED_PARAMETER(level);
	va_list ap; va_start(ap, fmtstr);
	void* buffer = _alloca(cchlen * sizeof(wchar_t));
	int ret = wvnsprintfW((LPWSTR)buffer, cchlen, fmtstr, ap); *((wchar_t*)buffer + min(ret, cchlen - 1)) = L'\0';
	OutputDebugStringW((wchar_t const*)buffer);
	return (0);
}
#pragma auto_inline(on)

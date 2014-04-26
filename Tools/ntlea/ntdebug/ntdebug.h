
#ifndef __NTDEBUG_H__
#define __NTDEBUG_H__

#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

	int ntprintfA(int cchlen, int level, char const* fmtstr, ...);
	int ntprintfW(int cchlen, int level, wchar_t const* fmtstr, ...);

#ifdef __cplusplus
}
#endif

#endif // __NTDEBUG_H__

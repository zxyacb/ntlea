
#ifndef __LIBLOCALE_H__
#define __LIBLOCALE_H__

#include <Mlang.h>

typedef struct {
	LONG	tzi;
	WCHAR	key[64];
	WCHAR	display[64];
	WCHAR	dlt[64];
	WCHAR	standard[64];
} TIMEZONEINFO, *PTIMEZONEINFO;

#ifdef __cplusplus 
#define DEF(n)	/**/=n
extern "C" {
#else
#define DEF(n)	/**/
#endif // 

// codepage : 
int EnumerateCodePage(void(*fnmimecallback)(UINT, PMIMECPINFO, LPVOID), LPVOID context, MIMECONTF mimefilter DEF(MIMECONTF_VALID));
int GetInfoFromCodePage(UINT uiCodePage, PMIMECPINFO pCodePageInfo/*OUT*/);

// locale : 
int EnumerateLocale(void(*fnlocalecallback)(UINT, PRFC1766INFO, LPVOID), LPVOID context);
int GetInfoFromLocale(LCID uiLocale, PRFC1766INFO pLocaleInfo/*OUT*/);

// timezone : 
int EnumerateTimezone(void(*fntimezonecallback)(UINT, PTIMEZONEINFO, LPVOID), LPVOID context);
int GetInfoFromTimezone(LPCWSTR pKeyname, PTIMEZONEINFO pTimezoneInfo/*OUT*/);
int GetInfoCountTimezone(void);

#ifdef __cplusplus 
}
#endif // 

#endif // __LIBLOCALE_H__

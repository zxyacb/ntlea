
#ifndef __NTHOOKFUN_H__
#define __NTHOOKFUN_H__

typedef struct {
	void* lpGetTimeZoneInformation;
	void* lpEnumFontFamiliesExA;
	void* lpEnumFontFamiliesExW;
	void* lpEnumFontsA;
	void* lpEnumFontsW;
	void* lpUser32SEHAddress;
	void* lpProcInfoAddress;
	void* lpCreateProcAnsiAddress;
	void* lpCreateProcUniAddress;
	void* lpCallWindowProcAddress;
	void* lpSetWindowLongAddress;
	void* lpParameterAddress;
	void* lpMBtoUniAddress;
	void* lpUnitoMBAddress;
	void* lpMBtoWCAddress;
	void* lpWCtoMBAddress;
	void* lpCPInfoAddress;
	void* lpGetWindowLongPtrAddress;
	void* lpCreateFileAddress;
	void* lpShellExecuteAddress;
	void* lpCompareStringAddress;
	void* lpVerQueryValueAddress;
	void* lpGetStockObjectAddress;
} Addresses;

#endif // __NTHOOKFUN_H__

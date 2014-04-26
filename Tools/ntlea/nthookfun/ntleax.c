
#include <Windows.h>

#include "nthookfun.h"		// require that struct for jump address 

extern Addresses addresses;

// --------------- hook head + tail ----------------- //
BOOL WINAPI CreateProcessInternalA(LPCVOID lpApplicationName, LPCVOID lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCVOID lpCurrentDirectory,
	LPCVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	typedef BOOL(WINAPI *Fn)(LPCSTR, LPCSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
		BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
	return ((Fn)(DWORD_PTR)addresses.lpCreateProcAnsiAddress)(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, (LPSTARTUPINFOA)lpStartupInfo, lpProcessInformation);
}
BOOL WINAPI CreateProcessInternalW(LPCVOID lpApplicationName, LPCVOID lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCVOID lpCurrentDirectory,
	LPCVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	typedef BOOL(WINAPI *Fn)(LPCWSTR, LPCWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
		BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
	return ((Fn)(DWORD_PTR)addresses.lpCreateProcUniAddress)(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, (LPSTARTUPINFOW)lpStartupInfo, lpProcessInformation);
}

int WINAPI EnumFontFamiliesExInternalA(HDC hdc, LPLOGFONTA lpLogfont, FONTENUMPROCA lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags)
{
	typedef int(WINAPI *Fn)(HDC, LPLOGFONTA, FONTENUMPROCA, LPARAM, DWORD);
	return ((Fn)(DWORD_PTR)addresses.lpEnumFontFamiliesExA)(hdc, lpLogfont, lpEnumFontFamExProc, lParam, dwFlags);
}
int WINAPI EnumFontFamiliesExInternalW(HDC hdc, LPLOGFONTW lpLogfont, FONTENUMPROCW lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags)
{
	typedef int(WINAPI *Fn)(HDC, LPLOGFONTW, FONTENUMPROCW, LPARAM, DWORD);
	return ((Fn)(DWORD_PTR)addresses.lpEnumFontFamiliesExW)(hdc, lpLogfont, lpEnumFontFamExProc, lParam, dwFlags);
}
int WINAPI EnumFontsInternalA(HDC hdc, LPCSTR lpszFamily, FONTENUMPROCA lpEnumFontFamProc, LPARAM lParam)
{
	typedef int(WINAPI *Fn)(HDC, LPCSTR, FONTENUMPROCA, LPARAM);
	return ((Fn)(DWORD_PTR)addresses.lpEnumFontsA)(hdc, lpszFamily, lpEnumFontFamProc, lParam);
}
int WINAPI EnumFontsInternalW(HDC hdc, LPCWSTR lpszFamily, FONTENUMPROCW lpEnumFontFamProc, LPARAM lParam)
{
	typedef int(WINAPI *Fn)(HDC, LPCWSTR, FONTENUMPROCW, LPARAM);
	return ((Fn)(DWORD_PTR)addresses.lpEnumFontsW)(hdc, lpszFamily, lpEnumFontFamProc, lParam);
}

BOOL WINAPI VerQueryValueInternal(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{
	typedef BOOL(WINAPI *Fn)(LPCVOID, LPCSTR, LPVOID, PUINT);
	return ((Fn)(DWORD_PTR)addresses.lpVerQueryValueAddress)(pBlock, lpSubBlock, lplpBuffer, puLen);
}

DWORD WINAPI GetTimeZoneInformationInternal(LPTIME_ZONE_INFORMATION lpTimeZoneInformation)
{
	typedef DWORD(WINAPI *Fn)(LPTIME_ZONE_INFORMATION);
	return ((Fn)(DWORD_PTR)addresses.lpGetTimeZoneInformation)(lpTimeZoneInformation);
}

LONG_PTR WINAPI SetWindowLongPtrJ(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
	typedef LONG_PTR(WINAPI *Fn)(HWND, int, LONG_PTR);
	return ((Fn)(DWORD_PTR)addresses.lpSetWindowLongAddress)(hWnd, nIndex, dwNewLong);
}

LRESULT WINAPI CallWindowProcJ(WNDPROC PrevWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	typedef LRESULT(WINAPI *Fn)(WNDPROC, HWND, UINT, WPARAM, LPARAM);
	return ((Fn)(DWORD_PTR)addresses.lpCallWindowProcAddress)(PrevWindowProc, hWnd, uMsg, wParam, lParam);
}

int WINAPI UnicodeToMultiByteJ(LPSTR AnsiBuffer, DWORD MultiByteLength,
	LPDWORD lpNumberOfBytesConverted, LPCWSTR UnicodeBuffer, DWORD WideCharLength)
{
	typedef int (WINAPI *Fn)(LPSTR, DWORD, LPDWORD, LPCWSTR, DWORD);
	return ((Fn)(DWORD_PTR)addresses.lpUnitoMBAddress)(AnsiBuffer, MultiByteLength, lpNumberOfBytesConverted, UnicodeBuffer, WideCharLength);
}

int WINAPI MultiByteToUnicodeJ(LPWSTR UnicodeBuffer, DWORD WideCharLength,
	LPDWORD lpNumberOfBytesConverted, LPCSTR AnsiBuffer, DWORD MultiByteLength)
{
	typedef int(WINAPI *Fn)(LPWSTR, DWORD, LPDWORD, LPCSTR, DWORD);
	return ((Fn)(DWORD_PTR)addresses.lpMBtoUniAddress)(UnicodeBuffer, WideCharLength, lpNumberOfBytesConverted, AnsiBuffer, MultiByteLength);
}

LONG WINAPI GetWindowLongPtrJ(HWND hWnd, int nIndex)
{
	typedef LONG(WINAPI *Fn)(HWND, int);
	return ((Fn)(DWORD_PTR)addresses.lpGetWindowLongPtrAddress)(hWnd, nIndex);
}

DWORD WINAPI VerQueryValueJ(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{
	typedef DWORD(WINAPI *Fn)(LPCVOID, LPCSTR, LPVOID*, PUINT);
	return ((Fn)(DWORD_PTR)addresses.lpVerQueryValueAddress)(pBlock, lpSubBlock, lplpBuffer, puLen);
}

int WINAPI CompareStringJ(LCID Locale, DWORD dwCmpFlags, LPCSTR lpString1, int cchCount1, LPCSTR lpString2, int cchCount2)
{
	typedef int(WINAPI *Fn)(LCID, DWORD, LPCSTR, int, LPCSTR, int);
	return ((Fn)(DWORD_PTR)addresses.lpCompareStringAddress)(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
}

BOOL WINAPI GetCPInfoJ(UINT CodePage, LPCPINFO lpCPInfo)
{
	typedef BOOL(WINAPI *Fn)(UINT, LPCPINFO);
	return ((Fn)(DWORD_PTR)addresses.lpCPInfoAddress)(CodePage, lpCPInfo);
}

HANDLE WINAPI CreateFileJ(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	typedef HANDLE(WINAPI *Fn)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
	return ((Fn)(DWORD_PTR)addresses.lpCreateFileAddress)(lpFileName, dwDesiredAccess, dwShareMode,
		lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HINSTANCE WINAPI ShellExecuteJ(HWND hwnd,
	LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	typedef HINSTANCE(WINAPI *Fn)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);
	return ((Fn)(DWORD_PTR)addresses.lpShellExecuteAddress)(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

int WINAPI MBtoWCJ(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	typedef int(WINAPI *Fn)(UINT, DWORD, LPCSTR, int, LPWSTR, int);
	return ((Fn)(DWORD_PTR)addresses.lpMBtoWCAddress)(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

int WINAPI WCtoMBJ(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
	typedef int(WINAPI *Fn)(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL);
	return ((Fn)(DWORD_PTR)addresses.lpWCtoMBAddress)(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte,
		lpDefaultChar, lpUsedDefaultChar);
}

HGDIOBJ WINAPI GetStockObjectJ(int fnObject) {
	typedef HGDIOBJ(WINAPI *Fn)(int);
	return ((Fn)(DWORD_PTR)addresses.lpGetStockObjectAddress)(fnObject);
}

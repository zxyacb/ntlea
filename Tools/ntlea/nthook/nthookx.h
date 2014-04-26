
#ifndef __NTLEAX_H__
#define __NTLEAX_H__

#include <Windows.h>

// call : 
BOOL WINAPI CreateProcessInternalA(LPCVOID lpApplicationName, LPCVOID lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCVOID lpCurrentDirectory, LPCVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
BOOL WINAPI CreateProcessInternalW(LPCVOID lpApplicationName, LPCVOID lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCVOID lpCurrentDirectory, LPCVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
int WINAPI EnumFontFamiliesExInternalA(HDC hdc, LPLOGFONTA lpLogfont, FONTENUMPROCA lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags);
int WINAPI EnumFontFamiliesExInternalW(HDC hdc, LPLOGFONTW lpLogfont, FONTENUMPROCW lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags); 
int WINAPI EnumFontsInternalA(HDC hdc, LPCSTR lpszFamily, FONTENUMPROCA lpEnumFontFamProc, LPARAM lParam);
int WINAPI EnumFontsInternalW(HDC hdc, LPCWSTR lpszFamily, FONTENUMPROCW lpEnumFontFamProc, LPARAM lParam);
BOOL WINAPI VerQueryValueInternal(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);
DWORD WINAPI GetTimeZoneInformationInternal(LPTIME_ZONE_INFORMATION lpTimeZoneInformation);
HWND WINAPI CreateWindowExInternal(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

// jump : 
LONG_PTR WINAPI SetWindowLongPtrJ(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
LRESULT WINAPI CallWindowProcJ(WNDPROC PrevWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI UnicodeToMultiByteJ(LPSTR AnsiBuffer, DWORD MultiByteLength,
	LPDWORD lpNumberOfBytesConverted, LPCWSTR UnicodeBuffer, DWORD WideCharLength);
int WINAPI MultiByteToUnicodeJ(LPWSTR UnicodeBuffer, DWORD WideCharLength,
	LPDWORD lpNumberOfBytesConverted, LPCSTR AnsiBuffer, DWORD MultiByteLength);
LONG_PTR WINAPI GetWindowLongPtrJ(HWND hWnd, int nIndex);
DWORD WINAPI VerQueryValueJ(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);
int WINAPI CompareStringJ(LCID Locale, DWORD dwCmpFlags, LPCSTR lpString1, int cchCount1, LPCSTR lpString2, int cchCount2);
BOOL WINAPI GetCPInfoJ(UINT CodePage, LPCPINFO lpCPInfo);
HANDLE WINAPI CreateFileJ(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
HINSTANCE WINAPI ShellExecuteJ(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
int WINAPI MBtoWCJ(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
int WINAPI WCtoMBJ(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
HGDIOBJ WINAPI GetStockObjectJ(int fnObject);

#endif // __NTLEAX_H__

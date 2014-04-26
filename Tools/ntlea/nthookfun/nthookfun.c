
#include <Windows.h>
#include <assert.h>

#include "../nthook/nthook.h"
#include "nthookfun.h"
#include "nthookextern.h"

static LCID		WINAPI HookGetThreadLocale(void) { return HookGetLocaleID(); }
static LANGID	WINAPI HookGetSystemDefaultUILanguage(void) { return (LANGID)HookGetLocaleID(); }
static LANGID	WINAPI HookGetUserDefaultUILanguage(void) { return (LANGID)HookGetLocaleID(); }
static LCID		WINAPI HookGetSystemDefaultLCID(void) { return HookGetLocaleID(); }
static LCID		WINAPI HookGetUserDefaultLCID(void) { return HookGetLocaleID(); }
static LANGID	WINAPI HookGetSystemDefaultLangID(void) { return (LANGID)HookGetLocaleID(); }
static LANGID	WINAPI HookGetUserDefaultLangID(void) { return (LANGID)HookGetLocaleID(); }
static UINT		WINAPI HookGetOEMCP(void) { return HookGetACP(); }

VOID HookAnsiFunctions(Settings const* psettings)
{
	HMODULE hDLL; FARPROC symbol; LPVOID retaddr;
	// --- 
	hDLL = LoadLibraryA("ntdll.dll");
	if (psettings->bChinesePath == 0) {
		retaddr = addresses.lpUnitoMBAddress = (LPBYTE)HookDllFunc("RtlUnicodeToMultiByteN", (LPVOID)(DWORD_PTR)HookUnicodeToMultiByte, hDLL);
		assert(retaddr);
	}

	retaddr = addresses.lpMBtoUniAddress = (LPBYTE)HookDllFunc("RtlMultiByteToUnicodeN", (LPVOID)(DWORD_PTR)HookMultiByteToUnicode, hDLL);
	assert(retaddr);
	retaddr = HookDllFunc("RtlMultiByteToUnicodeSize", (LPVOID)(DWORD_PTR)HookMultiByteToUnicodeSize, hDLL);
	assert(retaddr);
	retaddr = HookDllFunc("RtlUnicodeToMultiByteSize", (LPVOID)(DWORD_PTR)HookUnicodeToMultiByteSize, hDLL);
	assert(retaddr);
	retaddr = HookDllFunc("RtlUnicodeToOemN", (LPVOID)(DWORD_PTR)HookUnicodeToMultiByte, hDLL);
	assert(retaddr);
	retaddr = HookDllFunc("RtlOemToUnicodeN", (LPVOID)(DWORD_PTR)HookMultiByteToUnicode, hDLL);
	assert(retaddr);
	// fixed symbols : 
	symbol = GetProcAddress(hDLL, "NlsAnsiCodePage");
	if (symbol) {
		*(DWORD*)(DWORD_PTR)symbol = psettings->dwCodePage; // global var as symbol, change it
	}

	hDLL = LoadLibraryA("Shell32.dll");
	retaddr = HookDllFunc("SHBrowseForFolderA", (LPVOID)(DWORD_PTR)HookSHBrowseForFolder, hDLL);
	assert(retaddr);
	retaddr = HookDllFunc("SHGetPathFromIDListA", (LPVOID)(DWORD_PTR)HookSHGetPathFromIDList, hDLL);
	assert(retaddr);
	retaddr = addresses.lpShellExecuteAddress = (LPBYTE)HookDllFunc("ShellExecuteA", (LPVOID)(DWORD_PTR)HookShellExecute, hDLL);
	assert(retaddr);

	// --- 
	hDLL = LoadLibraryA("gdi32.dll");
	retaddr = HookDllFunc("GdiGetCodePage", (LPVOID)(DWORD_PTR)HookGdiGetCodePage, hDLL); // no needs free
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetACP, (LPVOID)(DWORD_PTR)HookGetACP, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetOEMCP, (LPVOID)(DWORD_PTR)HookGetOEMCP, NULL);
	assert(retaddr);
	retaddr = addresses.lpCPInfoAddress = HookDllFunc((LPCSTR)(DWORD_PTR)GetCPInfo, (LPVOID)(DWORD_PTR)HookGetCPInfo, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)WinExec, (LPVOID)(DWORD_PTR)HookWinExec, NULL);
	assert(retaddr);

#if NoUsed
	HookDllFunc((LPCSTR)(DWORD_PTR)GetCPInfoExA, (LPVOID)(DWORD_PTR)HookGetCPInfoEx, NULL);
#endif

	if (psettings->bChinesePath != 0) {
		retaddr = addresses.lpCreateFileAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CreateFileA, (LPVOID)(DWORD_PTR)HookCreateFile, NULL);
		assert(retaddr);
	}
	retaddr = addresses.lpCompareStringAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CompareStringA, (LPVOID)(DWORD_PTR)HookCompareString, NULL);
	assert(retaddr);
	retaddr = addresses.lpCreateProcAnsiAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CreateProcessA, (LPVOID)(DWORD_PTR)HookCreateProcessA, NULL);
	assert(retaddr);
	retaddr = addresses.lpCreateProcUniAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CreateProcessW, (LPVOID)(DWORD_PTR)HookCreateProcessW, NULL);
	assert(retaddr);
	retaddr = addresses.lpGetTimeZoneInformation = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)GetTimeZoneInformation, (LPVOID)(DWORD_PTR)HookGetTimeZoneInformation, NULL);
	assert(retaddr);
	retaddr = addresses.lpVerQueryValueAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)VerQueryValueA, (LPVOID)(DWORD_PTR)HookVerQueryValue, NULL);
	assert(retaddr);

	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetThreadLocale, (LPVOID)(DWORD_PTR)HookGetThreadLocale, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetSystemDefaultUILanguage, (LPVOID)(DWORD_PTR)HookGetSystemDefaultUILanguage, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetUserDefaultUILanguage, (LPVOID)(DWORD_PTR)HookGetUserDefaultUILanguage, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetSystemDefaultLCID, (LPVOID)(DWORD_PTR)HookGetSystemDefaultLCID, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetUserDefaultLCID, (LPVOID)(DWORD_PTR)HookGetUserDefaultLCID, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetSystemDefaultLangID, (LPVOID)(DWORD_PTR)HookGetSystemDefaultLangID, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetUserDefaultLangID, (LPVOID)(DWORD_PTR)HookGetUserDefaultLangID, NULL);
	assert(retaddr);

	LPSTR  cmdlinea = GetCommandLineA();
	LPWSTR cmdlinew = GetCommandLineW();
	WideCharToMultiByte(psettings->dwCodePage, 0, cmdlinew, -1, cmdlinea, MAX_PATH << 1, NULL, NULL); // overwrite it!!

	retaddr = addresses.lpWCtoMBAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)WideCharToMultiByte, (LPVOID)(DWORD_PTR)HookWCtoMB, NULL);
	assert(retaddr);
	retaddr = addresses.lpMBtoWCAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)MultiByteToWideChar, (LPVOID)(DWORD_PTR)HookMBtoWC, NULL);
	assert(retaddr);

#if NoUsed
	psettings->lpFindResourceAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)FindResourceExA, (LPVOID)(DWORD_PTR)HookFindResourceEx, NULL);
	psettings->lpRegisterClassAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)RegisterClassA, (LPVOID)(DWORD_PTR)HookRegisterClass, NULL);
	psettings->lpRegisterClassExAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)RegisterClassExA, (LPVOID)(DWORD_PTR)HookRegisterClassEx, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)SetClassLongA, (LPVOID)(DWORD_PTR)HookSetClassLong, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetClassInfoA, (LPVOID)(DWORD_PTR)HookGetClassInfo, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetClassInfoExA, (LPVOID)(DWORD_PTR)HookGetClassInfoEx, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)UnregisterClassA, (LPVOID)(DWORD_PTR)HookUnregisterClass, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetClassLongA, (LPVOID)(DWORD_PTR)HookGetClassLong, NULL);
#endif

	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)DefWindowProcA, (LPVOID)(DWORD_PTR)HookDefWindowProc, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)DefMDIChildProcA, (LPVOID)(DWORD_PTR)HookDefMDIChildProc, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)DefDlgProcA, (LPVOID)(DWORD_PTR)HookDefDlgProc, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)DefFrameProcA, (LPVOID)(DWORD_PTR)HookDefFrameProc, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)CreateWindowExA, (LPVOID)(DWORD_PTR)HookCreateWindowEx, NULL);
	assert(retaddr);
	retaddr = addresses.lpCallWindowProcAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CallWindowProcA, (LPVOID)(DWORD_PTR)HookCallWindowProc, NULL);
	assert(retaddr);

	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)SetWindowTextA, (LPVOID)(DWORD_PTR)HookSetWindowText, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetWindowTextA, (LPVOID)(DWORD_PTR)HookGetWindowText, NULL);
	assert(retaddr);

	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)DialogBoxParamA, (LPVOID)(DWORD_PTR)HookDialogBoxParam, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)DialogBoxIndirectParamA, (LPVOID)(DWORD_PTR)HookDialogBoxIndirectParam, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)CreateDialogParamA, (LPVOID)(DWORD_PTR)HookCreateDialogParam, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)CreateDialogIndirectParamA, (LPVOID)(DWORD_PTR)HookCreateDialogIndirectParam, NULL);
	assert(retaddr);

	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)SendMessageA, (LPVOID)(DWORD_PTR)HookSendMessage, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)SendMessageCallbackA, (LPVOID)(DWORD_PTR)HookSendMessageCallback, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)SendMessageTimeoutA, (LPVOID)(DWORD_PTR)HookSendMessageTimeout, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)SendNotifyMessageA, (LPVOID)(DWORD_PTR)HookSendNotifyMessage, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)PostMessageA, (LPVOID)(DWORD_PTR)HookPostMessage, NULL);
	assert(retaddr);

	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)SetWindowLongPtrA, (LPVOID)(DWORD_PTR)HookSetWindowLongPtr, NULL);
	assert(retaddr);
	retaddr = addresses.lpSetWindowLongAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)SetWindowLongPtrW, (LPVOID)(DWORD_PTR)HookSetWindowLongPtrW, NULL);
	assert(retaddr);
	retaddr = addresses.lpGetWindowLongPtrAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)GetWindowLongPtrA, (LPVOID)(DWORD_PTR)HookGetWindowLongPtr, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)GetMenuStringA, (LPVOID)(DWORD_PTR)HookGetMenuString, NULL);
	assert(retaddr);

#ifdef Testing
	HookDllFunc((LPCSTR)(DWORD_PTR)GetMenuItemInfoA, (LPVOID)(DWORD_PTR)HookGetMenuItemInfo, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)SetMenuItemInfoA, (LPVOID)(DWORD_PTR)HookSetMenuItemInfo, NULL);
#endif
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)IsDBCSLeadByte, (LPVOID)(DWORD_PTR)HookIsDBCSLeadByte, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)CharPrevA, (LPVOID)(DWORD_PTR)HookCharPrev, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)CharNextA, (LPVOID)(DWORD_PTR)HookCharNext, NULL);
	assert(retaddr);
	retaddr = addresses.lpEnumFontFamiliesExA = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontFamiliesExA, (LPVOID)(DWORD_PTR)HookEnumFontFamiliesExA, NULL);
	assert(retaddr);
	retaddr = addresses.lpEnumFontFamiliesExW = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontFamiliesExW, (LPVOID)(DWORD_PTR)HookEnumFontFamiliesExW, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontsA, (LPVOID)(DWORD_PTR)HookEnumFontsA, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontsW, (LPVOID)(DWORD_PTR)HookEnumFontsW, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontFamiliesA, (LPVOID)(DWORD_PTR)HookEnumFontFamiliesA, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontFamiliesW, (LPVOID)(DWORD_PTR)HookEnumFontFamiliesW, NULL);
	assert(retaddr);
	retaddr = HookDllFunc((LPCSTR)(DWORD_PTR)CreateFontIndirectA, (LPVOID)(DWORD_PTR)HookCreateFontIndirect, NULL);
	assert(retaddr);
}

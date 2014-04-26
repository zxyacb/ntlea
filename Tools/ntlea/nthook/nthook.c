
#include <Windows.h>

#include "../ntlea.h"
#include "../ntproc/ntproc.h"

#include "commctrldefs.h"
#include "nthook.h"
#include "nthookx.h"
#include "ntextern.h"

#include <crtdefs.h>
#include <assert.h>
#include <Shlwapi.h>

static LPCSTR szNtleaWndAscData = "NtleaWndAscData"; // CreateGlobalAtom -> ebx
static LPCSTR szNtleaDlgAscData = "NtleaDlgAscData";
Settings settings = { 0 };

INLINE LPVOID AllocateZeroedMemory(SIZE_T size/*eax*/) {
	assert(size);
	return HeapAlloc(settings.hHeap, HEAP_ZERO_MEMORY, size);
}
INLINE LPVOID AllocateHeapInternal(SIZE_T size/*ecx*/) {
	assert(size);
	return HeapAlloc(settings.hHeap, 0, size);
}
INLINE NTLEA_TLS_DATA* GetTlsValueInternal(void) {
	DWORD n = GetLastError();
	NTLEA_TLS_DATA* p = (NTLEA_TLS_DATA*)TlsGetValue(settings.nTlsIndex);
	SetLastError(n); // thus the tlsgetvalue won't affect the env 
	if (!p) {
		p = (NTLEA_TLS_DATA*)AllocateZeroedMemory(sizeof(NTLEA_TLS_DATA));
		TlsSetValue(settings.nTlsIndex, p);
		for (int i = 0; i < MAXSYSCLASSDESC; ++i) {
			WNDCLASSA wndclassa;
			if (GetClassInfoA(NULL, SystemClassNameA[i], &wndclassa)) {
				p->SystemClassDesc[i].AnsiSystemClassProc = wndclassa.lpfnWndProc;
			}
			WNDCLASSW wndclassw;
			if (GetClassInfoW(NULL, SystemClassNameW[i], &wndclassw)) {
				p->SystemClassDesc[i].UnicodeSystemClassProc = wndclassw.lpfnWndProc;
			}
			//	ntprintfA(256, 1, "info: %s - %p %p\n", SystemClassNameA[i], wndclassa.lpfnWndProc, wndclassw.lpfnWndProc);
		}
		SetLastError(0); // also restore the errorstate !
	}
	return p;
}
INLINE LPCWSTR MultiByteToWideCharInternal(LPCSTR lpString)
{
	int size = lstrlenA(lpString)/* size without '\0' */, n = 0;
	LPWSTR wstr = (LPWSTR)AllocateHeapInternal((size + 1) << 1);
	if (wstr) {
		n = MultiByteToWideChar(CP_ACP, 0, lpString, size, wstr, size);
		wstr[n] = L'\0'; // make tail ! 
	}
	return wstr;
}
INLINE LPCSTR WideCharToMultiByteInternal(LPCWSTR lpString)
{
	int size = lstrlenW(lpString)/* size without '\0' */, n = 0;
	LPSTR str = (LPSTR)AllocateHeapInternal((size + 1) << 1); // TODO: support UTF-8 3bytes ??? 
	if (str) {
		n = WideCharToMultiByte(CP_ACP, 0, lpString, size, str, size << 1, NULL, NULL);
		str[n] = '\0'; // make tail ! 
	}
	return str;
}
INLINE VOID FreeStringInternal(LPVOID pBuffer/*ecx*/)
{
	HeapFree(settings.hHeap, 0, pBuffer);
}

static VOID ShowUnhandledExceptionMessage(char const* errorstr)
{
	SetUnhandledExceptionFilter(NULL);
	if (settings.bErrorFlag == -1) { // in case crash recursively !! 
		TerminateProcess(GetCurrentProcess(), (DWORD)-1);
	}
	else {
		char errdesc[512];
		settings.bErrorFlag = -1;
		wsprintfA(errdesc, szUnhandledExceptionText, errorstr);
		int i = MessageBoxA(GetForegroundWindow(), errdesc, "NTLEA internal exception", MB_ICONHAND);
		ExitProcess(i);
	}
}

extern void InitMiniDump(void const* basename, void const* dumppath);
extern void FreeMiniDump(void);

static DWORD CALLBACK InitUnicodeLayer(DWORD param);
BOOL WINAPI RunDllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	LPVOID p; HANDLE thread; UNREFERENCED_PARAMETER((hinstDll, lpReserved));
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		InitMiniDump(L"ntleai", hinstDll);
		settings.hHeap = HeapCreate(0, 0, 0);
		settings.nTlsIndex = TlsAlloc();
		settings.dwFontSizePercent = 100;
		settings.dwTimeZone = (DWORD)-480;
		settings.hInstance = hinstDll;
#pragma warning(disable: 4028)
		thread = CreateThread(NULL, 0, InitUnicodeLayer, NULL, 0, NULL);
#pragma warning(default: 4028)
		CloseHandle(thread);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		p = TlsGetValue(settings.nTlsIndex);
		if (p) FreeStringInternal(p);
		break;
	case DLL_PROCESS_DETACH:
	//	TlsFree(settings.nTlsIndex);
	//	HeapDestroy(settings.hHeap); // dll leave won't means the heap is no longer used by other modules ... 
		break;
	}
	return TRUE;
}

static void CreateGlobalAtom(void)
{
	while (!InterlockedCompareExchange(&settings.bInternalLockCreateAtom, 1/*ecx*/, 0/*eax*/)) {
		Sleep(0);
	}

	if (!settings.bNtleaAtomInvoked) {
		if (!GlobalFindAtomA(szNtleaWndAscData)) {
			GlobalAddAtomA(szNtleaWndAscData);
		}
		++settings.bNtleaAtomInvoked;
		if (!GlobalFindAtomA(szNtleaDlgAscData)) {
			GlobalAddAtomA(szNtleaDlgAscData);
		}
	}

	InterlockedDecrement(&settings.bInternalLockCreateAtom);
}

static int CheckWindowStyle(HWND hWnd, DWORD type/*ebx*/) {

	LONG_PTR n = GetWindowLongPtrW(hWnd, GWL_STYLE);
	// window no needs conversion ?? 
	if (n == 0) {
		return (0);
	}
	else if (n == /*0x84C820E4*/(WS_POPUP | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_SYSMENU |
		WS_EX_RTLREADING | WS_EX_TOOLWINDOW | WS_EX_MDICHILD | WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY)) {
		return (0);
	}
	else if (!(n & (WS_EX_ACCEPTFILES | WS_EX_TRANSPARENT))) {
		return (0);
	}
	else if (!type && (n & WS_EX_CLIENTEDGE)) {
		return (0);
	}
	else if (n & WS_EX_MDICHILD) {
		return (0);
	}
	// other case : 
	return (-1); // xor ebx, ebx !
}

INLINE NTLEA_WND_ASC_DATA* CheckProp(HWND hWnd) {
	NTLEA_WND_ASC_DATA* p = GetPropA(hWnd, szNtleaWndAscData);
	if (!p) 
		ShowUnhandledExceptionMessage("Unacceptable Empty Window Prop Detected, Force Exit.");
	return p;
}
/*
	Simple guide: 
	TopLevelWindowProc	-> Call Named-Controls' Unicode WindowProc
						-> Call its Default Ansi WindowProc
	The Original Author here use TopLevelxxx to replace the windowProc so that any window would be act as an ansi-window 
	while shown as unicode(such as monitor through spyxx), but there are still many window won't be caught by hook method,
	like MessageBoxA/SHBrowseForFolder/CreateDialogIndirectParam/..., these window may handled seperately by current maintainer.
	tags CT_CREATE_NULL/CT_CREATE_PRESET/CT_CREATE_DIALOG, with specialhandle everywhere, i know this is not very godd though!
	*/
typedef LRESULT(CALLBACK *CALLPROC)(WNDPROC, HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK TopLevelWindowProcEx(CALLPROC DefaultCallWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCSTR lpAnsiWindowName = NULL, lpAnsiClassName = NULL;
	CHAR CharBuffer[2]; // for unicode conversion -> local 
	CHAR ClassNameBuffer[MAX_PATH];
	int type = 0;

	NTLEA_WND_ASC_DATA* wndasc = CheckProp(hWnd);
	WNDPROC PrevWndProc = wndasc->PrevAnsiWindowProc;
	++GetTlsValueInternal()->InternalCall; // <---

//	char classname[256]; GetClassNameA(hWnd, classname, sizeof(classname));
//	if (lstrcmpiA(classname, "TRadioButton") == 0) {
//	ntprintfA(256, 1, "%s: proc-%p hwnd=%p, msg=%04x, wParam=%d, lParam=%d\n", __FUNCTION__, PrevWndProc, hWnd, uMsg, wParam, lParam);
//	}

	switch (uMsg) {
	case WM_CREATE: // L103
	case WM_NCCREATE: // general case !! 
	{
		if (lParam) {
			CREATEWNDEX * p = AllocateHeapInternal(sizeof(CREATEWNDEX));
			memcpy(p, (LPVOID)lParam, sizeof(CREATEWNDEX));
			if (p->lpWindowName) { // L103
				p->lpWindowName = (LPVOID)(lpAnsiWindowName = WideCharToMultiByteInternal(p->lpWindowName));
			}
			if ((DWORD_PTR)p->lpClassName & WM_CLASSMASK) { // L101
				p->lpClassName = (LPVOID)(lpAnsiClassName = WideCharToMultiByteInternal(p->lpClassName));
			}
			// LN999
			LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)p);
			// LN995
			if (lpAnsiWindowName) FreeStringInternal((LPVOID)lpAnsiWindowName);
			if (lpAnsiClassName) FreeStringInternal((LPVOID)lpAnsiClassName);
			if (p/*ebx*/) FreeStringInternal(p);
			return (hr);
		}
	}	break;
	case WM_MDICREATE: // LN106
	{
		if (lParam) {
			CREATEMDIWND* p = AllocateHeapInternal(sizeof(CREATEMDIWND));
			memcpy(p, (LPVOID)lParam, sizeof(CREATEMDIWND));
			if (p->szTitle) {
				p->szTitle = lpAnsiWindowName = WideCharToMultiByteInternal((LPCWSTR)p->szTitle);
			}
			if ((DWORD_PTR)p->szClass & WM_CLASSMASK) {
				p->szClass = lpAnsiClassName = WideCharToMultiByteInternal((LPCWSTR)p->szClass);
			}
			// LN999
			LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)p);
			// LN995
			if (lpAnsiWindowName) FreeStringInternal((LPVOID)lpAnsiWindowName);
			if (lpAnsiClassName) FreeStringInternal((LPVOID)lpAnsiClassName);
			if (p/*ebx*/) FreeStringInternal(p);
			return (hr);
		}
	}	break;
	case EM_GETLINE: // LN124
	{
		int siz = *(short*)(DWORD_PTR)lParam + 1;
		LPSTR lParamA = AllocateZeroedMemory(siz * sizeof(wchar_t));
		*(short*)(DWORD_PTR)lParam = (short)(siz - 1);
		int len = (int)CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA);
		if (!len || len + 1 != MultiByteToWideChar(settings.dwCodePage, 0, lParamA, len + 1, (LPWSTR)lParam, siz)) {
			*(LPWSTR)lParam = L'\0';
		}
		if (lParamA) FreeStringInternal((LPVOID)lParamA);
		return len;
	}//	break;
	case WM_GETFONT:
	{
		LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, lParam);
		return (hr) ? (hr) : (LRESULT)GetStockObject(SYSTEM_FONT);
	}//	break;
	case EM_REPLACESEL: // LN113
	case WM_SETTEXT: // LN113
	case WM_SETTINGCHANGE: // LN113
	case WM_DEVMODECHANGE: // LN113
	{
		LPCSTR lParamA = lParam ? WideCharToMultiByteInternal((LPCWSTR)lParam) : NULL;
		//	ntprintfA(256, 1, "1. W(%S) -> A(%s)", (LPCWSTR)lParam, lParamA);
		LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA); // lParamA null also requied ??? 
		// LN999 LN995
		if (lParamA) FreeStringInternal((LPVOID)lParamA);
		return hr;
	}//	break;
	case WM_GETTEXTLENGTH: // LN127
	{
		LRESULT len = CallWindowProcA(PrevWndProc, hWnd, WM_GETTEXTLENGTH, 0, 0);
		if (len > 0) {
			GetTlsValueInternal()->InternalCall++;
			LPSTR lParamA = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
			CallWindowProcA(PrevWndProc, hWnd, WM_GETTEXT, (len + 1) * sizeof(wchar_t), (LPARAM)lParamA);
			len = MultiByteToWideChar(settings.dwCodePage, 0, lParamA, -1, NULL, 0) - 1;
			// LN995
			if (lParamA/*ebx*/) FreeStringInternal((LPVOID)lParamA);
		}
		return len;
	}//	break;
	case WM_GETTEXT: // LN114
	case WM_UNKNOWN: // LN114
	{
		if (IsBadWritePtr((LPVOID)lParam, 1)) {
			--GetTlsValueInternal()->InternalCall;
			return (0);
		} else {
			LRESULT len = CallWindowProcA(PrevWndProc, hWnd, WM_GETTEXTLENGTH, 0, 0);
			if (len == 0) {
				*((LPWSTR)lParam) = L'\0';
			}
			else { // L116
				GetTlsValueInternal()->InternalCall++;
				LPSTR lParamA = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
				CallWindowProcA(PrevWndProc, hWnd, uMsg, (len + 1) * sizeof(wchar_t), (LPARAM)lParamA);
				if (uMsg == WM_UNKNOWN) wParam = len + 1;
				len = MultiByteToWideChar(settings.dwCodePage, 0, lParamA, -1, (LPWSTR)lParam, (int)wParam) - 1;
				if (len > 0) {
					// L997
					if (lParamA) FreeStringInternal((LPVOID)lParamA);
				}
				else {
					// L119
					if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
						*((LPWSTR)lParam + wParam - 1) = L'\0';
					}
					else {
						// L120
						*((LPWSTR)lParam) = L'\0';
					}
					// LN994
					FreeStringInternal((LPVOID)lParamA);
				}
			}
			return len;
		}
	}//	break;
	case WM_IME_CHAR: // LN109
	case WM_CHAR: // LN109
	{
		if ((wchar_t)wParam > 0x7F) {
			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)&wParam, 1, CharBuffer, 2, NULL, NULL);
			// here we exchange the order right ?
			*((char*)&wParam + 0) = CharBuffer[1];
			*((char*)&wParam + 1) = CharBuffer[0];
			//	wParam = (CharBuffer[1] << 8) | (CharBuffer[0] << 0);
		}
	}	break;
	case WM_NOTIFYFORMAT: // LN121
	{
		--GetTlsValueInternal()->InternalCall; // fix 
		GetClassNameA(hWnd, ClassNameBuffer, MAX_PATH);
		if (lstrcmpiA(ClassNameBuffer, "SysTreeView32") == 0) {
			DWORD_PTR n = GetWindowLongPtrW(hWnd, DWLP_MSGRESULT/*0*/);
			if (n && *(LPBYTE)(n + 0x10) == 1) { // what ??? 
				*(LPBYTE)(n + 0x10) &= -2; // 
			}
		}
		// L122
		return (lParam == NF_QUERY) ? NFR_ANSI : (0);
	}//	break;
	case WM_NCDESTROY: // check steps move out !!
	{
		LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, lParam);
		FreeStringInternal((LPVOID)GetPropA(hWnd, szNtleaWndAscData));
		RemovePropA(hWnd, szNtleaWndAscData);
		return hr;
	}//	break;
		// -------------------------------- 
	case LB_GETTEXTLEN: // LN131
	{
		int ret = CheckWindowStyle(hWnd, 1); // 1, inc ebx 
		if (ret != -1) {
			LRESULT len = CallWindowProcA(PrevWndProc, hWnd, LB_GETTEXTLEN, wParam, 0);
			if (len > 0) {
				GetTlsValueInternal()->InternalCall++;
				LPSTR lParamA = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
				CallWindowProcA(PrevWndProc, hWnd, LB_GETTEXT, wParam, (LPARAM)lParamA);
				len = MultiByteToWideChar(settings.dwCodePage, 0, lParamA, -1, NULL, 0) - 1;
				// LN995
				if (lParamA/*ebx*/) FreeStringInternal((LPVOID)lParamA);
			}
			return len;
		}
	}	break;
	case LB_GETTEXT: // LN110
	{
		type = 1;
	}//	break;
	case CB_GETLBTEXT: // LN129
	{
		int ret = CheckWindowStyle(hWnd, type); // 0
		if (ret != -1) {
			LPSTR lParamA = AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t));
			int len = (int)CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA);
			if (!len || !MultiByteToWideChar(settings.dwCodePage, 0, lParamA, len + 1, (LPWSTR)lParam, MAX_PATH)) {
				if (lParam) *(LPWSTR)lParam = L'\0';
			}
			if (lParamA/*ebx*/) FreeStringInternal((LPVOID)lParamA);
			return len;
		}
	}	break;
	case LB_FINDSTRINGEXACT: // LN104
	case LB_ADDSTRING: // LN104
	case LB_INSERTSTRING: // L104
	case LB_SELECTSTRING: // L104
	case LB_DIR: // L104
	case LB_FINDSTRING: // L104
	case LB_ADDFILE: // L104
		type = 1; // ebx = 1
		//	break;
	case CB_FINDSTRINGEXACT: // LN105
	case CB_ADDSTRING: // LN105
	case CB_INSERTSTRING: // LN105
	case CB_SELECTSTRING: // LN105
	case CB_DIR: // LN105
	case CB_FINDSTRING: // LN105
	{
		int ret = CheckWindowStyle(hWnd, type); // ebx = 0 / 1
		if (ret != -1) { // send op
			// LN113
			LPCSTR lParamA = lParam ? WideCharToMultiByteInternal((LPCWSTR)lParam) : NULL;
			// LN999
			LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA);
			// LN995
			if (lParamA/*ebx*/) FreeStringInternal((LPVOID)lParamA);
			return hr;
		}
		else { // recv op 
			// LN110
			ret = CheckWindowStyle(hWnd, type + 1); // ebx = 1 / 2
			if (ret != -1) {
				// <-- ebx 
				LPSTR lParamA = AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t));
				// LN999
				int len = (int)CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA);
				if (!len || !MultiByteToWideChar(settings.dwCodePage, 0, lParamA, len + 1, (LPWSTR)lParam, MAX_PATH)) { // should be -1 ?? 
					// L111
					if (lParam) *(LPWSTR)lParam = L'\0';
				}
				// LN995
				if (lParamA) FreeStringInternal((LPVOID)lParamA);
				return len;
			}
		}
	}	break;
	default: // LN100	
		break;
	}
	// --------- 
	return DefaultCallWindowProc(PrevWndProc, hWnd, uMsg, wParam, lParam);
}
static LRESULT CALLBACK TopLevelWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// for normal custom window case : other messages also needs handled by named-controls (children) 
	return TopLevelWindowProcEx(CallWindowProcA, hWnd, uMsg, wParam, lParam);
}
static LRESULT CALLBACK TopLevelSimpleProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// for predefined window case : other messages won't pass to named-controls (children)
	return TopLevelWindowProcEx(CallWindowProcJ, hWnd, uMsg, wParam, lParam);
}
static INT_PTR CALLBACK TopLevelDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NTLEA_TLS_DATA* p = GetTlsValueInternal();
	if (p->DialogProc) {
		// NOTE: because we could not got hwnd proc from GetWindowLong on dialog, here we just store DialogProc and then put 
		// it into hwnd prop, and then handle it the same way as a normal window!
		CreateGlobalAtom();
		if (!GetPropA(hWnd, szNtleaWndAscData)) {
			NTLEA_WND_ASC_DATA* wndasc = (NTLEA_WND_ASC_DATA*)AllocateZeroedMemory(sizeof(NTLEA_WND_ASC_DATA));
			wndasc->PrevAnsiWindowProc = (WNDPROC)p->DialogProc;
			SetPropA(hWnd, szNtleaWndAscData, (HANDLE)wndasc);
			// NOTE: message will be cross between parent window & new child window, 
			// such as WM_PARENTNOTIFY, p->DialogProc also indicate there will be new window pending !
			// so don't move this clear op outside of the block !
			p->DialogProc = NULL;
		}
	}
	return TopLevelWindowProc(hWnd, uMsg, wParam, lParam);
}

INLINE LRESULT CallWindowSendMessage(LPVOID lpProcAddress, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	DWORD_PTR Param1/*ecx*/, DWORD_PTR Param2/*ecx*/, DWORD_PTR Param3/*ecx*/, int FunctionType)
{
	switch (FunctionType) {
	case 0:
	default:
		return ((LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM))(DWORD_PTR)lpProcAddress)
			(hWnd, uMsg, wParam, lParam);
	case 1:
		return ((LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR, DWORD_PTR))(DWORD_PTR)lpProcAddress)
			(hWnd, uMsg, wParam, lParam, Param1, Param2);
	case 2:
		return ((LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR, DWORD_PTR, DWORD_PTR))(DWORD_PTR)lpProcAddress)
			(hWnd, uMsg, wParam, lParam, Param1, Param2, Param3);
	case 3:
		return ((LRESULT(WINAPI*)(DWORD_PTR, HWND, UINT, WPARAM, LPARAM))(DWORD_PTR)lpProcAddress)
			(Param1, hWnd, uMsg, wParam, lParam);
	}
}
static LRESULT SendUnicodeMessage(LPVOID lpProcAddress, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	DWORD_PTR Param1/*ecx*/, DWORD_PTR Param2/*ecx*/, DWORD_PTR Param3/*ecx*/, int FunctionType)
{
	LPCWSTR lpUnicodeWindowName = NULL, lpUnicodeClassName = NULL;
	WCHAR CharBuffer[2];
	int type = 0;

//	char classname[256]; GetClassNameA(hWnd, classname, sizeof(classname));
//	if (lstrcmpiA(classname, "TRadioButton") == 0) {
//	ntprintfA(256, 1, "%s: proc-%p hwnd=%p, msg=%04x, wParam=%d, lParam=%d\n", __FUNCTION__, lpProcAddress, hWnd, uMsg, wParam, lParam);
//	}

	switch (uMsg) {
	case WM_CREATE: // L304
	case WM_NCCREATE:
	{
		if (lParam) {
			CREATEWNDEX * p = AllocateHeapInternal(sizeof(CREATEWNDEX));
			memcpy(p, (LPVOID)lParam, sizeof(CREATEWNDEX));
			if (p->lpWindowName) {
				p->lpWindowName = (LPVOID)(lpUnicodeWindowName = MultiByteToWideCharInternal(p->lpWindowName));
			}
			if ((DWORD_PTR)p->lpClassName & WM_CLASSMASK) {
				p->lpClassName = (LPVOID)(lpUnicodeClassName = MultiByteToWideCharInternal(p->lpClassName));
			}
			// LN310
			LRESULT hr = CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)p, Param1, Param2, Param3, FunctionType);
			// LN793
			if (lpUnicodeWindowName) FreeStringInternal((LPVOID)lpUnicodeWindowName);
			if (lpUnicodeClassName) FreeStringInternal((LPVOID)lpUnicodeClassName);
			if (p/*ebx*/) FreeStringInternal(p);
			return hr;
		}
	}	break;
	case WM_MDICREATE: // LN307
	{
		if (lParam) {
			CREATEMDIWND* p = AllocateHeapInternal(sizeof(CREATEMDIWND));
			memcpy(p, (LPVOID)lParam, sizeof(CREATEMDIWND));
			if (p->szTitle) p->szTitle = (LPCSTR)(lpUnicodeClassName = MultiByteToWideCharInternal(p->szTitle));
			if ((DWORD_PTR)p->szClass & WM_CLASSMASK) {
				p->szClass = (LPCSTR)(lpUnicodeClassName = MultiByteToWideCharInternal(p->szClass));
			}
			// LN310
			LRESULT hr = CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)p, Param1, Param2, Param3, FunctionType);
			// LN793
			if (lpUnicodeWindowName) FreeStringInternal((LPVOID)lpUnicodeWindowName);
			if (lpUnicodeClassName) FreeStringInternal((LPVOID)lpUnicodeClassName);
			if (p/*ebx*/) FreeStringInternal(p);
			return (hr);
		}
	}	break;
	case WM_IME_CHAR: // LN309
	case WM_CHAR: // LN309
	{
		if ((wchar_t)wParam > 0x7F) { // is multibyte ... 
			// here we exchange the order : 
			//	char t = *((char*)&wParam + 0);
			//	*((char*)&wParam + 0) = *((char*)&wParam + 1);
			//	*((char*)&wParam + 1) = t;
			wParam = ((wParam & 0xFF) << 8) | ((wParam & 0xFF00) >> 8);
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)&wParam, -1, CharBuffer, 2);
			//	*((wchar_t*)&wParam) = CharBuffer[0];
			wParam = CharBuffer[0];
		}
	}	break;
	case EM_GETLINE: // LN321
	{
		if (!IsBadWritePtr((LPVOID)lParam, 1)) {
			int siz = *(short*)(DWORD_PTR)lParam + 1;
			LPWSTR lParamW = AllocateZeroedMemory(siz * sizeof(wchar_t));
			*(short*)(DWORD_PTR)lParam = (short)(siz - 1);
			int len = (int)CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
			if (!len || len + 1 != WideCharToMultiByte(CP_ACP, 0, lParamW, len + 1, (LPSTR)lParam, siz, NULL, NULL)) {
				*(LPSTR)lParam = '\0';
			}
			if (lParamW) FreeStringInternal(lParamW);
			return len;
		}
	}	break;
	case EM_REPLACESEL: // LN320
	case WM_SETTEXT: // LN320
	case WM_SETTINGCHANGE: // LN320
	case WM_DEVMODECHANGE: // LN320
	{
		LPCWSTR lParamW = lParam ? MultiByteToWideCharInternal((LPCSTR)lParam) : NULL;
		//	ntprintfA(1024, 1, "3. A(%s) -> W(%S)", (LPCSTR)lParam, lParamW);
		LRESULT hr = CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
		// LN301
		if (lParamW) FreeStringInternal((LPVOID)lParamW);
		return hr;
	}	break;
	case WM_GETTEXTLENGTH: // LN327
	{
		LRESULT len = CallWindowSendMessage(lpProcAddress, hWnd, WM_GETTEXTLENGTH, 0, 0, Param1, Param2, Param3, FunctionType);
		if (len > 0) {
			LPWSTR lParamW = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
			CallWindowSendMessage(lpProcAddress, hWnd, WM_GETTEXT, (len + 1) * sizeof(wchar_t), (LPARAM)lParamW,
				Param1, Param2, Param3, FunctionType);
			len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, NULL, 0, NULL, NULL) - 1; // required
			// LN793
			if (lParamW) FreeStringInternal(lParamW);
		}
		return len;
	}	break;
	case WM_GETTEXT: // LN310
	case WM_UNKNOWN: // LN310
	{
		if (IsBadWritePtr((LPVOID)lParam, 1)) {
			return (0);
		} else {
			// L311
			int len = (int)CallWindowSendMessage(lpProcAddress, hWnd, WM_GETTEXTLENGTH, 0, 0, Param1, Param2, Param3, FunctionType);
			// no needs check len == 0 ?? 
			LPWSTR lParamW = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
			len = (int)CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
			if (uMsg != WM_UNKNOWN) len = (int)wParam; // wParam should be len + 1 !! 
			len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, (LPSTR)lParam, len, NULL, NULL) - 1;
			if (len > 0) {
				// LN793
				if (lParamW) FreeStringInternal(lParamW);
			}
			else {
				// L316
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					*((LPSTR)lParam + wParam - 1) = '\0';
				}
				else {
					// L317
					*((LPSTR)lParam) = '\0';
				}
			}
			return len;
		}
	}	break;
	// ------------------------------------------- 
	case LB_GETTEXTLEN: // LN329
	{
		int ret = CheckWindowStyle(hWnd, 1);
		if (ret != -1) {
			LRESULT len = CallWindowSendMessage(lpProcAddress, hWnd, LB_GETTEXTLEN, wParam, 0, Param1, Param2, Param3, FunctionType);
			if (len > 0) {
				LPWSTR lParamW = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
				CallWindowSendMessage(lpProcAddress, hWnd, LB_GETTEXT, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
				len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, NULL, 0, NULL, NULL) - 1;
				// LN793
				if (lParamW/*ebx*/) FreeStringInternal(lParamW);
			}
			return len;
		}
	}	break;
	case LB_GETTEXT: // LN322
		type = 1;
		//	break;
	case CB_GETLBTEXT: // LN323
	{
		int ret = CheckWindowStyle(hWnd, type); // 0
		if (ret != -1) {
			LPWSTR lParamW = AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t));
			int len = (int)CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
			if (!len || !WideCharToMultiByte(CP_ACP, 0, lParamW, len + 1, (LPSTR)lParam, MAX_PATH, NULL, NULL)) {
				if (lParam) *(LPSTR)lParam = '\0';
			}
			if (lParamW/*ebx*/) FreeStringInternal(lParamW);
			return len;
		}
	}	break;
	case LB_FINDSTRINGEXACT: // LN305
	case LB_ADDSTRING: // LN305
	case LB_INSERTSTRING: // LN305
	case LB_FINDSTRING: // LN305
	case LB_ADDFILE: // LN305
	case LB_SELECTSTRING: // LN305
	case LB_DIR: // LN305
		type = 1;
		//	break;
	case CB_FINDSTRINGEXACT: // LN306
	case CB_ADDSTRING: // LN306
	case CB_INSERTSTRING: // LN306
	case CB_SELECTSTRING: // LN306
	case CB_DIR: // LN306
	case CB_FINDSTRING: // LN306
	{
		int ret = CheckWindowStyle(hWnd, type); // ebx = 0 / 1
		if (ret != -1) {
			LPCWSTR lParamW = lParam ? MultiByteToWideCharInternal((LPCSTR)lParam) : NULL;
			// LN899
			LRESULT hr = CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, (LPARAM)lParamW, Param1, Param2, Param3, FunctionType);
			if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
			return hr;
		}
	}	break;
	// ----------- common controls ---------------
	case TCM_GETITEMA:
	{
		if (!settings.bNoFilterCommCtrl && lParam) {
			LPTCITEM pitem = (LPTCITEM)lParam;
			if (pitem->mask & TCIF_TEXT) {
				LRESULT hr = CallWindowSendMessage(lpProcAddress, hWnd, uMsg + TCM_ADD_UNICODE, wParam, lParam,
					Param1, Param2, Param3, FunctionType);
				LPCSTR pnewText = WideCharToMultiByteInternal((LPCWSTR)pitem->pszText);
				lstrcpyA(pitem->pszText, pnewText); FreeStringInternal((LPVOID)pnewText);
				return hr;
			}
		}
	}	break;
	case TCM_SETITEMA:
	case TCM_INSERTITEMA:
	{
		if (!settings.bNoFilterCommCtrl && lParam) {
			LPTCITEM pitem = (LPTCITEM)lParam;
			if (pitem->mask & TCIF_TEXT) {
				LPVOID poldText = pitem->pszText;
				pitem->pszText = (LPVOID)MultiByteToWideCharInternal((LPCSTR)poldText);
				LRESULT hr = CallWindowSendMessage(lpProcAddress, hWnd, uMsg + TCM_ADD_UNICODE, wParam, lParam,
					Param1, Param2, Param3, FunctionType);
				FreeStringInternal(pitem->pszText); pitem->pszText = poldText;
				return hr;
			}
		}
	}	break;
	// ----------- common controls end ---------------
	default: // LN301
		break;
	}
	// --------- 
	return CallWindowSendMessage(lpProcAddress, hWnd, uMsg, wParam, lParam, Param1, Param2, Param3, FunctionType);
}

INLINE LRESULT CallProcAddress(LPVOID lpProcAddress, HWND hWnd, HWND hMDIClient,
	BOOL bMDIClientEnabled, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	typedef LRESULT(WINAPI*fnWNDProcAddress)(HWND, int, WPARAM, LPARAM);
	typedef LRESULT(WINAPI*fnMDIProcAddress)(HWND, HWND, int, WPARAM, LPARAM);
	// MDI or not ??? 
	return (bMDIClientEnabled) ? ((fnMDIProcAddress)(DWORD_PTR)lpProcAddress)(hWnd, hMDIClient, uMsg, wParam, lParam)
		: ((fnWNDProcAddress)(DWORD_PTR)lpProcAddress)(hWnd, uMsg, wParam, lParam);
}
static LRESULT CALLBACK DefConversionProc(LPVOID lpProcAddress, HWND hWnd, HWND hMDIClient,
	BOOL bMDIClientEnabled, INT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCWSTR lpUnicodeWindowName = NULL, lpUnicodeClassName = NULL;
	WCHAR CharBuffer[2];
	int type = 0;

//	char classname[256]; GetClassNameA(hWnd, classname, sizeof(classname));
//	if (lstrcmpiA(classname, "TRadioButton") == 0) {
//	ntprintfA(256, 1, "%s: proc-%p hwnd=%p, msg=%04x, wParam=%d, lParam=%d\n", __FUNCTION__, lpProcAddress, hWnd, uMsg, wParam, lParam);
//	}

	// some game will give us hWnd == NULL, oops, patch it now ?!
	if (hWnd && !IsWindowUnicode(hWnd)) ShowUnhandledExceptionMessage(szAppCallDefConvProc); // this function will exit automatic!!
	switch (uMsg) {
	case WM_CREATE: // L204
	case WM_NCCREATE: // 
	{
		if (lParam && GetTlsValueInternal()->CurrentCallType != CT_CREATE_PRESET) {
			CREATEWNDEX * p = AllocateHeapInternal(sizeof(CREATEWNDEX));
			memcpy(p, (LPVOID)lParam, sizeof(CREATEWNDEX));
			// special handler : 
			// for preset window such as SHBrowseForFolder/..., the window name won't be changed !!
			if (p->lpWindowName) {
				p->lpWindowName = (LPVOID)(lpUnicodeWindowName = MultiByteToWideCharInternal(p->lpWindowName));
			}
			if ((DWORD_PTR)p->lpClassName & WM_CLASSMASK) {
				p->lpClassName = (LPVOID)(lpUnicodeClassName = MultiByteToWideCharInternal(p->lpClassName));
			}
			// LN899
			LRESULT hr = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)p);
			// LN893 
			if (lpUnicodeWindowName) FreeStringInternal((LPVOID)lpUnicodeWindowName);
			if (lpUnicodeClassName) FreeStringInternal((LPVOID)lpUnicodeClassName);
			if (p/*ebx*/) FreeStringInternal((LPVOID)p);
			return (hr);
		}
	}	break;
	case WM_MDICREATE: // LN207
	{
		if (lParam) {
			CREATEMDIWND* p = AllocateHeapInternal(sizeof(CREATEMDIWND));
			memcpy(p, (LPVOID)lParam, sizeof(CREATEMDIWND));
			if (p->szTitle) p->szTitle = (LPCSTR)(lpUnicodeWindowName = MultiByteToWideCharInternal(p->szTitle));
			if ((DWORD_PTR)p->szClass & WM_CLASSMASK) {
				p->szClass = (LPCSTR)(lpUnicodeClassName = MultiByteToWideCharInternal(p->szClass));
			}
			// LN899
			LRESULT hr = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)p);
			// LN893 
			if (lpUnicodeWindowName) FreeStringInternal((LPVOID)lpUnicodeWindowName);
			if (lpUnicodeClassName) FreeStringInternal((LPVOID)lpUnicodeClassName);
			if (p/*ebx*/) FreeStringInternal((LPVOID)p);
			return (hr);
		}
	}	break;
	case WM_IME_CHAR: // LN209
	case WM_CHAR: // LN209
	{
		if ((wchar_t)wParam > 0x7F) {
			MultiByteToWideChar(settings.dwCodePage, 0, (LPCSTR)&wParam, -1, CharBuffer, 2);
			*((char*)&wParam + 0) = (char)CharBuffer[1];
			*((char*)&wParam + 1) = (char)CharBuffer[0];
		}
	}	break;
	case EM_REPLACESEL: // LN220
	case WM_SETTEXT: // LN220
	case WM_SETTINGCHANGE: // LN220
	case WM_DEVMODECHANGE: // LN220
	{
		LPCWSTR lParamW = lParam ? MultiByteToWideCharInternal((LPCSTR)lParam) : NULL;
		//	ntprintfA(256, 1, "2. A(%s) -> W(%S)", (LPCSTR)lParam, lParamW);
		LRESULT hr = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)lParamW);
		// LN893
		if (lParamW) FreeStringInternal((LPVOID)lParamW);
		return hr;
	}// break;
	case WM_GETTEXTLENGTH: // LN223
	{
		LRESULT len = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, WM_GETTEXTLENGTH, 0, 0);
		if (len > 0) {
			LPWSTR lParamW = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
			CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, WM_GETTEXT, (len + 1) * sizeof(wchar_t), (LPARAM)lParamW);
			len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, NULL, 0, NULL, NULL) - 1; // required
			// LN893
			if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
		}
		return len;
	}// break;
	case WM_GETTEXT: // LN210
	case WM_UNKNOWN: // LN210
	{
		if (IsBadWritePtr((LPVOID)lParam, 1)) {
			return (0);
		}
		else {
			// L211
			int len = (int)CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, WM_GETTEXTLENGTH, 0, 0);
			// no needs check len == 0 ?? 
			LPSTR lParamW = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
			CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)lParamW);
			if (uMsg != WM_UNKNOWN) len = (int)wParam; // wParam should be len + 1 !! 
			len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lParamW, -1, (LPSTR)lParam, len, NULL, NULL) - 1; // required
			if (len > 0) {
				// LN893
				if (lParamW) FreeStringInternal((LPVOID)lParamW);
			}
			else {
				// L216
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					*((LPSTR)lParam + wParam - 1) = '\0';
				}
				else {
					// L217
					*((LPSTR)lParam) = '\0';
				}
			}
			return len;
		}
	}//	break;
	case LB_GETTEXTLEN: // LN226
	{
		int ret = CheckWindowStyle(hWnd, 1);
		if (ret != -1) {
			int len = (int)CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, LB_GETTEXTLEN, wParam, 0);
			if (len > 0) {
				LPWSTR lParamW = AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
				CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, LB_GETTEXT, wParam, (LPARAM)lParamW);
				len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, NULL, 0, NULL, NULL) - 1;
				// LN893
				if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
			}
			return len;
		}
	}	break;
	case LB_GETTEXT: // LN221
		type = 1;
		//	break;
	case CB_GETLBTEXT: // LN222
	{
		int ret = CheckWindowStyle(hWnd, type); // 0
		if (ret != -1) {
			LPWSTR lParamW = AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t));
			int len = (int)CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)lParamW);
			if (!len || !WideCharToMultiByte(CP_ACP, 0, lParamW, len + 1, (LPSTR)lParam, MAX_PATH * sizeof(wchar_t), NULL, NULL)) {
				if (lParam) *(LPSTR)lParam = '\0';
			}
			// LN893
			if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
			return len;
		}
	}	break;
	case LB_FINDSTRINGEXACT: // LN205
	case LB_ADDSTRING: // LN205
	case LB_INSERTSTRING: // L205
	case LB_FINDSTRING: // L205
	case LB_ADDFILE: // L205
	case LB_SELECTSTRING: // L205
	case LB_DIR: // L205
		type = 1; // ebx = 1
		//	break;
	case CB_FINDSTRINGEXACT: // LN206
	case CB_ADDSTRING: // LN206
	case CB_INSERTSTRING: // LN206
	case CB_SELECTSTRING: // LN206
	case CB_DIR: // LN206
	case CB_FINDSTRING: // LN206
	{
		int ret = CheckWindowStyle(hWnd, type); // ebx = 0 / 1
		if (ret != -1) {
			LPCWSTR lParamW = lParam ? MultiByteToWideCharInternal((LPCSTR)lParam) : NULL;
			// LN899
			LRESULT hr = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)lParamW);
			if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
			return hr;
		}
	}	break;
	default: // LN201
		break;
	}
	// --------- 
	return CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, lParam);
}

// this function should not be inlined !
static BOOL CheckDynamicWndProc(int i, WNDPROC PrevWindowProc, HWND hWnd) {
	UNREFERENCED_PARAMETER(PrevWindowProc);
	wchar_t classname[32]; // try recognize it!
	return GetClassNameW(hWnd, classname, ARRAYSIZE(classname)) && 
		(lstrcmpiW(classname, SystemClassNameW[i]) == 0);
}

static void SetTopWindowProc(NTLEA_TLS_DATA* p, HWND hWnd, LONG_PTR TopLevelWndProc)
{
	NTLEA_WND_ASC_DATA* wndasc = (NTLEA_WND_ASC_DATA*)AllocateZeroedMemory(sizeof(NTLEA_WND_ASC_DATA));
	++p->InternalCall;
	WNDPROC wndproca = (WNDPROC)GetWindowLongPtrA(hWnd, GWLP_WNDPROC);
	if (wndproca) {
		wndasc->PrevAnsiWindowProc = wndproca;
		SetWindowLongPtrJ(hWnd, GWLP_WNDPROC, TopLevelWndProc);
		SetPropA(hWnd, szNtleaWndAscData, (HANDLE)wndasc); // save previous wndproc 
	}
	else { // no wndproc, no needs ??? 
		FreeStringInternal(wndasc);
	}
}
static void HookWindowProc(NTLEA_TLS_DATA* p, HWND hWnd)
{
	CreateGlobalAtom();
	// initialize custom window proc storage : 
	if (!GetPropA(hWnd, szNtleaWndAscData)) {
		SetTopWindowProc(p, hWnd, (LONG_PTR)TopLevelWindowProc);
	}
}
static LRESULT CbtHookProc(NTLEA_TLS_DATA* p, HHOOK hhook, int nCode, WPARAM wParam, LPARAM lParam) {
	// we filter msg we don't take care : 
	if (nCode != HCBT_CREATEWND || // or a unicode dialog won't cause hook !! 
		(p->CurrentCallType != CT_CREATE_WINDOW && IsWindowUnicode((HWND)wParam)))
	{
		return CallNextHookEx(hhook, nCode, wParam, lParam);
	}
	// for createwindow ansi window : 
	WCHAR ClassNameBuffer[MAX_PATH];
	HWND hwnd = (HWND)wParam;
	if (GetClassNameW(hwnd, ClassNameBuffer, sizeof(ClassNameBuffer)) > 0) {
		// not a speicified dialog, we have to hook the whole custom window msg proc!! 
		if (lstrcmpiW(L"NewDlgClass", ClassNameBuffer) != 0) {
			HookWindowProc(p, hwnd);
		}
		// maybe more ??? 
	}
	return CallNextHookEx(hhook, nCode, wParam, lParam);
}
static LRESULT CALLBACK CbtHookProcA(int code, WPARAM wParam, LPARAM lParam) {
	NTLEA_TLS_DATA* p = GetTlsValueInternal();
	return CbtHookProc(p, p->hWindowCbtHookAnsi, code, wParam, lParam);
}
static LRESULT CALLBACK CbtHookProcW(int code, WPARAM wParam, LPARAM lParam) {
	NTLEA_TLS_DATA* p = GetTlsValueInternal();
	return CbtHookProc(p, p->hWindowCbtHookUnicode, code, wParam, lParam);
}
INLINE void InstallCbtHook(NTLEA_TLS_DATA * ptls) { // ebx 
	// most hook-based Locale Emulator used to hook : computer-based training (CBT) message  
	// for more info, see : http://msdn.microsoft.com/en-us/library/windows/desktop/ms644977(v=vs.85).aspx
	if (ptls->hWindowHooking == 0) { // use tls-reference on window hooking, only unblock the most outside !
		ptls->hWindowCbtHookAnsi = SetWindowsHookExA(WH_CBT, CbtHookProcA, NULL, GetCurrentThreadId());
		ptls->hWindowCbtHookUnicode = SetWindowsHookExW(WH_CBT, CbtHookProcW, NULL, GetCurrentThreadId());
	}
	ptls->hWindowHooking++;
}
INLINE void UninstallCbtHook(NTLEA_TLS_DATA * ptls) { // ebx 
	ptls->hWindowHooking--; // guard 
	if (ptls->hWindowHooking == 0) {
		UnhookWindowsHookEx(ptls->hWindowCbtHookAnsi);
		UnhookWindowsHookEx(ptls->hWindowCbtHookUnicode);
	}
	else if (ptls->hWindowHooking < 0) {
		// reference error occured : 
		ShowUnhandledExceptionMessage(szAppCallRefUnmatch);
	}
}
//static void NtleaExceptionHandler(void) {}

#pragma auto_inline(off)
LRESULT WINAPI HookCallWindowProc(WNDPROC PrevWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// NOTE: Don't use stack variable for TLSValue!
	if (GetTlsValueInternal()->InternalCall)
		GetTlsValueInternal()->InternalCall--;
	for (int i = 0; i < MAXSYSCLASSDESC; ++i) { // search windowproc of controls 
		// mapping to unicode version (check cache ??)
		if (GetTlsValueInternal()->SystemClassDesc[i].AnsiSystemClassProc == PrevWindowProc
			|| CheckDynamicWndProc(i, PrevWindowProc, hWnd)) {
			return SendUnicodeMessage((LPVOID)(DWORD_PTR)CallWindowProcW, hWnd, uMsg, wParam, lParam,
				(DWORD_PTR)GetTlsValueInternal()->SystemClassDesc[i].UnicodeSystemClassProc, i, i, 3);
		}
	}
	return CallWindowProcJ(PrevWindowProc, hWnd, uMsg, wParam, lParam);
}
#pragma auto_inline(on)

LRESULT WINAPI HookSendMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return SendUnicodeMessage((LPVOID)(DWORD_PTR)SendMessageW, hWnd, uMsg, wParam, lParam, 0, 0, 0, 0);
}
LRESULT WINAPI HookPostMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return SendUnicodeMessage((LPVOID)(DWORD_PTR)PostMessageW, hWnd, uMsg, wParam, lParam, 0, 0, 0, 0);
}
LRESULT WINAPI HookSendNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return SendUnicodeMessage((LPVOID)(DWORD_PTR)SendNotifyMessageW, hWnd, uMsg, wParam, lParam, 0, 0, 0, 0);
}
LRESULT WINAPI HookSendMessageCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, SENDASYNCPROC lpCallBack, ULONG_PTR dwData) {
	return SendUnicodeMessage((LPVOID)(DWORD_PTR)SendMessageCallbackW, hWnd, uMsg, wParam, lParam, (DWORD_PTR)lpCallBack, dwData, 0, 1);
}
LRESULT WINAPI HookSendMessageTimeout(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, PDWORD_PTR lpdwResult) {
	return SendUnicodeMessage((LPVOID)(DWORD_PTR)SendMessageTimeoutW, hWnd, uMsg, wParam, lParam, fuFlags, uTimeout, (DWORD_PTR)lpdwResult, 2);
}

typedef INT_PTR(CALLBACK *fnDialogBoxIndirectParamProc)(HINSTANCE hInstance, LPCDLGTEMPLATE hDialogTemplate,
	HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
typedef INT_PTR(CALLBACK *fnDialogBoxParamProc)(HINSTANCE hInstance, LPCWSTR lpTemplateName,
	HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);

static INT_PTR DialogBoxIndirectParamProc(fnDialogBoxIndirectParamProc dialogboxindirectparamproc, HINSTANCE hInstance,
	LPCDLGTEMPLATE hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) {
	NTLEA_TLS_DATA * p = GetTlsValueInternal();
	InstallCbtHook(p);
	DLGPROC DialogProc = p->DialogProc; p->DialogProc = lpDialogFunc;
	DWORD CurrentCallType = p->CurrentCallType; p->CurrentCallType = CT_CREATE_DIALOG;
	// ---- 
	INT_PTR ret = dialogboxindirectparamproc(hInstance, hDialogTemplate, hWndParent, TopLevelDialogProc, dwInitParam);
	p->CurrentCallType = CurrentCallType;
	p->DialogProc = DialogProc;
	UninstallCbtHook(p);
	return ret;
}
static INT_PTR DialogBoxParamProc(fnDialogBoxParamProc dialogboxparamproc, HINSTANCE hInstance, LPCSTR lpTemplateName,
	HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit) {
	NTLEA_TLS_DATA * p = GetTlsValueInternal();
	InstallCbtHook(p);
	DLGPROC PrevDialogFunc = p->DialogProc; p->DialogProc = lpDialogFunc;
	DWORD PrevCallType = p->CurrentCallType; p->CurrentCallType = CT_CREATE_DIALOG;
	LPCWSTR lpTemplateW = ((DWORD_PTR)lpTemplateName & WM_CLASSMASK ? MultiByteToWideCharInternal(lpTemplateName) : NULL);
	// Y105
	INT_PTR ret = dialogboxparamproc(hInstance, (lpTemplateW ? lpTemplateW : (LPCWSTR)lpTemplateName),
		hWndParent, TopLevelDialogProc, lParamInit);
	p->CurrentCallType = PrevCallType;
	p->DialogProc = PrevDialogFunc;
	UninstallCbtHook(p); // order change OK ?? 
	if (lpTemplateW) FreeStringInternal((LPVOID)lpTemplateW);
	return ret;
}
INT_PTR WINAPI HookDialogBoxIndirectParam(HINSTANCE hInstance, LPCDLGTEMPLATE hDialogTemplate,
	HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	return DialogBoxIndirectParamProc((fnDialogBoxIndirectParamProc)DialogBoxIndirectParamW,
		hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);
}
HWND WINAPI HookCreateDialogIndirectParam(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate,
	HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit)
{
	return (HWND)DialogBoxIndirectParamProc((fnDialogBoxIndirectParamProc)CreateDialogIndirectParamW,
		hInstance, lpTemplate, hWndParent, lpDialogFunc, lParamInit);
}
INT_PTR WINAPI HookDialogBoxParam(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent,
	DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	return DialogBoxParamProc((fnDialogBoxParamProc)DialogBoxParamW,
		hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}
HWND WINAPI HookCreateDialogParam(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent,
	DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	return (HWND)DialogBoxParamProc((fnDialogBoxParamProc)CreateDialogParamW,
		hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderW(LPBROWSEINFO lpbi);
PIDLIST_ABSOLUTE WINAPI HookSHBrowseForFolder(LPBROWSEINFO lpbi)
{
	NTLEA_TLS_DATA* p = GetTlsValueInternal();
	PIDLIST_ABSOLUTE pidlist; // the documents said : The size of 'pszDisplayName' is assumed to be MAX_PATH characters.
	BROWSEINFO bi; memcpy(&bi, lpbi, sizeof(bi)); bi.pszDisplayName = AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t)); 
	if (lpbi->lpszTitle) { bi.lpszTitle = MultiByteToWideCharInternal(lpbi->lpszTitle); }
	if (lpbi->pszDisplayName) { MultiByteToWideChar(CP_ACP, 0, lpbi->pszDisplayName, -1, (LPWSTR)bi.pszDisplayName, MAX_PATH); }
	DWORD PrevCallType = p->CurrentCallType; p->CurrentCallType = CT_CREATE_PRESET;
//	DWORD CodePage = settings.dwCodePage; settings.dwCodePage = CP_ACP;
	pidlist = SHBrowseForFolderW(&bi); 
//	settings.dwCodePage = CodePage;
	p->CurrentCallType = PrevCallType;
	if (lpbi->lpszTitle) { FreeStringInternal((LPVOID)bi.lpszTitle); }
	if (lpbi->pszDisplayName) { WideCharToMultiByte(CP_ACP, 0, bi.pszDisplayName, -1, (LPSTR)lpbi->pszDisplayName, MAX_PATH, NULL, NULL); }
	if (bi.pszDisplayName) FreeStringInternal((LPVOID)bi.pszDisplayName); // cleanup definitly ... 
	return pidlist;
}

BOOL WINAPI SHGetPathFromIDListW(PIDLIST_ABSOLUTE pidl, LPWSTR pszPath);
BOOL WINAPI HookSHGetPathFromIDList(PIDLIST_ABSOLUTE pidl, LPSTR pszPath)
{
	LPWSTR pszPathW = (LPWSTR)AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t));
	BOOL ret = SHGetPathFromIDListW(pidl, pszPathW);
	if (ret) {
		WideCharToMultiByte(CP_ACP, 0, pszPathW, -1, pszPath, MAX_PATH, NULL, NULL);
	}
	FreeStringInternal((LPVOID)pszPathW);
	return ret;
}

INLINE int WideCharToMultiBytePartial(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar) 
{
	int len = WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, NULL, 0, lpDefaultChar, lpUsedDefaultChar);
	assert(len >= cbMultiByte);
	LPSTR tempbuffer = (LPSTR)AllocateHeapInternal(len * sizeof(char));
	WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, tempbuffer, len, lpDefaultChar, lpUsedDefaultChar);
	memcpy(lpMultiByteStr, tempbuffer, cbMultiByte * sizeof(char)); FreeStringInternal(tempbuffer);
	return cbMultiByte;
}
INLINE int MultiByteToWideCharPartial(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	int len = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, NULL, 0);
	assert(len >= cchWideChar);
	LPWSTR tempbuffer = (LPWSTR)AllocateHeapInternal(len * sizeof(wchar_t));
	MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, tempbuffer, len);
	memcpy(lpWideCharStr, tempbuffer, cchWideChar * sizeof(wchar_t)); FreeStringInternal(tempbuffer);
	return cchWideChar;
}

#pragma auto_inline(off)
NTSTATUS WINAPI HookUnicodeToMultiByte(LPSTR AnsiBuffer, DWORD MultiByteLength,
	LPDWORD lpNumberOfBytesConverted, LPCWSTR UnicodeBuffer, DWORD WideCharLength/*bytes*/)
{
	while (!InterlockedCompareExchange(&settings.bInternalLockWCtoMB, 1, 0)
		&& GetCurrentThreadId() != settings.dwThreadIdWCtoMB) Sleep(0); // spin wait
	// hold the locker : 
	if (settings.bInternalCallWCtoMB) {
		return UnicodeToMultiByteJ(AnsiBuffer, MultiByteLength, lpNumberOfBytesConverted, UnicodeBuffer, WideCharLength);
	} else {
		// G0 : 
		settings.dwThreadIdWCtoMB = GetCurrentThreadId();
		// internal call : 
	//	assert(settings.bInternalCallWCtoMB == 0);
		++settings.bInternalCallWCtoMB;
		int ret = WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, WideCharLength >> 1, AnsiBuffer, MultiByteLength, NULL, NULL);
		if (!ret) { // widechar to multibyte won't support partial convert, so ... 
			DWORD err = GetLastError(); SetLastError(0); // reset state !! 
			if (err == ERROR_INSUFFICIENT_BUFFER) {
				ret = WideCharToMultiBytePartial(CP_ACP, 0, UnicodeBuffer, WideCharLength >> 1, AnsiBuffer, MultiByteLength, NULL, NULL);
			}
		}
		--settings.bInternalCallWCtoMB;
	//	assert(settings.bInternalCallWCtoMB == 0);

		InterlockedDecrement(&settings.bInternalLockWCtoMB);
		if (lpNumberOfBytesConverted) *lpNumberOfBytesConverted = ret;
		return (0);
	}
}
NTSTATUS WINAPI HookMultiByteToUnicode(
	LPWSTR UnicodeBuffer, DWORD WideCharLength, DWORD* lpNumberOfBytesConverted, LPCSTR AnsiBuffer, DWORD MultiByteLength)
{
	// [var] = 0
	// if (eax==[var]) { zf=1, [var]=ecx } else { zf=0 }
	while (!InterlockedCompareExchange(&settings.bInternalLockMBtoWC, 1/*ecx*/, 0/*eax*/)
		&& GetCurrentThreadId() != settings.dwThreadIdMBtoWC) Sleep(0); // spin wait
	// hold the locker : 
	if (settings.bInternalCallMBtoWC) {
		return MultiByteToUnicodeJ(UnicodeBuffer, WideCharLength, lpNumberOfBytesConverted, AnsiBuffer, MultiByteLength);
	} else {
		settings.dwThreadIdMBtoWC = GetCurrentThreadId(); // 
		// internal call : 
	//	assert(settings.bInternalCallMBtoWC == 0);
		++settings.bInternalCallMBtoWC;
		int ret = MultiByteToWideChar(CP_ACP, 0, AnsiBuffer, MultiByteLength, UnicodeBuffer, WideCharLength >> 1);
		if (!ret) { // multibyte to widechar won't support partial convert, so ... 
			DWORD err = GetLastError(); SetLastError(0); // reset state !! 
			if (err == ERROR_INSUFFICIENT_BUFFER) {
				ret = MultiByteToWideCharPartial(CP_ACP, 0, AnsiBuffer, MultiByteLength, UnicodeBuffer, WideCharLength >> 1);
			}
		}
		--settings.bInternalCallMBtoWC;
	//	assert(settings.bInternalCallMBtoWC == 0);

		InterlockedDecrement(&settings.bInternalLockMBtoWC);
		if (lpNumberOfBytesConverted) *lpNumberOfBytesConverted = ret << 1; // wchar_t to bytes
		return (0);
	}
}
NTSTATUS WINAPI HookUnicodeToMultiByteSize(int* lpNumberOfBytesConverted, LPCWSTR UnicodeBuffer, int WideCharLength/*bytes*/) {
	int ret = WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, WideCharLength >> 1, NULL, 0, NULL, NULL);
	if (lpNumberOfBytesConverted) *lpNumberOfBytesConverted = ret;
	return (0);
}
NTSTATUS WINAPI HookMultiByteToUnicodeSize(int* lpNumberOfBytesConverted, LPCSTR AnsiBuffer, int MultiByteLength)
{
	int ret = MultiByteToWideChar(CP_ACP, 0, AnsiBuffer, MultiByteLength, NULL, 0) << 1; // wchar_t to bytes
	if (lpNumberOfBytesConverted) *lpNumberOfBytesConverted = ret;
	return (0);
}
#pragma auto_inline(on)

// ---------------------- 

BOOL WINAPI HookIsDBCSLeadByte(BYTE TestChar) {
	return IsDBCSLeadByteEx(settings.dwCodePage, TestChar);
}
LPSTR WINAPI HookCharPrev(LPCSTR lpStart, LPCSTR lpCurrentChar) {
	//	UNREFERENCED_PARAMETER(dwFlags);
	return CharPrevExA((WORD)settings.dwCodePage, lpStart, lpCurrentChar, 0);
}
LPSTR WINAPI HookCharNext(LPCSTR lpCurrentChar) {
	//	UNREFERENCED_PARAMETER(dwFlags);
	return CharNextExA((WORD)settings.dwCodePage, lpCurrentChar, 0);
}

INLINE BYTE ToCharSet(int lfcharset) {
	return lfcharset > 0 ? (BYTE)lfcharset : DEFAULT_CHARSET;
}

#pragma auto_inline(off)
static HFONT GetFontFromFont(BYTE DefaultCharset, LPCSTR DefaultFontFace, HFONT Font) {
	LOGFONTW LogFont;
	if (GetObjectW(Font, sizeof(LogFont), &LogFont) == 0) return NULL;
	LogFont.lfCharSet = DefaultCharset;
	MultiByteToWideChar(CP_ACP, 0, DefaultFontFace, -1, LogFont.lfFaceName, LF_FACESIZE);
	return CreateFontIndirectW(&LogFont);
}
#pragma auto_inline(on)
HGDIOBJ WINAPI HookGetStockObject(int fnObject) {
	// this code is considered from LE : 
	// https://github.com/Arianrhod/Arianrhod/blob/master/Source/LocaleEmulator/LocaleEmulator/Gdi32Hook.cpp
	static const int StockObjectIndex[] = {
		OEM_FIXED_FONT, ANSI_FIXED_FONT, ANSI_VAR_FONT, 
		SYSTEM_FONT, DEVICE_DEFAULT_FONT, SYSTEM_FIXED_FONT, DEFAULT_GUI_FONT,
	};
	static HFONT fontstock[32] = { 0 };

	// only those fonts may changed ?? 
	for (int i = 0; i < ARRAYSIZE(StockObjectIndex); ++i) {
		if (StockObjectIndex[i] == fnObject) {
			if (!fontstock[fnObject]) 
				fontstock[fnObject] = GetFontFromFont(ToCharSet(settings.lfcharset), (LPCSTR)settings.lpFontFaceName, GetStockObjectJ(fnObject));
			return fontstock[fnObject];
		}
	}
	return GetStockObjectJ(fnObject);
}
HFONT WINAPI HookCreateFontIndirect(const LOGFONT *lplf) {
	LOGFONTW logfont = { sizeof(LOGFONTSIMILAR), };
	memcpy(&logfont, lplf, sizeof(LOGFONTSIMILAR));
#if 0
	if (settings.bForceSpecifyFont) {
		logfont.lfWidth  *= settings.dwFontSizePercent / 100;
		logfont.lfHeight *= settings.dwFontSizePercent / 100;
	}
#endif
	// any input CP_ACP or any value would be settings.dwCodePage finally ... 
	MultiByteToWideChar(settings.dwCodePage/*???*/, 0, (
#if 0
		settings.bForceSpecifyFont ? (LPCSTR)settings.lpFontFaceName : 
#endif
		(LPCSTR)lplf->lfFaceName), -1, logfont.lfFaceName, LF_FACESIZE);
	return CreateFontIndirectW(&logfont);
}
int WINAPI HookGetMenuString(HMENU hMenu, UINT uIDItem, LPSTR lpString, int nMaxCount, UINT uFlag)
{
	WCHAR StringBuffer[MAX_PATH];
	GetMenuStringW(hMenu, uIDItem, StringBuffer, MAX_PATH, uFlag);
	int ret = WideCharToMultiByte(CP_ACP, 0, StringBuffer, -1, lpString, nMaxCount, NULL, NULL);
	if (ret == 0) {
		lpString[nMaxCount - 1] = '\0';
		return nMaxCount - 1;
	}
	else {
		return ret - 1;
	}
}
BOOL WINAPI HookGetMenuItemInfo(HMENU hMenu, UINT uItem, BOOL fByPosition, LPMENUITEMINFOA lpmii)
{
	DWORD cchtmp = 0; // also used as ebx != NULL 
	MENUITEMINFOW miitmp;
	// duplicate for unicode usage : 
	memcpy(&miitmp, lpmii, lpmii->cbSize);
	if (((miitmp.fMask & MIIM_TYPE) && miitmp.fType != 0) || (miitmp.fMask & MIIM_STRING) || miitmp.cch > 0/*I think it default*/) {
		// L100
		cchtmp = miitmp.cch; // i.e. lpmii->cch
		miitmp.dwTypeData = /*ebx = */ (LPWSTR)AllocateZeroedMemory((cchtmp + 1) * sizeof(wchar_t));
	}
	//	if ((miitmp.fMask & MIIM_TYPE) || (miitmp.fMask & MIIM_STRING) || (miitmp.fMask & MIIM_FTYPE)) {
	// L105
	BOOL ret = GetMenuItemInfoW(hMenu, uItem, fByPosition, &miitmp);
	if (ret) memcpy(lpmii, &miitmp, miitmp.cbSize);
	if (cchtmp > 0) {
		// do conversion back : 
		int cch = WideCharToMultiByte(CP_ACP, 0, miitmp.dwTypeData, -1, lpmii->dwTypeData, lpmii->cch, NULL, NULL);
		if (cch > 0) lpmii->cch = cch - 1; // update without null-terminal char
		// L105 cleanup : 
		if (miitmp.dwTypeData) FreeStringInternal(miitmp.dwTypeData);
		// safe protection mainly for invalid op: 
		lpmii->dwTypeData[cchtmp - 1] = '\0';
	}
	//	}
	// L104
	return ret;
}
BOOL WINAPI HookSetMenuItemInfo(HMENU hMenu, UINT uItem, BOOL fByPosition, LPMENUITEMINFOA lpmii) {
	LPCSTR dwTypeDataA = NULL;
	if (((lpmii->fMask & MIIM_TYPE) && lpmii->fType != 0) || (lpmii->fMask & MIIM_STRING) || lpmii->cch > 0/*I think it default*/) {
		// L100
		dwTypeDataA = lpmii->dwTypeData; // keep the original one
		lpmii->dwTypeData = /* fake ebx = */ (LPSTR)MultiByteToWideCharInternal(lpmii->dwTypeData);
	}
	//	if ((lpmii->fMask & MIIM_TYPE) || (lpmii->fMask & MIIM_STRING) || (lpmii->fMask & MIIM_FTYPE)) {
	BOOL ret = SetMenuItemInfoW(hMenu, uItem, fByPosition, (LPMENUITEMINFOW)lpmii);
	if (dwTypeDataA) { // restore the fake one : 
		FreeStringInternal((LPVOID)lpmii->dwTypeData);
		lpmii->dwTypeData = (LPSTR)dwTypeDataA;
	}
	//	}
	// L103
	return ret;
}

#pragma auto_inline(off)
static int CALLBACK EnumFontFamExProc(const LOGFONTA *lpelfe, const TEXTMETRICA *lpntme, DWORD FontType, LPARAM lParam)
{
	UNREFERENCED_PARAMETER((lpelfe, lpntme, FontType, lParam));
	NTLEA_TLS_DATA* p = GetTlsValueInternal();
	++p->IsFontAvailable;
	return (FALSE);
}
static int CALLBACK EnumFontFamExProcConvA(const LOGFONTA *lpelfe, const TEXTMETRICA *lpntme, DWORD FontType, LPARAM lParam)
{
	FONTENUMPROCPARAM* para = (FONTENUMPROCPARAM*)lParam;
	// ------------- 
	if ( FontType & TRUETYPE_FONTTYPE ) 
	{	// this filter let only local charset convert without any other option !
		LOGFONTW logfont; memcpy(&logfont, lpelfe, sizeof(LOGFONTSIMILAR));
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpelfe->lfFaceName, -1, logfont.lfFaceName, LF_FACESIZE);
		// a lil speed up, but this could be removed for code simplifier ... 
		if (lstrcmpA((LPCSTR)para->fontenumface, lpelfe->lfFaceName) == 0) { // cmp the cache key
			lstrcpyA((LPSTR)lpelfe->lfFaceName, (LPCSTR)para->fontenumname); // use the cache value
		} else if (lstrcmpA((LPCSTR)para->fontenumface, lpelfe->lfFaceName + 1) == 0) { // cmp the cache @key
			lstrcpyA((LPSTR)lpelfe->lfFaceName + 1, (LPCSTR)para->fontenumname); // use the cache @value
		} else {
			if ((lpelfe->lfCharSet == (BYTE)settings.lfcharset && ConvertFontFacename(para->fontenumhdc, &logfont, TRUE)) ||
				(lpelfe->lfCharSet == (BYTE)settings.lfcharold && ConvertFontFacename(para->fontenumhdc, &logfont, FALSE))
				) { // do conversion check 
				lstrcpyA((LPSTR)para->fontenumface, lpelfe->lfFaceName); // cache the latest key
				WideCharToMultiByte(CP_ACP, 0, logfont.lfFaceName, -1, (LPSTR)para->fontenumname, LF_FACESIZE, 0, 0); // cache the latest value
				lstrcpyA((LPSTR)lpelfe->lfFaceName, (LPCSTR)para->fontenumname); // use the cache value
			}
		}
	}
	// ------------- 
//	TEXTMETRICA metrics = *lpntme;
	return ((FONTENUMPROCA)(DWORD_PTR)(para->fontenumproc))(lpelfe, lpntme, FontType, para->fontenumpara);
}
static int CALLBACK EnumFontFamExProcConvW(const LOGFONTW *lpelfe, const TEXTMETRICW *lpntme, DWORD FontType, LPARAM lParam)
{
	FONTENUMPROCPARAM* para = (FONTENUMPROCPARAM*)lParam;
	// ------------- 
	if ( FontType & TRUETYPE_FONTTYPE )
	{	// the same as above 
		LOGFONTW logfont; memcpy(&logfont, lpelfe, sizeof(LOGFONTSIMILAR));
		lstrcpyW(logfont.lfFaceName, lpelfe->lfFaceName);
		// a lil speed up, but this could be removed for code simplifier ... 
		if (lstrcmpW((LPCWSTR)para->fontenumface, lpelfe->lfFaceName) == 0) { // cmp the cache key
			lstrcpyW((LPWSTR)lpelfe->lfFaceName, (LPCWSTR)para->fontenumname); // use the cache value
		} else if (lstrcmpW((LPCWSTR)para->fontenumface, lpelfe->lfFaceName+1) == 0) { // cmp the cache @key
			lstrcpyW((LPWSTR)lpelfe->lfFaceName+1, (LPCWSTR)para->fontenumname); // use the cache @value
		} else {
			if ((lpelfe->lfCharSet == (BYTE)settings.lfcharset && ConvertFontFacename(para->fontenumhdc, &logfont, TRUE)) || 
				(lpelfe->lfCharSet == (BYTE)settings.lfcharold && ConvertFontFacename(para->fontenumhdc, &logfont, FALSE))
				) { // do conversion check 
				lstrcpyW((LPWSTR)para->fontenumface, lpelfe->lfFaceName); // cache the latest key
				lstrcpyW((LPWSTR)para->fontenumname, logfont.lfFaceName); // cache the latest value
				lstrcpyW((LPWSTR)lpelfe->lfFaceName, (LPCWSTR)para->fontenumname); // use the cache value
			}
		}
	}
	// ------------- 
//	TEXTMETRICW metrics = *lpntme;
	return ((FONTENUMPROCW)(DWORD_PTR)(para->fontenumproc))(lpelfe, lpntme, FontType, para->fontenumpara);
}
int WINAPI HookEnumFontFamiliesExA(HDC hdc, LPLOGFONTA lpLogfont, FONTENUMPROCA lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags) 
{
	NTLEA_TLS_DATA* p = GetTlsValueInternal();
	p->IsFontAvailable = 0;
	// ------------------------ //
	EnumFontFamiliesExInternalA(hdc, lpLogfont, EnumFontFamExProc, 0/*lParam*/, 0/*dwFlags*/);
	if (!p->IsFontAvailable) {
		LOGFONTW logfont;
		// save the data : 
		memcpy(&logfont, lpLogfont, sizeof(LOGFONTW));
		// save the ansi one ?? 
		if (settings.lpFontFaceName[0]) {
			// L102 : 
			lstrcpyA((LPSTR)logfont.lfFaceName, (LPCSTR)settings.lpFontFaceName);
		}
	}
	// -------------------- //
	int ret; FONTENUMPROCPARAM param;
	param.fontenumproc = (LPVOID)(DWORD_PTR)lpEnumFontFamExProc;
	param.fontenumpara = lParam;
	param.fontenumhdc = GetDC(NULL);
	param.fontenumface[0] = param.fontenumface[1] = 0; // for unicode 
	ret = EnumFontFamiliesExInternalA(hdc, lpLogfont, EnumFontFamExProcConvA, (LPARAM)&param, dwFlags);
	ReleaseDC(NULL, param.fontenumhdc);
	return ret;
}
int WINAPI HookEnumFontFamiliesExW(HDC hdc, LPLOGFONTW lpLogfont, FONTENUMPROCW lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags) 
{
	// -------------
	int ret; FONTENUMPROCPARAM param;
	param.fontenumproc = (LPVOID)(DWORD_PTR)lpEnumFontFamExProc;
	param.fontenumpara = lParam;
	param.fontenumhdc = GetDC(NULL);
	param.fontenumface[0] = param.fontenumface[1] = 0; // for unicode 
	ret = EnumFontFamiliesExInternalW(hdc, lpLogfont, EnumFontFamExProcConvW, (LPARAM)&param, dwFlags);
	ReleaseDC(NULL, param.fontenumhdc);
	return ret;
}
int WINAPI HookEnumFontFamiliesA(HDC hdc, LPCSTR lpFaceName, FONTENUMPROCA lpFontFunc, LPARAM lParam)
{
	LOGFONTA logfont = { 0 }; logfont.lfCharSet = DEFAULT_CHARSET;
	if (lpFaceName) lstrcpyA(logfont.lfFaceName, lpFaceName);
	return HookEnumFontFamiliesExA(hdc, &logfont, lpFontFunc, lParam, 0);
}
int WINAPI HookEnumFontFamiliesW(HDC hdc, LPCWSTR lpFaceName, FONTENUMPROCW lpFontFunc, LPARAM lParam)
{
	LOGFONTW logfont = { 0 }; logfont.lfCharSet = DEFAULT_CHARSET;
	if (lpFaceName) lstrcpyW(logfont.lfFaceName, lpFaceName);
	return HookEnumFontFamiliesExW(hdc, &logfont, lpFontFunc, lParam, 0);
}
int WINAPI HookEnumFontsA(HDC hdc, LPCSTR lpszFamily, FONTENUMPROCA lpEnumFontFamProc, LPARAM lParam)
{
#if 1
	int ret; FONTENUMPROCPARAM param;
	param.fontenumproc = (LPVOID)(DWORD_PTR)lpEnumFontFamProc;
	param.fontenumpara = lParam;
	param.fontenumhdc = GetDC(NULL);
	param.fontenumface[0] = param.fontenumface[1] = 0;
	ret = HookEnumFontFamiliesA(hdc, lpszFamily, EnumFontFamExProcConvA, (LPARAM)&param);
	ReleaseDC(NULL, param.fontenumhdc);
	return ret;
#else // still i don't know the difference between EnumFonts with EnumFontFamilies
	return HookEnumFontFamiliesA(hdc, lpszFamily, lpEnumFontFamProc, lParam);
#endif
}
int WINAPI HookEnumFontsW(HDC hdc, LPCWSTR lpszFamily, FONTENUMPROCW lpEnumFontFamProc, LPARAM lParam)
{
#if 1
	int ret; FONTENUMPROCPARAM param;
	param.fontenumproc = (LPVOID)(DWORD_PTR)lpEnumFontFamProc;
	param.fontenumpara = lParam;
	param.fontenumhdc = GetDC(NULL);
	param.fontenumface[0] = param.fontenumface[1] = 0;
	ret = HookEnumFontFamiliesW(hdc, lpszFamily, EnumFontFamExProcConvW, (LPARAM)&param);
	ReleaseDC(NULL, param.fontenumhdc);
	return ret;
#else // the same as above, actually, maybe the FontFace is not FontFamily, I think.
	return HookEnumFontFamiliesWA(hdc, lpszFamily, lpEnumFontFamProc, lParam);
#endif
}

HWND WINAPI HookCreateWindowEx(
	DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	NTLEA_TLS_DATA* p = GetTlsValueInternal();
	DWORD PrevCallType = p->CurrentCallType; p->CurrentCallType = CT_CREATE_WINDOW;
	// 1. prepare the thread hook for the next step windowproc hook, then createwindow and hook it!
	LPCWSTR lpWindowNameW = (lpWindowName) ? MultiByteToWideCharInternal(lpWindowName) : NULL;
	LPCWSTR lpClassNameW = ((DWORD_PTR)lpClassName & WM_CLASSMASK) ? MultiByteToWideCharInternal(lpClassName) : NULL;
	InstallCbtHook(p);
	// 2. createwindow unicode, each window msghandler is unrelated : http://hi.baidu.com/tiancao222/item/d2f0dc370617dff3e6bb7a61
	HWND hwnd = CreateWindowExW(dwExStyle, (lpClassNameW?lpClassNameW:(LPCWSTR)lpClassName), lpWindowNameW, dwStyle, 
		x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
//	ntprintfA(128, 1, "%p, %p - %p, %p\n", lpClassName, lpWindowName, lpClassNameW, lpWindowNameW);
	// 3. unprepare the thread hook
	UninstallCbtHook(p);
	if (lpWindowNameW) FreeStringInternal((LPVOID)lpWindowNameW);
	if (lpClassNameW) FreeStringInternal((LPVOID)lpClassNameW);
	// 4. cleanup resource 
	p->CurrentCallType = PrevCallType;
	return hwnd;
}

int WINAPI HookGetWindowText(HWND hWindow, LPSTR lpString, int nMaxCount)
{
	int len = (int)SendMessageW(hWindow, WM_GETTEXTLENGTH, 0, 0) + 1;
	LPWSTR lpStringW = (LPWSTR)AllocateZeroedMemory(len * sizeof(wchar_t));

	int ret = GetWindowTextW(hWindow, lpStringW, len);
	if (ret > 0) {
		int size = WideCharToMultiByte(CP_ACP, 0, lpStringW, -1, lpString, nMaxCount, NULL, NULL);
		if (size > 0) ret = size - 1;
		else {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				lpString[nMaxCount - 1] = '\0'; ret = nMaxCount - 1;
			}
			else {
				lpString[0] = '\0'; ret = 0;
			}
		}
	}
	else {
		lpString[0] = '\0'; ret = 0;
	}
	FreeStringInternal(lpStringW);
	return ret;
}

BOOL WINAPI HookSetWindowText(HWND hWindow, LPCSTR lpstrText)
{
	LPCWSTR wstr = NULL;
	if (lpstrText) {
		wstr = MultiByteToWideCharInternal(lpstrText);
	}
	BOOL ret = SetWindowTextW(hWindow, wstr);
	if (wstr) {
		FreeStringInternal((LPVOID)wstr);
	}
	return ret;
}

LONG_PTR WINAPI HookGetWindowLongPtr(HWND hWnd, int nIndex) {
	if (nIndex == GWLP_WNDPROC) {
		// G200: check internal call ? 
		if (GetTlsValueInternal()->InternalCall == 0) {
			// G202: retrieve the proc --> for subclass but why not took effect in some cases ??? 
			// TODO: confirm it with my patch.. 
			if (GetPropA(hWnd, szNtleaWndAscData)) {
				return (LONG_PTR)((NTLEA_WND_ASC_DATA*)GetPropA(hWnd, szNtleaWndAscData))->PrevAnsiWindowProc;
			}
		}
		else {
			--GetTlsValueInternal()->InternalCall;
		}
		// G201:
		return GetWindowLongPtrJ(hWnd, nIndex);
	}
	else {
		return GetWindowLongPtrW(hWnd, nIndex);
	}
}

INLINE VOID ValidateDialogProc(HWND hWnd) {
	NTLEA_TLS_DATA* p = GetTlsValueInternal();
	// we have to check&reset prop for dialog created by CreateDialogIndirect/CreateDialogIndirectParam/MessageBoxA/..., these functions
	// create items without using CreateWindowExA, or it may use CreateWindowExW or even NtCreateWindowEx ... 
	if (!GetPropA(hWnd, szNtleaWndAscData)) {
		if (p->CurrentCallType == CT_CREATE_DIALOG && p->DialogProc == NULL) {
			// use this trick is based on the rule: 
			// dialog items are always setting up internally, and most of them are already handled in TopLevelDialogProc, 
			// except those subclassed ones.
			// use p->DialogProc to check the hWnd is a dialog or not, if it is p->DialogProc should not be NULL
			SetTopWindowProc(p, hWnd, (LONG_PTR)TopLevelWindowProc);
		}
		else if (p->CurrentCallType == CT_CREATE_NULL) {
			SetTopWindowProc(p, hWnd, (LONG_PTR)TopLevelSimpleProc);
		}
	/*	else { // dynamic created case ???? 
			// TODO: handle this possibility ... 
		}*/
	}
}
INLINE LONG_PTR SetWindowLongPtrProc(HWND hWnd, LONG_PTR dwNewLong) {
	NTLEA_WND_ASC_DATA* p = (NTLEA_WND_ASC_DATA*)GetPropA(hWnd, szNtleaWndAscData);
	LONG_PTR PrevAnsiWindowProc = (LONG_PTR)p->PrevAnsiWindowProc; // in actual, new wndproc may be unicode ... 
	p->PrevAnsiWindowProc = (WNDPROC)dwNewLong;
	//	char classname[256]; GetClassNameA(hWnd, classname, sizeof(classname));
	//	ntprintfA(256, 1, "calltype=%d, hwnd=%p, class=%s, proc=%p, old=%p\n", GetTlsValueInternal()->CurrentCallType, hWnd, classname, dwNewLong);
	return PrevAnsiWindowProc;
}
LONG_PTR WINAPI HookSetWindowLongPtr(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
	if (nIndex == GWLP_WNDPROC) {
		ValidateDialogProc(hWnd);
		if (GetPropA(hWnd, szNtleaWndAscData)) { // G210:
			return SetWindowLongPtrProc(hWnd, dwNewLong);
		}
	}
	return SetWindowLongPtrJ(hWnd, nIndex, dwNewLong);
}
LONG_PTR WINAPI HookSetWindowLongPtrW(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
	if (nIndex == GWLP_WNDPROC) {
		// we hook non-unicode window & 
		if (!IsWindowUnicode(hWnd)) ValidateDialogProc(hWnd);
		// NOTE: here is the only thing totally unknown: why WindowProcW should be used as ANSI version and
		// thus could be alright ???? 
		if (GetPropA(hWnd, szNtleaWndAscData)) { // G210:
			return SetWindowLongPtrProc(hWnd, dwNewLong);
		}
	}
	return SetWindowLongPtrJ(hWnd, nIndex, dwNewLong);
}

//BOOL WINAPI HookSetWindowSubclass(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
//{
//	OutputDebugStringA("abc");
//}
//BOOL WINAPI HookRemoveWindowSubclass(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass)
//{
//	OutputDebugStringA("def");
//}

LRESULT WINAPI HookDefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefConversionProc((LPVOID)(DWORD_PTR)DefWindowProcW, hWnd, NULL, FALSE, uMsg, wParam, lParam);
}
LRESULT WINAPI HookDefMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefConversionProc((LPVOID)(DWORD_PTR)(DWORD_PTR)DefMDIChildProcW, hWnd, NULL, FALSE, uMsg, wParam, lParam);
}
LRESULT WINAPI HookDefDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefConversionProc((LPVOID)(DWORD_PTR)DefDlgProcW, hWnd, NULL, FALSE, uMsg, wParam, lParam);
}
LRESULT WINAPI HookDefFrameProc(HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefConversionProc((LPVOID)(DWORD_PTR)DefFrameProcW, hWnd, hWndMDIClient, TRUE, uMsg, wParam, lParam);
}

// BOOL WINAPI HookGetClassInfo(HINSTANCE hInstance,LPCSTR lpClassName,LPWNDCLASS lpWndClass);
// BOOL WINAPI HookGetClassInfoEx(HINSTANCE hInstance, LPCSTR lpClassName, LPWNDCLASSEX lpWndClassEx);
// ATOM WINAPI HookRegisterClass(const WNDCLASS *lpWndClass);
// ATOM WINAPI HookRegisterClassEx(const WNDCLASSEX *lpWndClassEx);
// BOOL WINAPI HookUnregisterClass(LPCSTR lpClassName, HINSTANCE hInstance);
// BOOL WINAPI HookSetClassLong(HWND hWindow, int nIndex, DWORD dwNewLong);
BOOL WINAPI HookVerQueryValue(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{
	while (!InterlockedCompareExchange(&settings.bInternalLockVQV, 1/*ecx*/, 0/*eax*/)) Sleep(0);
	// for \VarFileInfo\Translation 
	// The function retrieves a pointer to this array of language and code page identifiers. 
	if (lstrcmpA(lpSubBlock, szTranslation) == 0) {
		// if query location info, we change that value : 
		if (VerQueryValueInternal(pBlock, lpSubBlock, lplpBuffer, puLen) && *puLen >= sizeof(DWORD)){
			//	*(LPDWORD)(*lplpBuffer) = settings.dwLCID;
			// here we fixed it : see MSDN http://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx
			LANGANDCODEPAGE *lpTranslate = (LANGANDCODEPAGE*)(*lplpBuffer);
			settings.langcodepage.wLanguage = lpTranslate->wLanguage;
			settings.langcodepage.wCodePage = lpTranslate->wCodePage;
			lpTranslate->wLanguage = (WORD)settings.dwLCID;
			lpTranslate->wCodePage = (WORD)settings.dwCodePage; // change the first default one !
			InterlockedDecrement(&settings.bInternalLockVQV);
			return TRUE;
		}
		else {
			InterlockedDecrement(&settings.bInternalLockVQV);
			return FALSE;
		}
	}
	else if (lstrlenA(lpSubBlock)>2 && lpSubBlock[0] == '\\' && lpSubBlock[1] == 'S') { // \StringFileInfo\lang-codepage\string-name
		InterlockedDecrement(&settings.bInternalLockVQV);
	//--Comments------------InternalName--------ProductName----
	//	CompanyName			LegalCopyright		ProductVersion
	//	FileDescription		LegalTrademarks		PrivateBuild
	//	FileVersion			OriginalFilename	SpecialBuild
		// comment : 
		LPCSTR type = lpSubBlock + lstrlenA(lpSubBlock);
		while (type > lpSubBlock && '\\' != *type) --type;
		// build info : 
		char SubBlock[64]; wsprintfA(SubBlock, "\\StringFileInfo\\%04x%04x\\%s", 
			settings.langcodepage.wLanguage, settings.langcodepage.wCodePage, type + 1);
		return VerQueryValueJ(pBlock, SubBlock, lplpBuffer, puLen);
	}
	else {
		InterlockedDecrement(&settings.bInternalLockVQV);
		return VerQueryValueJ(pBlock, lpSubBlock, lplpBuffer, puLen);
	}
}

DWORD WINAPI HookGetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZoneInformation)
{
	DWORD ret = GetTimeZoneInformationInternal(lpTimeZoneInformation);
	if (ret != TIME_ZONE_ID_INVALID) {
		lpTimeZoneInformation->Bias/* [esp + 4] */ = settings.dwTimeZone;
	}
	return ret;
}
#pragma auto_inline(on)

INLINE VOID AcquireCreateProcLock(void) {
	while (!InterlockedCompareExchange(&settings.bInternalLockCreateProc, 1/*ecx*/, 0/*eax*/)) Sleep(0);
}
INLINE VOID ReleaseCreateProcLock(void) {
	InterlockedDecrement(&settings.bInternalLockCreateProc);
}
INLINE int CreateProcessAssocW(NtleaProcess* process, LPCWSTR path, LPVOID* newparam) {
	WCHAR PEname[MAX_PATH], ExeName[MAX_PATH * 2]; // incase bufferoverflow !
	if ((DWORD_PTR)FindExecutableW(path, NULL, PEname) > 32) {
		DWORD Exenamelen = ARRAYSIZE(ExeName);
		if (S_OK != AssocQueryStringW(ASSOCF_OPEN_BYEXENAME, ASSOCSTR_EXECUTABLE, PEname, NULL, ExeName, &Exenamelen)) {
			lstrcpyW(ExeName, PEname); Exenamelen = lstrlenW(PEname);
		}
		if (0 == CreateProcessStartW(process, ExeName)) { // ok now !
			LPWSTR buf = *newparam = AllocateHeapInternal(sizeof(WCHAR) * (Exenamelen + 1 + lstrlenW(path) + 1));
			wsprintfW(buf, L"%s %s", ExeName, path); // --- 
			return (0); // success !! 
		}
	}
	return -1; // still can not found !
}
static int GetApplicationPathA(NtleaProcess* process, LPCSTR path) {
	int ret, quotecnt = 0; DWORD binarytype;
	LPCSTR realpath = *path == '\"' ? (++quotecnt, path + 1) : path; // skip quote
	LPCSTR p = realpath;
	while (*p != '\0') { // skip white space
		if (quotecnt == 0 && (*p == ' ' || *p == '\t' || *p == '\v')) break;
		if (*p == '\"') ++quotecnt; // toggle flag !! 
		if (quotecnt >= 2) break;
		++p; // check next !
	}
	char code = *p; *(LPSTR)p = '\0'; // tmpsave 
	LPCSTR ext = p;
	while (ext > realpath && *ext != '.') --ext;
	if (GetBinaryTypeA(realpath, &binarytype) || lstrcmpiA(ext, ".exe")==0 ) {
		ret = CreateProcessPatchA(process, realpath);
	} else {
		ret = ERR_EXECUTABLE_INVALID;
	}
	*(LPSTR)p = code; // restore
	return ret;
}
static int GetApplicationPathW(NtleaProcess* process, LPCWSTR path) {
	int ret, quotecnt = 0; DWORD binarytype;
	LPCWSTR realpath = *path == L'\"' ? (++quotecnt, path + 1) : path; // skip quote
	LPCWSTR p = realpath;
	while (*p != L'\0') { // skip white space
		if (quotecnt == 0 && (*p == L' ' || *p == L'\t' || *p == L'\v')) break;
		if (*p == L'\"') ++quotecnt; // toggle flag !!
		if (quotecnt >= 2) break;
		++p; // check next !
	}
	wchar_t code = *p; *(LPWSTR)p = L'\0'; // tmpsave
	LPCWSTR ext = p;
	while (ext > realpath && *ext != L'.') --ext;
	if (GetBinaryTypeW(realpath, &binarytype) || lstrcmpiW(ext, L".exe")==0 ) {
		ret = CreateProcessPatchW(process, realpath);
	} else {
		ret = ERR_EXECUTABLE_INVALID;
	}
	*(LPWSTR)p = code; // restore
	return ret;
}

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_SUFFIX				L"_D"
#else
#define DEBUG_SUFFIX				L""
#endif

INLINE HANDLE FindFileExists(LPCWSTR Path) {
	WIN32_FIND_DATAW f; return FindFirstFileW(Path, &f);
}
static LPCWSTR GetAppPathW(LPCWSTR path, LPCWSTR* realpath) {
	int quotecnt = 0;
	*realpath = *path == L'\"' ? (++quotecnt, path + 1) : path; // skip quote
	LPCWSTR p = *realpath;
	while (*p != L'\0') { // skip white space
		if (quotecnt == 0 && (*p == L' ' || *p == L'\t' || *p == L'\v')) break;
		if (*p == L'\"') ++quotecnt; // toggle flag !!
		if (quotecnt >= 2) break;
		++p; // check next !
	}
	return p;
}
static BOOL DispatchToOtherProcessW(LPWSTR NtleasXPath, DWORD NtleasXSize, HINSTANCE hInstance, LPCWSTR commandstring) {
	HANDLE hfile = INVALID_HANDLE_VALUE;
	LPWSTR p = GetModuleFileNameW(hInstance, NtleasXPath, NtleasXSize) + NtleasXPath;
	while (p > NtleasXPath && *p != L'\\') --p; if (*p == L'\\') ++p;

	if (hfile == INVALID_HANDLE_VALUE) {
		wsprintfW(p, L"..\\" IMAGE_FILE_MACHINE_ESTRING L"\\ntleas" DEBUG_SUFFIX L".exe");
		hfile = FindFileExists(NtleasXPath);
	}
	if (hfile == INVALID_HANDLE_VALUE) {
		wsprintfW(p, L"..\\ntleas" DEBUG_SUFFIX L".exe");
		hfile = FindFileExists(NtleasXPath);
	}
	if (hfile == INVALID_HANDLE_VALUE) {
		wsprintfW(p, IMAGE_FILE_MACHINE_ESTRING L"\\ntleas" DEBUG_SUFFIX L".exe");
		hfile = FindFileExists(NtleasXPath);
	}
	if (hfile != INVALID_HANDLE_VALUE) {
		FindClose(hfile); // cleanup find handle !
		// ---------------- 
		LPWSTR ntleasx = NtleasXPath + lstrlenW(NtleasXPath);
		LPCWSTR tailpath = commandstring + lstrlenW(commandstring);
		LPCWSTR realpath, p = GetAppPathW(commandstring, &realpath);
		wchar_t code = *p; *(LPWSTR)p = L'\0'; // tmpsave
		if (tailpath <= p) wsprintfW(ntleasx, L" \"%s\"", realpath); // only process 
		else wsprintfW(ntleasx, L" \"%s\" \"A%s\"", realpath, p + 1); // not only process
	//	wsprintfW(NtleasXPath, L"%s %s", realpath, p + 1);
		*(LPWSTR)p = code; // restore
		// ---------------- 
		return (TRUE);
	}
	return (-1);
}

#pragma auto_inline(off)
BOOL WINAPI HookCreateProcess(LPCVOID lpApplicationName,
	LPCVOID lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment/*ebp+0x20*/, LPCVOID lpCurrentDirectory,
	LPVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation/**/)
{
	settings.bSuspendThreadFlag = !!(dwCreationFlags & CREATE_SUSPENDED); // original one 
	dwCreationFlags |= CREATE_SUSPENDED;
	// M1 
	int ret;
	if (lpApplicationName) {
		ret = settings.bCreateProcUniFunc ? CreateProcessStartW((NtleaProcess*)&settings, (LPCWSTR)lpApplicationName)
			: CreateProcessStartA((NtleaProcess*)&settings, (LPCSTR)lpApplicationName);
	} else if (lpCommandLine) {
		ret = settings.bCreateProcUniFunc ? GetApplicationPathW((NtleaProcess*)&settings, (LPCWSTR)lpCommandLine)
			: GetApplicationPathA((NtleaProcess*)&settings, (LPCSTR)lpCommandLine);
	} else {
		ret = -12345; // assume it as false without continue !
	}
	// P3 
	if (ret == 0) { // a lil dummy here : 
		ret = (settings.bCreateProcUniFunc ? CreateProcessInternalW : CreateProcessInternalA)(
			lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
			bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	} else if (!lpApplicationName && ret == ERR_EXECUTABLE_INVALID) { // for non-PE file such as .txt, .zip ... 
		// if it is external program without directory setting !! 
		LPVOID pCommandLineBuf = NULL;
		if (settings.bCreateProcUniFunc) {
			ret = CreateProcessAssocW((NtleaProcess*)&settings, (LPCWSTR)lpCommandLine, &pCommandLineBuf);
		} else {
			LPCWSTR realpathw = MultiByteToWideCharInternal((LPCSTR)lpCommandLine);
		//	if (lpCurrentDirectory) pCurrentDirectory = MultiByteToWideCharInternal((LPCSTR)lpCurrentDirectory);
			ret = CreateProcessAssocW((NtleaProcess*)&settings, realpathw, &pCommandLineBuf);
			if (realpathw) FreeStringInternal((LPVOID)realpathw);
			if (ret == -1) ret = ERR_EXECUTABLE_INVALID; // still this error !
		}
		if (ret == 0) {
			LPCWSTR lpCurrentDirectoryW = lpCurrentDirectory ? MultiByteToWideCharInternal((LPCSTR)lpCurrentDirectory) : NULL;
			LPCWSTR lpEnvironmentW = lpEnvironment ? MultiByteToWideCharInternal((LPCSTR)lpEnvironment) : NULL;
			ret = CreateProcessInternalW( // force replace commandline!
				lpApplicationName, pCommandLineBuf, lpProcessAttributes, lpThreadAttributes,
				bInheritHandles, dwCreationFlags, (LPVOID)lpEnvironmentW, lpCurrentDirectoryW, lpStartupInfo, lpProcessInformation);
			if (lpEnvironmentW) FreeStringInternal((LPVOID)lpEnvironmentW);
			if (lpCurrentDirectoryW) FreeStringInternal((LPVOID)lpCurrentDirectoryW);
		}
		if (pCommandLineBuf) FreeStringInternal(pCommandLineBuf);
	} else if (!lpApplicationName && ret == ERR_PE_FORMAT_INVALID) { // for non-Platform PE file cross platform ...
		// If 'lpCurrentDirectory' is NULL, the new process will have the same current drive and directory as the calling process.
		WCHAR NtleasXPath[MAX_PATH * 4]; LPCWSTR pCommandLineBuf = NULL;
		// check ansi or unicode version : 
		if (!settings.bCreateProcUniFunc) pCommandLineBuf = MultiByteToWideCharInternal((LPCSTR)lpCommandLine);
		// now adapt to ntleas calling : 
		ret = DispatchToOtherProcessW(NtleasXPath, ARRAYSIZE(NtleasXPath), settings.hInstance, 
			pCommandLineBuf ? pCommandLineBuf : lpCommandLine);
		if (ret >= 0) {
			LPCWSTR lpCurrentDirectoryW = lpCurrentDirectory ? MultiByteToWideCharInternal((LPCSTR)lpCurrentDirectory) : NULL;
			LPCWSTR lpEnvironmentW = lpEnvironment ? MultiByteToWideCharInternal((LPCSTR)lpEnvironment) : NULL;
			ret = CreateProcessInternalW( // force replace commandline!
				lpApplicationName, NtleasXPath, lpProcessAttributes, lpThreadAttributes,
				bInheritHandles, dwCreationFlags, (LPVOID)lpEnvironmentW, lpCurrentDirectoryW, lpStartupInfo, lpProcessInformation);
			if (lpEnvironmentW) FreeStringInternal((LPVOID)lpEnvironmentW);
			if (lpCurrentDirectoryW) FreeStringInternal((LPVOID)lpCurrentDirectoryW);
			if (ret == TRUE) {
				ResumeThread(lpProcessInformation->hThread); // startup at once !!
				ret = ERR_REDIRECTED_PLATFORM;
			}
		}
		if (pCommandLineBuf) FreeStringInternal((LPVOID)pCommandLineBuf);
	} else {
		ret = -54321;
	}
	if (ret <= 0 /*including FALSE*/ && ret != ERR_EXECUTABLE_INVALID) {
		if (settings.bCreateProcUniFunc) {
			WCHAR errbuf[MAX_PATH * 2]; wsprintfW(errbuf, L"app=%s param=%s ret=%d\n", lpApplicationName, lpCommandLine, ret);
			OutputDebugStringW(errbuf);
		} else {
			char  errbuf[MAX_PATH * 2]; wsprintfA(errbuf,  "app=%s param=%s ret=%d\n", lpApplicationName, lpCommandLine, ret);
			OutputDebugStringA(errbuf);
		}
	}
	// ----------------------------- 
	if (ret < 0 && ret != ERR_EXECUTABLE_INVALID) {
		MessageBoxA(GetForegroundWindow(), szNtleaiExeUnsupport, "NTLEA internal exception", MB_ICONHAND);
	} else if (ret == ERR_EXECUTABLE_INVALID) {
		ret = FALSE; // creation failed without prompt !
	} else if (ret == ERR_REDIRECTED_PLATFORM) {
		ret = TRUE; // no needs continue create process !
	} else if (ret == FALSE) {
		MessageBoxA(GetForegroundWindow(), szNtleaiExeFailed, "NTLEA internal exception", MB_ICONHAND);
	} else if (CreateProcessEnd((NtleaProcess*)&settings, lpProcessInformation, settings.bSuspendThreadFlag) < 0) {
		TerminateProcess(lpProcessInformation->hProcess, (UINT)(-1)); // exit !! 
		MessageBoxA(GetForegroundWindow(), szNtleaiDllMissing, "NTLEA internal exception", MB_ICONHAND);
	}
	// --------------------- 
	// P6 / PN6: Prepare exit : 
	settings.bCreateProcUniFunc = 0;
	ReleaseCreateProcLock();
	return ret;
}

BOOL WINAPI HookCreateProcessW(LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	AcquireCreateProcLock();
	++settings.bCreateProcUniFunc;
	return HookCreateProcess(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

BOOL WINAPI HookCreateProcessA(LPCSTR lpApplicationName,
	LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	AcquireCreateProcLock();
	// no unicode setting required !
	return HookCreateProcess(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

#pragma auto_inline(off)
#define STRSAFELENA(str)		(str?lstrlenA(str):0)
static HINSTANCE OpenExecuteFile(LPCSTR lpApplicationName, LPCSTR lpCmdLine, LPCSTR lpDirectory, INT nShowCmd)
{
	PROCESS_INFORMATION psinfo;
	LPSTR lpCommandLine = LocalAlloc(0, sizeof(char) * (STRSAFELENA(lpApplicationName) + 3 + STRSAFELENA(lpCmdLine) + 1)); // 
	STARTUPINFOA sinfo = { sizeof(STARTUPINFOA), };
	sinfo.dwFlags = STARTF_USESHOWWINDOW;
	sinfo.wShowWindow = (WORD)nShowCmd;
	// check if required, copy string with extra quote: 
	if (lpApplicationName && lpApplicationName[0] == '\"') {
		if (lpCmdLine && lpCmdLine[0]) {
			wsprintfA(lpCommandLine, "%s %s", lpApplicationName, lpCmdLine);
		} else {
			wsprintfA(lpCommandLine, "%s", lpApplicationName); // maybe it's just parameters like doc name ?!
		}
	} else if (lpApplicationName) {
		if (lpCmdLine && lpCmdLine[0]) {
			wsprintfA(lpCommandLine, "\"%s\" %s", lpApplicationName, lpCmdLine);
		} else {
			wsprintfA(lpCommandLine, "\"%s\"", lpApplicationName); // maybe it's just parameters like doc name ?!
		}
	} else if (lpCmdLine) {
		lstrcpyA(lpCommandLine, lpCmdLine);
	} else return NULL; // invalid !!
	// reset directory if it is empty string : 
	if (lpDirectory && lpDirectory[0] == '\0') { lpDirectory = NULL; }
	// createprocess ansi version : 
	BOOL ret = HookCreateProcess(NULL, lpCommandLine, NULL, NULL, FALSE, 0, NULL, lpDirectory, &sinfo, &psinfo);
	LocalFree(lpCommandLine);
	return ret ? (CloseHandle(psinfo.hThread), (HINSTANCE)psinfo.hProcess) : NULL; // fake ??? 
}
UINT WINAPI HookWinExec(LPCSTR lpCmdLine, UINT uCmdShow)
{
	return OpenExecuteFile(lpCmdLine, NULL, NULL, uCmdShow) ? (32) : ERROR_PATH_NOT_FOUND;
}
HINSTANCE WINAPI HookShellExecute(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	HINSTANCE inst = NULL;
	// see : http://msdn.microsoft.com/en-us/library/windows/desktop/bb773621(v=vs.85).aspx
	// sometimes : GetFileAttributes will return fail (not wanted value) when that file is being used : 
	if (lstrcmpiA(lpOperation, "open") == 0 && lpFile) {
		LPCWSTR lpFileW = MultiByteToWideCharInternal(lpFile);
		if (PathIsDirectoryW(lpFileW) == FILE_ATTRIBUTE_DIRECTORY) {
			LPCWSTR lpParametersW = lpParameters ? MultiByteToWideCharInternal(lpParameters) : NULL;
			LPCWSTR lpDirectoryW = lpDirectory ? MultiByteToWideCharInternal(lpDirectory) : NULL;
			inst = ShellExecuteW(hwnd, L"open", lpFileW, lpParametersW, lpDirectoryW, nShowCmd);
			if (lpDirectoryW) FreeStringInternal((LPVOID)lpDirectoryW);
			if (lpParametersW) FreeStringInternal((LPVOID)lpParametersW);
			return inst;
		} else {
			inst = OpenExecuteFile(lpFile, lpParameters, lpDirectory, nShowCmd);
		}
		FreeStringInternal((LPVOID)lpFileW);
	}
	return inst ? inst : ShellExecuteJ(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}
// we should also have HookShellExecuteW !! 
#pragma auto_inline(on)

int WINAPI HookCompareString(LCID Locale, DWORD dwCmpFlags, LPCSTR lpString1, int cchCount1, LPCSTR lpString2, int cchCount2)
{
	dwCmpFlags &= NORM_IGNORECASE;
	dwCmpFlags |= NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH;
	return CompareStringJ(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
}

HANDLE WINAPI HookCreateFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	++GetTlsValueInternal()->IsCreateFileCall;
	return CreateFileJ(lpFileName, dwDesiredAccess, dwShareMode,
		lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

int WINAPI HookMBtoWC(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
//	if (lpMultiByteStr && lpWideCharStr) OutputDebugStringA(lpMultiByteStr);
	if (GetTlsValueInternal()->IsCreateFileCall) {
		GetTlsValueInternal()->IsCreateFileCall = 0;
		CodePage/*[ebp + 8]*/ = (CodePage >= CP_UTF7/*support UTF-7 UTF-8*/) ? CodePage : CP_ACP; // create file should use default CP, or else file won't be found !! 
	} else {
		CodePage/*[ebp + 8]*/ = (CodePage >= CP_UTF7/*support UTF-7 UTF-8*/) ? CodePage : settings.dwCodePage;
	}
	return MBtoWCJ(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

int WINAPI HookWCtoMB(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
//	if (lpMultiByteStr && lpWideCharStr) OutputDebugStringW(lpWideCharStr);
	CodePage/*[ebp + 8]*/ = (CodePage >= CP_UTF7/*support UTF-7 UTF-8*/) ? CodePage : settings.dwCodePage;
	return WCtoMBJ(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
}

HRSRC WINAPI HookFindResourceEx(HMODULE hModule, LPCSTR lpType, LPCSTR lpName, WORD wLanguage)
{
	wLanguage = (WORD)settings.dwLCID;
	LPCWSTR lpTypeW = lpType ? MultiByteToWideCharInternal(lpType) : NULL;
	LPCWSTR lpNameW = ((DWORD_PTR)lpName & WM_CLASSMASK) ? MultiByteToWideCharInternal(lpName) : NULL;
	HRSRC hrsrc = FindResourceExW(hModule, lpTypeW, (lpNameW ? lpNameW : (LPCWSTR)lpName), wLanguage);
	if (lpNameW) FreeStringInternal((LPVOID)lpNameW);
	if (lpTypeW) FreeStringInternal((LPVOID)lpTypeW);
	return hrsrc;
}

LCID WINAPI HookGetLocaleID(void)
{
	return settings.dwLCID;
}

BOOL WINAPI HookGetCPInfo(UINT CodePage, LPCPINFO lpCPInfo)
{
	CodePage = settings.dwCodePage;
	return GetCPInfoJ(CodePage, lpCPInfo);
}

UINT WINAPI HookGetACP(void)
{
	return settings.dwCodePage;
}

UINT WINAPI HookGdiGetCodePage(HDC hdc)
{
	UNREFERENCED_PARAMETER(hdc);
	return settings.dwCodePage;
}
#pragma auto_inline(on)


static BOOL Is_WinXP_SP2_or_Later(void) {
	// from MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/ms725491(v=vs.85).aspx
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	int op = VER_GREATER_EQUAL;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;
	osvi.wServicePackMajor = 2;
	osvi.wServicePackMinor = 0;
#pragma warning(disable:4244)
	// Initialize the condition mask.
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);
#pragma warning(default:4244)
	// Perform the test.
	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		dwlConditionMask);
}

static BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCSTR lpszType, LPSTR lpszName, LONG_PTR lParam)
{
	NtleaProcess* settings = (NtleaProcess*)lParam;
	HRSRC hResInfo = FindResourceA(hModule, lpszName, lpszType);
	DWORD dwSize;
	HGLOBAL hResData;
	LPVOID pRes, pResCopy;
	UINT cbTranslate;
	LANGANDCODEPAGE *lpTranslate;

	dwSize = SizeofResource(hModule, hResInfo);
	hResData = LoadResource(hModule, hResInfo);
	pRes = LockResource(hResData);
	pResCopy = LocalAlloc(LMEM_FIXED, dwSize);
	CopyMemory(pResCopy, pRes, dwSize);
	FreeResource(hResData);

	if (VerQueryValueA(pResCopy, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate)) {
		if (cbTranslate >= sizeof(LANGANDCODEPAGE)) {
			settings->dwLCID = lpTranslate[0].wLanguage;
			settings->dwCodePage = lpTranslate[0].wCodePage;
		}
	}
	LocalFree(pResCopy);

	return FALSE; // stop, cuz i think any resource should have the same locale info!
}

static DWORD CALLBACK InitUnicodeLayer(DWORD param)
{
	UNREFERENCED_PARAMETER(param);
	HANDLE RcpEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, szRcpEvent);

	HANDLE hfilemapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(LPVOID), szRcpFileMap);
	if (GetLastError() != ERROR_ALREADY_EXISTS) { // put default value 
		settings.bChinesePath = 0;
		settings.bCreateProcNative = 0;
		settings.bForceSpecifyFont = 0;
		settings.bNoFilterCommCtrl = 0;
		// ---------------------- don't forget our codes : 
		settings.dwCodePage = 932/*936 or 950*/;
		settings.dwLCID = 0x0411/*0x0804*/;
		settings.dwTimeZone = (DWORD)-540;
		// ---------------------- 
		CloseHandle(hfilemapping);
	}
	else { // put input value
		LPVOID hfilemapview = MapViewOfFile(hfilemapping, FILE_MAP_READ, 0, 0, sizeof(LPVOID));
		if (hfilemapview) {
			// skip the first unicode string : 
			LPCWSTR p = *(LPCWSTR*)hfilemapview;
			while (*p) p++; p++;// skip to next 
			LPCSTR sp = (LPCSTR)p;
			LPBYTE dp = settings.lpFontFaceName;
			do {
				*dp++ = *sp;
			} while (*sp++); // skip to next			// ------------- 
			LPDWORD n = (LPDWORD)sp;
			DWORD dwCompOption = *(n + 0);
			settings.bChinesePath = (dwCompOption & 0x0001) >> 0;
			settings.bCreateProcNative = (dwCompOption & 0x0002) >> 1;
			settings.bForceSpecifyFont = (dwCompOption & 0x0004) >> 2;
			settings.bNoFilterCommCtrl = (dwCompOption & 0x0008) >> 3;
			//	settings.bNoAutoMatchLocal	= (dwCompOption & 0x0010) >>  4;
			settings.bMinidumpHelpFile = (dwCompOption & 0x8000) >> 15;
			settings.dwCodePage = *(n + 1);
			settings.dwLCID = *(n + 2);
			settings.dwTimeZone = *(n + 3);
			settings.dwFontSizePercent = *(n + 4); if (settings.dwFontSizePercent == 0) settings.dwFontSizePercent = 100; // reset !!
			// ------------- 
			UnmapViewOfFile(hfilemapview);
			CloseHandle(hfilemapping);

			settings.lfcharset = CodePageToCharset(settings.dwCodePage);
			settings.lfcharold = CodePageToCharset(GetACP());
		//	ntprintfA(260, 1, "font:%s, settings.dwCodePage=%d, settings.dwLCID=%d, settings.dwTimeZone=%d, settings.dwFontSizePercent=%d\n"
		//		, settings.lpFontFaceName, settings.dwCodePage, settings.dwLCID, settings.dwTimeZone, settings.dwFontSizePercent);
		}
		else {
			return (DWORD)(-1);
		}
		// apply : 
		if (!settings.bMinidumpHelpFile) FreeMiniDump();
	//	if (!settings.bNoAutoMatchLocal) EnumResourceNamesA(GetModuleHandleA(NULL), RT_VERSION, EnumResNameProc, (LONG_PTR)&settings);
	}

	if (!Is_WinXP_SP2_or_Later()) { // no longer support yet!!
		return (DWORD)(-1);
	}
	///	OSVERSIONINFOA osv = {sizeof(OSVERSIONINFOA),};
	///	GetVersionExA(&osv);
	///	if (osv.dwMajorVersion < VER_MAJOR_WIN2K) {
	///		return (-1); // not support 
	///	}
	///	else if (osv.dwMajorVersion == VER_MAJOR_WIN2K) { // NT5
	///		if (osv.dwMinorVersion == 0) { // WIN2K
	///			settings.nOSVer = VER_WIN2K;
	///		}
	///		else if (osv.dwMinorVersion == 1 && osv.szCSDVersion[13] < '2') { // WINXP
	///			settings.nOSVer = VER_WINXP_SP1;
	///		}
	///		else if (osv.szCSDVersion[0] == 0) {
	///			settings.nOSVer = VER_WINXP;
	///		}
	///	}
	///	else // else XP SP2 or later NT6 
	///		settings.nOSVer = VER_WINXP_SP2_OR_ABOVE;

	(HookAnsiFunctions)(&settings);

	if (RcpEvent) {
		SetEvent(RcpEvent); CloseHandle(RcpEvent);
	}

	return (0);
}


#ifndef __NTHOOK_H__
#define __NTHOOK_H__

#ifndef INLINE
#define INLINE __inline
#endif//INLINE

#include "constants.h"

typedef struct { // same as CREATESTRUCT
	LPVOID    	lpParam;
	HINSTANCE	hInstance;
	HMENU		hMenu;
	HWND		hWndParent;
	INT			nHeight;
	INT			nWidth;
	INT			y;
	INT			x;
	DWORD		dwStyle;
	LPVOID		lpWindowName;	// convert !!
	LPVOID		lpClassName;	// convert !!
	DWORD		dwExStyle;
} CREATEWNDEX;

typedef struct { // same as MDICREATESTRUCT
	LPCSTR		szClass;
	LPCSTR		szTitle;
	HWND		hOwner;
	INT			x;
	INT			y;
	INT			cx;
	INT			cy;
	DWORD		style;
	LPARAM		lParam;
} CREATEMDIWND;

typedef struct {
	LONG		lfHeight;
	LONG		lfWidth;
	LONG		lfEscapement;
	LONG		lfOrientation;
	LONG		lfWeight;
	BYTE		lfItalic;
	BYTE		lfUnderline;
	BYTE		lfStrikeOut;
	BYTE		lfCharSet;
	BYTE		lfOutPrecision;
	BYTE		lfClipPrecision;
	BYTE		lfQuality;
	BYTE		lfPitchAndFamily;
} LOGFONTSIMILAR;

// --------------------------- 
// high level control message 
// ---------------------------

// see : http://chokuto.ifdef.jp/urawaza/message/
typedef struct {
	UINT   mask;
	DWORD  dwState;
	DWORD  dwStateMask;
	LPVOID pszText;		// mask including 'TCIF_TEXT'
	int    cchTextMax;  // 
	int    iImage;
	LPARAM lParam;
} TCITEM, *LPTCITEM;

typedef struct {
	UINT   mask;
	int	   fmt;
	int    cx;
	LPVOID pszText;		// mask including 'TCIF_TEXT'
	int    cchTextMax;	// 
	int    iSubItem;
} LVCOLUMN, *LPLVCOLUMN;

typedef struct {
	UINT   mask;
	int    iItem;
	int    iSubItem;
	UINT   state;
	UINT   stateMask;
	LPVOID pszText;		// mask including 'LVIF_TEXT'
	int    cchTextMax;	// 
	int    iImage;
	LPARAM lParam;
} LVITEM, *LPLVITEM;

typedef struct {
	UINT    mask;
	HANDLE	hItem;
	UINT    state;
	UINT    stateMask;
	LPVOID  pszText;	// mask including 'LVIF_TEXT'
	int     cchTextMax;	// 
	int     iImage;
	int     iSelectedImage;
	int     cChildren;
	LPARAM  lParam;
	//	int     iIntegral;	// if ex 
} TVITEM, *LPTVITEM;

typedef struct {
	HANDLE  hParent;
	HANDLE  hInsertAfter;
	TVITEM	item;		// TVITEM or TVITEMEX
} TVINSERTSTRUCT, *LPTVINSERTSTRUCT;

#if defined(_M_IX86)
#define __unaligned
#endif//__unaligned

typedef struct _ITEMIDLIST			__unaligned	*LPITEMIDLIST;
typedef struct _ITEMIDLIST const	__unaligned *LPCITEMIDLIST;

#if defined(_M_IX86)
#undef  __unaligned
#endif//__unaligned

#ifndef PIDLIST_ABSOLUTE         
#define PIDLIST_ABSOLUTE        LPITEMIDLIST
#endif//PIDLIST_ABSOLUTE         
#ifndef PCIDLIST_ABSOLUTE
#define PCIDLIST_ABSOLUTE       LPCITEMIDLIST
#endif//PCIDLIST_ABSOLUTE
#ifndef PCUIDLIST_ABSOLUTE
#define PCUIDLIST_ABSOLUTE      LPCITEMIDLIST
#endif//PCUIDLIST_ABSOLUTE

typedef struct {
	HWND				hwndOwner;
	PCIDLIST_ABSOLUTE	pidlRoot;
	LPCVOID				pszDisplayName;			// Return display name of item selected.
	LPCVOID				lpszTitle;				// text to go in the banner over the tree.
	UINT				ulFlags;				// Flags that control the return stuff
	LPVOID				lpfn;
	LPARAM				lParam;					// extra info that's passed back in callbacks
	int					iImage;					// output var: where to return the Image index.
} BROWSEINFO, *LPBROWSEINFO;

// --------------------- 

typedef struct {
	WNDPROC		AnsiSystemClassProc;
	WNDPROC		UnicodeSystemClassProc;
} SYSTEM_CLASS_WNDPROC;

typedef struct {
	DWORD		InternalCall;
	DWORD		DBCSLeadByte;
	DWORD		IsFontAvailable;
	DWORD		CurrentCallType;
	DLGPROC		DialogProc;
	DWORD		IsCreateFileCall;
	LONG		hWindowHooking; // counter for avoiding setup/uninst hook-recursive
	HHOOK		hWindowCbtHookAnsi;
	HHOOK		hWindowCbtHookUnicode;
	SYSTEM_CLASS_WNDPROC SystemClassDesc[MAXSYSCLASSDESC];
//	WNDPROC		DynamicClassProc[MAXSYSCLASSDESC]; // runtime cache !?
} NTLEA_TLS_DATA;

typedef struct {
	WNDPROC		PrevAnsiWindowProc;
	WNDPROC		PrevUnicodeWindowProc;
} NTLEA_WND_ASC_DATA;

typedef struct {
	WORD		wLanguage;
	WORD		wCodePage;
} LANGANDCODEPAGE;

typedef struct {
	LPVOID		fontenumproc;
	LPARAM		fontenumpara;
	HDC			fontenumhdc;
	BYTE		fontenumface[LF_FACESIZE * 2];
	BYTE		fontenumname[LF_FACESIZE * 2];
} FONTENUMPROCPARAM;

// ------------------- 

typedef struct {
	// -------------- inputparam 
	DWORD dwCompOption;
	DWORD dwCodePage;
	DWORD dwLCID;
	DWORD dwTimeZone;
	DWORD dwFontSizePercent;
	BYTE lpFontFaceName[LF_FACESIZE];
	// -------------- internal 
	HANDLE RcpEvent, RcpFileMap;
	LPVOID FileMappingAddress, ImageBase, EntryPoint;
	HMODULE hInstance;

	HANDLE hHeap;
	LANGANDCODEPAGE langcodepage;
	int   lfcharset, lfcharold;
	// ----------------------------------------------- 
//	DWORD dwWindowProcCounter; // =0
//	DWORD dwWindowHandleCounter; // =0
	DWORD dwThreadIdMBtoWC;
	DWORD dwThreadIdWCtoMB;
///	DWORD MbCodePageTag;
///	DWORD nOSVer;
	DWORD nTlsIndex;
	// --- 
	LONG  bInternalLockMBtoWC;
	LONG  bInternalLockWCtoMB;
	LONG  bInternalLockCreateProc;
	LONG  bInternalLockVQV;
//	LONG  bInternalLockRegClass;
	LONG  bInternalLockCreateAtom;
	// --- 
	int	  bNtleaAtomInvoked;
	int	  bInternalCallMBtoWC;
	int	  bInternalCallWCtoMB;
	int	  bErrorFlag;
	int	  bCreateProcUniFunc;
	int	  bSuspendThreadFlag;
	// bits : 
	int	  bForceSpecifyFont;	//  0
	int	  bCreateProcNative;	//  1
	int	  bChinesePath;			//  2
	int	  bNoFilterCommCtrl;	//  3
	int	  bNoAutoMatchLocal;	//  4
	int	  bMinidumpHelpFile;	// 15
	// ------------------------------- 
	LOGFONTSIMILAR logfontw;
} Settings;

#define CT_CREATE_NULL			0	// used for checking system create without known yet!
#define CT_CREATE_WINDOW        1	// used by any window
#define CT_CREATE_DIALOG        2	// used by dialog & dialogindirect series
#define CT_CREATE_PRESET		3	// used by SHBrowseForFolderA

#define WM_UNKNOWN				0x43E

BOOL WINAPI
RunDllMain(HINSTANCE, DWORD, LPVOID);
LRESULT WINAPI
HookCallWindowProc(WNDPROC PrevWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI
HookSendMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI
HookPostMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI
HookSendNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI
HookSendMessageCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, SENDASYNCPROC lpCallBack, ULONG_PTR dwData);
LRESULT WINAPI
HookSendMessageTimeout(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, PDWORD_PTR lpdwResult);
INT_PTR WINAPI
HookDialogBoxIndirectParam(HINSTANCE hInstance, LPCDLGTEMPLATE hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
HWND WINAPI
HookCreateDialogIndirectParam(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit);
INT_PTR WINAPI
HookDialogBoxParam(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
HWND WINAPI
HookCreateDialogParam(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
NTSTATUS WINAPI
HookUnicodeToMultiByte(LPSTR AnsiBuffer, DWORD MultiByteLength, LPDWORD lpNumberOfBytesConverted, LPCWSTR UnicodeBuffer, DWORD WideCharLength);
NTSTATUS WINAPI
HookMultiByteToUnicode(LPWSTR UnicodeBuffer, DWORD WideCharLength, DWORD* lpNumberOfBytesConverted, LPCSTR AnsiBuffer, DWORD MultiByteLength);
NTSTATUS WINAPI
HookUnicodeToMultiByteSize(int* lpNumberOfBytesConverted, LPCWSTR UnicodeBuffer, int WideCharLength);
NTSTATUS WINAPI
HookMultiByteToUnicodeSize(int* lpNumberOfBytesConverted, LPCSTR AnsiBuffer, int MultiByteLength);
BOOL WINAPI
HookIsDBCSLeadByte(BYTE TestChar);
LPSTR WINAPI
HookCharPrev(LPCSTR lpStart, LPCSTR lpCurrentChar);
LPSTR WINAPI
HookCharNext(LPCSTR lpCurrentChar);
HGDIOBJ WINAPI
HookGetStockObject(int fnObject);
HFONT WINAPI
HookCreateFontIndirect(const LOGFONT *lplf);
int WINAPI
HookGetMenuString(HMENU hMenu, UINT uIDItem, LPSTR lpString, int nMaxCount, UINT uFlag);
BOOL WINAPI
HookGetMenuItemInfo(HMENU hMenu, UINT uItem, BOOL fByPosition, LPMENUITEMINFOA lpmii);
BOOL WINAPI
HookSetMenuItemInfo(HMENU hMenu, UINT uItem, BOOL fByPosition, LPMENUITEMINFOA lpmii);
int WINAPI
HookEnumFontFamiliesExA(HDC hdc, LPLOGFONTA lpLogfont, FONTENUMPROCA lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags);
int WINAPI
HookEnumFontFamiliesExW(HDC hdc, LPLOGFONTW lpLogfont, FONTENUMPROCW lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags);
int WINAPI
HookEnumFontFamiliesA(HDC hdc, LPCSTR lpszFamily, FONTENUMPROCA lpEnumFontFamProc, LPARAM lParam);
int WINAPI
HookEnumFontFamiliesW(HDC hdc, LPCWSTR lpszFamily, FONTENUMPROCW lpEnumFontFamProc, LPARAM lParam);
int WINAPI 
HookEnumFontsA(HDC hdc, LPCSTR lpFaceName, FONTENUMPROCA lpFontFunc, LPARAM lParam);
int WINAPI
HookEnumFontsW(HDC hdc, LPCWSTR lpFaceName, FONTENUMPROCW lpFontFunc, LPARAM lParam);
HWND WINAPI
HookCreateWindowEx(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
int WINAPI
HookGetWindowText(HWND hWindow, LPSTR lpString, int nMaxCount);
BOOL WINAPI
HookSetWindowText(HWND hWindow, LPCSTR lpstrText);
LONG_PTR WINAPI
HookGetWindowLongPtr(HWND hWnd, int nIndex);
LONG_PTR WINAPI
HookSetWindowLongPtr(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
LONG_PTR WINAPI
HookSetWindowLongPtrW(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
LRESULT WINAPI
HookDefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI
HookDefMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI
HookDefDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI
HookDefFrameProc(HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam);
PIDLIST_ABSOLUTE WINAPI
HookSHBrowseForFolder(LPBROWSEINFO lpbi);
BOOL WINAPI
HookSHGetPathFromIDList(PIDLIST_ABSOLUTE pidl, LPSTR pszPath);
BOOL WINAPI
HookVerQueryValue(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);
DWORD WINAPI
HookGetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZoneInformation);
BOOL WINAPI
HookCreateProcess(LPCVOID lpApplicationName, LPCVOID lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment/*ebp+0x20*/, LPCVOID lpCurrentDirectory, LPVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation/**/);
BOOL WINAPI
HookCreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
BOOL WINAPI
HookCreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
UINT WINAPI
HookWinExec(LPCSTR lpCmdLine, UINT uCmdShow);
HINSTANCE WINAPI
HookShellExecute(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
int WINAPI
HookCompareString(LCID Locale, DWORD dwCmpFlags, LPCSTR lpString1, int cchCount1, LPCSTR lpString2, int cchCount2);
HANDLE WINAPI
HookCreateFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
int WINAPI
HookMBtoWC(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
int WINAPI
HookWCtoMB(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
HRSRC WINAPI
HookFindResourceEx(HMODULE hModule, LPCSTR lpType, LPCSTR lpName, WORD wLanguage);
LCID WINAPI
HookGetLocaleID(void);
BOOL WINAPI
HookGetCPInfo(UINT CodePage, LPCPINFO lpCPInfo);
UINT WINAPI
HookGetACP(void);
UINT WINAPI
HookGdiGetCodePage(HDC hdc);

#endif // __NTHOOK_H__

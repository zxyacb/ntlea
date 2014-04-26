
#include "ntleasCtxExt.h"
#include "../ntprofile/ntprofile.h"
#include "../ntleasWin/ntleasRunApp.h"
#include <liblocale.h>
#include <Windows.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <assert.h>

#define IDB_OK					101		// 
#define IDB_CONFIG				102		// 
#define IDB_ENTRY				103		// 

#define IDM_CONFIG				0		// 
#define IDM_DIVIDE				1		// 
#define IDM_PREDEF				2		// 

#define MAX_PROFILE				128		// 

static HINSTANCE g_hInst;

struct NtleasContextExtend {
	PWSTR	m_pwszMenuText;
	HANDLE	m_hMenuBmp;
	HANDLE	m_hMenuCfg;
	HANDLE	m_hMenuEnt;
	PCSTR	m_pszVerb;
	PCWSTR	m_pwszVerb;
	PCWSTR	m_pwszVerbCanonicalName;
	PCWSTR	m_pwszVerbHelpText;

	LPVOID		m_profileb;
	int			m_profilen;
	NtProfile	m_profiles[MAX_PROFILE];
};

inline void GetDirectoryFilePath(WCHAR szPath[], UINT nPath, LPCWSTR filename)
{
	DWORD size = GetModuleFileNameW(g_hInst, szPath, nPath);
	assert(ERROR_INSUFFICIENT_BUFFER != GetLastError());
	LPWSTR p = wcsrchr(szPath, L'\\'); if (p) *p = L'\0';
	StringCchCatW(szPath, nPath - (p ? (p - szPath) : size), filename);
}

int Ntleas_SizeCtxExt(void) {
	return sizeof(NtleasContextExtend);
}

int Ntleas_InitCtxExt(NtleasContextExtend* pExt) {

	pExt->m_pwszMenuText = L"Run With Ntleas";
	pExt->m_pszVerb = "NtleasCtx";
	pExt->m_pwszVerb = L"NtleasCtx";
	pExt->m_pwszVerbCanonicalName = L"NtLocaleEmualtorAdvance";
	pExt->m_pwszVerbHelpText = L"Run Application With Ntleas";

	pExt->m_profileb = NULL;
	pExt->m_profilen = 0;
	
	// Load the bitmap for the menu item. 
	// If you want the menu item bitmap to be transparent, the color depth of 
	// the bitmap must not be greater than 8bpp.
	pExt->m_hMenuBmp = LoadImage(g_hInst, MAKEINTRESOURCEW(IDB_OK), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADTRANSPARENT);
	pExt->m_hMenuCfg = LoadImage(g_hInst, MAKEINTRESOURCEW(IDB_CONFIG), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADTRANSPARENT);
	pExt->m_hMenuEnt = LoadImage(g_hInst, MAKEINTRESOURCEW(IDB_ENTRY), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADTRANSPARENT);

	// find config file : 
	WCHAR szCfgPath[MAX_PATH * 2];
	GetDirectoryFilePath(szCfgPath, ARRAYSIZE(szCfgPath), L"\\ntleasWin.cfg");
	// load config file :
	HANDLE hfile = CreateFileW(szCfgPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile != INVALID_HANDLE_VALUE) {
		// load all data in : 
		DWORD length = GetFileSize(hfile, NULL); // length += high << 32;
		pExt->m_profileb = LocalAlloc(0, length);
		DWORD dwread; ReadFile(hfile, pExt->m_profileb, (DWORD)length, &dwread, NULL); CloseHandle(hfile);
	//	StringCchPrintfW(szCfgPath, ARRAYSIZE(szCfgPath), L"read size : %d", dwread); OutputDebugStringW(szCfgPath);
		// parse data : 
		InitNtProfiles(pExt->m_profileb, dwread, [](NtProfile* profile, void* ctx){
			NtleasContextExtend& ext = *(NtleasContextExtend*)ctx;
			if (ext.m_profilen >= MAX_PROFILE) return (FALSE);
			ext.m_profiles[ext.m_profilen++] = *profile;
		//	OutputDebugStringW(ext.m_profiles[ext.m_profilen - 1].entrys[PROFILE_CONFIGNAME]);
			return (TRUE);
		}, pExt);
	}

	return (0);
}

int Ntleas_FreeCtxExt(NtleasContextExtend* pExt) {

	if (pExt->m_profileb)
	{
		LocalFree(pExt->m_profileb);
	}
	if (pExt->m_hMenuEnt)
	{
		DeleteObject(pExt->m_hMenuEnt);
		pExt->m_hMenuEnt = NULL;
	}
	if (pExt->m_hMenuBmp) 
	{
		DeleteObject(pExt->m_hMenuBmp);
		pExt->m_hMenuBmp = NULL;
	}
	if (pExt->m_hMenuCfg)
	{
		DeleteObject(pExt->m_hMenuCfg);
		pExt->m_hMenuCfg = NULL;
	}

	return (0);
}

int Ntleas_QueryContextMenu(NtleasContextExtend* pext, 
	HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	UNREFERENCED_PARAMETER(idCmdLast);
	UNREFERENCED_PARAMETER(uFlags);
	// Use either InsertMenu or InsertMenuItem to add menu items.
	// Learn how to add sub-menu from:
	// http://www.codeproject.com/KB/shell/ctxextsubmenu.aspx
	HMENU hSubmenu = CreatePopupMenu();
	UINT uID = idCmdFirst;
	int menucount = GetMenuItemCount(hMenu); // and try append at the tail ?? 
	UNREFERENCED_PARAMETER(menucount);
	// build entry menu item : 
	{
		MENUITEMINFOW mii = { sizeof(mii) };
		mii.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_ID;
		mii.dwTypeData = TEXT("Open Config Dialog");
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED;
		mii.wID = uID++;
		mii.hbmpItem = static_cast<HBITMAP>(pext->m_hMenuCfg);
		if (!InsertMenuItem(hSubmenu, IDM_CONFIG, TRUE, &mii))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}
	{
		// Add a separator.
		MENUITEMINFOW sep = { sizeof(sep) };
		sep.fMask = MIIM_TYPE;
		sep.fType = MFT_SEPARATOR;
		if (!InsertMenuItem(hSubmenu, IDM_DIVIDE, TRUE, &sep))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}
//	WCHAR buf[64]; StringCchPrintfW(buf, ARRAYSIZE(buf), L"%p %d\n", pext->m_profileb, pext->m_profilen); OutputDebugStringW(buf);
	for (int i = 0; i < pext->m_profilen; ++i) {
		MENUITEMINFOW mii = { sizeof(mii) };
		mii.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_ID;
		mii.dwTypeData = (LPWSTR)pext->m_profiles[i].entrys[PROFILE_CONFIGNAME];
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED;
		mii.wID = uID++;
		mii.hbmpItem = static_cast<HBITMAP>(pext->m_hMenuEnt); // I believe that one day some one will use country-flags instead of the ball ;)
		InsertMenuItemW(hSubmenu, IDM_PREDEF + i, TRUE, &mii);
	}
	{
		MENUITEMINFOW mii = { sizeof(mii) };
		mii.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_SUBMENU;
		mii.fType = MFT_STRING;
		mii.dwTypeData = pext->m_pwszMenuText;
		mii.fState = MFS_ENABLED;
		mii.hbmpItem = static_cast<HBITMAP>(pext->m_hMenuBmp);
		mii.hSubMenu = hSubmenu;
		if (!InsertMenuItem(hMenu, indexMenu, TRUE, &mii))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}
	{
		// Add a separator.
		MENUITEMINFOW sep = { sizeof(sep) };
		sep.fMask = MIIM_TYPE;
		sep.fType = MFT_SEPARATOR;
		if (!InsertMenuItem(hMenu, indexMenu + 1, TRUE, &sep))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}

	// Return an HRESULT value with the severity set to SEVERITY_SUCCESS. 
	// Set the code value to the offset of the largest command identifier 
	// that was assigned, plus one (1).
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT((uID - idCmdFirst) + 1));
}

static BOOL InvokeProcessConfig(NtleasContextExtend* /*pext*/, LPCWSTR AppPath) {
	WCHAR szDirPath[MAX_PATH * 2], szExePath[MAX_PATH * 2];
	GetDirectoryFilePath(szDirPath, ARRAYSIZE(szDirPath), L"\\");
	StringCchPrintfW(szExePath, ARRAYSIZE(szExePath), L"\"%s%s\" \"%s\"", szDirPath, L"ntleasWin" SUFFIX L".exe", AppPath);

	PROCESS_INFORMATION proinfo = { NULL, NULL, };
	STARTUPINFOW stinfo = { sizeof(STARTUPINFOW), };
	BOOL ret = CreateProcessW(NULL, szExePath, NULL, NULL, FALSE, 0, NULL, szDirPath, &stinfo, &proinfo);
	CloseHandle(proinfo.hThread);
	CloseHandle(proinfo.hProcess);
	return ret;
}

static BOOL InvokeProcessProfile(NtleasContextExtend* /*pext*/, NtProfile const& profile, LPCWSTR AppPath) {
	WCHAR szExePath[MAX_PATH * 2], szAppPath[MAX_PATH * 2];
	auto fnPresent = [](LPCWSTR entry) { return lstrcmpiW(entry, L"") != 0; };

	DWORD dwflags = 0x0000;
	WCHAR params[256], *p = params;
	*p = L'\0';
	if (fnPresent(profile.entrys[PROFILE_CODEPAGECODE])) {
		p += (StringCchPrintfW(p, ARRAYSIZE(params) - (p - params), L" \"C%s\"", profile.entrys[PROFILE_CODEPAGECODE]), lstrlenW(p));
	}
	if (fnPresent(profile.entrys[PROFILE_LOCALECODE])) {
		p += (StringCchPrintfW(p, ARRAYSIZE(params) - (p - params), L" \"L%s\"", profile.entrys[PROFILE_LOCALECODE]), lstrlenW(p));
	}
	if (fnPresent(profile.entrys[PROFILE_TIMEZONEDESC])) { // search in registry !
		TIMEZONEINFO info; GetInfoFromTimezone(profile.entrys[PROFILE_TIMEZONEDESC], &info);
		p += (StringCchPrintfW(p, ARRAYSIZE(params) - (p - params), L" \"T%d\"", info.tzi), lstrlenW(p));
	}
	if (fnPresent(profile.entrys[PROFILE_FONT])) {
		p += (StringCchPrintfW(p, ARRAYSIZE(params) - (p - params), L" \"F%s\"", profile.entrys[PROFILE_FONT]), lstrlenW(p)); dwflags |= (1 << 2); // force flag set ... 
	}
	if (fnPresent(profile.entrys[PROFILE_ARGUMENT])) {
		p += (StringCchPrintfW(p, ARRAYSIZE(params) - (p - params), L" \"A%s\"", profile.entrys[PROFILE_ARGUMENT]), lstrlenW(p));
	}
	if (fnPresent(profile.entrys[PROFILE_COMPONENT])) {
		p += (StringCchPrintfW(p, ARRAYSIZE(params) - (p - params), L" \"M%c\"", profile.entrys[PROFILE_COMPONENT][5]), lstrlenW(p));
	}
	if (fnPresent(profile.entrys[PROFILE_DUMPERR])) {
		dwflags |= (1 << 15);
	}
	if (fnPresent(profile.entrys[PROFILE_SPECPATH])) {
		dwflags |= (1 << 0);
	}
	if (dwflags > 0) {
		p += (StringCchPrintfW(p, ARRAYSIZE(params) - (p - params), L" \"P%u\"", dwflags), lstrlenW(p));
	}
	// everything ok, now generate : 
//	MessageBoxW(NULL, params, L"DebugInfo", MB_OK);
	// generate arch : 
	DWORD arch = NtleasGetArch(AppPath);
	StringCchCopyW(szAppPath, ARRAYSIZE(szAppPath), AppPath);
	GetDirectoryFilePath(szExePath, ARRAYSIZE(szExePath), L"\\ntleasWin.exe");
	// run application : 
	return NtleasRunApp(arch, szExePath, szAppPath, params);
}

int Ntleas_InvokeCommand(NtleasContextExtend* pext, LPCMINVOKECOMMANDINFO pici, LPCWSTR AppPath) 
{
	BOOL fUnicode = FALSE;

	// Determine which structure is being passed in, CMINVOKECOMMANDINFO or 
	// CMINVOKECOMMANDINFOEX based on the cbSize member of lpcmi. Although 
	// the lpcmi parameter is declared in Shlobj.h as a CMINVOKECOMMANDINFO 
	// structure, in practice it often points to a CMINVOKECOMMANDINFOEX 
	// structure. This struct is an extended version of CMINVOKECOMMANDINFO 
	// and has additional members that allow Unicode strings to be passed.
	if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
	{
		if (pici->fMask & CMIC_MASK_UNICODE)
		{
			fUnicode = TRUE;
		}
	}

	// Determines whether the command is identified by its offset or verb.
	// There are two ways to identify commands:
	// 
	//   1) The command's verb string 
	//   2) The command's identifier offset
	// 
	// If the high-order word of lpcmi->lpVerb (for the ANSI case) or 
	// lpcmi->lpVerbW (for the Unicode case) is nonzero, lpVerb or lpVerbW 
	// holds a verb string. If the high-order word is zero, the command 
	// offset is in the low-order word of lpcmi->lpVerb.

	// For the ANSI case, if the high-order word is not zero, the command's 
	// verb string is in lpcmi->lpVerb. 
	if (!fUnicode && HIWORD(pici->lpVerb))
	{
		// Is the verb supported by this context menu extension?
		if (StrCmpIA(pici->lpVerb, pext->m_pszVerb) == 0)
		{
		//	OnVerbDisplayFileName(pici->hwnd);
		}
		else
		{
			// If the verb is not recognized by the context menu handler, it 
			// must return E_FAIL to allow it to be passed on to the other 
			// context menu handlers that might implement that verb.
			return E_FAIL;
		}
	}
	// For the Unicode case, if the high-order word is not zero, the 
	// command's verb string is in lpcmi->lpVerbW. 
	else if (fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW))
	{
		// Is the verb supported by this context menu extension?
		if (StrCmpIW(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW, pext->m_pwszVerb) == 0)
		{
		//	OnVerbDisplayFileName(pici->hwnd);
		}
		else
		{
			// If the verb is not recognized by the context menu handler, it 
			// must return E_FAIL to allow it to be passed on to the other 
			// context menu handlers that might implement that verb.
			return E_FAIL;
		}
	}
	// If the command cannot be identified through the verb string, then 
	// check the identifier offset.
	else
	{
		// Is the command identifier offset supported by this context menu 
		// extension?
		WORD ids = (LOWORD(pici->lpVerb));
	//	WCHAR buf[64]; StringCchPrintfW(buf, ARRAYSIZE(buf), L"%p %d - %d\n", pext->m_profileb, pext->m_profilen, ids); OutputDebugStringW(buf);
		if (ids == IDM_CONFIG) {
		//	MessageBoxA(pici->hwnd, "Open Config Exe", "info", MB_OK);
			InvokeProcessConfig(pext, AppPath);
		}
		else if (ids - IDM_DIVIDE >= 0 && ids - IDM_DIVIDE < pext->m_profilen) {
		//	MessageBoxA(pici->hwnd, "Open Predef With App", "info", MB_OK);
			InvokeProcessProfile(pext, pext->m_profiles[ids - IDM_DIVIDE], AppPath);
		}
		else {
			// If the verb is not recognized by the context menu handler, it 
			// must return E_FAIL to allow it to be passed on to the other 
			// context menu handlers that might implement that verb.
			return E_FAIL;
		}
	}

	return S_OK;
}

int Ntleas_GetCommandString(NtleasContextExtend* pext, UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
	UNREFERENCED_PARAMETER(pwReserved);

	HRESULT hr = E_INVALIDARG;

	if (idCommand == IDM_CONFIG)
	{
		switch (uFlags)
		{
		case GCS_HELPTEXTW:
			// Only useful for pre-Vista versions of Windows that have a 
			// Status bar.
			hr = StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, pext->m_pwszVerbHelpText);
			break;

		case GCS_VERBW:
			// GCS_VERBW is an optional feature that enables a caller to 
			// discover the canonical name for the verb passed in through 
			// idCommand.
			hr = StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, pext->m_pwszVerbCanonicalName);
			break;

		default:
			hr = S_OK;
		}
	}

	// If the command (idCommand) is not supported by this context menu 
	// extension handler, return E_INVALIDARG.

	return hr;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH: g_hInst = hModule; break;
	}
	return TRUE;
}

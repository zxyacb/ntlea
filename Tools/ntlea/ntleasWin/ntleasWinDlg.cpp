
#include "ntleasInc.h"
#include "resource.h"
#include "ntleasWinDlg.h"
#include "ntleasWinApp.h"

#include "../ntprofile/ntprofile.h"

#include <liblocale.h>
#include <shortcut.h>
#include <Shellapi.h>

// Definitions for the CPSPVSDialog class
CNtleasDialog::CNtleasDialog(UINT nResID, CWnd* pParent)
	: CDialog(nResID, pParent)
	, m_bkgbrush(RGB(233, 235, 254))
	, m_modiflag(FALSE)
{
}

CNtleasDialog::CNtleasDialog(LPCTSTR lpszResName, CWnd* pParent)
	: CDialog(lpszResName, pParent)
	, m_bkgbrush(RGB(233, 235, 254))
	, m_modiflag(FALSE)
{
}

CNtleasDialog::~CNtleasDialog()
{
	delete [] m_mapFonts;
	delete [] m_mapZones;
}

INT_PTR CNtleasDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CTLCOLORSTATIC: 
	{	// handle by myself, references: 
		// 1. http://hi.baidu.com/3_dream_3/item/10b934354b78db86c2cf29b0
		// 2. http://blog.csdn.net/morewindows/article/details/8470452
		// the background color is black when ctl setting readonly, this might be a bug see also: 
		// ==> http://connect.microsoft.com/VisualStudio/feedback/details/430916/wm-ctlcolorstatic-null-brush-windows-common-control-issue
		SetBkColor((HDC)(INT_PTR)wParam, RGB(233, 235, 254));
		SetTextColor((HDC)(INT_PTR)wParam, RGB(0x22, 0x22, 0x22)); // change text
	}	return (INT_PTR)(HBRUSH)m_bkgbrush/*::GetStockObject(NULL_BRUSH)*/;
	case WM_DROPFILES:
	{	// we only accept the first file ?? 
		WCHAR execfilename[MAX_PATH * 2];
		/*UINT namesize = */DragQueryFileW((HDROP)(DWORD_PTR)wParam, 0, execfilename, ARRAYSIZE(execfilename));
		ChangeExeFile(execfilename);
		DragFinish((HDROP)(DWORD_PTR)wParam);
	}	return (0);
	case WM_CLOSE:
		if (m_modiflag) {
			int ret = MessageBoxW(L"Discard all changes and exit?", L"Confirm", MB_YESNO);
			if (IDNO == ret) {
				return (TRUE);
			}
		}
		break;
	}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL CNtleasDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam))
	{
	case IDC_BUTTON_CREATE:
	{
		CreateNewProfile();
	}	return TRUE;
	case IDC_BUTTON_DELETE:
	{
		DeleteCurProfile();
	}	return TRUE;
	case IDC_BUTTON_SAVE:
	{
		SaveCurrentProfile();
	}	return TRUE;
	case IDC_COMBO_PROFILE:
	{
		if (HIWORD(wParam) == CBN_SELCHANGE) { LoadCurrentProfile(); }
	}	return TRUE;
	case IDC_COMBO_CODEPAGE:
	{
		// use recommand font : 
		if (HIWORD(wParam) == CBN_SELCHANGE) { ChangeMapFont(); }
	}	return TRUE;
	case IDC_BUTTON_BROWSE:
	{
		SelectExeFile();
	}	return TRUE;
	// ----------------------------------------------------------- 
	case IDC_BUTTON_SHORTCUT:
	{
		ToggleShortCut();
	}	return TRUE;
	case IDC_BUTTON_CONTEXT:
	{
		ToggleContextMenu();
	}	return TRUE;
	case IDC_BUTTON_OK:
	{
		SaveAllProfiles();
		BuildParamAndRun();
	}	return TRUE;
	case IDC_BUTTON_CANCEL:
	{
		if (SaveAllProfiles())
			EndDialog(0);
	}	return TRUE;
	} //switch (nID)

	return FALSE;
}

void CNtleasDialog::ToggleShortCut(void)
{
	WCHAR linkpath[MAX_PATH * 2];
	// 0. prepare check : 
	if (GetDlgApp().GetApplication() == NULL) return;

	// 1. select destination path 
	int ret = MessageBoxW(L"Put short cut link file on the desktop(YES) or\nSelect your own favor path (NO)?", L"Info", MB_YESNO);
	if (ret == IDYES) {
		if (!SHGetSpecialFolderPathW(HWND_DESKTOP, linkpath, CSIDL_DESKTOPDIRECTORY, FALSE)) {
			MessageBoxW(L"Failed Open Desktop", L"Error", MB_OK);
			ret = (-1);
		} else {
			ret = (0);
		}
	} else {
		BROWSEINFOW brsinfo = { 0 };
		brsinfo.hwndOwner = m_hWnd;
		brsinfo.lpszTitle = L"Select Folder for Creating ShortCut link file ..";
		brsinfo.pszDisplayName = NULL;
		brsinfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_VALIDATE;
		auto pidl = SHBrowseForFolderW(&brsinfo);
		if (pidl == NULL) {
			ret = 1; // user canceled ... 
		} else {
			ret = SHGetPathFromIDListW(pidl, linkpath) ? (0) : (-1);
		}
	}
	// 2. build lnk file at the given path 
	if (ret == 0) {
		WCHAR fullpath[MAX_PATH * 2], arguments[MAX_PATH * 3]; // 
		// get correspond app filebasename(no extension) 
		LPWSTR approot = _wcsdup(GetDlgApp().GetAppFullPath());
		LPWSTR apppath = _wcsdup(GetDlgApp().GetApplication());
		LPWSTR apppatx = _wcsdup(GetDlgApp().GetApplication());
		// divide info : 
		LPWSTR dirroot = wcsrchr(approot, L'\\');	if (dirroot) *dirroot++ = L'\0';
		LPWSTR appname = wcsrchr(apppath, L'\\');	if (appname) *appname++ = L'\0'; else appname = L"unnamed";
		LPWSTR appexts = wcsrchr(appname, L'.');	if (appexts) *appexts++ = L'\0'; 
		// build fullpath : 
		auto FindFileExists = [](LPCWSTR p) {
			WIN32_FIND_DATAW f; auto n = FindFirstFileW(p, &f); return(n == INVALID_HANDLE_VALUE) ? 0 : (FindClose(n), 1); };
		int n = wsprintfW(fullpath, L"\"%s\\%s\\%s", approot, GetDlgApp().GetArchiString(), GetDlgApp().GetNtleaString());
		if (!FindFileExists(fullpath+1)) // try to find ntleas with arch 
			n = wsprintfW(fullpath, L"\"%s\\%s", approot, GetDlgApp().GetNtleaString());
		if (!FindFileExists(fullpath+1)) { // try to find ntleas without arch 
			MessageBoxW(L"Cannot find ntleas.exe to setup shortcut", L"Error", MB_OK);
		} else {
			fullpath[n + 0] = L'\"'; // tag as tail !
			fullpath[n + 1] = L'\0';
			// build the extra parameters : 
			int argoff = wsprintfW(arguments, L"\"%s\" ", apppatx);
			// build the output linkpath : 
			int lnkoff = lstrlenW(linkpath); if (linkpath[lnkoff - 1] == L'\\') --lnkoff;
			linkpath[lnkoff + 0] = L'\\'; // tag as tail !
			linkpath[lnkoff + 1] = L'\0';
			wsprintfW(linkpath + lstrlenW(linkpath), L"%s.lnk", appname);
			ret = IDYES;
			if (FindFileExists(linkpath)) {
				ret = MessageBoxW(L"Find same shortcut name existing at the specified path, overwrite?", L"Info", MB_YESNO);
			}
			if (ret == IDYES) {
				// icon path : 
				WCHAR iconpath[MAX_PATH * 2]; 
				if (GetDlgApp().IsAppBinary()) { lstrcpyW(iconpath, apppatx); } else {
					WCHAR PEname[MAX_PATH]; // find executable !
					if ((DWORD_PTR)FindExecutableW(apppatx, NULL, PEname) > 32) {
						DWORD Exenamelen = ARRAYSIZE(iconpath);
						if (S_OK != AssocQueryStringW(ASSOCF_OPEN_BYEXENAME, ASSOCSTR_EXECUTABLE, PEname, NULL, iconpath, &Exenamelen))
							lstrcpyW(iconpath, PEname);
					}
				}
				// get aguments : 
				BuildParameters(arguments + argoff);
				// now create link : 
				HRESULT hr = CreateFileLink(fullpath, arguments, apppath, iconpath, linkpath, L"Created By Ntlea(s)");
				// check result : 
				if (FAILED(hr)) {
					MessageBoxW(L"Failed Create Shortcut On Desktop", L"Error", MB_OK);
				}
			}
		}
		// cleanup data : 
		free(apppatx);
		free(apppath);
		free(approot);
	}
}

void CNtleasDialog::ToggleContextMenu(void)
{
	HKEY hKeyResult;
	LSTATUS s = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"*\\shellex\\ContextMenuHandlers\\{9C31DD66-412C-4B28-BD17-1F0BEBE29E8B}", 
		0, KEY_READ, &hKeyResult);
	if (s == ERROR_SUCCESS) {
		RegCloseKey(hKeyResult);
		// now prompt to delete it !
		int ret = MessageBoxW(L"Are you sure to remove ntleas-helper from context menu?", L"Prompt", MB_YESNO);
		if (ret == IDYES) {
			HRESULT hr = GetDlgApp().UnregisterDll();
			if (FAILED(hr)) {
				MessageBoxW(L"Failed unregister DLL from context menu!", L"Error", MB_OK);
			} else if (hr == S_OK) {
				MessageBoxW(L"Unregister DLL from context menu Successfully.", L"Info", MB_OK);
			}
		}
	}
	else {
		// now prompt to create it !
		int ret = MessageBoxW(L"Are you sure to append ntleas-helper into context menu?", L"Prompt", MB_YESNO);
		if (ret == IDYES) {
			HRESULT hr = GetDlgApp().RegisterDll();
			if (FAILED(hr)) {
				MessageBoxW(L"Failed register DLL to context menu!", L"Error", MB_OK);
			} else if (hr == S_OK) {
				MessageBoxW(L"Register DLL to context menu Successfully.", L"Info", MB_OK);
			}
		}
	}
}

void CNtleasDialog::CreateNewProfile(bool inner)
{
	int newid = GetDlgApp().CreatePreConfig();
	if (newid >= 0) {
		auto defaultname = L"<Unnamed Profile>";
		int entry = m_cbProfile.AddString(defaultname);
		m_cbProfile.SetItemData(entry, newid);
		m_cbProfile.SetCurSel(entry);
		UpdateProfiles(const_cast<NtProfile*>(&GetDlgApp().GetConfig(newid)), PROFILE_CONFIGNAME, defaultname);
		// ====== 
		if (inner) { // internal create, only loadcurrent profilename while empty!
			auto defaultnameini = m_edProfileName.GetWindowTextW();
			if (defaultnameini == L"") {
				m_edProfileName.SetWindowTextW(defaultname);
			}
		} else 
			LoadCurrentProfile();
		// ====== 
		AddModifyTag();
	} else {
		MessageBoxW(L"Failed Create New Profile, Max is 128!", L"Prompt", MB_OK);
	}
}

void CNtleasDialog::ReplacingProfile(void)
{
	// change current profile name in profilename
	int entry = m_cbProfile.GetCurSel();
	int curid = m_cbProfile.GetItemData(entry);
	// The return value is a count of the strings remaining in the list.
	if (CB_ERR != m_cbProfile.DeleteString(entry)) {
		auto & profile = GetDlgApp().GetConfig(curid);
		entry = m_cbProfile.AddString(profile.entrys[PROFILE_CONFIGNAME]);
		m_cbProfile.SetItemData(entry, curid); 
		m_cbProfile.SetCurSel(entry);
		// ====== 
		// no needs reload current profile ... 
		AddModifyTag();
	}
}

void CNtleasDialog::DeleteCurProfile(void)
{
	int ret = MessageBoxW(L"Are you sure to Remove Current Profile?", L"Prompt", MB_YESNO);
	if (ret == IDYES) {
		int entry = m_cbProfile.GetCurSel();
		int newid = m_cbProfile.GetItemData(entry);
		GetDlgApp().DeletePreConfig(newid);
		int count = m_cbProfile.DeleteString(entry);
		if (count > 0) {
			m_cbProfile.SetCurSel(entry-1>=0 ? entry-1 : 0);
			// ====== 
			LoadCurrentProfile();
		} else {
			m_cbProfile.SetCurSel(0); // just force it repaint !!
			// ======
			ResetCurrentProfile();
		}
		AddModifyTag();
	}
}

void CNtleasDialog::ResetCurrentProfile(void)
{
	auto edText = [](CEdit const& ed) { ed.SetWindowTextW(L""); };
	auto cbText = [](CComboBox const& cb) { cb.SetCurSel(0); };
	auto ckText = [](CButton const& ck) { ck.SetCheck(BST_UNCHECKED); };

	edText(m_edProfileName);
	cbText(m_cbCodepage);
	cbText(m_cbLocale);
	cbText(m_cbTimezone);
	cbText(m_cbFonts);
	edText(m_edParam);
	cbText(m_cbComp);
	ckText(m_ckDumpErr);
	ckText(m_ckSpecpath);
}

void CNtleasDialog::SaveCurrentProfile(void)
{
	int curr = m_cbProfile.GetCurSel();
	if (curr < 0) {
		CreateNewProfile(true);
		curr = m_cbProfile.GetCurSel();
	}

	auto & profile = GetDlgApp().GetConfig(m_cbProfile.GetItemData(curr));
	auto edText = [](CEdit const& ed) { return ed.GetWindowTextW(); };
	auto cbText = [](CComboBox const& cb) { CString ret;cb.GetLBText(cb.GetCurSel(),ret.GetBuffer(64));ret.ReleaseBuffer();return ret; };
	auto ckText = [](CButton const& ck) { return (ck.GetCheck() == BST_CHECKED) ? L"True" : L""; };
	auto nuText = [](DWORD n, LPCWSTR p){ return n == (DWORD)-1 ? L"" : p; };
	auto guardx = [](DWORD n) { return n == (DWORD)-1 ? 0 : n; }; // this guard would choose safe boundary value ... 
	WCHAR number[16];

	if (CString(profile.entrys[PROFILE_CONFIGNAME]) != edText(m_edProfileName)) {
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_CONFIGNAME, edText(m_edProfileName));
		ReplacingProfile();
	}
	if (CString(profile.entrys[PROFILE_CODEPAGE]) != cbText(m_cbCodepage)) {
		auto i = m_cbCodepage.GetCurSel();
		auto n = m_cbCodepage.GetItemData(i);
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_CODEPAGE, cbText(m_cbCodepage));
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_CODEPAGECODE, nuText(n,_itow(m_mapFonts[guardx(n)].codepage, number, 10)));
		AddModifyTag();
	}
	if (CString(profile.entrys[PROFILE_LOCALE]) != cbText(m_cbLocale)) {
		auto i = m_cbLocale.GetCurSel();
		auto n = m_cbLocale.GetItemData(i);
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_LOCALE, cbText(m_cbLocale));
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_LOCALECODE, nuText(n,_itow(n, number, 10)));
		AddModifyTag();
	}
	if (CString(profile.entrys[PROFILE_TIMEZONE]) != cbText(m_cbTimezone)) {
		auto i = m_cbTimezone.GetCurSel();
		auto n = m_cbTimezone.GetItemData(i);
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_TIMEZONE, cbText(m_cbTimezone));
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_TIMEZONEDESC, nuText(n, m_mapZones[guardx(n)].zonename));
		AddModifyTag();
	}
	if (CString(profile.entrys[PROFILE_FONT]) != cbText(m_cbFonts)) {
		auto i = m_cbFonts.GetCurSel();
		auto n = m_cbFonts.GetItemData(i);
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_FONT, nuText(n,cbText(m_cbFonts)));
		AddModifyTag();
	}
	if (CString(profile.entrys[PROFILE_ARGUMENT]) != edText(m_edParam)) {
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_ARGUMENT, edText(m_edParam));
		AddModifyTag();
	}
	if (CString(profile.entrys[PROFILE_COMPONENT]) != cbText(m_cbComp)) {
		auto i = m_cbComp.GetCurSel();
		auto n = m_cbComp.GetItemData(i);
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_COMPONENT, nuText(n,cbText(m_cbComp)));
		AddModifyTag();
	}
	if (CString(profile.entrys[PROFILE_DUMPERR]) != ckText(m_ckDumpErr)) {
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_DUMPERR, ckText(m_ckDumpErr));
		AddModifyTag();
	}
	if (CString(profile.entrys[PROFILE_SPECPATH]) != ckText(m_ckSpecpath)) {
		UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_SPECPATH, ckText(m_ckSpecpath));
		AddModifyTag();
	}
}

void CNtleasDialog::LoadCurrentProfile(void)
{
	int curr = m_cbProfile.GetCurSel();
	if (curr < 0) return; // no current item ... 

	auto & profile = GetDlgApp().GetConfig(m_cbProfile.GetItemData(m_cbProfile.GetCurSel()));
	auto edText = [](CEdit const& ed, LPCWSTR str) { return ed.SetWindowTextW(str); };
	auto cbText = [](CComboBox const& cb, LPCWSTR str) { int n = cb.FindStringExact(0, str); cb.SetCurSel(n>0?n:0); return n; };
	auto ckText = [](CButton const& ck, LPCWSTR str) { ck.SetCheck(CString(str)==L"True"?BST_CHECKED:BST_UNCHECKED); };

	edText(m_edProfileName,	profile.entrys[PROFILE_CONFIGNAME]);
	cbText(m_cbCodepage,	profile.entrys[PROFILE_CODEPAGE]);
	cbText(m_cbLocale,		profile.entrys[PROFILE_LOCALE]);
	cbText(m_cbTimezone,	profile.entrys[PROFILE_TIMEZONE]);
	cbText(m_cbFonts,		profile.entrys[PROFILE_FONT]);
	edText(m_edParam,		profile.entrys[PROFILE_ARGUMENT]);
	cbText(m_cbComp,		profile.entrys[PROFILE_COMPONENT]);
	ckText(m_ckDumpErr,		profile.entrys[PROFILE_DUMPERR]);
	ckText(m_ckSpecpath,	profile.entrys[PROFILE_SPECPATH]);
}

// -------------------------------- 

void CNtleasDialog::AddModifyTag(void)
{
	m_modiflag = TRUE;
}

void CNtleasDialog::ClrModifyTag(void)
{
	m_modiflag = FALSE;
}

void CNtleasDialog::ChangeArchComponents(LPCWSTR archstr)
{
	WCHAR name[64]; m_cbComp.GetLBText(m_cbComp.GetCurSel(), name);
	m_cbComp.ResetContent(); // first cleanup 
	m_cbComp.SetItemData(m_cbComp.AddString(L"( System Default )"), (DWORD)-1); // set as the first 
	// ------------------ 
	CString archstring(archstr);
	if (archstring == L"x64") {
		m_cbComp.SetItemData(m_cbComp.AddString(L"ntleaj.dll"), L'j');
		m_cbComp.SetItemData(m_cbComp.AddString(L"ntleak.dll"), L'k');
	} else if (archstring == L"x86") {
		m_cbComp.SetItemData(m_cbComp.AddString(L"ntleah.dll"), L'h');
		m_cbComp.SetItemData(m_cbComp.AddString(L"ntleai.dll"), L'i');
		m_cbComp.SetItemData(m_cbComp.AddString(L"ntleaj.dll"), L'j');
		m_cbComp.SetItemData(m_cbComp.AddString(L"ntleak.dll"), L'k');
	}
	// ------------------ 
	int n = m_cbComp.FindStringExact(0, name);
	m_cbComp.SetCurSel( n > 0 ? n : 0 );
}

void CNtleasDialog::ChangeExeFile(LPCWSTR execfilename)
{
	// notify the dialog app : 
	GetDlgApp().SetApplication(execfilename);
	// get architecture string 
	ChangeArchComponents(GetDlgApp().GetArchiString());

	// activate the path info : 
	m_edAppPath.SetWindowTextW(execfilename);
	// activate the button to run : 
	m_btRunApp.EnableWindow(TRUE);
	m_btShortCut.EnableWindow(TRUE);
}

void CNtleasDialog::SelectExeFile(void)
{
	OPENFILENAMEW ofn = { sizeof(OPENFILENAMEW), };
	WCHAR exepath[MAX_PATH * 2]; exepath[0] = L'\0'; // REQUIRED!
	// Set lpstrFile[0] to '\0' so that GetOpenFileName won't use the contents of szFile to initialize itself.
	ofn.lpstrFilter = L"Executable File(*.exe)\0*.exe\0All files(*.*)\0*.*\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = exepath;
	ofn.nMaxFile = ARRAYSIZE(exepath);
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if (GetOpenFileNameW(&ofn)) ChangeExeFile(exepath);
}

void CNtleasDialog::ChangeMapFont(void)
{
	int cursel = m_cbCodepage.GetCurSel();
	if (cursel > 0) {
		int idmap = m_cbCodepage.GetItemData(cursel);
		// ---------- try and find recommended font --------------- //
		int index = m_cbFonts.FindString(0, m_mapFonts[idmap].fontname);
		m_cbFonts.SetCurSel(index > 0 ? index : 0);
		if (index < 0) {
			wchar_t buf[128]; wsprintfW(buf, L"Could not find font : %s\n", m_mapFonts[idmap].fontname);
			MessageBoxW(buf, L"Prompt", MB_OK);
		}
	}
}

void CNtleasDialog::BuildParameters(WCHAR output[/*256*/])
{
	DWORD dwflags = 0x0000;
	LPWSTR p = output; *p = L'\0';
	if (m_cbCodepage.GetCurSel() > 0) {
		int index = (int)m_cbCodepage.GetItemData(m_cbCodepage.GetCurSel());
		p += wsprintfW(p, L" \"C%u\"", m_mapFonts[index].codepage);
	}
	if (m_cbLocale.GetCurSel() > 0) {
		p += wsprintfW(p, L" \"L%u\"", m_cbLocale.GetItemData(m_cbLocale.GetCurSel()));
	}
	if (m_cbTimezone.GetCurSel() > 0) {
		int index = (int)m_cbTimezone.GetItemData(m_cbTimezone.GetCurSel());
		p += wsprintfW(p, L" \"T%d\"", m_mapZones[index].timezone);
	}
	if (m_cbFonts.GetCurSel() > 0) {
		WCHAR tmp[64]; m_cbFonts.GetLBText(m_cbFonts.GetCurSel(), tmp);
		p += wsprintfW(p, L" \"F%s\"", tmp); dwflags |= (1 << 2); // force flag set ... 
	}
	if (m_edParam.GetWindowTextLengthW() > 0) {
		p += wsprintfW(p, L" \"A%s\"", m_edParam.GetWindowTextW().c_str());
	}
	if (m_cbComp.GetCurSel() > 0) {
		p += wsprintfW(p, L" \"M%c\"", m_cbComp.GetItemData(m_cbComp.GetCurSel()));
	}
	if (m_ckDumpErr.GetCheck() == BST_CHECKED) {
		dwflags |= (1 << 15);
	}
	if (m_ckSpecpath.GetCheck() == BST_CHECKED) {
		dwflags |= (1 << 0);
	}
	if (dwflags > 0) {
		p += wsprintfW(p, L" \"P%u\"", dwflags);
	}
}

void CNtleasDialog::BuildParamAndRun(void)
{
	WCHAR params[256]; BuildParameters(params);
	
	// everything ok, now generate : 
//	MessageBoxW(params, L"DebugInfo", MB_OK);
	if (!GetDlgApp().RunApplication(params)) {
		MessageBoxW(L"Failed Create Process!\nDoes All Ntleas Components Exist?", L"Error", MB_OK);
	}
	else if (!(m_ckNoExit.GetCheck() == BST_CHECKED)) {
		EndDialog(0); // succeeded ... 
	}
}

// -------------------------------- 

//void CPSPVSDialog::PreRegisterClass(WNDCLASS& wc)
//{
//	CDialog::PreRegisterClass(wc);
//	wc.hbrBackground = CreateSolidBrush(RGB(0,235,254));
//}

BOOL CNtleasDialog::OnEraseBkgnd(CDC* pDC)
{
	pDC->FillRect(GetClientRect(), &m_bkgbrush);
	return TRUE;
}

BOOL CNtleasDialog::OnInitDialog()
{
	// Set the Icon
	SetIconLarge(IDI_ICON1);
	SetIconSmall(IDI_ICON1);

	// support drag drop : 
	// see : http://blog.sina.com.cn/s/blog_6294abe70101bko6.html
	do {
		typedef BOOL (WINAPI *fnChangeWindowMessageFilter)(UINT, DWORD);
		fnChangeWindowMessageFilter _ChangeWindowMessageFilter = NULL;
		// only vista or later support this function : 
		HMODULE huser32 = LoadLibraryW(L"user32.dll");
		if (huser32) _ChangeWindowMessageFilter = (fnChangeWindowMessageFilter)GetProcAddress(huser32, "ChangeWindowMessageFilter");
		// xp system do not need this setting : 
		if (_ChangeWindowMessageFilter) {
#define WM_COPYGLOBALDATA 0x0049
			_ChangeWindowMessageFilter(WM_COPYGLOBALDATA, MSGFLT_ADD);
			_ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
#undef  WM_COPYGLOBALDATA
		}
	} while (this != this/*false*/);
	DragAcceptFiles(GetHwnd(), TRUE);

	//change state : 
//	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
//	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	AttachItem(IDC_COMBO_PROFILE, m_cbProfile);
	AttachItem(IDC_COMBO_CODEPAGE, m_cbCodepage);
	AttachItem(IDC_COMBO_LOCALE, m_cbLocale);
	AttachItem(IDC_COMBO_TIMEZONE, m_cbTimezone);
	AttachItem(IDC_COMBO_FONTS, m_cbFonts);
	AttachItem(IDC_COMBO_COMP, m_cbComp);
	AttachItem(IDC_EDIT_PARAM, m_edParam);

	AttachItem(IDC_EDIT_NAME, m_edProfileName);
	AttachItem(IDC_CHECKBOX_DUMPERR, m_ckDumpErr);
	AttachItem(IDC_CHECKBOX_SPECPATH, m_ckSpecpath);
	AttachItem(IDC_CHECKBOX_NOEXIT, m_ckNoExit);
	AttachItem(IDC_EDIT_PATH, m_edAppPath);
	AttachItem(IDC_BUTTON_OK, m_btRunApp);
	AttachItem(IDC_BUTTON_SHORTCUT, m_btShortCut);

	m_cbCodepage.SetItemData(m_cbCodepage.AddString(L"( System Default )"), (DWORD)-1);
	EnumerateCodePage([](UINT n, PMIMECPINFO mimeinfos, LPVOID p){ // we know codepage is limited once !
		CNtleasDialog* pThis = (CNtleasDialog*)p;
		pThis->m_mapFonts = new FontNameBuf[n];
		for (UINT i = 0; i < n; ++i) {
			pThis->m_cbCodepage.SetItemData(
				pThis->m_cbCodepage.AddString(mimeinfos[i].wszDescription), i);
			pThis->m_mapFonts[i].codepage = mimeinfos[i].uiCodePage;
			lstrcpyW(pThis->m_mapFonts[i].fontname, mimeinfos[i].wszProportionalFont);
		}
	}, this);
	m_cbCodepage.SetCurSel(0); // system default

	m_cbLocale.SetItemData(m_cbLocale.AddString(L"( System Default )"), (DWORD)-1);
	EnumerateLocale([](UINT n, PRFC1766INFO localeinfos, LPVOID p){
		CNtleasDialog* pThis = (CNtleasDialog*)p;
		for (UINT i = 0; i < n; ++i) {
			pThis->m_cbLocale.SetItemData(
				pThis->m_cbLocale.AddString(localeinfos[i].wszLocaleName), localeinfos[i].lcid);
		}
	}, this);
	m_cbLocale.SetCurSel(0); // system default

	m_mapZones = new ZoneNameBuf[GetInfoCountTimezone()];
	m_mapZonen = 0;
	m_cbTimezone.SetItemData(m_cbTimezone.AddString(L"( System Default )"), (DWORD)-1);
	EnumerateTimezone([](UINT n, PTIMEZONEINFO timezoneinfos, LPVOID p){
		CNtleasDialog* pThis = (CNtleasDialog*)p;
		for (UINT i = 0; i < n; ++i) {
			pThis->m_cbTimezone.SetItemData(
				pThis->m_cbTimezone.AddString(timezoneinfos[i].display), pThis->m_mapZonen);
			pThis->m_mapZones[pThis->m_mapZonen].timezone = timezoneinfos[i].tzi;
			lstrcpyW(pThis->m_mapZones[pThis->m_mapZonen].zonename, timezoneinfos[i].key);
			pThis->m_mapZonen++;
		}
	}, this);
	m_cbTimezone.SetCurSel(0); // system default

	m_cbFonts.SetItemData(m_cbFonts.AddString(L"( System Default )"), (DWORD)-1);
	EnumFontFamiliesExW(GetDC()->GetHDC(), NULL, (FONTENUMPROCW)[](LOGFONTW const*plf, TEXTMETRIC const*, DWORD, LPARAM lParam){
		CNtleasDialog* pThis = (CNtleasDialog*)(DWORD_PTR)lParam;
		if (pThis->m_cbFonts.FindString(0, plf->lfFaceName) < 0) {
			pThis->m_cbFonts.SetItemData(
				pThis->m_cbFonts.AddString(plf->lfFaceName), 1);
		}
		return TRUE; // continue search ... 
	}, (LPARAM)(DWORD_PTR)this, 0/*dwFlags*/);
	m_cbFonts.SetCurSel(0); // system default

	m_edAppPath.SetReadOnly(TRUE);
//	m_edAppPath.EnableWindow(FALSE);

	if (GetDlgApp().HasApplication()) {
		ChangeArchComponents(GetDlgApp().GetArchiString());
		m_edAppPath.SetWindowTextW(GetDlgApp().GetApplication());
	} else { // hide the button 
		ChangeArchComponents(L"");
		m_btRunApp.EnableWindow(FALSE); m_btShortCut.EnableWindow(FALSE);
	}

	int ret = GetDlgApp().LoadPreConfig();
	if (ret == 0) {
		MessageBoxW(L"Config File is Missing or Empty", L"Error", MB_OK);
	}
	else if (ret < 0) {
		MessageBoxW(L"Config File is invalid maybe corrupted", L"Error", MB_OK);
	} 
	else {
		int n = GetDlgApp().GetConfigCount();
		for (int i = 0; i < n; ++i) { // bind info !
			NtProfile const& profile = GetDlgApp().GetConfig(i);
			// ---------------------------------------------------------------------------------------------------
			// check profile code-string with profile readable-string: 
			MIMECPINFO cpinfo; GetInfoFromCodePage(_wtoi(profile.entrys[PROFILE_CODEPAGECODE]), &cpinfo);
			if (profile.entrys[PROFILE_CODEPAGECODE][0] && CString(cpinfo.wszDescription) != CString(profile.entrys[PROFILE_CODEPAGE])) {
				UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_CODEPAGE, cpinfo.wszDescription);
			}
			RFC1766INFO lcinfo; GetInfoFromLocale(_wtoi(profile.entrys[PROFILE_LOCALECODE]), &lcinfo);
			if (profile.entrys[PROFILE_LOCALECODE][0] && CString(lcinfo.wszLocaleName) != CString(profile.entrys[PROFILE_LOCALE])) {
				UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_LOCALE, lcinfo.wszLocaleName);
			}
			TIMEZONEINFO tzinfo; GetInfoFromTimezone(profile.entrys[PROFILE_TIMEZONEDESC], &tzinfo);
			if (profile.entrys[PROFILE_TIMEZONEDESC][0] && CString(tzinfo.display) != CString(profile.entrys[PROFILE_TIMEZONE])) {
				UpdateProfiles(const_cast<NtProfile*>(&profile), PROFILE_TIMEZONE, tzinfo.display);
			}
			// ---------------------------------------------------------------------------------------------------
			// add profile string into buffer : 
			m_cbProfile.SetItemData(
				m_cbProfile.AddString(profile.entrys[PROFILE_CONFIGNAME]), i);
		}
		m_cbProfile.SetCurSel(0); LoadCurrentProfile();
	}

	return TRUE;
}

BOOL CNtleasDialog::SaveAllProfiles(void) {
	// i think many users may use save & run without apply, 
	// so always trigger it first !
	if (m_cbProfile.GetCount() > 0)
		SaveCurrentProfile();
	// ---------------- 
	if (m_modiflag) {
		int ret = GetDlgApp().SavePreConfig();
		if (ret < 0) {
			MessageBoxW(L"Failed Save Cfg File!", L"Error", MB_OK);
			return FALSE;
		} else {
			ClrModifyTag();
		}
	}
	return TRUE;
}


#include "ntleasInc.h"
#include "resource.h"
#include "ntleasWinApp.h"
#include "ntleasRunApp.h"

#include "../ntprofile/ntprofile.h"

#include <ShellAPI.h>

// Definitions for the CDialogApp class
CNtleasWinApp::CNtleasWinApp() 
	: m_dlg(IDD_DIALOG1)
	, m_app(NULL)
{
	m_argv = CommandLineToArgvW(GetCommandLineW(), &m_argc);
}

CNtleasWinApp::~CNtleasWinApp()
{
	LocalFree(m_argv);
}

BOOL CNtleasWinApp::InitInstance()
{
	m_arch = m_argc > 1 ? NtleasGetArch(m_argv[1], &m_binary) : IMAGE_FILE_MACHINE_UNKNOWN;
	// -------------------------- 
	m_dlg.DoModal();
	// -------------------------- 

	// End the program
	::PostQuitMessage(0);

	return TRUE;
}

void CNtleasWinApp::ExitInstance()
{
	if (m_app) LocalFree((LPVOID)m_app);
	if (m_profiles) delete[] m_profiles;
	if (m_profileb) LocalFree(m_profileb);
}

class ConfigNameChanger {
	LPWSTR approot; // map config file name : 
public:
	ConfigNameChanger(LPWSTR path) : approot(path)	{ lstrcpyW(approot + lstrlenW(approot) - 3, L"cfg"); }
	~ConfigNameChanger(void)						{ lstrcpyW(approot + lstrlenW(approot) - 3, L"exe"); }
	operator LPCWSTR(void) const { return approot; }
};

int CNtleasWinApp::LoadPreConfig(void)
{
	m_profiles = new NtProfile[MAX_PROFILE];
	m_profilen = 0;
//	FILE* fp = _wfopen(ConfigNameChanger(m_argv[0]), L"rb");
	CFile file;
	if (file.Open(ConfigNameChanger(m_argv[0]), OPEN_EXISTING)) {
		// load all data in : 
		SIZE_T length = (SIZE_T)file.GetLength();
		m_profileb = LocalAlloc(0, length);
		file.Read(m_profileb, (UINT)length); file.Close();
		// parse data : 
		return InitNtProfiles(m_profileb, length, [](NtProfile* profile, void* ctx){
			CNtleasWinApp* pThis = (CNtleasWinApp*)ctx;
			pThis->m_profiles[pThis->m_profilen++] = *profile;
			return int(pThis->m_profilen <= MAX_PROFILE);
		}, this);
	} else {
		return (0);
	}
}

int CNtleasWinApp::SavePreConfig(void)
{
//	FILE* fp = _wfopen(ConfigNameChanger(m_argv[0]), L"wb");
	CFile file;
	if (file.Open(ConfigNameChanger(m_argv[0]), CREATE_ALWAYS)) {
		return FreeNtProfiles(m_profiles, m_profilen, [](wchar_t const* buf, void* ctx){
			CFile& file = *(CFile*)ctx;
			file.Write(buf, sizeof(wchar_t) * (lstrlenW(buf) + 1));
			return (1); // always continue !
		}, &file);
	} else {
		return (-1);
	}
}

int CNtleasWinApp::CreatePreConfig(void)
{
	if (m_profilen >= MAX_PROFILE) return -1; // max 
	for (int i = 0; i < m_profilen; ++i) {
		if (IsDeletedProfile(&m_profiles[i])) {
			return CreateProfiles(&m_profiles[i]), i; // use profile deleted !
		}
	}
	int newid = m_profilen++;
	return CreateProfiles(&m_profiles[newid]), newid;
}

int CNtleasWinApp::DeletePreConfig(int index)
{
	if (0 > index || index >= m_profilen) return -1; // overflow
	return DeleteProfiles(&m_profiles[index], TRUE, FALSE);
}

NtProfile const& CNtleasWinApp::GetConfig(int index) const
{
	return m_profiles[index < MAX_PROFILE ? index : (0)];
}

VOID CNtleasWinApp::SetApplication(LPCWSTR appname)
{
	if (m_app) LocalFree((LPVOID)m_app);
	auto ptr = LocalAlloc(0, sizeof(WCHAR) * (lstrlenW(appname) + 1));
	lstrcpyW((LPWSTR)ptr, appname);
	m_app = (LPWSTR)ptr;
	m_arch = NtleasGetArch(m_app, &m_binary);
}

BOOL CNtleasWinApp::RunApplication(LPCWSTR parameter)
{
	LPWSTR approot = m_argv[0];
	LPWSTR appname = m_app ? m_app : m_argv[1];
	return NtleasRunApp(m_arch, approot, appname, parameter);
}

LPCWSTR CNtleasWinApp::GetArchiString(void) const
{
	if (m_arch == IMAGE_FILE_MACHINE_AMD64) return L"x64";
	else if (m_arch == IMAGE_FILE_MACHINE_I386) return L"x86";
	else return L"unk";
}

LPCWSTR CNtleasWinApp::GetNtleaString(void) const 
{
#if defined(DEBUG) || defined(_DEBUG)
	return L"ntleas_D.exe";
#else
	return L"ntleas.exe";
#endif
}

class ContextNameChanger {
	LPWSTR approot; // map config file name : 
public:
	ContextNameChanger(LPWSTR path) : approot(path)	{ lstrcpyW(approot + lstrlenW(approot) - 7, L"Ctx.dll"); }
	~ContextNameChanger(void)						{ lstrcpyW(approot + lstrlenW(approot) - 7, L"Win.exe"); }
	operator LPCWSTR(void) const { return approot; }
};

// http://stackoverflow.com/questions/11586139/how-to-run-application-which-requires-admin-rights-from-one-that-doesnt-have-th

HRESULT CNtleasWinApp::RegisterDll(void)
{
	HMODULE hmod = LoadLibraryW(ContextNameChanger(m_argv[0]));
	if (hmod) {
		typedef HRESULT(__stdcall*fnDllRegisterServer)(void);
		HRESULT hr = ((fnDllRegisterServer)GetProcAddress(hmod, "DllRegisterServer"))();
		FreeLibrary(hmod);
		// -------------------- 
		if (FAILED(hr)) {
		//	char buf[64]; wsprintfA(buf, __FUNCTION__": hr=0x%08X", hr); MessageBoxA(NULL, buf, "err", MB_OK);
			ShellExecuteW(NULL, L"runas", L"regsvr32", ContextNameChanger(m_argv[0]), NULL, SW_SHOWNORMAL);
			return S_FALSE;
		}
		return hr;
	}
	return E_NOTIMPL;
}

HRESULT CNtleasWinApp::UnregisterDll(void)
{
	HMODULE hmod = LoadLibraryW(ContextNameChanger(m_argv[0]));
	if (hmod) {
		typedef HRESULT(__stdcall*fnDllRegisterServer)(void);
		HRESULT hr = ((fnDllRegisterServer)GetProcAddress(hmod, "DllUnregisterServer"))();
		FreeLibrary(hmod);
		// -------------------- 
		if (FAILED(hr)) {
		//	char buf[64]; wsprintfA(buf, __FUNCTION__": hr=0x%08X", hr); MessageBoxA(NULL, buf, "err", MB_OK);
			ShellExecuteW(NULL, L"runas", L"regsvr32", CString("-u ")+ContextNameChanger(m_argv[0]), NULL, SW_SHOWNORMAL);
			return S_FALSE;
		}
		return hr;
	}
	return E_NOTIMPL;
}

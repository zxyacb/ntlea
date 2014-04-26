
#ifndef __NTLEASWINAPP_H__
#define __NTLEASWINAPP_H__

#include "ntleasWinDlg.h"

// Declaration of the CDialogApp class
class CNtleasWinApp : public CWinApp
{
	enum { MAX_PROFILE = 128, };
public:
	CNtleasWinApp();
	virtual ~CNtleasWinApp();

	BOOL RunApplication(LPCWSTR parameter);
	VOID SetApplication(LPCWSTR appname);
	BOOL HasApplication(void) { return m_app || m_argc > 1; }
	LPCWSTR GetApplication(void) const { return m_app ? m_app : (m_argc > 1 ? m_argv[1] : NULL); }
	LPCWSTR GetAppFullPath(void) const { return m_argc > 0 ? m_argv[0] : NULL; }
	LPCWSTR GetArchiString(void) const;
	LPCWSTR GetNtleaString(void) const;
	BOOL IsAppBinary(void) const { return m_binary; }

	int LoadPreConfig(void);
	int SavePreConfig(void);
	int CreatePreConfig(void); // <-- index
	int DeletePreConfig(int index);

	NtProfile const& GetConfig(int index) const;
	int GetConfigCount(void) const { return m_profilen; }

	HRESULT RegisterDll(void);
	HRESULT UnregisterDll(void);

protected:
	virtual BOOL InitInstance();
	virtual void ExitInstance();

protected:
	CNtleasDialog	m_dlg;
	LPWSTR			m_app;	// runtime appname assigned !
	// app param : 
	LPWSTR *		m_argv;
	int				m_argc;
	DWORD			m_arch; // IMAGE_FILE_MACHINE_AMD64 or IMAGE_FILE_MACHINE_I386
	int				m_binary; // 
	// 
	NtProfile*		m_profiles;
	int				m_profilen;
	void*			m_profileb;
};

// returns a reference to the CPSPVSWinApp object
inline CNtleasWinApp& GetDlgApp() { return *(static_cast<CNtleasWinApp*>(GetApp())); }

#endif // __NTLEASWINAPP_H__

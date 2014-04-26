
#ifndef __NTLEASWINDLG_H__
#define __NTLEASWINDLG_H__

// Declaration of the CMyDialog class
struct NtProfile;
class CNtleasDialog : public CDialog
{
	typedef struct { UINT codepage; WCHAR fontname[32]; } FontNameBuf;
	typedef struct { UINT timezone; WCHAR zonename[64]; } ZoneNameBuf;
public:
	CNtleasDialog(UINT nResID, CWnd* pParent = NULL);
	CNtleasDialog(LPCTSTR lpszResName, CWnd* pParent = NULL);
	virtual ~CNtleasDialog();

protected:
	//	virtual void PreRegisterClass(WNDCLASS& wc);
	virtual BOOL OnEraseBkgnd(CDC* pDC);

protected:
	virtual BOOL OnInitDialog();
	virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	void ToggleShortCut(void);
	void ToggleContextMenu(void);
	void BuildParamAndRun(void);
	void BuildParameters(WCHAR output[/*256*/]);
	void ChangeMapFont(void);
	void SelectExeFile(void);
	void ChangeExeFile(LPCWSTR exepath);
	void ChangeArchComponents(LPCWSTR arch);

	void AddModifyTag(void);
	void ClrModifyTag(void);
	BOOL SaveAllProfiles(void);

	void LoadCurrentProfile(void);
	void SaveCurrentProfile(void);
	void CreateNewProfile(bool inner=false);
	void ReplacingProfile(void);
	void DeleteCurProfile(void);
	void ResetCurrentProfile(void);

private:
	CBrush			m_bkgbrush;
	CEdit			m_edProfileName, m_edParam, m_edAppPath;
	CButton			m_ckDumpErr, m_ckSpecpath, m_ckNoExit, m_btRunApp, m_btShortCut;
	CComboBox		m_cbProfile, m_cbCodepage, m_cbLocale, m_cbTimezone, m_cbFonts, m_cbComp;
	FontNameBuf*	m_mapFonts;
	ZoneNameBuf*	m_mapZones; int m_mapZonen;
	BOOL			m_modiflag;
};

#endif // __NTLEASWINDLG_H__

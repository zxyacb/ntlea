
#ifndef __NTLEASCTX_H__
#define __NTLEASCTX_H__

#include <windows.h>
#include <shlobj.h>     // For IShellExtInit and IContextMenu

#define DynamicLoadModule
#include "../ntleasCtxExt/ntleasCtxExt.h"
#undef DynamicLoadModule

class NtleasContextMenuExt : public IShellExtInit, public IContextMenu
{
public:
	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IShellExtInit
	IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

	// IContextMenu
	IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
	IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
	IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);

	NtleasContextMenuExt(void);

protected:
	~NtleasContextMenuExt(void);

private:
	void LoadComponent(void);
	void UnloadComponent(void);
	void ReloadComponent(void) { UnloadComponent(); LoadComponent(); }

private:
	// Reference count of component.
	long m_cRef;

	// The name of the selected file.
	wchar_t m_szSelectedFile[MAX_PATH * 2];

	// The path of dll file.
	wchar_t m_szDllPath[MAX_PATH * 2];

	// the handle : 
	NtleasContextExtend* m_pExt;
};

#endif // __NTLEASCTX_H__

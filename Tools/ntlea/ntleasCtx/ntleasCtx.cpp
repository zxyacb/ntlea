
#include "ntleasCtx.h"
#include <Shlwapi.h>

extern long g_cDllRef;
extern HMODULE g_hInst;
static HMODULE g_hmodule = NULL;

NtleasContextMenuExt::NtleasContextMenuExt(void) 
	: m_cRef(1)
	, m_pExt(NULL)
{
	GetModuleFileNameW(g_hInst, m_szDllPath, ARRAYSIZE(m_szDllPath));
//	if (ERROR_INSUFFICIENT_BUFFER == GetLastError()) 
	wchar_t * p = wcsrchr(m_szDllPath, L'\\'); if (p) *p = L'\0';
//	SetDllDirectoryW(m_szDllPath);
	lstrcatW(m_szDllPath, L"\\ntleasCtxExt"  L".dll");
//	OutputDebugStringW(m_szDllPath);

	InterlockedIncrement(&g_cDllRef);
	LoadComponent();
}

NtleasContextMenuExt::~NtleasContextMenuExt(void)
{
	UnloadComponent();
	InterlockedDecrement(&g_cDllRef);
}

void NtleasContextMenuExt::LoadComponent(void)
{
	if (!g_hmodule) {
		g_hmodule = (HMODULE)Ntleas_DllCtl(-1, LoadLibraryW(m_szDllPath));
	}
	if (g_hmodule && !m_pExt) {
		m_pExt = (NtleasContextExtend*)HeapAlloc(GetProcessHeap(), 0, Ntleas_SizeCtxExt());
		Ntleas_InitCtxExt(m_pExt);
	}
}

void NtleasContextMenuExt::UnloadComponent(void)
{
	if (g_hmodule && m_pExt) {
		Ntleas_FreeCtxExt(m_pExt);
		HeapFree(GetProcessHeap(), 0, m_pExt);
	}
	if (g_hmodule) {
		FreeLibrary(g_hmodule); g_hmodule = NULL;
	}
}

#pragma region IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP NtleasContextMenuExt::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(NtleasContextMenuExt, IContextMenu),
		QITABENT(NtleasContextMenuExt, IShellExtInit),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) NtleasContextMenuExt::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) NtleasContextMenuExt::Release()
{
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if (0 == cRef)
	{
		delete this;
	}

	return cRef;
}

#pragma endregion


#pragma region IShellExtInit

// Initialize the context menu handler.
IFACEMETHODIMP NtleasContextMenuExt::Initialize(
	LPCITEMIDLIST /*pidlFolder*/, LPDATAOBJECT pDataObj, HKEY /*hKeyProgID*/)
{
	if (NULL == pDataObj)
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stm;

	// The pDataObj pointer contains the objects being acted upon. In this 
	// example, we get an HDROP handle for enumerating the selected files and 
	// folders.
	if (SUCCEEDED(pDataObj->GetData(&fe, &stm)))
	{
		// Get an HDROP handle.
		HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
		if (hDrop != NULL)
		{
			// Determine how many files are involved in this operation. This 
			// code sample displays the custom context menu item when only 
			// one file is selected. 
			UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			if (nFiles == 1)
			{
				// Get the path of the file.
				if (0 != DragQueryFile(hDrop, 0, m_szSelectedFile,
					ARRAYSIZE(m_szSelectedFile)))
				{
					hr = S_OK;
				}
			}

			GlobalUnlock(stm.hGlobal);
		}

		ReleaseStgMedium(&stm);
	}

	// If any value other than S_OK is returned from the method, the context 
	// menu item is not displayed.
	return hr;
}

#pragma endregion


#pragma region IContextMenu

//
//   FUNCTION: NtleasContextMenuExt::QueryContextMenu
//
//   PURPOSE: The Shell calls IContextMenu::QueryContextMenu to allow the 
//            context menu handler to add its menu items to the menu. It 
//            passes in the HMENU handle in the hmenu parameter. The 
//            indexMenu parameter is set to the index to be used for the 
//            first menu item that is to be added.
//
IFACEMETHODIMP NtleasContextMenuExt::QueryContextMenu(
	HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	// If uFlags include CMF_DEFAULTONLY then we should not do anything.
	if (CMF_DEFAULTONLY & uFlags)
	{
		return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
	}

	return g_hmodule ? 
		Ntleas_QueryContextMenu(m_pExt, hMenu, indexMenu, idCmdFirst, idCmdLast, uFlags):
		MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
}


//
//   FUNCTION: NtleasContextMenuExt::InvokeCommand
//
//   PURPOSE: This method is called when a user clicks a menu item to tell 
//            the handler to run the associated command. The lpcmi parameter 
//            points to a structure that contains the needed information.
//
IFACEMETHODIMP NtleasContextMenuExt::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
	return Ntleas_InvokeCommand(m_pExt, pici, m_szSelectedFile);
}

//
//   FUNCTION: CNtleasContextMenuExt::GetCommandString
//
//   PURPOSE: If a user highlights one of the items added by a context menu 
//            handler, the handler's IContextMenu::GetCommandString method is 
//            called to request a Help text string that will be displayed on 
//            the Windows Explorer status bar. This method can also be called 
//            to request the verb string that is assigned to a command. 
//            Either ANSI or Unicode verb strings can be requested. This 
//            example only implements support for the Unicode values of 
//            uFlags, because only those have been used in Windows Explorer 
//            since Windows 2000.
//
IFACEMETHODIMP NtleasContextMenuExt::GetCommandString(UINT_PTR idCommand,
	UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
	return Ntleas_GetCommandString(m_pExt, idCommand, uFlags, pwReserved, pszName, cchMax);
}

#pragma endregion

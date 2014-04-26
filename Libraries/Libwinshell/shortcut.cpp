
#include "shortcut.h"

#include <WinNls.h>
#include <ShlObj.h>
#include <ObjBase.h>
#include <ObjIdl.h>
#include <ShlGuid.h>

HRESULT CreateFileLink(LPCWSTR lpszPathObj, LPCWSTR lpszArgsObj, LPCWSTR lpszDirObj, 
	LPCWSTR lpszIconPath, LPCWSTR lpszPathLink, LPCWSTR lpszDesc)
{
	HRESULT state = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	__try {
		HRESULT hres;
		IShellLinkW* psl;

		// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
		// has already been called.
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
		if (SUCCEEDED(hres)) {
			IPersistFile* ppf;

			// Set the path to the shortcut target and add the description. 
			psl->SetPath(lpszPathObj);
			psl->SetIconLocation(lpszIconPath ? lpszIconPath : lpszPathObj, 0);
			if (lpszArgsObj) psl->SetArguments(lpszArgsObj);
			if (lpszDirObj) psl->SetWorkingDirectory(lpszDirObj);
			if (lpszDesc) psl->SetDescription(lpszDesc);

			// Query IShellLink for the IPersistFile interface, used for saving the 
			// shortcut in persistent storage. 
			hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

			if (SUCCEEDED(hres)) {
				// Save the link by calling IPersistFile::Save. 
				hres = ppf->Save(lpszPathLink, TRUE);
				ppf->Release();
			}
			psl->Release();
		}

		return hres;
	}
	__finally {
		if (state == S_OK) CoUninitialize(); // if init locally, we uninit COM ... 
	}
}

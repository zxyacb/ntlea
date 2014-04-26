
#include "liblocale.h"
#include <assert.h>

int EnumerateCodePage(void(*fnmimecallback)(UINT, PMIMECPINFO, LPVOID), LPVOID context, MIMECONTF mimefilter)
{
	HRESULT state = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	__try {
		IMultiLanguage * pMultiLanguage;
		HRESULT hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (void**)&pMultiLanguage);
		if (hr == S_OK) {
			IEnumCodePage *pEnumCodePage; 
			// if filter required see : http://msdn.microsoft.com/en-us/library/ie/aa741210(v=vs.85).aspx
			hr = pMultiLanguage->EnumCodePages(mimefilter, &pEnumCodePage);
			if (hr == S_OK) {
				UINT cnum = 0;
				PMIMECPINFO pcpInfo; ULONG ccpInfo;
				// ------------------ 
				hr = pMultiLanguage->GetNumberOfCodePageInfo(&cnum);
				if (SUCCEEDED(hr)) {
					pcpInfo = (PMIMECPINFO)CoTaskMemAlloc(sizeof(MIMECPINFO)*cnum);
					hr = pEnumCodePage->Next(cnum, pcpInfo, &ccpInfo);
					if (SUCCEEDED(hr) && cnum > 0) {
						// Use the MIMECPINFO structures returned to allow the
						// user to select his or her own preferences.
						fnmimecallback(cnum, pcpInfo, context);
					}
					CoTaskMemFree((void*)pcpInfo);
				}
				pEnumCodePage->Release();
			}
			else {
				return (-2);
			}
			pMultiLanguage->Release();
		}
		else {
			return (-1);
		}
		return (0);
	}
	__finally {
		if (state == S_OK) CoUninitialize(); // if init locally, we uninit COM ... 
	}
}

int GetInfoFromCodePage(UINT uiCodePage, PMIMECPINFO pCodePageInfo/*OUT*/)
{
	HRESULT state = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	__try {
		IMultiLanguage * pMultiLanguage;
		HRESULT hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (void**)&pMultiLanguage);
		if (hr == S_OK) {
			hr = pMultiLanguage->GetCodePageInfo(uiCodePage, pCodePageInfo);
			if (FAILED(hr)) {
				return (-2);
			}
		}
		else {
			return (-1);
		}
		return (0);
	}
	__finally {
		if (state == S_OK) CoUninitialize(); // if init locally, we uninit COM ... 
	}
}

int EnumerateLocale(void(*fnlocalecallback)(UINT, PRFC1766INFO, LPVOID), LPVOID context)
{
	HRESULT state = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	__try {
		IMultiLanguage * pMultiLanguage;
		HRESULT hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (void**)&pMultiLanguage);
		if (hr == S_OK) {
			IEnumRfc1766 *pEnumLocale;
			// if filter required see : http://msdn.microsoft.com/en-us/library/ie/aa741210(v=vs.85).aspx
			hr = pMultiLanguage->EnumRfc1766(&pEnumLocale);
			if (hr == S_OK) {
				UINT cnum = 10;
				PRFC1766INFO plcInfo; ULONG clcInfo;
				// ------------------ 
				plcInfo = (PRFC1766INFO)CoTaskMemAlloc(sizeof(RFC1766INFO)*cnum);
				do {
					hr = pEnumLocale->Next(cnum, plcInfo, &clcInfo);
					if (SUCCEEDED(hr) && clcInfo > 0) {
						fnlocalecallback(clcInfo, plcInfo, context);
					}
				} while (clcInfo == cnum);
				CoTaskMemFree((void*)plcInfo);
				// ------------------ 
				pEnumLocale->Release();
			}
			else {
				return (-2);
			}
			pMultiLanguage->Release();
		}
		else {
			return (-1);
		}
		return (0);
	}
	__finally{
		if (state == S_OK) CoUninitialize(); // if init locally, we uninit COM ... 
	}
}

int GetInfoFromLocale(LCID uiLocale, PRFC1766INFO pLocaleInfo/*OUT*/)
{
	HRESULT state = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	__try {
		IMultiLanguage2 * pMultiLanguage;
		HRESULT hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage2, (void**)&pMultiLanguage);
		if (hr == S_OK) {
			// if use old version, this only return the english version ?!
			hr = pMultiLanguage->GetRfc1766Info(uiLocale, GetSystemDefaultLangID(), pLocaleInfo);
			if (FAILED(hr)) {
				return (-2);
			}
		}
		else {
			return (-1);
		}
		return (0);
	}
	__finally {
		if (state == S_OK) CoUninitialize(); // if init locally, we uninit COM ... 
	}
}

static LSTATUS GetTimezoneInfo(LPCWSTR strvalue, LPVOID timezonebuff, TIMEZONEINFO& timezoneinfo) {
	HKEY hKeyItem;
	LSTATUS lRetVal = RegOpenKeyExW(HKEY_LOCAL_MACHINE, strvalue, 0, KEY_ALL_ACCESS, &hKeyItem);
	if (lRetVal == ERROR_SUCCESS) {
		DWORD dwType, cbSize = sizeof(TIME_ZONE_INFORMATION);
		lRetVal = RegQueryValueExW(hKeyItem, L"TZI", NULL, &dwType, (LPBYTE)timezonebuff, &cbSize);
		assert(cbSize <= sizeof(TIME_ZONE_INFORMATION));
		timezoneinfo.tzi = *(LONG*)timezonebuff;
	}
	if (lRetVal == ERROR_SUCCESS) {
		DWORD dwType, cbSize = sizeof(timezoneinfo.display);
		lRetVal = RegQueryValueExW(hKeyItem, L"Display", NULL, &dwType, (LPBYTE)timezoneinfo.display, &cbSize);
		assert(cbSize <= sizeof(timezoneinfo.display));
	}
	if (lRetVal == ERROR_SUCCESS) {
		DWORD dwType, cbSize = sizeof(timezoneinfo.dlt);
		lRetVal = RegQueryValueExW(hKeyItem, L"Dlt", NULL, &dwType, (LPBYTE)timezoneinfo.dlt, &cbSize);
		assert(cbSize <= sizeof(timezoneinfo.dlt));
	}
	if (lRetVal == ERROR_SUCCESS) {
		DWORD dwType, cbSize = sizeof(timezoneinfo.standard);
		lRetVal = RegQueryValueExW(hKeyItem, L"Std", NULL, &dwType, (LPBYTE)timezoneinfo.standard, &cbSize);
		assert(cbSize <= sizeof(timezoneinfo.standard));
	}
	RegCloseKey(hKeyItem);
	return lRetVal;
}

// this method read regkey : HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Time Zones
int EnumerateTimezone(void(*fntimezonecallback)(UINT, PTIMEZONEINFO, LPVOID), LPVOID context)
{
	// for ref see: http://binaryworld.net/Main/CodeDetail.aspx?CodeId=3633
	static int const MAX_STRING = 64;
	static LPCWSTR SKEY_NT = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\";
//	static LPCWSTR SKEY_9X = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Time Zones\\"; // old system 

	HKEY hKeyResult;
	LONG lRetVal = RegOpenKeyExW(HKEY_LOCAL_MACHINE, SKEY_NT, 0, KEY_READ, &hKeyResult);
	if (lRetVal == ERROR_SUCCESS) {
		DWORD count, maxlength;
		// prepare key string base and offset : 
		LONG offset = lstrlenW(SKEY_NT);
		LPWSTR strvalue = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR) * (offset + MAX_STRING));
		LPVOID timezone = HeapAlloc(GetProcessHeap(), 0, 256/*large enough*/);
		lstrcpyW(strvalue, SKEY_NT);
		// enumerate prepare : 
		lRetVal = RegQueryInfoKeyW(hKeyResult, NULL, NULL, NULL, &count, &maxlength, NULL, NULL, NULL, NULL, NULL, NULL);
		if (lRetVal != ERROR_SUCCESS) count = DWORD(-1);
		else assert(MAX_STRING > maxlength);
		// enumerate all : 
		for (int lCurIdx = 0; (DWORD)lCurIdx < count && lRetVal == ERROR_SUCCESS; lCurIdx++) {
			lRetVal = RegEnumKeyW(hKeyResult, lCurIdx, strvalue + offset, MAX_STRING);
			if (lRetVal == ERROR_SUCCESS) {
				TIMEZONEINFO timezoneinfo;
				lstrcpynW(timezoneinfo.key, strvalue + offset, MAX_STRING); // save the universal name in english ?? 
				lRetVal = GetTimezoneInfo(strvalue, timezone, timezoneinfo);
				if (lRetVal == ERROR_SUCCESS) {
					fntimezonecallback(1, &timezoneinfo, context);
				}
			}
		}
		// clear enumerate 
		HeapFree(GetProcessHeap(), 0, timezone);
		HeapFree(GetProcessHeap(), 0, strvalue);
		// cleanup 
		RegCloseKey(hKeyResult);
		return (0);
	}
	else {
		return (-1);
	}
}

int GetInfoFromTimezone(LPCWSTR pKeyname, PTIMEZONEINFO pTimezoneInfo/*OUT*/)
{
	static int const MAX_STRING = 64;
	static LPCWSTR SKEY_NT = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\";

	LPWSTR strvalue = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR) * (lstrlenW(SKEY_NT) + MAX_STRING));
	BYTE timezone[sizeof(TIME_ZONE_INFORMATION)];

	wsprintfW(strvalue, L"%s%s", SKEY_NT, pKeyname);
	
	lstrcpyW(pTimezoneInfo->key, pKeyname);
	LONG lRetVal = GetTimezoneInfo(strvalue, timezone, *pTimezoneInfo);
	
	HeapFree(GetProcessHeap(), 0, strvalue);
	return (lRetVal == ERROR_SUCCESS) ? (0) : (-1);
}

int GetInfoCountTimezone(void)
{
	static LPCWSTR SKEY_NT = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\";

	HKEY hKeyResult;
	LONG lRetVal = RegOpenKeyExW(HKEY_LOCAL_MACHINE, SKEY_NT, 0, KEY_READ, &hKeyResult);
	if (lRetVal == ERROR_SUCCESS) {
		DWORD count;
		// enumerate all : 
		lRetVal = RegQueryInfoKeyW(hKeyResult, NULL, NULL, NULL, &count, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		// cleanup 
		RegCloseKey(hKeyResult);
		return int(count);
	}
	else {
		return (-1);
	}
}

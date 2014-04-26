
#include <Mlang.h>

#define INITGUID
#include <basetyps.h>
// Multi Language Support : http://www.winehq.org/pipermail/wine-patches/2004-August/011948.html
DEFINE_GUID(IID_IMultiLanguage,		0x275C23E1, 0x3747, 0x11D0, 0x9F, 0xEA, 0x00, 0xAA, 0x00, 0x3F, 0x86, 0x46);
DEFINE_GUID(CLSID_CMultiLanguage,	0x275C23E2, 0x3747, 0x11D0, 0x9F, 0xEA, 0x00, 0xAA, 0x00, 0x3F, 0x86, 0x46);
DEFINE_GUID(IID_IEnumCodePage,		0x275C23E3, 0x3747, 0x11D0, 0x9F, 0xEA, 0x00, 0xAA, 0x00, 0x3F, 0x86, 0x46);

int CodePageToCharset(UINT uiCodePage) {
	int charset = -1;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	IMultiLanguage * pMultiLanguage;
	HRESULT hr = CoCreateInstance(&CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, &IID_IMultiLanguage, (void**)&pMultiLanguage);
	if (hr == S_OK) {
		MIMECPINFO CodePageInfo;
		hr = pMultiLanguage->lpVtbl->GetCodePageInfo(pMultiLanguage, uiCodePage, &CodePageInfo);
		if (SUCCEEDED(hr)) { charset = CodePageInfo.bGDICharset; }
		pMultiLanguage->lpVtbl->Release(pMultiLanguage);
	}

	CoUninitialize();
	return charset;
}

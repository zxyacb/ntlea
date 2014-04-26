
#include "truetype.h"

static char const tag_name[] = "name";
__inline unsigned short WordSwap(unsigned short w) {
#ifdef _X86_
	__asm mov ax, w
	__asm xchg al, ah
#else
	return ((w >> 8) & 0x00FF) | ((w << 8) & 0xFF00);
#endif
}
#define SWAPWORD(x)		WordSwap(x)

BOOL ConvertFontFacename(HDC hDC, LOGFONTW* plf, BOOL flagnative)
{
	BOOL converted = FALSE;

	HFONT hFont = CreateFontIndirectW(plf);
	HGDIOBJ hOld = SelectObject(hDC, hFont);

	DWORD FCsize = GetFontData(hDC, *(DWORD *)tag_name, 0, NULL, 0);
	if (FCsize != GDI_ERROR) {
		LPVOID pFontData = (TableDirectory *)LocalAlloc(0, FCsize);
		GetFontData(hDC, *(DWORD *)tag_name, 0, pFontData, FCsize);

		NameTableHeader ttNTHeader;
		ttNTHeader.uFSelector = SWAPWORD(*((PUSHORT)pFontData + 0));
		ttNTHeader.uNRCount = SWAPWORD(*((PUSHORT)pFontData + 1));
		ttNTHeader.uStorageOffset = SWAPWORD(*((PUSHORT)pFontData + 2));

		NameRecord* ttRecords = (NameRecord*)((NameTableHeader*)pFontData + 1);
		for (int i = 0; i<ttNTHeader.uNRCount; i++){
			NameRecord ttRecord;
			ttRecord.uNameID = SWAPWORD(ttRecords[i].uNameID);
			if (ttRecord.uNameID != 4) continue;

			ttRecord.uPlatformID = SWAPWORD(ttRecords[i].uPlatformID);
			ttRecord.uEncodingID = SWAPWORD(ttRecords[i].uEncodingID);
			ttRecord.uLanguageID = SWAPWORD(ttRecords[i].uLanguageID);
			ttRecord.uStringLength = SWAPWORD(ttRecords[i].uStringLength);
			ttRecord.uStringOffset = SWAPWORD(ttRecords[i].uStringOffset);

			if ((!flagnative && 1033 == ttRecord.uLanguageID) || (flagnative && ttRecord.uLanguageID && 1033 != ttRecord.uLanguageID)) {
				// save file position, so we can return to continue with search
				LPCSTR p = (LPCSTR)pFontData + ttNTHeader.uStorageOffset + ttRecord.uStringOffset;
				// -------- 
				if ( ttRecord.uPlatformID == 0 || ttRecord.uLanguageID != 0) {
					for (int i = 0; i < ttRecord.uStringLength / 2; ++i){
						plf->lfFaceName[i] = SWAPWORD(*((PUSHORT)p + i));
					}
				} else {
					MultiByteToWideChar(ttRecord.uLanguageID, 0, p, ttRecord.uStringLength, 
						plf->lfFaceName, ARRAYSIZE(plf->lfFaceName));
				}
				// -------- 
				plf->lfFaceName[ttRecord.uStringLength / 2] = L'\0';
				converted = TRUE; 
				break;
			}
		}

		LocalFree(pFontData);
	}

	SelectObject(hDC, hOld);
	DeleteObject(hFont);

	return converted;
}

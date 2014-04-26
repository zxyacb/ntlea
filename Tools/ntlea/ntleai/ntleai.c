
#include <Windows.h>

#include "../nthook/nthook.h"
#include "ntleai.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_SUFFIX				L"_D"
#else
#define DEBUG_SUFFIX				L""
#endif
extern LPCWSTR szRcpHookDLL = L"\\ntleai" DEBUG_SUFFIX L".dll";
extern Addresses addresses = { NULL, };

static LPVOID HookDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, HMODULE hDLL)
{
	LPBYTE funcptr = hDLL ? (LPBYTE)(DWORD_PTR)GetProcAddress(hDLL, lpszFuncName) : (LPBYTE)lpszFuncName;
	DWORD fOldProt; DWORD_PTR offset = (DWORD_PTR)((LPBYTE)lpHookAddress - funcptr - (1 + sizeof(FARPROC)));
	VirtualProtect(funcptr, 1 + sizeof(FARPROC)/*jump addr*/, PAGE_EXECUTE_READWRITE, &fOldProt);
	funcptr[0] = 0xE9; // as jump code 
	((DWORD_PTR*)(funcptr + 1))[0] = offset; // set hook func 
	FlushInstructionCache(GetCurrentProcess(), funcptr, (1 + sizeof(FARPROC))); // esi := funcptr 
	// change back the original access protect : 
	VirtualProtect(funcptr, 1 + sizeof(FARPROC)/*jump addr*/, fOldProt, NULL);
	return funcptr + (1 + sizeof(FARPROC)); // return the original hook absolute address + 5. 
											// code overide by jmp will append manually before jmp !
}

VOID HookAnsiFunctions(Settings const* psettings)
{
	HMODULE hDLL; FARPROC symbol;
	// --- 
	hDLL = LoadLibraryA("ntdll.dll");
	// cmp	[bChinesePath],bl; jne >A7	why ? check chinese path ?? we know to use chinesepath is not recommanded by NTLEA-advance!
	addresses.lpUnitoMBAddress = (psettings->bChinesePath == 0) ? (LPBYTE)HookDllFunc("RtlUnicodeToMultiByteN", (LPVOID)(DWORD_PTR)HookUnicodeToMultiByte, hDLL)
		/*+ (psettings->nOSVer == VER_WIN2K ? 5 : 0)*/ : NULL;
	addresses.lpMBtoUniAddress = (LPBYTE)HookDllFunc("RtlMultiByteToUnicodeN", (LPVOID)(DWORD_PTR)HookMultiByteToUnicode, hDLL)
		/*+ (psettings->nOSVer == VER_WIN2K ? 5 : 0)*/;
	HookDllFunc("RtlMultiByteToUnicodeSize", (LPVOID)(DWORD_PTR)HookMultiByteToUnicodeSize, hDLL);
	HookDllFunc("RtlUnicodeToMultiByteSize", (LPVOID)(DWORD_PTR)HookUnicodeToMultiByteSize, hDLL);
	HookDllFunc("RtlUnicodeToOemN", (LPVOID)(DWORD_PTR)HookUnicodeToMultiByte, hDLL);
	HookDllFunc("RtlOemToUnicodeN", (LPVOID)(DWORD_PTR)HookMultiByteToUnicode, hDLL); // no needs free 

	// fixed symbols : 
	symbol = GetProcAddress(hDLL, "NlsAnsiCodePage");
	if (symbol) *(DWORD*)(DWORD_PTR)symbol = psettings->dwCodePage; // global var as symbol, change it
///	symbol = GetProcAddress(hDLL, "NlsMbCodePageTag");
///	if (symbol) psettings->MbCodePageTag = *(DWORD*)(DWORD_PTR)symbol & 0xFF; // global var as symbol, save it for win2k (deprecated)

	// --- 
	hDLL = LoadLibraryA("Shell32.dll");
	HookDllFunc("SHBrowseForFolderA", (LPVOID)(DWORD_PTR)HookSHBrowseForFolder, hDLL);
	addresses.lpShellExecuteAddress = (LPBYTE)HookDllFunc("ShellExecuteA", (LPVOID)(DWORD_PTR)HookShellExecute, hDLL);

	// --- 
	hDLL = LoadLibraryA("gdi32.dll");
	HookDllFunc("GdiGetCodePage", (LPVOID)(DWORD_PTR)HookGdiGetCodePage, hDLL); // no needs free
	HookDllFunc((LPCSTR)(DWORD_PTR)GetACP, (LPVOID)(DWORD_PTR)HookGetACP, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetOEMCP, (LPVOID)(DWORD_PTR)HookGetACP, NULL);
	addresses.lpCPInfoAddress = HookDllFunc((LPCSTR)(DWORD_PTR)GetCPInfo, (LPVOID)(DWORD_PTR)HookGetCPInfo, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)WinExec, (LPVOID)(DWORD_PTR)HookWinExec, NULL);

#if NoUsed
	HookDllFunc((LPCSTR)(DWORD_PTR)GetCPInfoExA, (LPVOID)(DWORD_PTR)HookGetCPInfoEx, NULL);
#endif
	// cmp	[bChinesePath],bl; je >A6
	if (psettings->bChinesePath != 0) {
		// patch 002 for win8/win8.1
		if (0x25FF == *(LPWORD)(DWORD_PTR)CreateFileA) {// check code : JMP [xxxxxxxx] address address !! 
			DWORD_PTR createfileaddr = *(LPDWORD)(DWORD_PTR)((LPWORD)(DWORD_PTR)CreateFileA + 1);
			DWORD_PTR createfilea = *(LPDWORD)createfileaddr;
			addresses.lpCreateFileAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)createfilea, (LPVOID)(DWORD_PTR)HookCreateFile, NULL);
		}
		else {
			addresses.lpCreateFileAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CreateFileA, (LPVOID)(DWORD_PTR)HookCreateFile, NULL)
				/*+ (psettings->nOSVer == VER_WINXP_SP2_OR_ABOVE ? 0 : 1)*/;
		}
	}
	// A6: 
	addresses.lpCompareStringAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CompareStringA, (LPVOID)(DWORD_PTR)HookCompareString, NULL)
		/*+ (psettings->nOSVer == VER_WINXP_SP2_OR_ABOVE ? 0 : 4)*/;
	if (0x8B55FF8B == *(LPDWORD)(DWORD_PTR)CreateProcessA) {
		addresses.lpAcGenralAnsiType = 0;
		addresses.lpCreateProcAnsiAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CreateProcessA, (LPVOID)(DWORD_PTR)HookCreateProcessA, NULL)
			/*+ (psettings->nOSVer == VER_WIN2K ? 1 : 0)*/;
	} else if (0xB8106A == (*(LPDWORD)(DWORD_PTR)CreateProcessA & 0xFFFFFF)) {
	//	7027DF4D   6A 10                push        10h
	//	7027DF4F   B8 AC 88 21 70       mov         eax, 702188ACh
	//	7027DF54   E8 92 51 F8 FF       call        702030EB
		addresses.lpAcGenralAnsiType = *(LPDWORD)((LPBYTE)(DWORD_PTR)CreateProcessA + 3);
		addresses.lpCreateProcAnsiAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CreateProcessA, (LPVOID)(DWORD_PTR)HookCreateProcessA, NULL) + 2;
	}// else
	if (0x8B55FF8B == *(LPDWORD)(DWORD_PTR)CreateProcessW) {
		addresses.lpAcGenralUniType = 0;
		addresses.lpCreateProcUniAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CreateProcessW, (LPVOID)(DWORD_PTR)HookCreateProcessW, NULL)
			/*+ (psettings->nOSVer == VER_WIN2K ? 1 : 0)*/;
	} else if (0xB8106A == (*(LPDWORD)(DWORD_PTR)CreateProcessW & 0xFFFFFF)) {
	//	7027E122   6A 10                push        10h
	//	7027E124   B8 14 89 21 70       mov         eax, 70218914h
	//	7027E129   E8 BD 4F F8 FF       call        702030EB
		addresses.lpAcGenralUniType = *(LPDWORD)((LPBYTE)(DWORD_PTR)CreateProcessW + 3);
		addresses.lpCreateProcUniAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CreateProcessW, (LPVOID)(DWORD_PTR)HookCreateProcessW, NULL) + 2;
	}// else
	addresses.lpGetTimeZoneInformation = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)GetTimeZoneInformation, (LPVOID)(DWORD_PTR)HookGetTimeZoneInformation, NULL)
		/*+ (psettings->nOSVer == VER_WIN2K ? 4 : 0)*/;
	addresses.lpVerQueryValueAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)VerQueryValueA, (LPVOID)(DWORD_PTR)HookVerQueryValue, NULL)
		/*+ (psettings->nOSVer == VER_WINXP_SP2_OR_ABOVE ? 0 : 1)*/;

	HookDllFunc((LPCSTR)(DWORD_PTR)GetThreadLocale, (LPVOID)(DWORD_PTR)HookGetLocaleID, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetSystemDefaultUILanguage, (LPVOID)(DWORD_PTR)HookGetLocaleID, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetUserDefaultUILanguage, (LPVOID)(DWORD_PTR)HookGetLocaleID, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetSystemDefaultLCID, (LPVOID)(DWORD_PTR)HookGetLocaleID, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetUserDefaultLCID, (LPVOID)(DWORD_PTR)HookGetLocaleID, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetSystemDefaultLangID, (LPVOID)(DWORD_PTR)HookGetLocaleID, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetUserDefaultLangID, (LPVOID)(DWORD_PTR)HookGetLocaleID, NULL);
	LPSTR  cmdlinea = GetCommandLineA();
	LPWSTR cmdlinew = GetCommandLineW();
	WideCharToMultiByte(psettings->dwCodePage, 0, cmdlinew, -1, cmdlinea, MAX_PATH << 1, NULL, NULL); // overwrite it!!

	addresses.lpWCtoMBAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)WideCharToMultiByte, (LPVOID)(DWORD_PTR)HookWCtoMB, NULL)
		/*+ (psettings->nOSVer == VER_WINXP_SP2_OR_ABOVE ? 0 : 2)*/;
	addresses.lpMBtoWCAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)MultiByteToWideChar, (LPVOID)(DWORD_PTR)HookMBtoWC, NULL)
		/*+ (psettings->nOSVer == VER_WINXP_SP2_OR_ABOVE ? 0 : 1)*/;

#if NoUsed
	HookDllFunc((LPCSTR)(DWORD_PTR)FindResourceExA, (LPVOID)(DWORD_PTR)HookFindResourceEx, NULL);	
	psettings->lpRegisterClassAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)RegisterClassA, (LPVOID)(DWORD_PTR)HookRegisterClass, NULL);
	psettings->lpRegisterClassExAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)RegisterClassExA, (LPVOID)(DWORD_PTR)HookRegisterClassEx, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)SetClassLongA, (LPVOID)(DWORD_PTR)HookSetClassLong, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetClassInfoA, (LPVOID)(DWORD_PTR)HookGetClassInfo, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetClassInfoExA, (LPVOID)(DWORD_PTR)HookGetClassInfoEx, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)UnregisterClassA, (LPVOID)(DWORD_PTR)HookUnregisterClass, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetClassLongA, (LPVOID)(DWORD_PTR)HookGetClassLong, NULL);
#endif

	HookDllFunc((LPCSTR)(DWORD_PTR)DefWindowProcA, (LPVOID)(DWORD_PTR)HookDefWindowProc, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)DefMDIChildProcA, (LPVOID)(DWORD_PTR)HookDefMDIChildProc, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)DefDlgProcA, (LPVOID)(DWORD_PTR)HookDefDlgProc, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)DefFrameProcA, (LPVOID)(DWORD_PTR)HookDefFrameProc, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)CreateWindowExA, (LPVOID)(DWORD_PTR)HookCreateWindowEx, NULL);
	addresses.lpCallWindowProcAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)CallWindowProcA, (LPVOID)(DWORD_PTR)HookCallWindowProc, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)SetWindowTextA, (LPVOID)(DWORD_PTR)HookSetWindowText, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetWindowTextA, (LPVOID)(DWORD_PTR)HookGetWindowText, NULL);

	HookDllFunc((LPCSTR)(DWORD_PTR)DialogBoxParamA, (LPVOID)(DWORD_PTR)HookDialogBoxParam, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)DialogBoxIndirectParamA, (LPVOID)(DWORD_PTR)HookDialogBoxIndirectParam, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)CreateDialogParamA, (LPVOID)(DWORD_PTR)HookCreateDialogParam, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)CreateDialogIndirectParamA, (LPVOID)(DWORD_PTR)HookCreateDialogIndirectParam, NULL);

	HookDllFunc((LPCSTR)(DWORD_PTR)SendMessageA, (LPVOID)(DWORD_PTR)HookSendMessage, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)SendMessageCallbackA, (LPVOID)(DWORD_PTR)HookSendMessageCallback, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)SendMessageTimeoutA, (LPVOID)(DWORD_PTR)HookSendMessageTimeout, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)SendNotifyMessageA, (LPVOID)(DWORD_PTR)HookSendNotifyMessage, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)PostMessageA, (LPVOID)(DWORD_PTR)HookPostMessage, NULL);

	HookDllFunc((LPCSTR)(DWORD_PTR)SetWindowLongPtrA, (LPVOID)(DWORD_PTR)HookSetWindowLongPtr, NULL);
	addresses.lpSetWindowLongAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)SetWindowLongPtrW, (LPVOID)(DWORD_PTR)HookSetWindowLongPtrW, NULL);

	// patch 001 for win8/win8.1
	if (0x8B55FF8B == *(LPDWORD)(DWORD_PTR)GetWindowLongPtrA) {// check code : mov edi, edi | push ebp | mov ebp, esp
		addresses.lpGetWindowLongPtrSelect = (LPVOID)(DWORD_PTR)GetWindowLongPtrJ8; // 8/8.1
		addresses.lpUser32SEHAddress = NULL; // --------- 
	} else { // code will overwrite user32seh handle address !
		addresses.lpGetWindowLongPtrSelect = (LPVOID)(DWORD_PTR)GetWindowLongPtrJ0; // XP/vista/7
		addresses.lpUser32SEHAddress = (LPVOID)(DWORD_PTR)(*(LPDWORD)((LPBYTE)(DWORD_PTR)GetWindowLongPtrA + 3));
	}
	addresses.lpGetWindowLongPtrAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)GetWindowLongPtrA, (LPVOID)(DWORD_PTR)HookGetWindowLongPtr, NULL)
		+ (/*psettings->nOSVer == VER_WIN2K ? 0 : */2);
	HookDllFunc((LPCSTR)(DWORD_PTR)GetMenuStringA, (LPVOID)(DWORD_PTR)HookGetMenuString, NULL);
#ifdef Testing
	HookDllFunc((LPCSTR)(DWORD_PTR)GetMenuItemInfoA, (LPVOID)(DWORD_PTR)HookGetMenuItemInfo, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)SetMenuItemInfoA, (LPVOID)(DWORD_PTR)HookSetMenuItemInfo, NULL);
#endif

	HookDllFunc((LPCSTR)(DWORD_PTR)IsDBCSLeadByte, (LPVOID)(DWORD_PTR)HookIsDBCSLeadByte, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)CharPrevA, (LPVOID)(DWORD_PTR)HookCharPrev, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)CharNextA, (LPVOID)(DWORD_PTR)HookCharNext, NULL);
	addresses.lpEnumFontFamiliesExA = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontFamiliesExA, (LPVOID)(DWORD_PTR)HookEnumFontFamiliesExA, NULL)
		/*+ (psettings->nOSVer == VER_WINXP_SP2_OR_ABOVE ? 0 : 1)*/;
	addresses.lpEnumFontFamiliesExW = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontFamiliesExW, (LPVOID)(DWORD_PTR)HookEnumFontFamiliesExW, NULL);
	
	addresses.lpEnumFontsA = HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontsA, (LPVOID)(DWORD_PTR)HookEnumFontsA, NULL);
	addresses.lpEnumFontsW = HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontsW, (LPVOID)(DWORD_PTR)HookEnumFontsW, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontFamiliesA, (LPVOID)(DWORD_PTR)HookEnumFontFamiliesA, NULL);
	HookDllFunc((LPCSTR)(DWORD_PTR)EnumFontFamiliesW, (LPVOID)(DWORD_PTR)HookEnumFontFamiliesW, NULL);

	HookDllFunc((LPCSTR)(DWORD_PTR)CreateFontIndirectA, (LPVOID)(DWORD_PTR)HookCreateFontIndirect, NULL);
	addresses.lpGetStockObjectAddress = (LPBYTE)HookDllFunc((LPCSTR)(DWORD_PTR)GetStockObject, (LPVOID)(DWORD_PTR)HookGetStockObject, NULL);
}

#if !defined(_DEBUG) && !defined(DEBUG)
#define DllMain						_DllMainCRTStartup
#endif
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	return RunDllMain(hinstDll, dwReason, lpReserved);
}

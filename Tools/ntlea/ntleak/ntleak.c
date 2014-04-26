
#include <minhook.h>
#include "../nthook/nthook.h"
#include "../nthookfun/nthookfun.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_SUFFIX				L"_D"
#else
#define DEBUG_SUFFIX				L""
#endif
extern LPCWSTR szRcpHookDLL = L"\\ntleak" DEBUG_SUFFIX L".dll";
extern Addresses addresses = { NULL, };

extern LPVOID HookDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, HMODULE hDLL)
{
	LPVOID funcptr = hDLL ? (LPVOID)(DWORD_PTR)GetProcAddress(hDLL, lpszFuncName) : (LPVOID)lpszFuncName;
	LPVOID outputptr;
	return MH_CreateHook(funcptr, lpHookAddress, &outputptr) == MH_OK ? (MH_EnableHook(funcptr), outputptr) : NULL;
	// return the original funcaddress !
}

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		MH_Initialize(); // global init !! 
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		//TODO: restore any hook func ?? 
		MH_Uninitialize(); // global free !!
	}
	return RunDllMain(hinstDll, dwReason, lpReserved);
}

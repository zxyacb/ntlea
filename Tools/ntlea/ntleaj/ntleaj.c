
#include <mhook-lib/mhook.h>
#include <Windows.h>

#include "../nthook/nthook.h"
#include "../nthookfun/nthookfun.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_SUFFIX				L"_D"
#else
#define DEBUG_SUFFIX				L""
#endif
extern LPCWSTR szRcpHookDLL = L"\\ntleaj" DEBUG_SUFFIX L".dll";
extern Addresses addresses = { NULL, };

extern LPVOID HookDllFunc(LPCSTR lpszFuncName, LPVOID lpHookAddress, HMODULE hDLL)
{
	LPVOID funcptr = hDLL ? (LPVOID)(DWORD_PTR)GetProcAddress(hDLL, lpszFuncName) : (LPVOID)lpszFuncName;
	return Mhook_SetHook(&funcptr, lpHookAddress) ? funcptr : NULL; // return the original funcaddress !
}

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	return RunDllMain(hinstDll, dwReason, lpReserved);
}

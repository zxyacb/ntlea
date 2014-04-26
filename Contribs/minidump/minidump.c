
#include <Windows.h>
#include <DbgHelp.h>

static wchar_t* g_basename = NULL;
static wchar_t* g_dumppath = NULL;
static LPTOP_LEVEL_EXCEPTION_FILTER g_olddump = NULL;

static VOID GetExecutableTimeName(wchar_t exetimename[/*MAX_PATH*/])
{
	SYSTEMTIME systime; GetLocalTime(&systime);
	HMODULE psapi_module = LoadLibraryW(L"psapi.dll");
	if (psapi_module) {
		typedef BOOL(WINAPI *sGetModuleBaseName)(HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize);
		sGetModuleBaseName fn = (sGetModuleBaseName)GetProcAddress(psapi_module, "GetModuleBaseNameW");
		if (fn) {
			LPWSTR p = exetimename; *p++ = L'_';
			p += fn(GetCurrentProcess(), NULL, p, MAX_PATH);
			wsprintfW(p, L"_%04u-%02u-%02u_%02u-%02u-%02u",
				systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
		}
		FreeLibrary(psapi_module);
	}
}

static LONG WINAPI ClientErrorHandlerProc(EXCEPTION_POINTERS *pExpInfo)
{// LPTOP_LEVEL_EXCEPTION_FILTER
	HMODULE dbghelp_module = LoadLibraryW(L"dbghelp.dll");
	if (dbghelp_module) {
		typedef BOOL(WINAPI *sMiniDumpWriteDump)(
			HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType,
			PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
			PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
			PMINIDUMP_CALLBACK_INFORMATION CallbackParam
			);
		sMiniDumpWriteDump fn = (sMiniDumpWriteDump)GetProcAddress(dbghelp_module, "MiniDumpWriteDump");
		if (fn) {
			wchar_t dumppath[MAX_PATH * 2];
			wsprintfW(dumppath, L"%s\\crashinfo_%s", g_dumppath ? g_dumppath : L".", g_basename ? g_basename : L"0");
			GetExecutableTimeName(dumppath + lstrlenW(dumppath)); lstrcatW(dumppath, L".dmp");
			// now open file to write : 
			HANDLE hFile = CreateFileW(dumppath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE) {
				MINIDUMP_EXCEPTION_INFORMATION einfo;
				einfo.ThreadId = GetCurrentThreadId();
				einfo.ExceptionPointers = pExpInfo;
				einfo.ClientPointers = FALSE;
				fn(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &einfo, NULL, NULL);
				CloseHandle(hFile);
			}
		}
		FreeLibrary(dbghelp_module);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void InitMiniDump(void const* basename, void const* dumpinst) {
	if (!g_basename && basename) g_basename = HeapAlloc(GetProcessHeap(), 0, (lstrlenW((wchar_t const*)basename) + 1) * sizeof(wchar_t));
	if ( g_basename && basename) lstrcpyW(g_basename, (wchar_t const*)basename);
	if (!g_dumppath) {
		wchar_t* exepath = g_dumppath = HeapAlloc(GetProcessHeap(), 0, MAX_PATH * 2 * sizeof(wchar_t));
		DWORD len = GetModuleFileNameW((HANDLE)dumpinst, exepath, MAX_PATH * 2);
		LPCWSTR p = len + exepath;
		while (p > exepath && *p != L'\\') --p;
		if (*p == L'\\') {
			exepath[p - exepath] = L'\0'; // make as tail 
		}
	}
	g_olddump = SetUnhandledExceptionFilter(ClientErrorHandlerProc);
}

void FreeMiniDump(void) {
	SetUnhandledExceptionFilter(g_olddump); // reset as default one !
	if (g_basename) {
		HeapFree(GetProcessHeap(), 0, g_basename); g_basename = NULL;
	}
	if (g_dumppath) {
		HeapFree(GetProcessHeap(), 0, g_dumppath); g_dumppath = NULL;
	}
}

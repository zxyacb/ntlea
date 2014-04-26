
// == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =
// NT Locale Emulator Advance Successor
// Written by Littlewater Start at 2013.11 (original : LOVEHINA - AVC) 2006.11
// -----------------------------------------------------------------------------
// requires VC++ 2013 Express/Professional/Ultimate
// == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =
//
//   Win32 subsystem user mode only.The "Users" privilege is required.
//
// Remark:
//
//   The launcher.

#include <Windows.h>
#include <Shlwapi.h>

#include "../ntlea.h"
#include "../ntproc/ntproc.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_SUFFIX				L"_D"
#else
#define DEBUG_SUFFIX				L""
#endif
#if defined(_AMD64_)
static WCHAR rcpHookDll[] = L"\\ntleak" DEBUG_SUFFIX L".dll"; // default use ... 
#elif defined(_X86_)
static WCHAR rcpHookDll[] = L"\\ntleai" DEBUG_SUFFIX L".dll"; // default use ... 
#endif
extern LPCWSTR szRcpHookDLL = rcpHookDll;

DWORD GetParameterValue(LPCWSTR);
LPCWSTR GetParameterString(LPCWSTR, BOOL dupflag);
LPWSTR MkApplicationName(LPCWSTR string);
LPWSTR MkStringQuote(LPCWSTR string);
LPWSTR MkCommandLine(LPCWSTR, LPCWSTR, BOOL clearflag);
NtleaErrorNo PrintErrorString(NtleaErrorNo errnum);
LPCVOID ResetBuf(LPCVOID oldptr, LPCVOID newptr);

#if !defined(DEBUG) && !defined(_DEBUG)
#define main WinMainCRTStartup // mainCRTStartup
#endif

int main(void)
{
	// 0. accept only 1 instance ... 
	CreateMutexA(NULL, FALSE, szRcpIntMtx);
	if (GetLastError()) { return PrintErrorString(ERR_MULTIPLE_INSTANCE); }

	NtleaProcess ntproc = { 0 };
	LPCWSTR pApplicationName = NULL, pCommandLine = NULL;
	LPCVOID pApplicationNameBuf = NULL, pCommandLineBuf = NULL;
	// 1. parse params and prepare helper data : 
	int dbg = 0, dir = 0, qit = 0, ret = 0;
	int argc; LPWSTR * argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	// 2. apply input param value : 
	if (argv && argc > 1) {
		ret = CreateProcessBeginW(&ntproc, (pApplicationName=argv[1]));
		for (int i = 2; i < argc; ++i) {
			switch (argv[i][0]) {
			case 'P': ntproc.dwCompOption = GetParameterValue(argv[i] + 1); break;
			case 'C': ntproc.dwCodePage = GetParameterValue(argv[i] + 1); break;
			case 'L': ntproc.dwLCID = GetParameterValue(argv[i] + 1); break;
			case 'T': ntproc.dwTimeZone = GetParameterValue(argv[i] + 1); break;
			case 'S': ntproc.dwSpApp = GetParameterValue(argv[i] + 1); break;
			case 'F': WideCharToMultiByte(CP_ACP, 0, GetParameterString(argv[i] + 1, FALSE), -1, 
				(LPSTR)ntproc.FontFaceName, sizeof(ntproc.FontFaceName), NULL, NULL); break; // only ansi string accept ?? 
			case 'A': pCommandLine = MkCommandLine(argv[1], GetParameterString(argv[i] + 1, TRUE), TRUE); break;
			// ----- inner ----- //
			case 'M': rcpHookDll[1 + 5] = (argv[i] + 1)[0]; break;
			case 'D': dbg = GetParameterValue(argv[i] + 1); break;
			case 'W': dir = GetParameterValue(argv[i] + 1); break;
			case 'Q': qit = GetParameterValue(argv[i] + 1); break;
			default: /* unknown indicator ! */ break;
			}
		}
	}
	// 3. else use default value !
	else /*if (argc <= 1)*/ { // NullPath : 
		OPENFILENAMEW ofn = { sizeof(OPENFILENAMEW), };
		WCHAR exepath[MAX_PATH]; exepath[0] = L'\0'; // REQUIRED!
		// Set lpstrFile[0] to '\0' so that GetOpenFileName won't use the contents of szFile to initialize itself.
		ofn.lpstrFilter = L"Executable File(*.exe)\0*.exe\0All files(*.*)\0*.*\0\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = exepath;
		ofn.nMaxFile = ARRAYSIZE(exepath);
		ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		if (GetOpenFileNameW(&ofn)) {
			pApplicationName = MkApplicationName(exepath);
			pApplicationNameBuf = ResetBuf(pApplicationNameBuf, pApplicationName);
			ret = CreateProcessStartW(&ntproc, pApplicationName);
		}
		else {
			ExitProcess(0); // exit !! 
		}
	}
	// 4. command line replacement doc referenced exe filepath.. 
	if (ret == ERR_EXECUTABLE_INVALID && !pCommandLine) {
		// reference : http://msdn.microsoft.com/en-us/library/windows/desktop/bb776419(v=vs.85).aspx
		WCHAR PEname[MAX_PATH], ExeName[MAX_PATH * 2];
		if ((DWORD_PTR)FindExecutableW(pApplicationName, NULL, PEname) > 32) {
			DWORD Exenamelen = sizeof(ExeName) / sizeof(ExeName[0]);
			if (S_OK != AssocQueryStringW(ASSOCF_OPEN_BYEXENAME, ASSOCSTR_EXECUTABLE, PEname, NULL, ExeName, &Exenamelen)) {
				lstrcpyW(ExeName, PEname);
			}
			pCommandLine = MkCommandLine(ExeName, MkStringQuote(pApplicationName), TRUE);
			pCommandLineBuf = ResetBuf(pCommandLineBuf, pCommandLine);
			pApplicationName = MkApplicationName(ExeName); // changed to that path !! 
			pApplicationNameBuf = ResetBuf(pApplicationNameBuf, pApplicationName);
			ret = CreateProcessStartW(&ntproc, pApplicationName); // ok now !
		}
	}
	// 5. if failed create process, exit ... 
	if (ret < 0) {
		ExitProcess(qit ? ret : PrintErrorString(ret));
	}
	// 6. now create process : 
	PROCESS_INFORMATION proinfo = { NULL, NULL, };
	STARTUPINFOW stinfo = { sizeof(STARTUPINFOW), };
	DWORD dwflags = dbg ? (DEBUG_ONLY_THIS_PROCESS) : (CREATE_SUSPENDED);
	if (!CreateProcessW(pApplicationName, (LPWSTR)pCommandLine, NULL, NULL, 0, dwflags, NULL, NULL, &stinfo, &proinfo)) {
		ExitProcess((UINT)-1); // exit !! 
	}
	if (pApplicationNameBuf) LocalFree((LPVOID)pApplicationNameBuf); // 
	if (pCommandLineBuf) LocalFree((LPVOID)pCommandLineBuf); // no needs any more !
	if (argv) LocalFree(argv); // cleanup the parser-params !
	if (dir) ResetWorkingDirectory(proinfo.hProcess); // 
	// 7. hook process : 
	ret = (dbg ? CreateProcessEndExt : CreateProcessEnd)(&ntproc, &proinfo, FALSE);
	// 8. exit and free : 
	CloseHandle(proinfo.hThread);
	if (ret < 0) {
		TerminateProcess(proinfo.hProcess, (UINT)ret); // exit !! 
		ExitProcess(qit ? ret : PrintErrorString(ret));
	} else {
		CloseHandle(proinfo.hProcess);
	}
	// -------------- 
	ExitProcess(0); // explicit exit process ... 
}

DWORD GetParameterValue(LPCWSTR numstr)
{
	LPCWSTR ptr = numstr;
	// 1. check neg / pos : 
	int neg = (*ptr == L'-') ? (++ptr, -1) : (1);
	int n = 0;
	while (*ptr) {
		// 2.1 skip invalid charactors : 
		while (*ptr < L'0' || *ptr > L'9') ++ptr;
		// 2.2 convert with 
		n = 10 * n + (*ptr - L'0'); ++ptr;
	}
	// 3. return the actual number : 
	return (DWORD)neg * n;
}

LPCWSTR GetParameterString(LPCWSTR string, BOOL dupflag)
{
	if (dupflag) {
		int count = lstrlenW(string);
		LPWSTR params = (LPWSTR)LocalAlloc(0, (count + 1) * sizeof(WCHAR));
		for (int i = 0; i < count; ++i) { // duplicate with \' replace with \"
			params[i] = (L'\'' == string[i]) ? L'\"' : string[i];
		}
		params[count] = L'\0'; // make as tail !!
		return params;
	} else {
		return string;
	}
}

LPWSTR MkStringQuote(LPCWSTR string)
{
	int count = lstrlenW(string);
	LPWSTR quotestring = (LPWSTR)LocalAlloc(0, (count + 3) * sizeof(WCHAR));
	wsprintfW(quotestring, L"\"%s\"", string);
	return quotestring;
}

LPWSTR MkApplicationName(LPCWSTR string)
{
	int count = lstrlenW(string);
	LPWSTR cmdline = (LPWSTR)LocalAlloc(0, (count + 1) * sizeof(WCHAR));
	lstrcpyW(cmdline, string);
	return cmdline;
}

LPWSTR MkCommandLine(LPCWSTR command, LPCWSTR params, BOOL clearflag)
{
	int count = lstrlenW(command) + lstrlenW(params);
	LPWSTR cmdline = (LPWSTR)LocalAlloc(0, (count + 4) * sizeof(WCHAR));
	wsprintfW(cmdline, L"\"%s\" %s", command, params); // double string ... 
	if (clearflag) LocalFree((LPVOID)params); // no longer required 
	return cmdline;
}

LPCVOID ResetBuf(LPCVOID oldptr, LPCVOID newptr)
{
	if (oldptr) LocalFree((LPVOID)oldptr);
	return newptr;
}

NtleaErrorNo PrintErrorString(NtleaErrorNo errnum)
{
#if 1
	char const* errdesc = "<err_uknown>!";
	switch (errnum) {
	case ERR_PE_FORMAT_INVALID:
		errdesc = "Err: NTLEAS detect that the PE Machine could not support.";
		break;
	case ERR_MULTIPLE_INSTANCE:
		errdesc = "Err: NTLEAS could not startup two instances at one time.";
		break;
	case ERR_EXECUTABLE_MISSING:
		errdesc = "Err: NTLEAS could not find or open specified PE file.";
		break;
	case ERR_EXECUTABLE_INVALID:
		errdesc = "Err: NTLEAS detect that the given is an invalid PE file.";
		break;
	case ERR_PROCESS_CREATE_FAILED:
		errdesc = "Err: NTLEAS could not create specified process of Exe.";
		break;
	case ERR_NTLEA_DLL_MISSING:
		errdesc = "Err: NTLEAS could not find inject ntleai.dll.";
		break;
	case ERR_FAILED_ALLOCATE_MEM:
		errdesc = "Err: NTLEAS was failed to virtual allocate memory.";
		break;
	case ERR_HOOKPROCESS_MISSING:
		errdesc = "Err: NTLEAS may be lost connection with hook process.";
		break;
	case ERR_FAILED_QUERY_PROCESS:
		errdesc = "Err: NTLEAS failed query process information.";
		break;
	}
	MessageBoxA(GetForegroundWindow(), errdesc, szCaption, MB_OK);
#endif
	return errnum;
}


#include <Windows.h>
#include <Psapi.h>

#include "../ntlea.h"
#include "ntproc.h"

#ifndef INLINE
#define INLINE __inline
#endif//INLINE

static const wchar_t* szAppHelp = L"\\AppHelp.dll";

INLINE void ParseFileMapParams(NtleaProcess * process) {
	HANDLE hRshFileMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, NTLEA_PARAMETERS_LENGTH, szRshFileMap);
	if (GetLastError()) { // mapping not found
		LPVOID hView = MapViewOfFile(hRshFileMap, FILE_MAP_ALL_ACCESS, 0, 0, NTLEA_PARAMETERS_LENGTH);
		process->dwCompOption = *((LPDWORD)hView + 0);
		process->dwCodePage = *((LPDWORD)hView + 1);
		process->dwLCID = *((LPDWORD)hView + 2);
		process->dwTimeZone = *((LPDWORD)hView + 3);
		process->dwSpApp = *((LPDWORD)hView + 4);
		lstrcpyA((char*)process->FontFaceName, (char const*)hView + 20);
	}
	else { // 20 BYTES 
		process->dwCompOption = 0;
		process->dwCodePage = 932;
		process->dwLCID = 0x411;
		process->dwTimeZone = (DWORD)-540;
		process->dwSpApp = 100; // ntlea use this value /100 for font-ratio !
		*(LPDWORD)process->FontFaceName = 0; // copy empty 
	}
	CloseHandle(hRshFileMap);
}

INLINE int CreateFileMapInf(NtleaProcess * process, HANDLE hexecute, LPBYTE FileBuffer) {
	DWORD dwread;
	if (ReadFile(hexecute, FileBuffer, 1024, &dwread, NULL) && (*(WORD*)FileBuffer==0x5A4D)) {
		CloseHandle(hexecute);
		// now parse the entrypoint : 
		IMAGE_DOS_HEADER* pdoshead = (IMAGE_DOS_HEADER*)(FileBuffer);
		IMAGE_NT_HEADERS* pntheads = (IMAGE_NT_HEADERS*)(FileBuffer + pdoshead->e_lfanew);
		// check x86 or x64 or ... ??? 
		if (pntheads->FileHeader.Machine == IMAGE_FILE_MACHINE_VALID || 
			pntheads->FileHeader.Machine == IMAGE_FILE_MACHINE_INVALID) {
			// calc the entry point ?? 
			process->ImageBase = (LPVOID)(DWORD_PTR)(pntheads->OptionalHeader.ImageBase);
			process->EntryPoint = (LPVOID)(DWORD_PTR)(pntheads->OptionalHeader.AddressOfEntryPoint);
			// build event & filemapping : 
			process->RcpEvent = CreateEventA(NULL, FALSE, FALSE, szRcpEvent);
			process->RcpFileMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(LPVOID), szRcpFileMap);
			process->FileMappingAddress = MapViewOfFile(process->RcpFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LPVOID));
			// return continue flag or not : 
			return (pntheads->FileHeader.Machine == IMAGE_FILE_MACHINE_VALID) ? (0) : (ERR_PE_FORMAT_INVALID);
		} else {
			return (ERR_UNKNOWN_FORMAT_INVALID);
		}
	} else {
		return (ERR_FAILED_READFILEINFO);
	}
}

INLINE int DeleteFileMapping(NtleaProcess * process) {
	if (process->FileMappingAddress) UnmapViewOfFile(process->FileMappingAddress);
	if (process->RcpFileMap) CloseHandle(process->RcpFileMap);
	if (process->RcpEvent) CloseHandle(process->RcpEvent);
	return (0);
}

// -------------------------------- 

static void PatchAppCheckRunApp(HMODULE hmod) {
	DWORD nNumberOfBytesRead;
	FARPROC fncheck = GetProcAddress(hmod, "ApphelpCheckRunApp");
	if (fncheck) { // do replace this !! 
		if (VirtualProtect((LPVOID)(DWORD_PTR)fncheck, 6, PAGE_EXECUTE_READWRITE, &nNumberOfBytesRead)) {
			// as ntlea said, just hard coded instructions : 
			*(DWORD*)((LPBYTE)(DWORD_PTR)fncheck + 0) = 0xC240C033; // xor eax,eax | inc eax | retn 20
			*(WORD*)((LPBYTE)(DWORD_PTR)fncheck + 4) = 0x20;
			FlushInstructionCache(GetCurrentProcess(), (LPVOID)(DWORD_PTR)fncheck, 6);
		}
	}
	FreeLibrary(hmod); // i'll append this!
}

// ansi version used inside DLL createprocess: 
int CreateProcessPatchA(NtleaProcess * process, char const* lpFileName) {
	BYTE FileBuffer[1024];
	HMODULE hmod = GetModuleHandleW(szAppHelp + 1);
	if (!hmod) {
		LPSTR filepath = (LPSTR)FileBuffer;
		GetSystemDirectoryA(filepath, sizeof(FileBuffer) / sizeof(char)); lstrcatA(filepath, "\\");
		lstrcatA(filepath, lpFileName);
		hmod = LoadLibraryA(filepath);
		if (hmod) PatchAppCheckRunApp(hmod); // i'll append this!
	}
	HANDLE hexecute = CreateFileA((LPCSTR)lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hexecute == INVALID_HANDLE_VALUE) {
		LPSTR lpFilepath = (LPSTR)FileBuffer;
		GetSystemDirectoryA(lpFilepath, MAX_PATH); lstrcatA(lpFilepath, "\\");
		lstrcatA(lpFilepath, lpFileName);
		hexecute = CreateFileA(lpFilepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	}
	if (hexecute == INVALID_HANDLE_VALUE) {
		return ERR_EXECUTABLE_MISSING;
	}
	return CreateFileMapInf(process, hexecute, FileBuffer);
}
// unicode version, function as above 
int CreateProcessPatchW(NtleaProcess * process, wchar_t const* lpFileName) {
	BYTE FileBuffer[1024];
	HMODULE hmod = GetModuleHandleW(szAppHelp + 1);
	if (!hmod) {
		LPWSTR filepath = (LPWSTR)FileBuffer;
		GetSystemDirectoryW(filepath, sizeof(FileBuffer) / sizeof(wchar_t)); lstrcatW(filepath, L"\\"); // required !! 
		lstrcatW(filepath, lpFileName);
		hmod = LoadLibraryW(filepath);
		if (hmod) PatchAppCheckRunApp(hmod);
	}
	HANDLE hexecute = CreateFileW((LPCWSTR)lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hexecute == INVALID_HANDLE_VALUE) {
		LPWSTR lpFilepath = (LPWSTR)FileBuffer;
		GetSystemDirectoryW(lpFilepath, MAX_PATH); lstrcatW(lpFilepath, L"\\"); 
		lstrcatW(lpFilepath, lpFileName);
		hexecute = CreateFileW(lpFilepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	}
	if (hexecute == INVALID_HANDLE_VALUE) {
		return ERR_EXECUTABLE_MISSING;
	}
	return CreateFileMapInf(process, hexecute, FileBuffer);
}
// ansi version prepare executable, check validation then set filemapping for swapping data !
int CreateProcessBeginA(NtleaProcess * process, char const* applicationPath)
{
	ParseFileMapParams(process);
	return CreateProcessStartA(process, applicationPath);
}
int CreateProcessStartA(NtleaProcess * process, char const* applicationPath)
{
	DWORD bintype; HANDLE hexecute; BYTE FileBuffer[1024];
	if (GetBinaryTypeA(applicationPath, &bintype)) {
		hexecute = CreateFileA(applicationPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hexecute == INVALID_HANDLE_VALUE) {
			return ERR_EXECUTABLE_MISSING; // exit !!
		} else {
			return CreateFileMapInf(process, hexecute, FileBuffer);
		}
	} else {
		return ERR_EXECUTABLE_INVALID; // exit !! 
	}
}
int CreateProcessBeginW(NtleaProcess * process, wchar_t const* applicationPath)
{
	ParseFileMapParams(process);
	return CreateProcessStartW(process, applicationPath);
}
int CreateProcessStartW(NtleaProcess * process, wchar_t const* applicationPath)
{
	DWORD bintype; HANDLE hexecute; BYTE FileBuffer[1024];
	if (GetBinaryTypeW(applicationPath, &bintype)) {
		hexecute = CreateFileW(applicationPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hexecute == INVALID_HANDLE_VALUE) {
			return ERR_EXECUTABLE_MISSING; // exit !!
		}
		else {
			return CreateFileMapInf(process, hexecute, FileBuffer);
		}
	}
	else {
		return ERR_EXECUTABLE_INVALID; // exit !! 
	}
}

// first see MSDN : http://msdn.microsoft.com/en-us/library/windows/desktop/ms684280(v=vs.85).aspx
// read process address, see also : http://www.cnblogs.com/jeJee/archive/2013/03/08/2950345.html
// for more info, see : http://stackoverflow.com/questions/8447801/getting-a-module-handle-from-other-process
// and : http://stackoverflow.com/questions/8336214/how-can-i-get-a-process-entry-point-address
typedef struct {
	DWORD_PTR ExitStatus;
	DWORD_PTR PebBaseAddress;
	DWORD_PTR AffinityMask;
	DWORD_PTR BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

typedef enum {
	ProcessBasicInformation = 0,
	ProcessDebugPort = 7,
	ProcessWow64Information = 26,
	ProcessImageFileName = 27,
	ProcessBreakOnTermination = 28,
} PROCESSINFOCLASS;

INLINE int ResetBaseAddress(NtleaProcess * process, PROCESS_INFORMATION const* proinfo) {
	PROCESS_BASIC_INFORMATION pbi;
	SIZE_T bytesRead;
	DWORD_PTR baseAddr;
	// ------------------------------- 
	typedef NTSTATUS(WINAPI *PROCNTQSIP)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
	PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(GetModuleHandleA("ntdll"), "NtQueryInformationProcess");
	if (!NtQueryInformationProcess) { return ERR_FAILED_QUERY_PROCESS; }
	// Retrieve information : 
	NTSTATUS ret = NtQueryInformationProcess(proinfo->hProcess, ProcessBasicInformation, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
	fprintf(stderr, "QueryProcess info pointer : %p, ret : %d\n", NtQueryInformationProcess, ret);
	if (ret >= 0) {
		ReadProcessMemory(proinfo->hProcess, (PVOID)(pbi.PebBaseAddress + 2 * sizeof(PVOID)), &baseAddr, sizeof(baseAddr), &bytesRead);
		process->EntryPoint = (LPBYTE)process->EntryPoint + baseAddr;
		fprintf(stderr, "ProcessID : %u, EntryPoint_1 : %p\n", proinfo->dwProcessId, process->EntryPoint);
	} else {
		process->EntryPoint = (LPBYTE)process->EntryPoint + (DWORD_PTR)process->ImageBase;
		fprintf(stderr, "ProcessID : %u, EntryPoint_0 : %p\n", proinfo->dwProcessId, process->EntryPoint);
	}
	return (0);
}

INLINE HANDLE FindFileExists(LPCWSTR Path) {
	WIN32_FIND_DATAW f; return FindFirstFileW(Path, &f);
}

INLINE UINT FindHookDLL(LPWSTR CurrentDir, DWORD MaxPath, HMODULE hInstance) {
	extern LPCWSTR szRcpHookDLL; // required outer module support !!
	UINT PathLength = 0; HANDLE hfile = INVALID_HANDLE_VALUE;
	// search in order : 
	if (hfile == INVALID_HANDLE_VALUE) { // 1. ntlea original 
		PathLength = GetWindowsDirectoryW(CurrentDir, MaxPath);
		lstrcatW(CurrentDir, szRcpHookDLL); // check DLL : 
		hfile = FindFileExists(CurrentDir);
	}
	if (hfile == INVALID_HANDLE_VALUE) { // 2. ntlea original 
		PathLength = GetCurrentDirectoryW(MaxPath, CurrentDir);
		lstrcatW(CurrentDir, szRcpHookDLL);
		hfile = FindFileExists(CurrentDir);
	}
	if (hfile == INVALID_HANDLE_VALUE) { // 3. add-on current exe file path
		LPCWSTR p = GetModuleFileNameW(hInstance, CurrentDir, MaxPath) + CurrentDir;
		while (p > CurrentDir && *p != L'\\') --p;
		if (*p == L'\\') {
			PathLength = (UINT)(p - CurrentDir); CurrentDir[PathLength] = L'\0'; // make as tail 
			lstrcatW(CurrentDir, szRcpHookDLL);
			hfile = FindFileExists(CurrentDir);
		}
	}
	if (hfile != INVALID_HANDLE_VALUE) FindClose/*CloseHandle*/(hfile);
	else {
		fprintf(stderr, "can't find %S anywhere!\n", (szRcpHookDLL + 1));
		PathLength = 0; // clear to 0 !! 
	}
	return PathLength;
}

int ResetWorkingDirectory(HANDLE hprocess) {
	WCHAR exepath[MAX_PATH*2]; DWORD len = ARRAYSIZE(exepath); // i think it should be enough ??
#if 1
	BOOL(WINAPI *pfnQueryFullProcessImageName)(HANDLE,DWORD,LPWSTR,PDWORD);
	pfnQueryFullProcessImageName = (BOOL(WINAPI*)(HANDLE, DWORD, LPWSTR, PDWORD))
		GetProcAddress(LoadLibraryW(L"kernel32.dll"), "QueryFullProcessImageNameW");
	// ----------------------------- XP won't support this, but just skip that's ok 
	if (pfnQueryFullProcessImageName && pfnQueryFullProcessImageName(hprocess, 0, exepath, &len))
#else
	DWORD(WINAPI *pfnGetModuleFileNameEx)(HANDLE hProcess,HMODULE hModule,LPWSTR lpFilename,DWORD nSize);
	HMODULE hModuleLib = LoadLibraryW(L"psapi.dll");
	pfnGetModuleFileNameEx = (DWORD(WINAPI*)(HANDLE,HMODULE,LPWSTR,DWORD))GetProcAddress(hModuleLib, "GetModuleFileNameExW");
	len = pfnGetModuleFileNameEx(hProcess, NULL, exepath, len);
	FreeLibrary(hModuleLib);
	// ----------------------------- 
	if (len > 0)
#endif
	{
		LPCWSTR p = len + exepath;
		while (p > exepath && *p != L'\\') --p;
		if (*p == L'\\') {
			exepath[p - exepath] = L'\0'; // make as tail 
			SetCurrentDirectoryW(exepath);
			fprintf(stderr, "set current working directory to %S\n", exepath);
		}
	}
	return (0);
}

int InjectProcessDLL(NtleaProcess const* process, PROCESS_INFORMATION const* proinfo)
{
	// ParameterExists : CurrentDir with PathLength + szRcpHookDLL, then : FontFaceName with strlen, 4 * DWORD
	WCHAR CurrentDir[MAX_PATH + (NTLEA_PARAMETERS_LENGTH / 2)];
	// Search Directories : 
	UINT PathLength = FindHookDLL(CurrentDir, MAX_PATH, process->hInstance);
	if (PathLength == 0) { return ERR_NTLEA_DLL_MISSING; } // can't find dll !
	// ----------------------- 
	LPBYTE p = (LPBYTE)(CurrentDir + lstrlenW(CurrentDir) + 1);
	lstrcpyA((LPSTR)p, (LPSTR)process->FontFaceName);
	p += lstrlenA((LPSTR)process->FontFaceName) + 1;
	*((LPDWORD)p) = process->dwCompOption;	p += sizeof(DWORD);
	*((LPDWORD)p) = process->dwCodePage;	p += sizeof(DWORD);
	*((LPDWORD)p) = process->dwLCID;		p += sizeof(DWORD);
	*((LPDWORD)p) = process->dwTimeZone;	p += sizeof(DWORD);
	// allocate memory for remote-process DLL parameters : 
	DWORD length = (DWORD)(p - (LPBYTE)CurrentDir);
	LPVOID BaseAddress = VirtualAllocEx(proinfo->hProcess, NULL, length, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!BaseAddress) {
		return ERR_FAILED_ALLOCATE_MEM; // failed exit 
	}
	*(LPVOID*)process->FileMappingAddress = BaseAddress; // required for other thread usage ?? 
	WriteProcessMemory(proinfo->hProcess, BaseAddress, CurrentDir, length, NULL);
	// why [ecx(LoadLibraryW) + 2] for LoadLibraryW in NTLEA ???
	HANDLE hRemoteThread; DWORD dwRemoteThreadId;
	hRemoteThread = CreateRemoteThread(proinfo->hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, BaseAddress, 0, &dwRemoteThreadId);
	fprintf(stderr, "remote thread handle : 0x%p, threadid : %u\n", hRemoteThread, dwRemoteThreadId);
	CloseHandle(hRemoteThread);
	// ----------------------- 
	return (0);
}

static void checkNtleaSignal(void)
{
	HANDLE ntleasig = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"ntleasig");
	if (ntleasig) { WaitForSingleObject(ntleasig, INFINITE); CloseHandle(ntleasig); }
}

int CreateProcessEnd(NtleaProcess * process, PROCESS_INFORMATION const* proinfo, BOOL bSuspendFlag)
{
	static BYTE const BreakPoint[] = { 0xEB, 0xFE, };
	BYTE instrcache[sizeof(BreakPoint)];
	int ret;

	// just wait for the initial breakpoint : 
	ret = ResetBaseAddress(process, proinfo);
	if (ret < 0) {
		fprintf(stderr, "failed rebase address ret = %d!\n", ret);
		return ret;
	}

	// try to add an breakpoint at the entrypoint !
	ReadProcessMemory(proinfo->hProcess, process->EntryPoint, instrcache, sizeof(instrcache), NULL);
	WriteProcessMemory(proinfo->hProcess, process->EntryPoint, BreakPoint, sizeof(BreakPoint), NULL);
	FlushInstructionCache(proinfo->hProcess, process->EntryPoint, sizeof(instrcache));

	// wait thread arrived their ?? 
	ResumeThread(proinfo->hThread);
	Sleep(32); // sleep shorter time 
	SuspendThread(proinfo->hThread);
	while (proinfo->hThread == proinfo->hThread/*true*/) { // Wow64GetThreadContext
		CONTEXT threadctx = { 0, };
		// see : http://stackoverflow.com/questions/11396034/cant-get-thread-context-from-a-windows-64-bit-process
		// ContextFlags on x64 offset no longer at the very beginning !
		threadctx.ContextFlags = CONTEXT_CONTROL;
		if (!GetThreadContext(proinfo->hThread, &threadctx)) {
			fprintf(stderr, "Failed GetThreadContext : %u\n", GetLastError());
			return ERR_HOOKPROCESS_MISSING;
		}
		ResumeThread(proinfo->hThread);
		Sleep(32); // sleep shorter time 
		SuspendThread(proinfo->hThread);
		// AVC said that, x64 version of GetThreadContext may not require the correct value if different sys-bandwidth.
		if (threadctx.RegIP == (DWORD_PTR)process->EntryPoint)
			break; // X86 only
	}

	if (!process->hInstance) { checkNtleaSignal(); }

	// inject DLL : 
	ret = InjectProcessDLL(process, proinfo);
	if (ret < 0) {
		fprintf(stderr, "failed inject DLL ret = %d!\n", ret);
		return ret;
	}

	// wait remote process hooking prepared : 
	if (WAIT_OBJECT_0 == WaitForSingleObject(process->RcpEvent, 30 * 1000)) {
		fprintf(stderr, "success wait signal!\n");
	}
	else {
		fprintf(stderr, "failed wait signal timeout!\n");
	}

	if (!process->hInstance) { checkNtleaSignal(); }

	// free all resource : 
	DeleteFileMapping(process);
	// put back the instruction : 
	WriteProcessMemory(proinfo->hProcess, process->EntryPoint, instrcache, sizeof(instrcache), NULL);
	FlushInstructionCache(proinfo->hProcess, process->EntryPoint, sizeof(instrcache));

	if (!bSuspendFlag) ResumeThread(proinfo->hThread);

	return (0);
}

int CreateProcessEndExt(NtleaProcess * process, PROCESS_INFORMATION const* proinfo, BOOL bSuspendFlag)
{
	static BYTE const BreakPoint[] = { 0xCC, 0xCC, 0xCC, 0xCC, };
	BYTE instrcache[sizeof(BreakPoint)];
	DEBUG_EVENT dbgevent;
	int ret, fin = 0;
	void* dbgstrbuf = HeapAlloc(GetProcessHeap(), 0, 1024);

	// just wait for the initial breakpoint : 
	ret = ResetBaseAddress(process, proinfo);
	if (ret < 0) {
		fprintf(stderr, "failed rebase address ret = %d!\n", ret);
		return ret;
	}

//	DebugActiveProcess(proinfo->dwProcessId);
	// just wait for the initial breakpoint : 
	while (!fin) {
		ret = WaitForDebugEvent(&dbgevent, 25);
		if (ret) {
			switch (dbgevent.dwDebugEventCode)
			{
			case CREATE_PROCESS_DEBUG_EVENT:
			{ // prepare ! ParameterExists :
				fprintf(stderr, "proc start address %p vs calc address %p\n", 
					(LPVOID)(DWORD_PTR)dbgevent.u.CreateProcessInfo.lpStartAddress, process->EntryPoint);
				// try to add an breakpoint at the entrypoint !
				ReadProcessMemory(proinfo->hProcess, process->EntryPoint, instrcache, sizeof(instrcache), NULL);
				WriteProcessMemory(proinfo->hProcess, process->EntryPoint, BreakPoint, sizeof(BreakPoint), NULL);
				// inject DLL : 
				InjectProcessDLL(process, proinfo);
				// now continue ?? 
				ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_CONTINUE);
			}	break;
		/*	case CREATE_THREAD_DEBUG_EVENT:
			{
				CREATE_THREAD_DEBUG_INFO* dbginfo = &dbgevent.u.CreateThread;
				fprintf(stderr, "threadstartup baseaddr : 0x%p, handle : 0x%p, ID : %u\n", dbginfo->lpStartAddress, dbginfo->hThread, dbgevent.dwThreadId);
				ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_CONTINUE);
			}	break;
			case EXIT_THREAD_DEBUG_EVENT:
			{
				EXIT_THREAD_DEBUG_INFO* dbginfo = &dbgevent.u.ExitThread;
				fprintf(stderr, "threadexit code : %u, ID : %u\n", dbginfo->dwExitCode, dbgevent.dwThreadId);
				ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_CONTINUE);
			}	break; */
			case LOAD_DLL_DEBUG_EVENT:
			{
				LOAD_DLL_DEBUG_INFO * dbginfo = &dbgevent.u.LoadDll;
			/*	if (dbginfo->lpImageName) {
					void* lpImageNameStr = NULL;
					ReadProcessMemory(proinfo->hProcess, dbginfo->lpImageName, &lpImageNameStr, sizeof(lpImageNameStr), NULL);
					fprintf(stderr, (dbginfo->fUnicode ? "loadDLLevent %S basedlladdr : 0x%p, ID : %u\n" 
						: "loadDLLevent %s basedlladdr : 0x%p, ID : %u\n"), lpImageNameStr, dbginfo->lpBaseOfDll, dbgevent.dwThreadId);
				}*/
				// we have the responsibility to cleanup the handle of DLL file : 
				if (dbginfo->hFile) CloseHandle(dbginfo->hFile);
				ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_CONTINUE);
			}	break;
			case EXCEPTION_DEBUG_EVENT:
			{
				EXCEPTION_DEBUG_INFO * dbginfo = &dbgevent.u.Exception;
				// EXCEPTION_ACCESS_VIOLATION
				fprintf(stderr, "Exception code : 0x%08X, ID : %u, Address : 0x%p\n", 
					dbginfo->ExceptionRecord.ExceptionCode, dbgevent.dwThreadId,
					dbginfo->ExceptionRecord.ExceptionAddress);
				// EXCEPTION_BREAKPOINT 0x80000003 
				if (EXCEPTION_BREAKPOINT == dbginfo->ExceptionRecord.ExceptionCode) {
					// we check breakpoint interrupt, then 
					if (dbgevent.u.Exception.ExceptionRecord.ExceptionAddress == (LPVOID)(DWORD_PTR)process->EntryPoint) {
						assert(dbgevent.dwThreadId == proinfo->dwThreadId);
						SuspendThread(proinfo->hThread);
						// put back the instruction : 
						WriteProcessMemory(proinfo->hProcess, process->EntryPoint, instrcache, sizeof(instrcache), NULL);
						FlushInstructionCache(proinfo->hProcess, process->EntryPoint, sizeof(instrcache));
						// put back the eip : 
						CONTEXT threadctx = { CONTEXT_CONTROL, };
						if (GetThreadContext(proinfo->hThread, &threadctx)) { // Wow64GetThreadContext
							fprintf(stderr, "EIP at 0x%p to 0x%p\n", threadctx.RegIP, process->EntryPoint);
							threadctx.RegIP = (DWORD)(DWORD_PTR)process->EntryPoint; // X86 only ?? 
							SetThreadContext(proinfo->hThread, &threadctx);
						}
					}
					ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_CONTINUE);
				} else {
					switch (dbginfo->ExceptionRecord.ExceptionCode) {
					case EXCEPTION_ACCESS_VIOLATION:
						fprintf(stderr, "error : EXCEPTION_ACCESS_VIOLATION(chance=%u)\n", dbginfo->dwFirstChance);
						break;
					case EXCEPTION_STACK_OVERFLOW:
						fprintf(stderr, "error EXCEPTION_STACK_OVERFLOW(chance=%u)\n", dbginfo->dwFirstChance);
						break;
					case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
					case EXCEPTION_DATATYPE_MISALIGNMENT:
					case EXCEPTION_FLT_DENORMAL_OPERAND:
					case EXCEPTION_FLT_DIVIDE_BY_ZERO:
					case EXCEPTION_FLT_INEXACT_RESULT:
					case EXCEPTION_FLT_INVALID_OPERATION:
					case EXCEPTION_FLT_OVERFLOW:
					case EXCEPTION_FLT_STACK_CHECK:
					case EXCEPTION_FLT_UNDERFLOW:
						fprintf(stderr, "error : EXCEPTION_FLT_XXX(code=%08X)\n", dbginfo->ExceptionRecord.ExceptionCode);
						break;
					case EXCEPTION_ILLEGAL_INSTRUCTION:
					case EXCEPTION_IN_PAGE_ERROR:
					case EXCEPTION_INT_DIVIDE_BY_ZERO:
					case EXCEPTION_INT_OVERFLOW:
					case EXCEPTION_INVALID_DISPOSITION:
					case EXCEPTION_NONCONTINUABLE_EXCEPTION:
					case EXCEPTION_PRIV_INSTRUCTION:
					case EXCEPTION_SINGLE_STEP:
						fprintf(stderr, "error : exception other(code=%08X)\n", dbginfo->ExceptionRecord.ExceptionCode);
						break;
					default:
						fprintf(stderr, "error : unknown(code=%08X)\n", dbginfo->ExceptionRecord.ExceptionCode);
						break;
					}
					ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
					fin = 1;
				}
			}	break;
			case EXIT_PROCESS_DEBUG_EVENT:
			{
			//	fprintf(stderr, "Debugging Process Exit with %u(0x%08X)!\n", dbgevent.u.ExitProcess.dwExitCode, dbgevent.u.ExitProcess.dwExitCode);
				fin = 1; // breakout !!
				ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_CONTINUE);
			}	break;
			case OUTPUT_DEBUG_STRING_EVENT:
			{
				OUTPUT_DEBUG_STRING_INFO* dbginfo = (OUTPUT_DEBUG_STRING_INFO*)&dbgevent.u.DebugString;
				unsigned char *msg = dbgstrbuf;
				// read debug string from debugee process :
				ReadProcessMemory(proinfo->hProcess, dbginfo->lpDebugStringData, msg, min(1024, dbginfo->nDebugStringLength), NULL);
				fprintf(stderr, dbginfo->fUnicode?"%S\n":"%s\n", msg);
				ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_CONTINUE);
			}	break;
			default:
				ContinueDebugEvent(dbgevent.dwProcessId, dbgevent.dwThreadId, DBG_CONTINUE);
				break;
			}
		}
		if (WAIT_OBJECT_0 == WaitForSingleObject(process->RcpEvent, 0)) {
			DeleteFileMapping(process);
		//	fin = 1;
			if (!bSuspendFlag) ResumeThread(proinfo->hThread);
		} else { SetLastError(ERROR_SUCCESS); }
	}
	HeapFree(GetProcessHeap(), 0, dbgstrbuf);

//	DebugActiveProcessStop(proinfo->dwProcessId);

	return (0);
}

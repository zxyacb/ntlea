
#ifndef __NTPROC_H__
#define __NTPROC_H__

#include <crtdefs.h>

typedef enum {
	ERR_UKNOWN = -10000,
	ERR_PE_FORMAT_INVALID		= -10001,
	ERR_MULTIPLE_INSTANCE		= -10002,
	ERR_EXECUTABLE_MISSING		= -10003,
	ERR_EXECUTABLE_INVALID		= -10004,
	ERR_PROCESS_CREATE_FAILED	= -10005,
	ERR_NTLEA_DLL_MISSING		= -10006,
	ERR_FAILED_ALLOCATE_MEM		= -10007,
	ERR_HOOKPROCESS_MISSING		= -10008,
	ERR_FAILED_QUERY_PROCESS	= -10009,
	ERR_FAILED_READFILEINFO		= -10010,
	ERR_UNKNOWN_FORMAT_INVALID	= -10011,
	// ----- 
	ERR_REDIRECTED_PLATFORM		=  10001,
} NtleaErrorNo;

typedef struct {
	// -------------- inputparam 
	DWORD dwCompOption;
	DWORD dwCodePage;
	DWORD dwLCID;
	DWORD dwTimeZone;
	DWORD dwSpApp;
	BYTE FontFaceName[LF_FACESIZE];
	// -------------- internal 
	HANDLE RcpEvent, RcpFileMap; // HANDLE
	LPVOID FileMappingAddress, ImageBase, EntryPoint; // LPVOID
	HMODULE hInstance;
	// ... 
} NtleaProcess;

int CreateProcessPatchA(NtleaProcess * process, char const* lpFileName);
int CreateProcessPatchW(NtleaProcess * process, wchar_t const* lpFileName);
int CreateProcessBeginA(NtleaProcess * process, char const* applicationPath);
int CreateProcessBeginW(NtleaProcess * process, wchar_t const* applicationPath);
int CreateProcessStartA(NtleaProcess * process, char const* applicationPath);
int CreateProcessStartW(NtleaProcess * process, wchar_t const* applicationPath);
int CreateProcessEnd(NtleaProcess * process, PROCESS_INFORMATION const* proinfo, BOOL bSuspendFlag);
int CreateProcessEndExt(NtleaProcess * process, PROCESS_INFORMATION const* proinfo, BOOL bSuspendFlag);
int ResetWorkingDirectory(HANDLE hprocess);

#endif // __NTPROC_H__

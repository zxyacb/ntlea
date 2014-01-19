
;=======================================================================
;NT Locale Emulator Advance  ver0.70 beta
;Written by 吃软饭的！(LOVEHINA-AVC) 2006.11
;-----------------------------------------------------------------------
;Develop environment : Windows XP Pro SP2 & MASM v9.0 with OllyDbg v1.10
;=======================================================================
;
;Source file name
;
;    rcpcore.asm
;
;Module name:
;
;    ntleac.exe
;
;Author:
;
;    Advance  30/11/2006
;
;Execution environment:
;
;    Win32 subsystem user mode only. The "Users" privilege is required.
;
;Remark:
;
;    The launcher.



.386
.model flat,stdcall
option casemap:none

include \masm32\include\windows.inc
include \masm32\include\kernel32.inc
includelib \masm32\lib\kernel32.lib
include \masm32\include\shell32.inc
includelib \masm32\lib\shell32.lib
include \masm32\include\user32.inc
includelib \masm32\lib\user32.lib
include \masm32\include\comdlg32.inc
includelib \masm32\lib\comdlg32.lib

.CONST

	MAX_APPLICATION_SUPPORTED    equ    2
	NTLEA_PARAMETERS_LENGTH      equ    LF_FACESIZE + 20

.DATA

ALIGN 4

	pCommandLine         dd    0

	SpAppHandlerTable    dd    OFFSET SpAppMuvluv12
                       dd    OFFSET SpAppMuvluvAlt

	szSASLFileName  db    "ntleasl.dat",0
	szRcpHookDLL    db    "\",0,"n",0,"t",0,"l",0,"e",0,"a",0,"h",0,".",0,"d",0,"l",0,"l",0,0,0
	szFilter        db    "E",0,"x",0,"e",0,"c",0,"u",0,"t",0,"a",0,"b",0,"l",0,"e",0," ",0,"F",0,"i",0,"l",0,"e",0,"(",0
	                db    "*",0,".",0,"e",0,"x",0,"e",0,")",0,0,0,"*",0,".",0,"e",0,"x",0,"e",0,0,0,"A",0,"l",0,"l",0
	                db    " ",0,"f",0,"i",0,"l",0,"e",0,"s",0,"(",0,"*",0,".",0,"*",0,")",0,0,0,"*",0,".",0,"*",0,0,0,0,0
	szRcpIntMtx     db    "RcpInternalMutex",0
	szRcpEvent      db    "RcpEvent000",0
	szRcpFileMap    db    "RcpFileMap000",0
	szRshFileMap    db    "RshFileMap000",0
	szCaption       db    "NT Locale Emulator Advance",0
	szErrorCancel   db    "An error was occurred, or the operation has been cancelled by the user currently.",0
	szFailedToCreateProcess db "Exception : Unable to create the process.",0
	szNotExecutableFile     db "Exception : The specified file is not an executable file.",0
	szFailedToAcquireMutex  db "Exception : NTLEA could not acquire the mutual exclusion lock for the critical section, a previous NTLEA process is running.",0
	
	szConMessage    db    "确认信息",0
	szSpAppOption   db    "您设置了针对特定应用程序的兼容性选项，并且调用进程通过ANSI字符串来传递参数。"
	                db    "为了避免可能产生的异常，您需要再次给出包含目标应用程序的文件夹路径，该路径通常与您在NTLEA GUI中指定的路径相同。"
	                db    0Dh,0Ah,0Dh,0Ah,"点击“确定”按钮继续下一步的操作",0

	
.CODE

start:
	
	call	RcpLauncherStartup
	invoke	ExitProcess,eax
	
	
RcpLauncherStartup    PROC

	LOCAL    BaseAddress : DWORD
	LOCAL    dwCompOption : DWORD
	LOCAL    dwCodePage : DWORD
	LOCAL    dwLCID : DWORD
	LOCAL    dwSpApp : DWORD
	LOCAL    EntryPoint : LPVOID
	LOCAL    FileMappingAddress : LPVOID
	LOCAL    ArgumentCounter : UINT
	LOCAL    PathLength : UINT
	LOCAL    dwTimeZone : LONG
	LOCAL    RcpEvent : HANDLE
	LOCAL    RcpFileMap : HANDLE
	LOCAL    FileHandle : HANDLE
	LOCAL    ProcInfo : PROCESS_INFORMATION
	LOCAL    FontFaceName[LF_FACESIZE] : BYTE
	LOCAL    StartInfo : STARTUPINFO
	LOCAL    ofn : OPENFILENAME
	LOCAL    CurrentDir[MAX_PATH + (NTLEA_PARAMETERS_LENGTH / 2)] : WORD
	LOCAL    PathBuffer[MAX_PATH] : WORD
	LOCAL    PathBufferTemp[MAX_PATH] : WORD
	LOCAL    FileBuffer[1024] : BYTE
	LOCAL    ThreadContext : CONTEXT

	xor	edi,edi
	invoke	CreateMutex,edi,edi,ADDR szRcpIntMtx
	invoke	GetLastError
	cmp	eax,edi
	je	@f
	push	MB_ICONSTOP
	push	OFFSET szCaption
	push	OFFSET szFailedToAcquireMutex
	push	edi
	call	MessageBox
	ret
	
@@:
	
	invoke	CreateFileMapping,INVALID_HANDLE_VALUE,edi,PAGE_READWRITE,edi,NTLEA_PARAMETERS_LENGTH,ADDR szRshFileMap
	mov	esi,eax
	invoke	GetLastError
	cmp	eax,edi
	je	MappingNotFound
	invoke	MapViewOfFile,esi,FILE_MAP_ALL_ACCESS,edi,edi,NTLEA_PARAMETERS_LENGTH
	mov	ecx,[eax]
	mov	edx,[eax + 4]
	mov	[dwCompOption],ecx
	mov	[dwCodePage],edx
	mov	ecx,[eax + 8]
	mov	edx,[eax + 12]
	mov	[dwLCID],ecx
	mov	[dwTimeZone],edx
	mov	ecx,[eax + 16]
	push	eax
	mov	[dwSpApp],ecx
	lea	ecx,[FontFaceName]
	
@@:
	
	mov	dl,[eax + edi + 20]
	mov	[ecx + edi],dl
	inc	edi
	test	dl,dl
	jnz	@b
	call	UnmapViewOfFile
	jmp	ParameterExists
	
MappingNotFound:

	mov	[dwCodePage],932
	mov	[dwLCID],411h
	mov	[dwCompOption],edi
	mov	[dwSpApp],edi
	mov	[dwTimeZone],-540
	mov	DWORD PTR[FontFaceName],edi
	
ParameterExists:

	invoke	CloseHandle,esi
	lea	esi,[CurrentDir]
	invoke	GetWindowsDirectoryW,esi,MAX_PATH
	mov	[PathLength],eax
	lea	ecx,[esi + eax * 2]
	xor	eax,eax
	
@@:
	
	mov	edx,[OFFSET szRcpHookDLL + eax * 4]
	mov	[ecx + eax * 4],edx
	inc	eax
	cmp	eax,6
	jne	@b
	xor	ecx,ecx
	invoke	CreateFileW,esi,ecx,ecx,ecx,OPEN_EXISTING,ecx,ecx
	cmp	eax,INVALID_HANDLE_VALUE
	je	@f
	push	eax
	call	CloseHandle
	jmp	HookDLLFound
	
@@:
	
	invoke	GetCurrentDirectoryW,MAX_PATH,esi
	mov	[PathLength],eax
	
HookDLLFound:
	
	lea	esi,[ArgumentCounter]
	invoke	GetCommandLineW
	invoke	CommandLineToArgvW,eax,esi
	test	eax,eax
	mov	ecx,[esi]
	lea	edi,[PathBuffer]
	jz	CommandLineEmpty
	dec	ecx
	jz	CommandLineEmpty
	mov	esi,[eax + 4]
	xor	ebx,ebx
	xor	edx,edx
	
@@:
	
	mov	dx,[esi + ebx]
	mov	[edi + ebx],dx
	add	ebx,2
	test	edx,edx
	jnz	@b
	dec	ecx
	
TraversalArgsRepeat:
	
	dec	ecx
	test	ecx,ecx
	jl	TraversalArgsEnd
	mov	esi,[eax + ecx * 4 + 8]
	mov	dl,[esi]
	add	esi,2
	cmp	dl,'P'
	je	ParameterCompOption
	cmp	dl,'C'
	je	ParameterCodePage
	cmp	dl,'L'
	je	ParameterLocaleId
	cmp	dl,'T'
	je	ParameterTimeZone
	cmp	dl,'S'
	je	ParameterSpApp
	jmp	TraversalArgsRepeat
	
ParameterCompOption:

	call	GetParameterValue
	mov	[dwCompOption],ebx
	jmp	TraversalArgsRepeat
	
ParameterCodePage:

	call	GetParameterValue
	mov	[dwCodePage],ebx
	jmp	TraversalArgsRepeat
	
ParameterLocaleId:

	call	GetParameterValue
	mov	[dwLCID],ebx
	jmp	TraversalArgsRepeat
	
ParameterTimeZone:

	call	GetParameterValue
	mov	[dwTimeZone],ebx
	jmp	TraversalArgsRepeat
	
ParameterSpApp:

	call	GetParameterValue
	mov	[dwSpApp],ebx
	jmp	TraversalArgsRepeat
	
GetParameterValue:
	
	push	ecx
	xor	ebx,ebx
	xor	ecx,ecx
	
GetParameterRepeat:
	
	mov	dl,[esi]
	add	esi,2
	cmp	dl,'-'
	jne	@f
	inc	ecx
	jmp	GetParameterRepeat
	
@@:
	
	cmp	dl,'0'
	jb	@f
	cmp	dl,'9'
	ja	@f
	sub	dl,'0'
	lea	ebx,[ebx + ebx * 4]
	lea	ebx,[edx + ebx * 2]
	jmp	GetParameterRepeat
	
@@:

	test	ecx,ecx
	jz	@f
	neg	ebx
	
@@:

	pop	ecx
	retn

CommandLineEmpty:

	xor	eax,eax
	lea	edi,[ofn]
	mov	ecx,SIZEOF ofn / 4
	push	edi
	rep	stosd
	lea	edi,[PathBuffer]
	pop	ecx
	mov	[edi],ax
	mov	[ecx + OPENFILENAME.lStructSize],SIZEOF ofn
	mov	[ecx + OPENFILENAME.lpstrFilter],OFFSET szFilter
	inc	[ecx + OPENFILENAME.nFilterIndex]
	mov	[ecx + OPENFILENAME.lpstrFile],edi
	mov	[ecx + OPENFILENAME.nMaxFile],MAX_PATH
	mov	[ecx + OPENFILENAME.Flags],OFN_EXPLORER OR OFN_PATHMUSTEXIST OR OFN_FILEMUSTEXIST OR OFN_HIDEREADONLY
	invoke	GetOpenFileNameW,ecx
	test	eax,eax
	jz	NullPath

TraversalArgsEnd:	
GotPath:

	mov	eax,[dwSpApp]
	lea	esi,[PathBufferTemp]
	test	eax,eax
	je	@f
	cmp	eax,MAX_APPLICATION_SUPPORTED
	ja	@f
	dec	eax
	lea	ebx,[dwCodePage]
	call	[SpAppHandlerTable + eax * 4]
	jmp	SpAppEnd
	
@@:
	
	push	ecx
	push	esp
	push	esi
	push	MAX_PATH
	push	edi
	call	GetFullPathNameW
	test	eax,eax
	pop	ecx
	jz	FailedToCreateProcess
	xor	ebx,ebx
	inc	eax

@@:

	mov	dx,[esi + ebx]
	mov	[edi + ebx],dx
	add	ebx,2
	dec	eax
	jnz	@b
	mov	[ecx],eax
	
SpAppEnd:

	push	eax
	invoke	GetBinaryTypeW,edi,esp
	test	eax,eax
	pop	ecx
	jz	NotExecutableFile
	test	ecx,ecx
	jnz	NotExecutableFile
	invoke	CreateFileW,edi,GENERIC_READ,FILE_SHARE_READ,ecx,OPEN_EXISTING,ecx,ecx
	cmp	eax,INVALID_HANDLE_VALUE
	mov	[FileHandle],eax
	jz	FailedToCreateProcess
	lea	ecx,[EntryPoint]
	lea	ebx,[FileBuffer]
	invoke	ReadFile,eax,ebx,1024,ecx,NULL
	invoke	CloseHandle,[FileHandle]
	mov	eax,[ebx + 3Ch]
	mov	ecx,[ebx + eax + IMAGE_NT_HEADERS.OptionalHeader.AddressOfEntryPoint]
	add	ecx,[ebx + eax + IMAGE_NT_HEADERS.OptionalHeader.ImageBase]
	xor	eax,eax
	mov	[EntryPoint],ecx
	invoke	CreateEvent,eax,eax,eax,ADDR szRcpEvent
	test	eax,eax
	mov	[RcpEvent],eax
	jz	FailedToCreateProcess
	invoke	GetLastError
	test	eax,eax
	jnz	FailedToCreateProcess
	invoke	CreateFileMapping,INVALID_HANDLE_VALUE,eax,PAGE_READWRITE,eax,SIZEOF DWORD,ADDR szRcpFileMap
	test	eax,eax
	mov	[RcpFileMap],eax
	jz	FailedToCreateProcess
	xor	ecx,ecx
	invoke	MapViewOfFile,eax,FILE_MAP_ALL_ACCESS,ecx,ecx,SIZEOF DWORD
	lea	ebx,[StartInfo]
	mov	[FileMappingAddress],eax
	mov	[ebx + STARTUPINFO.cb],SIZEOF STARTUPINFO
	push	ebx
	call	GetStartupInfoW
	lea	eax,[ProcInfo]
	mov	edx,[pCommandLine]
	xor	ecx,ecx
	push	eax
	push	ebx
	push	esi
	push	ecx
	push	CREATE_SUSPENDED
	push	ecx
	push	ecx
	push	ecx
	push	edx
	push	edi	
	call	CreateProcessW
	test	eax,eax
	jz	FailedToCreateProcess
	mov	ebx,[ProcInfo.hProcess]
	lea	esi,[PathBufferTemp]
	invoke	ReadProcessMemory,ebx,[EntryPoint],esi,2,NULL
	mov	WORD PTR[edi],0FEEBh ; jmp [eip-2]
	invoke	WriteProcessMemory,ebx,[EntryPoint],edi,2,NULL
	invoke	FlushInstructionCache,ebx,[EntryPoint],2
	mov	esi,[ProcInfo.hThread]
	lea	edi,[ThreadContext]
	invoke	ResumeThread,esi
	mov	[edi + CONTEXT.ContextFlags],CONTEXT_CONTROL
	
@@:

	invoke	Sleep,100
	invoke	SuspendThread,esi
	invoke	GetThreadContext,esi,edi
	invoke	ResumeThread,esi
	mov	eax,[EntryPoint]
	cmp	[edi + CONTEXT.regEip],eax
	jne	@b
	lea	edi,[CurrentDir]
	mov	esi,[PathLength]
	lea	esi,[edi + esi * 2]
	xor	eax,eax
	
@@:
	
	mov	edx,[OFFSET szRcpHookDLL + eax * 4]
	mov	[esi + eax * 4],edx
	inc	eax
	cmp	eax,6
	jne	@b
	lea	esi,[esi + eax * 4]
	lea	edx,[FontFaceName]
	
@@:
	
	mov	cl,[edx]
	mov	[esi],cl
	inc	edx
	inc	esi
	test	cl,cl
	jnz	@b
	mov	ecx,[dwCodePage]
	mov	eax,[dwCompOption]
	mov	[esi + 4],ecx
	mov	[esi],eax
	mov	ecx,[dwLCID]
	mov	eax,[dwTimeZone]
	mov	[esi + 8],ecx
	mov	[esi + 12],eax
	add	esi,16
	sub	esi,edi
	push	PAGE_EXECUTE_READWRITE
	push	MEM_COMMIT
	push	esi
	push	NULL
	push	ebx
	call	VirtualAllocEx
	test	eax,eax
	mov	[BaseAddress],eax
	mov	edx,[FileMappingAddress]
	jz	FailedToCreateProcess
	mov	[edx],eax
	invoke	WriteProcessMemory,ebx,eax,edi,esi,NULL
	xor	eax,eax
	mov	ecx,OFFSET LoadLibraryW
	mov	edx,[BaseAddress]
	mov	ebx,[ecx + 2]
	mov	esi,[ProcInfo.hProcess]
	mov	ebx,[ebx]
	push	eax
	push	esp
	push	eax
	push	edx
	push	ebx
	push	eax
	push	eax
	push	esi
	call	CreateRemoteThread
	pop	eax
	invoke	WaitForSingleObject,[RcpEvent],30 * 1000
	invoke	CloseHandle,[RcpEvent]
	invoke	UnmapViewOfFile,[FileMappingAddress]
	invoke	CloseHandle,[RcpFileMap]
	lea	esi,[PathBufferTemp]
	invoke	WriteProcessMemory,[ProcInfo.hProcess],[EntryPoint],esi,2,NULL
	invoke	FlushInstructionCache,[ProcInfo.hProcess],[EntryPoint],2
	
NullPath:
	
	ret
	
NotExecutableFile:

	push	MB_ICONSTOP
	push	OFFSET szCaption
	push	OFFSET szNotExecutableFile
	push	0
	call	MessageBox
	ret
	
FailedToCreateProcess:

	push	MB_ICONSTOP
	push	OFFSET szCaption
	push	OFFSET szFailedToCreateProcess
	push	0
	call	MessageBox
	ret

RcpLauncherStartup    ENDP

SpAppMuvluv12:

	mov	DWORD PTR[ebx],932
	mov	DWORD PTR[ebx - 4],411h
	call	SelectFolder
	xor	ebx,ebx
	call	GetStartupInformationFromSASL
	retn
	
SpAppMuvluvAlt:

	mov	DWORD PTR[ebx],932
	mov	DWORD PTR[ebx - 4],411h
	call	SelectFolder
	mov	ebx,1
	call	GetStartupInformationFromSASL
	retn

SelectFolder    PROC    USES	edi

	LOCAL		BrowserInfo : BROWSEINFO

	push	MB_ICONINFORMATION
	push	OFFSET szConMessage
	push	OFFSET szSpAppOption
	push	0
	call	MessageBox
	lea	edi,[BrowserInfo]
	xor	eax,eax
	mov	edx,edi
	mov	ecx,SIZEOF BrowserInfo / 4
	mov	[esi],eax
	rep	stosd
	mov	[BrowserInfo.pszDisplayName],esi
	mov	[BrowserInfo.ulFlags],BIF_DONTGOBELOWDOMAIN
	invoke	SHBrowseForFolderW,edx
	test	eax,eax
	mov	edi,eax
	jz	ShowErrorCancelMessage
	invoke	SHGetPathFromIDListW,edi,esi
	test	eax,eax
	jz	ShowErrorCancelMessage
	invoke	LocalFree,edi
	ret
	
SelectFolder    ENDP

GetStartupInformationFromSASL:

	xor	eax,eax
	push	ebp
	sub	esp,16
	invoke	CreateFile,ADDR szSASLFileName,GENERIC_READ,FILE_SHARE_READ,eax,OPEN_EXISTING,eax,eax
	cmp	eax,INVALID_HANDLE_VALUE
	mov	ebp,eax
	je	ShowErrorCancelMessage
	invoke	GetFileSize,eax,NULL
	cmp	eax,32
	mov	[esp],eax
	jb	ShowErrorCancelMessage
	lea	eax,[esp + 4]
	lea	ecx,[esp + 8]
	invoke	ReadFile,ebp,ecx,4,eax,NULL
	mov	ecx,[esp + 8]
	mov	[esp + 12],ebx
	cmp	ecx,ebx
	jbe	ShowErrorCancelMessage
	lea	ecx,[ecx * 4 + 4]
	cmp	ecx,[esp]
	jae	ShowErrorCancelMessage
	lea	eax,[esp + 4]
	push	NULL
	push	eax
	push	ecx
	push	ecx
	invoke	GetProcessHeap
	push	0
	push	eax
	call	HeapAlloc
	test	eax,eax
	mov	ebx,eax
	jz	ShowErrorCancelMessage
	push	eax
	push	ebp
	call	ReadFile
	mov	eax,[esp + 12]
	shl	eax,2
	mov	ecx,[ebx + eax]
	mov	edx,[ebx + eax + 4]
	cmp	ecx,edx
	jae	ShowErrorCancelMessage
	cmp	edx,[esp]
	ja	ShowErrorCancelMessage
	sub	edx,ecx
	lea	eax,[esp + 4]
	push	NULL
	push	eax
	push	edx
	push	edx
	invoke	SetFilePointer,ebp,ecx,NULL,FILE_BEGIN
	invoke	GetProcessHeap
	push	0
	push	eax
	call	HeapAlloc
	test	eax,eax
	mov	ebx,eax
	jz	ShowErrorCancelMessage
	push	eax
	push	ebp
	call	ReadFile
	invoke	GetProcessHeap
	push	MAX_PATH * 2
	push	0
	push	eax
	call	HeapAlloc
	test	eax,eax
	jz	ShowErrorCancelMessage
	xor	ecx,ecx
	xor	edx,edx
	
@@:
	
	mov	dx,[esi + ecx]
	mov	[edi + ecx],dx
	mov	[eax + ecx],dx
	add	ecx,2
	test	edx,edx
	jnz	@b
	mov	edx,5Ch
	push	esi
	cmp	[esi + ecx - 4],dx
	lea	esi,[ecx - 2]
	je	@f
	mov	[edi + esi],dx
	mov	[eax + esi],dx
	add	esi,2

@@:

	xor	ecx,ecx
	
@@:
	
	mov	dx,[ebx + ecx]
	mov	[edi + esi],dx
	mov	[eax + esi],dx
	add	ecx,2
	add	esi,2
	test	edx,edx
	jnz	@b
	mov	BYTE PTR[eax + esi - 2],20h

@@:

	mov	dx,[ebx + ecx]
	mov	[eax + esi],dx
	add	ecx,2
	add	esi,2
	test	edx,edx
	jnz	@b
	mov	[pCommandLine],eax
	pop	esi
	xor	eax,eax
	add	esp,16
	pop	ebp
	retn

ShowErrorCancelMessage:

	push	MB_ICONINFORMATION
	push	OFFSET szCaption
	push	OFFSET szErrorCancel
	push	0
	call	MessageBox
	invoke	ExitProcess,eax



db	'P' + 80h,'r' + 80h,'o' + 80h,'g' + 80h,'r' + 80h,'a' + 80h,'m' + 80h,' ' + 80h,'b' + 80h,'y' + 80h,' ' + 80h
db	'L' + 80h,'O' + 80h,'V' + 80h,'E' + 80h,'H' + 80h,'I' + 80h,'N' + 80h,'A' + 80h,'-' + 80h,'A' + 80h,'V' + 80h,'C' + 80h

end start
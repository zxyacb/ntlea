
;.686
;.model FLAT,STDCALL
option casemap:none

BOOL					typedef DWORD

PVOID					typedef QWORD
LPVOID					typedef QWORD
LPCVOID					typedef QWORD

LPPROCESS_INFORMATION	typedef QWORD
LPSECURITY_ATTRIBUTES	typedef QWORD

Addresses_1 STRUCT 
	lpGetTimeZoneInformation		LPVOID	?
	lpEnumFontFamiliesExA			LPVOID	?
	lpEnumFontFamiliesExW			LPVOID	?
	lpEnumFontsA					LPVOID	?
	lpEnumFontsW					LPVOID	?
	lpUser32SEHAddress				LPVOID	?
	lpProcInfoAddress				LPVOID	?
	lpCreateProcAddress				LPVOID	?
	lpCreateProcAnsiAddress			LPVOID	?
	lpCreateProcUniAddress			LPVOID	?
	lpCreateWindowExAddress			LPVOID	?
	lpCreateWindowExWddress			LPVOID	?
	lpCallWindowProcAddress			LPVOID	?
	lpSetWindowLongAddress			LPVOID	?
	lpParameterAddress				LPVOID	?
	lpMBtoUniAddress				LPVOID	?
	lpUnitoMBAddress				LPVOID	?
	lpMBtoWCAddress					LPVOID	?
	lpWCtoMBAddress					LPVOID	?
	lpCPInfoAddress					LPVOID	?
	lpGetWindowLongPtrSelect		LPVOID	?
	lpGetWindowLongPtrAddress		LPVOID	?
	lpCreateFileAddress				LPVOID	?
	lpShellExecuteAddress			LPVOID	?
	lpCompareStringAddress			LPVOID	?
	lpVerQueryValueAddress			LPVOID	?
	lpGetStockObjectAddress			LPVOID	?
;;	lpFindResourceAddress			LPVOID	?
;;	lpReturnAddress					LPVOID	?
;;	lpReturnAddressVQV				LPVOID	?
;;	lpWindowProcTable				LPVOID	?
;;	lpWindowHandleTable				LPVOID	?
;;	lpAcGenralType					DWORD	?
	lpAcGenralAnsiType				DWORD	?
	lpAcGenralUniType				DWORD	?
Addresses_1 ENDS

CreateProcessInternalAorW PROTO	:DWORD,:LPCVOID,:LPCVOID,:LPSECURITY_ATTRIBUTES,:LPSECURITY_ATTRIBUTES,:QWORD,:QWORD,:LPCVOID,:LPCVOID,:LPCVOID,:LPPROCESS_INFORMATION

.DATA
; -------------------------------------------------- ;
EXTERN addresses:Addresses_1

.CODE
; --------------- hook head + tail ----------------- ;
CreateProcessInternalAorW PROC	lpAcGenralType			:DWORD,
								lpApplicationName		:LPCVOID,
								lpCommandLine			:LPCVOID, 
								lpProcessAttributes		:LPSECURITY_ATTRIBUTES,
								lpThreadAttributes		:LPSECURITY_ATTRIBUTES,
								bInheritHandles			:QWORD,
								dwCreationFlags			:QWORD,
								lpEnvironment			:LPCVOID, 
								lpCurrentDirectory		:LPCVOID,
								lpStartupInfo			:LPCVOID,
								lpProcessInformation	:LPPROCESS_INFORMATION

	; we know x64 should use registers instead of directly stack !!
	
	push	lpProcessInformation
	push	lpStartupInfo
	push	lpCurrentDirectory
	push	lpEnvironment
	push	dwCreationFlags
	push	bInheritHandles
	push	lpThreadAttributes
	push	lpProcessAttributes
	push	lpCommandLine
	push	lpApplicationName

	push	qword ptr [LCreateProcessEnd]

	xor		eax, eax
	cmp		lpAcGenralType, eax
	je		@f
	push	10h
	mov		eax, lpAcGenralType
	jmp		CALLING

@@:
	push	rbp
	mov		rbp, rsp

CALLING:
	jmp		qword ptr [addresses.lpCreateProcAddress]

LCreateProcessEnd:
	ret

CreateProcessInternalAorW ENDP

; ---- ;
CreateProcessInternalA PROC		lpApplicationName		:LPCVOID, 
								lpCommandLine			:LPCVOID,
								lpProcessAttributes		:LPSECURITY_ATTRIBUTES, 
								lpThreadAttributes		:LPSECURITY_ATTRIBUTES,
								bInheritHandles			:BOOL,
								dwCreationFlags			:DWORD,
								lpEnvironment			:LPVOID, 
								lpCurrentDirectory		:LPCVOID,
								lpStartupInfo			:LPCVOID, 
								lpProcessInformation	:LPPROCESS_INFORMATION

	mov rax, addresses.lpCreateProcAnsiAddress
	mov addresses.lpCreateProcAddress, rax
	mov eax, bInheritHandles
	mov edx, dwCreationFlags
	invoke CreateProcessInternalAorW, addresses.lpAcGenralAnsiType, 
		lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, rax, rdx, 
		lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation
	ret

CreateProcessInternalA ENDP

; ---- ;
CreateProcessInternalW PROC		lpApplicationName		:LPCVOID, 
								lpCommandLine			:LPCVOID,
								lpProcessAttributes		:LPSECURITY_ATTRIBUTES, 
								lpThreadAttributes		:LPSECURITY_ATTRIBUTES,
								bInheritHandles			:BOOL,
								dwCreationFlags			:DWORD,
								lpEnvironment			:LPVOID, 
								lpCurrentDirectory		:LPCVOID,
								lpStartupInfo			:LPCVOID, 
								lpProcessInformation	:LPPROCESS_INFORMATION

	mov rax, addresses.lpCreateProcUniAddress
	mov addresses.lpCreateProcAddress, rax
	mov eax, bInheritHandles
	mov edx, dwCreationFlags
	invoke CreateProcessInternalAorW, addresses.lpAcGenralUniType, 
		lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, rax, rdx, 
		lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation
	ret

CreateProcessInternalW ENDP

; ---- ;

END

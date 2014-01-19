/*++

=====================================================================
NT Locale Emulator Advance  ver0.86 beta
            Modify Littlewater ver0.87 beta fix (2013/11/18)
---------------------------------------------------------------------
Develop environment : Windows XP SP2 & GoASM v0.47 with OllyDbg v1.10
=====================================================================

Source file name:

    rcphook.asm

Module name:

    ntleah.dll

Date:

    13/6/2007
    
Author:

    LOVEHINA-AVC

Description:

    The hook DLL.

--*/


CONST SECTION

/*

#define	DBG
#define	DBG_ENTRY
#define	TRAP

*/

;#define DBG_ENTRY

ERROR_INSUFFICIENT_BUFFER equ    122
WM_LBUTTONDOWN            equ    0x201
MF_STRING                 equ    0
MIIM_STATE                equ    1
MIIM_ID                   equ    2
MIIM_SUBMENU              equ    4
MIIM_CHECKMARKS           equ    8
MIIM_TYPE                 equ    0x10
MIIM_DATA                 equ    0x20
MIIM_STRING               equ    0x40
MIIM_BITMAP               equ    0x80
MIIM_FTYPE                equ    0x100
LF_FACESIZE               equ    32
LF_FULLFACESIZE           equ    64
;MAX_CLASS_SUPPORTED      equ    128
EVENT_MODIFY_STATE        equ    2
DLL_PROCESS_ATTACH        equ    1
VER_WINXP_SP2_OR_ABOVE    equ    0
VER_WINXP_SP1             equ    1
VER_WIN2K                 equ    2
VER_WINXP                 equ    3
VER_MAJOR_WIN2K           equ    5
OPEN_EXISTING             equ    3
FILE_SHARE_READ           equ    1
GENERIC_READ              equ    0x80000000
VER_MINOR_WIN2K           equ    0
CREATE_SUSPENDED          equ    4
MEM_COMMIT                equ    0x1000
PAGE_READONLY             equ    2
PAGE_READWRITE            equ    4
PAGE_EXECUTE_READWRITE    equ    0x40
INVALID_HANDLE_VALUE      equ    -1
MAX_PATH                  equ    260
NORM_IGNORECASE           equ    1
NORM_IGNOREKANATYPE       equ    0x10000
NORM_IGNOREWIDTH          equ    0x20000
FILE_MAP_COPY             equ    0x00001
FILE_MAP_WRITE            equ    0x00002
FILE_MAP_READ             equ    0x00004
FILE_MAP_ALL_ACCESS       equ    0xF001F
CONTEXT_CONTROL           equ    0x10001
regEip                    equ    0xB8
HEAP_ZERO_MEMORY          equ    8
MB_ICONHAND               equ    0x10
GCL_WNDPROC               equ    -24
GWL_STYLE                 equ    -16
GWL_WNDPROC               equ    -4
DWL_DLGPROC               equ    4
GCL_MENUNAME              equ    -8
WH_CBT                    equ    5
HCBT_MOVESIZE             equ    0
HCBT_MINMAX               equ    1
HCBT_QS                   equ    2
HCBT_CREATEWND            equ    3
HCBT_DESTROYWND           equ    4
HCBT_ACTIVATE             equ    5
HCBT_CLICKSKIPPED         equ    6
HCBT_KEYSKIPPED           equ    7
HCBT_SYSCOMMAND           equ    8
HCBT_SETFOCUS             equ    9
WM_CREATE                 equ    1
WM_DESTROY                equ    2
WM_NCDESTROY              equ    0x82
WM_NCCREATE               equ    0x81
WM_SETTINGCHANGE          equ    0x1A
WM_DEVMODECHANGE          equ    0x1B
WM_SETTEXT                equ    0x0C
WM_GETTEXT                equ    0x0D
WM_GETTEXTLENGTH          equ    0x0E
EM_REPLACESEL             equ    0xC2
EM_GETLINE                equ    0xC4
WM_NOTIFYFORMAT           equ    0x55
WM_INITDIALOG             equ    0x110
CB_ADDSTRING              equ    0x143
CB_DIR                    equ    0x145
CB_GETLBTEXT              equ    0x148
CB_FINDSTRING             equ    0x14C
CB_SELECTSTRING           equ    0x14D
CB_INSERTSTRING           equ    0x14A
CB_FINDSTRINGEXACT        equ    0x158
LB_ADDSTRING              equ    0x180
LB_INSERTSTRING           equ    0x181
LB_GETTEXTLEN             equ    0x18A
LB_FINDSTRING             equ    0x18F
LB_ADDFILE                equ    0x196
LB_FINDSTRINGEXACT        equ    0x1A2
LB_GETTEXT                equ    0x189
LB_SELECTSTRING           equ    0x18C
LB_DIR                    equ    0x18D
WM_MDICREATE              equ    0x220
WM_CHAR                   equ    0x102
WM_IME_CHAR               equ    0x286
WM_UNKNOWN                equ    0x43E
NFR_ANSI                  equ    1
NFR_UNICODE               equ    2
NF_QUERY                  equ    3
NF_REQUERY                equ    4
CT_CREATE_WINDOW          equ    1
CT_CREATE_DIALOG          equ    2


	lpWndProc     equ    esp + 4
	hWnd          equ    esp + 4
	hWndMDIClient equ    esp + 8
	nIdx          equ    esp + 8
	dwNewValue    equ    esp + 12
	Msg           equ    esp + 8
	wParam        equ    esp + 12
	lParam        equ    esp + 16
	lpCallBack    equ    esp + 20
	fuFlags       equ    esp + 20
	uTimeout      equ    esp + 24
	dwData        equ    esp + 24
	lpdwResult    equ    esp + 28


;	NTSYSAPI NTSTATUS NTAPI
;	RtlMultiByteToUnicodeN (
;			PWCHAR UnicodeString,
;			ULONG	UnicodeSize,
;			PULONG ResultSize,
;			PCSTR	MbString,
;			ULONG	MbSize
;			);


OSVERSIONINFO    STRUCT

	dwOSVersionInfoSize   dd    ?
	dwMajorVersion        dd    ?
	dwMinorVersion        dd    ?
	dwBuildNumber         dd    ?
	dwPlatformId          dd    ?
	szCSDVersion          db    128    dup   ?
	
ENDS

CREATEWNDEX    STRUCT

	lpParam       dd    ?
	hInstance     dd    ?
	hMenu         dd    ?
	hWndParent    dd    ?
	nHeight       dd    ?
	nWidth        dd    ?
	y             dd    ?
	x             dd    ?
	dwStyle       dd    ?
	lpWindowName  dd    ?
	lpClassName   dd    ?
	dwExStyle     dd    ?
    
ENDS

CREATEMDIWND    STRUCT

	szClass     dd    ?
	szTitle     dd    ?
	hOwner      dd    ?
	x           dd    ?
	y           dd    ?
	cx          dd    ?
	cy          dd    ?
	style       dd    ?
	Param       dd    ?

ENDS

WNDCLASSEX    STRUCT

	cbSize        dd    ?
	style         dd    ?
	lpfnWndProc   dd    ?
	cbClsExtra    dd    ?
	cbWndExtra    dd    ?
	hInstance     dd    ?
	hIcon         dd    ?
	hCursor       dd    ?
	hbrBackground dd    ?
	lpszMenuName  dd    ?
	lpszClassName dd    ?
	hIconSm       dd    ?

ENDS

WNDCLASS    STRUCT

	style         dd    ?
	lpfnWndProc   dd    ?
	cbClsExtra    dd    ?
	cbWndExtra    dd    ?
	hInstance     dd    ?
	hIcon         dd    ?
	hCursor       dd    ?
	hbrBackground dd    ?
	lpszMenuName  dd    ?
	lpszClassName dd    ?

ENDS

LOGFONTSIMILAR    STRUCT

  lfHeight          dd    ?
  lfWidth           dd    ?
  lfEscapement      dd    ?
  lfOrientation     dd    ?
  lfWeight          dd    ?
  lfItalic          db    ?
  lfUnderline       db    ?
  lfStrikeOut       db    ?
  lfCharSet         db    ?
  lfOutPrecision    db    ?
  lfClipPrecision   db    ?
  lfQuality         db    ?
  lfPitchAndFamily  db    ?

ENDS

LOGFONT    STRUCT

  lfHeight          dd    ?
  lfWidth           dd    ?
  lfEscapement      dd    ?
  lfOrientation     dd    ?
  lfWeight          dd    ?
  lfItalic          db    ?
  lfUnderline       db    ?
  lfStrikeOut       db    ?
  lfCharSet         db    ?
  lfOutPrecision    db    ?
  lfClipPrecision   db    ?
  lfQuality         db    ?
  lfPitchAndFamily  db    ?
  lfFaceName        db    LF_FACESIZE   dup  ?

ENDS

LOGFONTW    STRUCT

  lfHeight          dd    ?
  lfWidth           dd    ?
  lfEscapement      dd    ?
  lfOrientation     dd    ?
  lfWeight          dd    ?
  lfItalic          db    ?
  lfUnderline       db    ?
  lfStrikeOut       db    ?
  lfCharSet         db    ?
  lfOutPrecision    db    ?
  lfClipPrecision   db    ?
  lfQuality         db    ?
  lfPitchAndFamily  db    ?
  lfFaceName        dw    LF_FACESIZE   dup  ?

ENDS

ENUMLOGFONTEX    STRUCT

  elfLogFont     LOGFONT    <>
  elfFullName    db    LF_FULLFACESIZE  dup  ?
  elfStyle       db    LF_FACESIZE      dup  ?
  elfScript      db    LF_FACESIZE      dup  ?
  
ENDS

DESIGNVECTOR    STRUCT

	notcare    dd    ?
	
ENDS

ENUMLOGFONTEXDV    STRUCT

  elfEnumLogfontEx   ENUMLOGFONTEX   <>
  elfDesignVector    DESIGNVECTOR    <>

ENDS

MENUITEMINFO    STRUCT

  cbSize        dd    ?
  fMask         dd    ?
  fType         dd    ?
  fState        dd    ?
  wID           dd    ?
  hSubMenu      dd    ?
  hbmpChecked   dd    ?
  hbmpUnchecked dd    ?
  dwItemData    dd    ?
  dwTypeData    dd    ?
  cch           dd    ?
  hbmpItem      dd    ?
  
ENDS

PROCESS_INFORMATION    STRUCT

	hProcess      dd    ?
	hThread       dd    ?
	dwProcessId   dd    ?
	dwThreadId    dd    ?

ENDS

SYSTEM_CLASS_WNDPROC    STRUCT

	AnsiSystemClassProc       dd    ?
	UnicodeSystemClassProc    dd    ?

ENDS

NTLEA_TLS_DATA    STRUCT

	InternalCall      dd    ?
	DBCSLeadByte      dd    ?
	IsFontAvailable   dd    ?
	CurrentCallType   dd    ?
	DialogProc        dd    ?
	IsCreateFileCall  dd    ?
	hWindowCbtHookNext     dd   ?
	hWindowCbtHookAnsi     dd   ?
	hWindowCbtHookUnicode  dd   ?
	SystemClassDesc   SYSTEM_CLASS_WNDPROC   <>
	SystemClassDescStorageSpace    dd    (14 - 1) * 2    dup    ?

ENDS

NTLEA_WND_ASC_DATA    STRUCT

	PrevAnsiWindowProc       dd    ?
	PrevUnicodeWindowProc    dd    ?

ENDS
	
DATA SECTION("ntlead")

ALIGN	4

	context             db    0x2CC dup ?
	
	SystemClassNameA    dd    ADDR szSystemClassButtonA
	                    dd    ADDR szSystemClassComboBoxA
	                    dd    ADDR szSystemClassEditA
	                    dd    ADDR szSystemClassListBoxA
	                    dd    ADDR szSystemClassMdiClientA
	                    dd    ADDR szSystemClassRichEditA
	                    dd    ADDR szSystemClassRichEditClassA
	                    dd    ADDR szSystemClassScrollBarA
	                    dd    ADDR szSystemClassStaticA
	                    dd    ADDR szSysTreeView32A
	                    dd    ADDR szSysListView32A
	                    dd    ADDR szSysAnimate32A
	                    dd    ADDR szSysHeader32A
	                    dd    ADDR szToolTipClass32A


	SystemClassNameW    dd    ADDR szSystemClassButtonW
	                    dd    ADDR szSystemClassComboBoxW
	                    dd    ADDR szSystemClassEditW
	                    dd    ADDR szSystemClassListBoxW
	                    dd    ADDR szSystemClassMdiClientW
	                    dd    ADDR szSystemClassRichEditW
	                    dd    ADDR szSystemClassRichEditClassW
	                    dd    ADDR szSystemClassScrollBarW
	                    dd    ADDR szSystemClassStaticW
	                    dd    ADDR szSysTreeView32W
	                    dd    ADDR szSysListView32W
	                    dd    ADDR szSysAnimate32W
	                    dd    ADDR szSysHeader32W
	                    dd    ADDR szToolTipClass32W

#ifdef	TRAP
	bTrapFlag    dd    0
#endif

	lpGetTimeZoneInformation  dd   ?
	lpEnumFontFamiliesEx    dd     ?
	dwTempCP                dd     ?
	dwTempLCID              dd     ?
	dwCodePage              dd     0
	dwLocaleID              dd     0
	dwFontSizePercent       dd     100
	dwTimeZone              dd     -480
	lpEntryPoint            dd     ?
	lpFileMappingAddress    dd     ?
	lpFontFaceName          dd     ?
	hEvent                  dd     ?
	                        dd     ?
	hFileMapping            dd     ?
	hHeap                   dd     ?
	nTlsIndex               dd     ?
	
/*
	
	lpRegisterClassAddress    dd   ?
	lpRegisterClassExAddress  dd   ?
	
*/

	lpUser32SEHAddress      dd     ?
	lpProcInfoAddress       dd     ?
	lpCreateProcAnsiAddress dd     ?
	lpCreateProcUniAddress  dd     ?
	lpCallWindowProcAddress dd     ?
	lpParameterAddress      dd     ?
	lpMBtoUniAddress        dd     ?
	lpUnitoMBAddress        dd     ?
	lpMBtoWCAddress         dd     ?
	lpWCtoMBAddress         dd     ?
	lpCPInfoAddress         dd     ?
	lpGetWindowLongAddress  dd     ?
	lpCreateFileAddress     dd     ?
	lpCompareStringAddress  dd     ?
	lpVerQueryValueAddress  dd     ?
	lpReturnAddress         dd     ?
	lpReturnAddressVQV      dd     ?
	
/*
	
	lpWindowProcTable       dd     ?
	lpWindowHandleTable     dd     ?
	dwWindowProcCounter     dd     0
	dwWindowHandleCounter   dd     0
	
*/
	
	dwThreadId              dd     0
	dwThreadIdWCtoMB        dd     0
	bNtleaAtomInvoked       db     0
	MbCodePageTag           db     0
	nOSVer                  db     0
	bInternalCall           db     0
	bInternalLock           db     0
	bInternalCallWCtoMB     db     0
	bInternalLockWCtoMB     db     0
	bInternalLockCreateProc db     0
	bInternalLockVQV        db     0
	;bInternalLockRegClass  db     0
	bInternalLockCreateAtom db     0
	bForceSpecifyFont       db     0
	bChinesePath            db     0
	bCreateProcUniFunc      db     0
	bCreateProcNative       db     0
	bSuspendThreadFlag      db     0
	bErrorFlag              db     0


	szTranslation						db		 "\VarFileInfo\Translation"
	szRcpEvent							db		 "RcpEvent000",0
	szRcpFileMap						db		 "RcpFileMap000",0
	szRcpEventName					db		 "RcpEvent000",0
	szNtDLLName							db		 "ntdll.dll",0
	szGdiDLLName						db		 "gdi32.dll",0
	szMultiByteToUnicode		db		 "RtlMultiByteToUnicodeN",0
	szUnicodeToMultiByte		db		 "RtlUnicodeToMultiByteN",0
	szNlsAnsiCodePage				db		 "NlsAnsiCodePage",0
	szNlsMbCodePageTag			db		 "NlsMbCodePageTag",0
	szGdiGetCodePage				db		 "GdiGetCodePage",0
	szCaption               db     "DebugMsg"
	szErrorFilemapping      db     "FailedCreateFilemapping",0
	szErrorFilemapview      db     "FailedMapViewOfFile",0

	szSystemClassButtonA    db     "BUTTON",0
	szSystemClassButtonW    db     L"BUTTON",0,0
	
	szSystemClassComboBoxA    db     "COMBOBOX",0
	szSystemClassComboBoxW    db     L"COMBOBOX",0,0
	
	szSystemClassEditA    db     "EDIT",0
	szSystemClassEditW    db     L"EDIT",0,0
	
	szSystemClassListBoxA    db     "LISTBOX",0
	szSystemClassListBoxW    db     L"LISTBOX",0,0
	
	szSystemClassMdiClientA    db     "MDICLIENT",0
	szSystemClassMdiClientW    db     L"MDICLIENT",0,0
	
	szSystemClassRichEditA    db     "RichEdit",0
	szSystemClassRichEditW    db     L"RichEdit",0,0
	
	szSystemClassRichEditClassA    db     "RICHEDIT_CLASS",0
	szSystemClassRichEditClassW    db     L"RICHEDIT_CLASS",0,0
	
	szSystemClassScrollBarA    db     "SCROLLBAR",0
	szSystemClassScrollBarW    db     L"SCROLLBAR",0,0
	
	szSystemClassStaticA    db     "STATIC",0
	szSystemClassStaticW    db     L"STATIC",0,0
	
	szSysTreeView32A        db    "SysTreeView32",0
	szSysTreeView32W        db    L"SysTreeView32",0,0
	
	szSysListView32A        db    "SysListView32",0
	szSysListView32W        db    L"SysListView32",0,0
	
	szSysAnimate32A         db    "SysAnimate32",0
	szSysAnimate32W         db    L"SysAnimate32",0,0
	
	szSysHeader32A          db    "SysHeader32",0
	szSysHeader32W          db    L"SysHeader32",0,0
	
	szToolTipClass32A	      db    "tooltips_class32",0
	szToolTipClass32W	      db    L"tooltips_class32",0,0
	
	szAppHelp                  db  L"\AppHelp.dll",0,0
	szAppCheck                 db  "ApphelpCheckRunApp",0
	szAlLayer                  db  "AlLayer.dll",0
	szNtleaWndAscData          db  "NtleaWndAscData",0
	szNtleaDlgAscData          db  "NtleaDlgAscData",0
	szNewDlgClass              db  "NewDlgClass",0
	szUnhandledExceptionTitle  db  "NTLEA internal exception",0
		
	szMultipleHookNotAllowed   db "The Microsoft Applocale has been detected, NTLEA is not compatible with it.",0Dh,0Ah,"Please unload it from memory and run the application again.",0
	szUnhandledExceptionText   db  "Sorry, NTLEA got a fatal exception during the execution, the current application will be terminated from its context immediately.",0Dh,0Ah,0Dh,0Ah,"Exception description:",0Dh,0Ah,0Dh,0Ah,"%s",0
	;szClassTableFull          db  "The class table is full, NTLEA could not record any more information for a registered window class.",0
	szAppCallDefConvProc       db  "The executing application submitted a call to one of the DefWindowProc/DefFrameProc/DefDlgProc/DefMDIChildProc functions for a non-unicode window with incorrect timing.",0
	
CODE SECTION("ntleat")

#ifdef	TRAP
TrapFunc:

	cmp	D[bTrapFlag],0
	je	>X9999
	int 3
	
X9999:
	
	retn
#endif

ALIGN 4

DllMain    FRAME    hInst, Reason, Reserved

	dec	D[Reason]	;	cmp	D[Reason],DLL_PROCESS_ATTACH
	jnz	>E0
	call	[kernel32:TlsAlloc]
	cmp	eax,-1
	jz	>E1
	xor	ecx,ecx
	mov	[nTlsIndex],eax
	push	ecx
	push	ecx
	push	ecx
	call	[kernel32:HeapCreate]
	mov	[hHeap],eax
	xor	edx,edx
	push	ecx
	push	esp
	push	edx
	push	edx
	push	ADDR InitUnicodeLayer
	push	edx
	push	edx
	call	[kernel32:CreateThread]/**/
	xor	eax,eax
	pop	ecx
	inc	eax

E0:

	sub	D[Reason],2	;	cmp	D[Reason],DLL_THREAD_DETACH
	jnz	>E2
	push	[nTlsIndex]
	call	[kernel32:TlsGetValue]
	test	eax,eax
	jz	>E2
	mov	ecx,eax
	call	FreeStringInternal
	
E2:
	
	ret
	
E1:

	xor	eax,eax
	jmp	<E2
	
ENDF

ALIGN 4

AllocateZeroedMemory:

	push	eax
	push	HEAP_ZERO_MEMORY
	jmp	>S997
	
ALIGN 4
	
AllocateHeapInternal:

	push	ecx
	push	0
	
S997:
	
	push	[hHeap]
	call	[kernel32:HeapAlloc]
	retn
	
ALIGN 4
	
GetTlsValueInternal:

	call	[kernel32:GetLastError]
	push	eax
	push	[nTlsIndex]
	call	[kernel32:TlsGetValue]
	pop	ecx
	push	eax
	push	ecx
	call	[kernel32:SetLastError]
	pop	eax
	test	eax,eax
	jnz	>LN110GTV
	push	SIZEOF NTLEA_TLS_DATA
	pop	eax
	call	AllocateZeroedMemory
	push	esi
	push	edi
	push	eax
	mov	esi,eax
	push	[nTlsIndex]
	call	[kernel32:TlsSetValue]
	sub	esp,SIZEOF WNDCLASS
	xor	edi,edi
	
L111:
	
	push	esp
	push	[edi * 4 + SystemClassNameA]
	push	0
	call	[user32:GetClassInfoA]
	mov	eax,[esp + WNDCLASS.lpfnWndProc]
	mov	[esi + edi * 8 + NTLEA_TLS_DATA.SystemClassDesc],eax
	push	esp
	push	[edi * 4 + SystemClassNameW]
	push	0
	call	[user32:GetClassInfoW]
	mov	eax,[esp + WNDCLASS.lpfnWndProc]
	mov	[esi + edi * 8 + NTLEA_TLS_DATA.SystemClassDesc + 4],eax
	inc	edi
	cmp	edi,14
	jne	<L111	
	push	0
	call	[kernel32:SetLastError]
	mov	eax,esi
	add	esp,SIZEOF WNDCLASS
	pop	edi
	pop	esi

LN110GTV:
L110:
	
	retn
	
ALIGN 4

CbtHookProcA:

	call	GetTlsValueInternal
	mov	ecx,[eax + NTLEA_TLS_DATA.hWindowCbtHookAnsi]
	jmp	>C300

CbtHookProcW:

	call	GetTlsValueInternal
	mov	ecx,[eax + NTLEA_TLS_DATA.hWindowCbtHookUnicode]
	
C300:
	
	mov	[eax + NTLEA_TLS_DATA.hWindowCbtHookNext],ecx

CbtHookProc    FRAME    nCode, wParameter, lParameter

	LOCAL   ClassNameBuffer[MAX_PATH] : B

	USES    esi,edi,ebx
	
	mov	esi,eax
	cmp	D[nCode],HCBT_CREATEWND
	jne	>L100
	cmp	D[esi + NTLEA_TLS_DATA.CurrentCallType],CT_CREATE_WINDOW
	mov	edi,[wParameter]
	je	>L102
	push	edi
	call	[user32:IsWindowUnicode]
	test	eax,eax
	jnz	>L100
	
L102:
		
	mov	ebx,esp ; lea	ebx,[ClassNameBuffer]
	push	MAX_PATH
	push	ebx
	push	edi
	call	[user32:GetClassNameA]
	push	ADDR szNewDlgClass
	push	ebx
	call	[kernel32:lstrcmpiA]
	test	eax,eax
	jz	>L100
	call	HookWindowProc
	
L100:

	push	[lParameter]
	push	edi
	push	[nCode]
	push	[esi + NTLEA_TLS_DATA.hWindowCbtHookNext]
	call	[user32:CallNextHookEx]
	ret

ENDF

CreateGlobalAtom:

	mov	ebx,ADDR szNtleaWndAscData
	mov	cl,1

L531:

	xor	eax,eax
	lock	cmpxchg	[bInternalLockCreateAtom],cl
	dec	eax
	jz	<L531
	cmp	B[bNtleaAtomInvoked],0
	jne	>L534
	push	ebx
	call	[kernel32:GlobalFindAtomA]
	test	ax,ax
	jnz	>L535
	push	ebx
	call	[kernel32:GlobalAddAtomA]
	
L535:
	
	inc	B[bNtleaAtomInvoked]
	push	ADDR szNtleaDlgAscData
	call	[kernel32:GlobalFindAtomA]
	test	ax,ax
	jnz	>L534
	push	ADDR szNtleaDlgAscData
	call	[kernel32:GlobalAddAtomA]
	
L534:

	lock	dec	B[bInternalLockCreateAtom]
	retn

HookWindowProc:

	call	CreateGlobalAtom
	push	ebx
	push	edi
	call	[user32:GetPropA]
	test	eax,eax
	jnz	>L530
	add	eax,SIZEOF NTLEA_WND_ASC_DATA
	call	AllocateZeroedMemory	
	push	ebp
	inc	D[esi + NTLEA_TLS_DATA.InternalCall]
	mov	ebp,eax
	push	ebp
	push	ebx
	push	edi
	call	[user32:SetPropA]
	test	eax,eax
	jz	>L532
	push	GWL_WNDPROC
	push	edi
	call	[user32:GetWindowLongA]
	test	eax,eax
	mov	[ebp + NTLEA_WND_ASC_DATA.PrevAnsiWindowProc],eax
	jz	>L532	
	push	ADDR TopLevelWindowProc
	push	GWL_WNDPROC
	push	edi
	call	[user32:SetWindowLongW]
	mov	[ebp + NTLEA_WND_ASC_DATA.PrevUnicodeWindowProc],eax
	jmp	>L533

L532:

	mov	ecx,ebp
	call	FreeStringInternal
	
L533:
	
	pop	ebp
	
L530:
	
	retn
	
ALIGN 4

TopLevelDialogProc:

	call	GetTlsValueInternal
	cmp	D[eax + NTLEA_TLS_DATA.DialogProc],0
	je	>T1
	push	ebx
	push	eax
	call	CreateGlobalAtom
	push	ebx
	push	[hWnd + 12]
	call	[user32:GetPropA]
	test	eax,eax
	jz	>T3
	pop	ecx
	pop	ebx
	jmp	>T1
	
T3:
	
	add	eax,SIZEOF NTLEA_WND_ASC_DATA
	call	AllocateZeroedMemory
	push	eax
	push	eax
	push	ebx
	push	[hWnd + 20]
	call	[user32:SetPropA]
	pop	eax
		
T2:

	pop	edx
	pop	ebx
	mov	ecx,[edx + NTLEA_TLS_DATA.DialogProc]
	mov	[eax + NTLEA_WND_ASC_DATA.PrevAnsiWindowProc],ecx
	and	D[edx + NTLEA_TLS_DATA.DialogProc],0
	
T1:

TopLevelWindowProc    FRAME    hWindow, Message, wParameter, lParameter

	USES    esi,edi,ebx
	
	LOCAL    PrevWndProc
	LOCAL    lpAnsiWindowName
	LOCAL    lpAnsiClassName
	LOCAL    CharBuffer
	LOCAL    ClassNameBuffer[MAX_PATH] : B

#ifdef	DBG
	int 3
#endif

#ifdef	TRAP
	call	TrapFunc
#endif

	push	ADDR szNtleaWndAscData
	push	[hWindow]
	call	[user32:GetPropA]
	mov	eax,[eax + NTLEA_WND_ASC_DATA.PrevAnsiWindowProc]
	xor	ebx,ebx
	mov	[PrevWndProc],eax
	call	GetTlsValueInternal
	mov	esi,[Message]	
	inc	D[eax + NTLEA_TLS_DATA.InternalCall]
	cmp	esi,WM_MDICREATE
	mov	[lpAnsiWindowName],ebx
	mov	[lpAnsiClassName],ebx
	mov	[CharBuffer],ebx
	je	>LN106	
	cmp	esi,CB_GETLBTEXT
	je	>LN129
	cmp	esi,WM_GETTEXTLENGTH
	je	>LN127
	cmp	esi,LB_GETTEXTLEN
	je	>LN131
	cmp	esi,EM_GETLINE
	je	>LN124
	cmp	esi,LB_GETTEXT
	je	>LN110
	cmp	esi,WM_NOTIFYFORMAT
	je	>LN121
	cmp	esi,EM_REPLACESEL
	je	>LN113
	cmp	esi,WM_SETTEXT
	je	>LN113
	cmp	esi,WM_SETTINGCHANGE
	je	>LN113
	cmp	esi,WM_DEVMODECHANGE
	je	>LN113
	cmp	esi,WM_GETTEXT
	je	>LN114
	cmp	esi,WM_UNKNOWN
	JE	>LN114
	cmp	esi,WM_IME_CHAR
	je	>LN109
	cmp	esi,WM_CHAR
	je	>LN109
	cmp	esi,CB_FINDSTRINGEXACT
	je	>LN105
	cmp	esi,CB_ADDSTRING
	je	>LN105
	cmp	esi,CB_INSERTSTRING
	je	>LN105
	cmp	esi,CB_SELECTSTRING
	je	>LN105
	cmp	esi,CB_DIR
	je	>LN105
	cmp	esi,CB_FINDSTRING
	je	>LN105
	cmp	esi,LB_FINDSTRINGEXACT
	je	>LN104
	cmp	esi,LB_ADDSTRING
	je	>LN104
	cmp	esi,WM_CREATE
	je	>L103
	cmp	esi,LB_INSERTSTRING
	je	>L104
	cmp	esi,LB_SELECTSTRING
	je	>L104
	cmp	esi,LB_DIR
	je	>L104
	cmp	esi,LB_FINDSTRING
	je	>L104
	cmp	esi,LB_ADDFILE
	je	>L104
	cmp	esi,WM_NCCREATE
	jne	>LN100
	
L103:
	
	mov	esi,[lParameter]
	push	SIZEOF CREATEWNDEX
	pop	ecx
	call	AllocateHeapInternal
	mov	edi,eax
	push	SIZEOF CREATEWNDEX / 4
	pop	ecx
	mov	ebx,eax
	rep	movsd
	cmp	[ebx + CREATEWNDEX.lpWindowName],ecx
	je	>L101
	push	[ebx + CREATEWNDEX.lpWindowName]
	call	WideCharToMultiByteInternal
	mov	[ebx + CREATEWNDEX.lpWindowName],eax
	mov	[lpAnsiWindowName],eax
	
L101:

	test	D[ebx + CREATEWNDEX.lpClassName],0xFFFF0000
	jz	>L102
	push	[ebx + CREATEWNDEX.lpClassName]
	call	WideCharToMultiByteInternal
	mov	[ebx + CREATEWNDEX.lpClassName],eax
	
L108:
	
	mov	[lpAnsiClassName],eax
	
L102:

	push	ebx
	jmp	>LN999

LN104:
L104:

	inc	ebx
	
LN105:

	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN100
	
LN113:
	
	mov	eax,[lParameter]
	test	eax,eax
	jz	>L1002
	push	eax
	call	WideCharToMultiByteInternal
	mov	ebx,eax
	
L1002:
	
	push	eax
	jmp	>LN999

LN106:

	mov	esi,[lParameter]
	push	SIZEOF CREATEMDIWND
	pop	ecx
	call	AllocateHeapInternal
	mov	edi,eax
	push	SIZEOF CREATEMDIWND / 4
	pop	ecx
	mov	ebx,eax
	rep	movsd
	cmp	[ebx + CREATEMDIWND.szTitle],ecx
	je	>L107
	push	[ebx + CREATEMDIWND.szTitle]
	call	WideCharToMultiByteInternal
	mov	[ebx + CREATEMDIWND.szTitle],eax
	mov	[lpAnsiWindowName],eax
	
L107:

	test	D[ebx + CREATEMDIWND.szClass],0xFFFF0000
	jz	<L102
	push	[ebx + CREATEMDIWND.szClass]
	call	WideCharToMultiByteInternal
	mov	[ebx + CREATEMDIWND.szClass],eax
	jmp	<L108
	
LN109:

	lea	esi,[wParameter]
	lea	edi,[CharBuffer]
	movzx	eax,W[esi]
	cmp	eax,0x7F
	jle	>L140
	push	ebx
	push	ebx
	push	2
	push	edi
	push	1
	push	esi
	push	ebx
	push	ebx
	call	[kernel32:WideCharToMultiByte]
	mov	eax,[edi]
	mov	ecx,eax
	shr	eax,8
	mov	ah,cl
	mov	[esi],eax
	
L140:
	
	jmp	>LN100
	
LN110:

	inc	ebx	
	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN100
	mov	eax,MAX_PATH * 2
	call	AllocateZeroedMemory
	mov	ebx,eax
	push	eax
	push	[wParameter]	
	push	esi
	push	[hWindow]
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	test	eax,eax
	mov	edi,[lParameter]
	jz	>L111
	inc	eax
	push	MAX_PATH
	push	edi
	push	eax
	push	ebx
	push	0
	push	eax
	call	[kernel32:MultiByteToWideChar]
	test	eax,eax
	jz	>L111
	dec	eax
	
L500:
	
	jmp	>LN995
	
L111:

	cmp	edi,eax
	jz	<L500
	mov	[edi],ax
	jmp	<L500
	
L599:

	mov	[esi],ax
	jmp	>LN995	
	
LN124:

	mov	esi,[lParameter]
	movzx	edi,W[esi]
	inc	edi
	lea	eax,[edi + edi]
	call	AllocateZeroedMemory
	lea	ecx,[edi + edi]
	mov	ebx,eax
	mov	[eax],cx
	push	eax
	push	[wParameter]
	push	[Message]
	push	[hWindow]
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	test	eax,eax
	jz	<L599
	inc	eax
	push	edi
	push	esi
	push	eax
	push	ebx
	push	0
	push	eax
	call	[kernel32:MultiByteToWideChar]
	test	eax,eax
	jz	<L500
	dec	eax
	jmp	<L500
	
LN129:

	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN100
	mov	eax,MAX_PATH * 2
	call	AllocateZeroedMemory
	mov	ebx,eax
	push	eax
	push	[wParameter]
	push	esi
	push	[hWindow]
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	test	eax,eax
	mov	edi,[lParameter]
	jz	>L130
	inc	eax
	push	MAX_PATH
	push	edi
	push	eax
	push	ebx
	push	0
	push	eax
	call	[kernel32:MultiByteToWideChar]
	test	eax,eax
	jz	>L130
	dec	eax
	
L131:

	jmp	>LN995
	
L130:

	cmp	edi,eax
	je	<L131
	mov	[edi],al
	jmp	<L131
	
LN131:

	mov	esi,eax
	mov	eax,[hWindow]
	inc	ebx
	mov	edi,eax
	call	CheckWindowStyle
	inc	eax
	jz	>LN100
	push	ebx
	push	[wParameter]
	push	LB_GETTEXTLEN
	push	edi
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	test	eax,eax
	jz	>LN995
	inc	D[esi + NTLEA_TLS_DATA.InternalCall]
	lea	eax,[eax + eax + 2]
	mov	esi,eax
	call	AllocateZeroedMemory
	mov	ebx,eax
	push	eax
	push	[wParameter]
	push	LB_GETTEXT
	jmp	>L132
	
LN127:

	mov	esi,eax
	mov	edi,[hWindow]
	push	ebx
	push	ebx
	push	ebx
	push	ebx
	push	WM_GETTEXTLENGTH
	push	edi
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	test	eax,eax
	jz	>LN995
	inc	D[esi + NTLEA_TLS_DATA.InternalCall]
	lea	eax,[eax + eax + 2]
	mov	esi,eax
	call	AllocateZeroedMemory
	mov	ebx,eax
	push	eax
	push	esi
	push	WM_GETTEXT
	
L132:
	
	push	edi
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	xor	eax,eax
	push	eax
	push	eax
	push	-1
	push	ebx
	push	eax
	push	eax
	call	[kernel32:MultiByteToWideChar]
	test	eax,eax
	jz	>L128
	dec	eax

L128:
	
	jmp	>LN995
	
LN121:

	dec	D[eax + NTLEA_TLS_DATA.InternalCall] ; Fix
	
	lea	esi,[ClassNameBuffer]
	mov	edi,[hWindow]
	push	MAX_PATH
	push	esi
	push	edi
	call	[user32:GetClassNameA]
	push	ADDR szSysTreeView32A
	push	esi
	call	[kernel32:lstrcmpiA]
	test	eax,eax
	jnz	>L122
	push	ebx
	push	edi
	call	[user32:GetWindowLongW]
	cmp	eax,ebx
	je	>L122
	mov	ecx,[eax + 0x10]
	test	cl,1
	je	>L122
	and	D[eax + 0x10],-2
	
L122:

	cmp	D[lParameter],NF_QUERY
	jne	>L123
	mov	eax,ebx
	inc	eax ; mov eax,NFR_ANSI
	jmp	>L117

LN114:

	push	1
	push	[lParameter]
	call	[kernel32:IsBadWritePtr]
	test	eax,eax
	jz	>L115
	call	GetTlsValueInternal
	dec	D[eax + NTLEA_TLS_DATA.InternalCall]
	
L123:
	
	xor	eax,eax
	
L117:
	
	push	eax
	jmp	>LN996
	
L115:

	push	ebx
	push	ebx
	push	WM_GETTEXTLENGTH
	push	[hWindow]
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	cmp	eax,ebx
	jne	>L116
	mov	edx,[lParameter]
	mov	[edx],bx
	jmp	<L117
	
L116:

	mov	edi,eax
	call	GetTlsValueInternal
	inc	D[eax + NTLEA_TLS_DATA.InternalCall]
	inc	edi
	lea	eax,[edi + edi]
	call	AllocateZeroedMemory
	mov	ebx,eax
	push	eax
	push	edi
	push	esi
	push	[hWindow]
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	cmp	esi,WM_UNKNOWN
	jne	>L118
	mov	[wParameter],edi
	
L118:

	mov	esi,[lParameter]
	push	eax
	push	[wParameter]
	push	esi
	push	-1
	push	ebx
	push	0
	push	eax
	call	[kernel32:MultiByteToWideChar]
	test	eax,eax
	jnz	>L997
	
L119:
	
	call	[kernel32:GetLastError]
	cmp	eax,ERROR_INSUFFICIENT_BUFFER
	jne	>L120
	mov	eax,[wParameter]
	and	W[esi + eax * 2 - 2],0
	jmp	>L994
	
L120:
	
	and	W[esi],0
	jmp	>L994

LN100:
L100:

	push	[lParameter]

LN999:
L999:
	
	push	[wParameter]	
	push	[Message]
	push	[hWindow]
	push	[PrevWndProc]
	call	[user32:CallWindowProcA]
	cmp	D[Message],WM_NCDESTROY
	jne	>L995
	mov	esi,ADDR szNtleaWndAscData
	mov	edi,[hWindow]
	push	esi
	push	edi
	call	[user32:GetPropA]
	mov	ecx,eax
	call	FreeStringInternal
	push	esi
	push	edi
	call	[user32:RemovePropA]

LN995:
L995:
	
	mov	ecx,[lpAnsiWindowName]
	push	eax
	test	ecx,ecx
	jz	>L998
	call	FreeStringInternal
	
L998:

	mov	ecx,[lpAnsiClassName]
	test	ecx,ecx
	jz	>L997
	call	FreeStringInternal

L997:
	
	test	ebx,ebx
	jz	>L996

L994:
	
	mov	ecx,ebx
	call	FreeStringInternal

L996:
LN996:

#ifdef	TRAP
	call	TrapFunc
#endif
	
	pop	eax
	ret

ENDF

ALIGN 4

CheckWindowStyle:

	push	GWL_STYLE
	push	eax
	call	[user32:GetWindowLongW]
	test	eax,eax
	jz	>C100
	cmp	eax,0x84C820E4
	je	>C101
	test	al,0x30
	jz	>C101
	test	ebx,ebx
	jz	>C102
	test	al,0x40
	jmp	>C103
	
C102:

	test	ah,2
	
C103:

	jnz	>C101
	or	eax,-1
	jmp	>C100
	
C101:
	
	xor	eax,eax
	
C100:

	xor	ebx,ebx
	retn

ALIGN 4
DefConversionProc    FRAME    lpProcAddress, hWindow, hMDIClient, bMDIClientEnabled, Message, wParameter, lParameter

	USES    esi,edi,ebx
	
	LOCAL    lpUnicodeWindowName
	LOCAL    lpUnicodeClassName
	LOCAL    CharBuffer
	
#ifdef	DBG
	int 3
#endif

#ifdef	TRAP
	call	TrapFunc
#endif

	push	[hWindow]
	call	[user32:IsWindowUnicode]
	test	eax,eax
	jnz	>L200
	mov	esi,ADDR szAppCallDefConvProc
	int	3;jmp	>ShowUnhandledExceptionMessage
	
L200:

	mov	esi,[Message]
	xor	ebx,ebx
	cmp	esi,WM_MDICREATE
	mov	[lpUnicodeWindowName],ebx
	mov	[lpUnicodeClassName],ebx
	mov	[CharBuffer],ebx
	je	>LN207
	cmp	esi,WM_IME_CHAR
	je	>LN209
	cmp	esi,WM_CHAR
	je	>LN209
	cmp	esi,WM_GETTEXTLENGTH
	je	>LN223
	cmp	esi,LB_GETTEXTLEN
	je	>LN226
	cmp	esi,LB_GETTEXT
	je	>LN221
	cmp	esi,CB_GETLBTEXT
	je	>LN222
	cmp	esi,EM_REPLACESEL
	je	>LN220
	cmp	esi,WM_SETTEXT
	je	>LN220
	cmp	esi,WM_SETTINGCHANGE
	je	>LN220
	cmp	esi,WM_DEVMODECHANGE
	je	>LN220
	cmp	esi,WM_GETTEXT
	je	>LN210
	cmp	esi,WM_UNKNOWN
	JE	>LN210
	cmp	esi,CB_FINDSTRINGEXACT
	je	>LN206
	cmp	esi,CB_ADDSTRING
	je	>LN206
	cmp	esi,CB_INSERTSTRING
	je	>LN206
	cmp	esi,CB_SELECTSTRING
	je	>LN206
	cmp	esi,CB_DIR
	je	>LN206
	cmp	esi,CB_FINDSTRING
	je	>LN206
	cmp	esi,LB_FINDSTRINGEXACT
	je	>LN205
	cmp	esi,LB_ADDSTRING
	je	>LN205
	cmp	esi,WM_CREATE
	je	>L204
	cmp	esi,LB_INSERTSTRING
	je	>L205
	cmp	esi,LB_FINDSTRING
	je	>L205
	cmp	esi,LB_ADDFILE
	je	>L205
	cmp	esi,LB_SELECTSTRING
	je	>L205
	cmp	esi,LB_DIR
	je	>L205
	cmp	esi,WM_NCCREATE
	jne	>LN201
	
L204:
	
	mov	esi,[lParameter]
	push	SIZEOF CREATEWNDEX
	pop	ecx
	call	AllocateHeapInternal
	mov	edi,eax
	push	SIZEOF CREATEWNDEX / 4
	pop	ecx
	mov	ebx,eax
	rep	movsd
	cmp	[ebx + CREATEWNDEX.lpWindowName],ecx
	je	>L202
	push	[ebx + CREATEWNDEX.lpWindowName]
	call	MultiByteToWideCharInternal
	mov	[ebx + CREATEWNDEX.lpWindowName],eax
	mov	[lpUnicodeWindowName],eax
	
L202:

	test	D[ebx + CREATEWNDEX.lpClassName],0xFFFF0000
	jz	>L203
	push	[ebx + CREATEWNDEX.lpClassName]
	call	MultiByteToWideCharInternal
	mov	[ebx + CREATEWNDEX.lpClassName],eax
	
L208:
	
	mov	[lpUnicodeClassName],eax
	
L203:

	push	ebx
	jmp	>LN899

LN205:
L205:

	inc	ebx
	
LN206:
	
	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN201
	push	[lParameter]
	call	MultiByteToWideCharInternal
	mov	ebx,eax
	push	eax
	jmp	>LN899
	
LN207:

	mov	esi,[lParameter]
	push	SIZEOF CREATEMDIWND
	pop	ecx
	call	AllocateHeapInternal
	mov	edi,eax
	push	SIZEOF CREATEMDIWND / 4
	pop	ecx
	mov	ebx,eax
	rep	movsd
	cmp	[ebx + CREATEMDIWND.szTitle],ecx
	je	>L202
	push	[ebx + CREATEMDIWND.szTitle]
	call	MultiByteToWideCharInternal
	mov	[ebx + CREATEMDIWND.szTitle],eax
	mov	[lpUnicodeWindowName],eax
	
L202:

	test	D[ebx + CREATEMDIWND.szClass],0xFFFF0000
	jz	<L203
	push	[ebx + CREATEMDIWND.szClass]
	call	MultiByteToWideCharInternal
	mov	[ebx + CREATEMDIWND.szClass],eax
	jmp	<L208
	
LN209:
	
	lea	esi,[wParameter]
	lea	edi,[CharBuffer]
	movzx	eax,W[esi]
	cmp	eax,0x7F
	jle	>L260
	mov	ecx,eax
	push	2
	push	edi
	shr	eax,8
	push	-1
	push	esi
	mov	ah,cl
	push	ebx
	push	ebx
	mov	[esi],eax
	call	[kernel32:MultiByteToWideChar]
	mov	eax,[edi]
	mov	[esi],eax
	
L260:
	
	jmp	>LN201
	
/*
	
LN218:

	call	GetTlsValueInternal
	mov	esi,eax
	mov	cl,[eax + NTLEA_TLS_DATA.DBCSLeadByte]
	lea	edi,[wParameter]
	test	cl,cl
	jnz	>L219
	mov	al,[edi]
	test	al,0x80
	jz	>LN201
	mov	[esi + NTLEA_TLS_DATA.DBCSLeadByte],al
	xor	eax,eax
	inc	eax
	jmp	>LN893
	
L219:

	mov	B[esi + NTLEA_TLS_DATA.DBCSLeadByte],0
	shl	D[edi],8
	lea	esi,[CharBuffer]
	mov	[edi],cl
	push	2
	push	esi
	push	-1
	push	edi
	push	0
	push	eax
	call	[kernel32:MultiByteToWideChar]
	mov	eax,[esi]
	mov	[edi],eax
	jmp	>LN201
	
*/
	
LN221:

	inc	ebx

LN222:

	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN201
	mov	eax,MAX_PATH * 2
	call	AllocateZeroedMemory
	cmp	[bMDIClientEnabled],ebx
	push	eax
	push	[wParameter]
	push	esi
	je	>L406
	push	[hMDIClient]
	
L406:

	push	[hWindow]
	mov	ebx,eax
	call	[lpProcAddress]
	test	eax,eax
	mov	edi,[lParameter]
	jz	>L240
	xor	ecx,ecx
	inc	eax
	push	ecx
	push	ecx
	push	MAX_PATH * 2
	push	edi
	push	eax
	push	ebx
	push	ecx
	push	ecx
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L240
	dec	eax
	
L502:
	
	jmp	>LN893
	
L240:

	cmp	edi,eax
	je	<L502
	mov	[edi],al
	jmp	<L502
	
LN226:

	inc	ebx
	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN201
	cmp	[bMDIClientEnabled],ebx
	push	ebx
	push	[wParameter]
	push	LB_GETTEXTLEN
	je	>L227
	push	[hMDIClient]
	
L227:

	push	[hWindow]
	call	[lpProcAddress]
	test	eax,eax
	jz	>LN893
	inc	eax
	mov	edi,eax
	add	eax,eax
	call	AllocateZeroedMemory
	cmp	[bMDIClientEnabled],ebx
	push	eax
	push	[wParameter]
	push	LB_GETTEXT
	jmp	>L228
	
LN223:

	cmp	[bMDIClientEnabled],ebx
	push	ebx
	push	ebx
	push	WM_GETTEXTLENGTH
	je	>L224
	push	[hMDIClient]
	
L224:

	push	[hWindow]
	call	[lpProcAddress]
	test	eax,eax
	jz	>LN893
	inc	eax
	mov	edi,eax
	add	eax,eax
	call	AllocateZeroedMemory
	cmp	[bMDIClientEnabled],ebx
	push	eax
	push	edi
	push	WM_GETTEXT
	
L228:
	
	mov	ebx,eax
	je	>L225
	push	[hMDIClient]
	
L225:

	push	[hWindow]
	call	[lpProcAddress]
	xor	eax,eax
	push	eax
	push	eax
	push	eax
	push	eax
	push	-1
	push	ebx
	push	eax
	push	eax
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L250
	dec	eax
	
L250:
		
	jmp	>LN893
	
LN220:

	mov	eax,[lParameter]
	test	eax,eax
	jz	>L1000
	push	eax
	call	MultiByteToWideCharInternal
	mov	ebx,eax
	
L1000:
	
	push	eax
	jmp	>LN899
	
LN210:

	push	1
	push	[lParameter]
	call	[kernel32:IsBadWritePtr]
	test	eax,eax
	jz	>L211
	xor	eax,eax
	
L212:
	
	ret
	
L211:

	cmp	[bMDIClientEnabled],eax
	push	eax
	push	eax
	push	WM_GETTEXTLENGTH
	je	>L213
	push	[hMDIClient]
	
L213:

	push	[hWindow]
	call	[lpProcAddress]
	lea	eax,[eax + eax + 2]
	mov	edi,eax
	call	AllocateZeroedMemory
	cmp	[bMDIClientEnabled],ebx
	push	eax
	push	[wParameter]
	push	esi
	je	>L214
	push	[hMDIClient]

L214:

	push	[hWindow]
	mov	ebx,eax
	call	[lpProcAddress]
	xor	eax,eax
	cmp	esi,WM_UNKNOWN
	je	>L215
	mov	edi,[wParameter]
	
L215:

	mov	esi,[lParameter]
	push	eax
	push	eax
	push	edi
	push	esi
	push	-1
	push	ebx
	push	eax
	push	eax
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L216
	dec	eax
	jmp	>L893
	
L216:

	call	[kernel32:GetLastError]
	cmp	eax,ERROR_INSUFFICIENT_BUFFER
	jne	>L217
	mov	eax,[wParameter]
	dec	eax
	mov	B[esi + eax],0
	jmp	>L893
	
L217:
	
	xor	eax,eax
	mov	[esi],al
	jmp	>L893

LN201:
L201:

	push	[lParameter]

LN899:
L899:
	
	cmp	D[bMDIClientEnabled],0
	push	[wParameter]
	push	[Message]
	je	>L898
	push	[hMDIClient]
	
L898:

	push	[hWindow]
	call	[lpProcAddress]

LN893:
L893:
	
	mov	ecx,[lpUnicodeWindowName]
	push	eax
	test	ecx,ecx
	jz	>L896
	call	FreeStringInternal
	
L896:

	mov	ecx,[lpUnicodeClassName]
	test	ecx,ecx
	jz	>L895
	call	FreeStringInternal
	
L895:
	
	test	ebx,ebx
	jz	>L894
	mov	ecx,ebx
	call	FreeStringInternal

L894:

	pop	eax
	ret

ENDF

ALIGN 4
HookCallWindowProc:

#ifdef	DBG
	int 3
#endif

	call	GetTlsValueInternal
	cmp	D[eax + NTLEA_TLS_DATA.InternalCall],0
	je	>C200
	dec	D[eax + NTLEA_TLS_DATA.InternalCall]
	jmp	>C200
	
C202:
	
	push	ebp
	mov	ebp,esp
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>C210
	push	1
	
C210:
	
	jmp	[lpCallWindowProcAddress]

C200:

	mov	edx,[esp + 4]
	xor	ecx,ecx
	
C203:

	cmp	edx,[eax + ecx * 8 + NTLEA_TLS_DATA.SystemClassDesc]
	je	>C204
	inc	ecx
	cmp	ecx,14
	jne	<C203
	jmp	<C202
	
C204:

#ifdef	TRAP
	call	TrapFunc
#endif

	mov	edx,[user32:CallWindowProcW]
	push	3
	push	ecx
	push	ecx
	push	[eax + ecx * 8 + NTLEA_TLS_DATA.SystemClassDesc + 4]
	push	[lParam + 20]
	push	[wParam + 24]
	push	[Msg + 28]
	push	[hWnd + 32]
	push	edx
	call	SendUnicodeMessage
	retn	20

ALIGN 4

HookSendMessage:

	mov	eax,[user32:SendMessageW]
	
Y100:
	
	push	0
	push	ecx
	push	ecx
	push	ecx
	push	[lParam + 16]
	push	[wParam + 20]
	push	[Msg + 24]
	push	[hWnd + 28]
	push	eax
	call	SendUnicodeMessage
	retn	16

ALIGN 4

HookPostMessage:

	mov	eax,[user32:PostMessageW]
	jmp	<Y100
	
ALIGN 4

HookSendNotifyMessage:

	mov	eax,[user32:SendNotifyMessageW]
	jmp	<Y100
	
ALIGN 4

HookSendMessageCallback:

	mov	eax,[user32:SendMessageCallbackW]
	push	1
	push	ecx
	push	[dwData + 8]
	push	[lpCallBack + 12]
	push	[lParam + 16]
	push	[wParam + 20]
	push	[Msg + 24]
	push	[hWnd + 28]
	push	eax
	call	SendUnicodeMessage
	retn	24
	
ALIGN 4

HookSendMessageTimeout:

	mov	eax,[user32:SendMessageTimeoutW]
	push	2
	push	[lpdwResult + 4]
	push	[uTimeout + 8]
	push	[fuFlags + 12]
	push	[lParam + 16]
	push	[wParam + 20]
	push	[Msg + 24]
	push	[hWnd + 28]
	push	eax
	call	SendUnicodeMessage
	retn	28

ALIGN 4

HookDialogBoxIndirectParam:

	mov	eax,[user32:DialogBoxIndirectParamW]
	
Y103:

	push	ebx
	push	eax
	call	GetTlsValueInternal
	mov	ebx,eax
	call	InstallCbtHook
	mov	edx,[esp + 24]
	pop	ecx
	push	[ebx + NTLEA_TLS_DATA.DialogProc]
	push	[ebx + NTLEA_TLS_DATA.CurrentCallType]
	mov	[ebx + NTLEA_TLS_DATA.DialogProc],edx
	mov	D[ebx + NTLEA_TLS_DATA.CurrentCallType],CT_CREATE_DIALOG
	push	[esp + 32]
	push	ADDR TopLevelDialogProc
	push	[esp + 32]
	push	[esp + 32]
	push	[esp + 32]
	call	ecx
	pop	[ebx + NTLEA_TLS_DATA.CurrentCallType]
	pop	[ebx + NTLEA_TLS_DATA.DialogProc]
	push	eax
	call	UninstallCbtHook
	pop	eax
	pop	ebx
	retn	20
	
ALIGN 4
	
HookCreateDialogIndirectParam:

	mov	eax,[user32:CreateDialogIndirectParamW]
	jmp	<Y103
	
ALIGN 4
	
HookCreateDialogParam:

	mov	eax,[user32:CreateDialogParamW]
	jmp	>Y104
	
ALIGN 4

HookDialogBoxParam:

	mov	eax,[user32:DialogBoxParamW]
	
Y104:

DialogBoxParamProc    FRAME    hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam

	LOCAL   PrevDialogFunc
	LOCAL   PrevCallType

	USES    esi,ebx

	push	eax
	call	GetTlsValueInternal
	mov	ebx,eax
	call	InstallCbtHook
	mov	ecx,[lpDialogFunc]
	mov	edx,[ebx + NTLEA_TLS_DATA.DialogProc]
	mov	esi,[lpTemplate]
	mov	[ebx + NTLEA_TLS_DATA.DialogProc],ecx
	mov	[PrevDialogFunc],edx
	mov	ecx,[ebx + NTLEA_TLS_DATA.CurrentCallType]
	test	esi,0xFFFF0000
	mov	D[ebx + NTLEA_TLS_DATA.CurrentCallType],CT_CREATE_DIALOG
	mov	[PrevCallType],ecx
	jz	>Y105
	push	esi
	call	MultiByteToWideCharInternal
	mov	esi,eax
	
Y105:

	pop	ecx
	push	[dwInitParam]
	push	ADDR TopLevelDialogProc
	push	[hWndParent]
	push	esi
	push	[hInstance]
	call	ecx
	push	eax
	call	UninstallCbtHook
	mov	edx,[PrevDialogFunc]
	mov	ecx,[PrevCallType]
	test	esi,0xFFFF0000
	mov	[ebx + NTLEA_TLS_DATA.DialogProc],edx
	mov	[ebx + NTLEA_TLS_DATA.CurrentCallType],ecx
	jz	>Y106
	mov	ecx,esi
	call	FreeStringInternal
	
Y106:

	pop	eax
	ret

ENDF

ALIGN 4

SendUnicodeMessage    FRAME    lpProcAddress, hWindow, Message, wParameter, lParameter, Parameter1, Parameter2, Parameter3, FunctionType
	
	USES    esi,edi,ebx
	
	LOCAL    lpUnicodeWindowName
	LOCAL    lpUnicodeClassName
	LOCAL    CharBuffer
	
#ifdef	DBG
	int 3
#endif

#ifdef TRAP
	call	TrapFunc
#endif
	
	mov	esi,[Message]
	xor	ebx,ebx
	cmp	esi,WM_MDICREATE
	mov	[lpUnicodeWindowName],ebx
	mov	[lpUnicodeClassName],ebx
	mov	[CharBuffer],ebx
	je	>LN307
	cmp	esi,EM_GETLINE
	je	>LN321
	cmp	esi,WM_GETTEXTLENGTH
	je	>LN327
	cmp	esi,LB_GETTEXTLEN
	je	>LN329
	cmp	esi,LB_GETTEXT
	je	>LN322
	cmp	esi,CB_GETLBTEXT
	je	>LN323
	cmp	esi,WM_IME_CHAR
	je	>LN309
	cmp	esi,WM_CHAR
	je	>LN309
	cmp	esi,EM_REPLACESEL
	je	>LN320
	cmp	esi,WM_SETTEXT
	je	>LN320
	cmp	esi,WM_SETTINGCHANGE
	je	>LN320
	cmp	esi,WM_DEVMODECHANGE
	je	>LN320
	cmp	esi,WM_GETTEXT
	je	>LN310
	cmp	esi,WM_UNKNOWN
	JE	>LN310
	cmp	esi,CB_FINDSTRINGEXACT
	je	>LN306
	cmp	esi,CB_ADDSTRING
	je	>LN306
	cmp	esi,CB_INSERTSTRING
	je	>LN306
	cmp	esi,CB_SELECTSTRING
	je	>LN306
	cmp	esi,CB_DIR
	je	>LN306
	cmp	esi,CB_FINDSTRING
	je	>LN306
	cmp	esi,LB_FINDSTRINGEXACT
	je	>LN305
	cmp	esi,LB_ADDSTRING
	je	>LN305
	cmp	esi,WM_CREATE
	je	>L304
	cmp	esi,LB_INSERTSTRING
	je	>L305
	cmp	esi,LB_FINDSTRING
	je	>L305
	cmp	esi,LB_ADDFILE
	je	>L305
	cmp	esi,LB_SELECTSTRING
	je	>L305
	cmp	esi,LB_DIR
	je	>L305
	cmp	esi,WM_NCCREATE
	jne	>LN301
	
L304:
	
	mov	esi,[lParameter]
	push	SIZEOF CREATEWNDEX
	pop	ecx
	call	AllocateHeapInternal
	mov	edi,eax
	push	SIZEOF CREATEWNDEX / 4
	pop	ecx
	mov	ebx,eax
	rep	movsd
	cmp	[ebx + CREATEWNDEX.lpWindowName],ecx
	je	>L302
	push	[ebx + CREATEWNDEX.lpWindowName]
	call	MultiByteToWideCharInternal
	mov	[ebx + CREATEWNDEX.lpWindowName],eax
	mov	[lpUnicodeWindowName],eax
	
L302:

	test	D[ebx + CREATEWNDEX.lpClassName],0xFFFF0000
	jz	>L303
	push	[ebx + CREATEWNDEX.lpClassName]
	call	MultiByteToWideCharInternal
	mov	[ebx + CREATEWNDEX.lpClassName],eax
	
L308:
	
	mov	[lpUnicodeClassName],eax
	
L303:

	mov	[lParameter],ebx
	jmp	>LN301

LN305:
L305:

	inc	ebx
	
LN306:
	
	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN301
	push	[lParameter]
	call	MultiByteToWideCharInternal
	mov	ebx,eax
	mov	[lParameter],eax
	jmp	>LN301
	
LN307:

	mov	esi,[lParameter]
	push	SIZEOF CREATEMDIWND
	pop	ecx
	call	AllocateHeapInternal
	mov	edi,eax
	push	SIZEOF CREATEMDIWND / 4
	pop	ecx
	mov	ebx,eax
	rep	movsd
	cmp	[ebx + CREATEMDIWND.szTitle],ecx
	je	>L302
	push	[ebx + CREATEMDIWND.szTitle]
	call	MultiByteToWideCharInternal
	mov	[ebx + CREATEMDIWND.szTitle],eax
	mov	[lpUnicodeWindowName],eax
	
L302:

	test	D[ebx + CREATEMDIWND.szClass],0xFFFF0000
	jz	<L303
	push	[ebx + CREATEMDIWND.szClass]
	call	MultiByteToWideCharInternal
	mov	[ebx + CREATEMDIWND.szClass],eax
	jmp	<L308
	
LN309:
	
	lea	esi,[wParameter]
	lea	edi,[CharBuffer]
	movzx	eax,W[esi]
	cmp	eax,0x7F
	jle	>L340
	mov	ecx,eax
	push	2
	push	edi
	shr	eax,8
	push	-1
	push	esi
	mov	ah,cl
	push	ebx
	push	ebx
	mov	[esi],eax
	call	[kernel32:MultiByteToWideChar]
	mov	eax,[edi]
	mov	[esi],eax
	
L340:
	
	jmp	>LN301
	
/*
	
LN318:

	call	GetTlsValueInternal
	mov	esi,eax
	mov	cl,[eax + NTLEA_TLS_DATA.DBCSLeadByte]
	lea	edi,[wParameter]
	test	cl,cl
	jnz	>L319
	mov	al,[edi]
	test	al,0x80
	jz	>LN301
	mov	[esi + NTLEA_TLS_DATA.DBCSLeadByte],al
	xor	eax,eax
	inc	eax
	jmp	>LN793
	
L319:

	mov	B[esi + NTLEA_TLS_DATA.DBCSLeadByte],0
	shl	D[edi],8
	lea	esi,[CharBuffer]
	mov	[edi],cl
	push	2
	push	esi
	push	-1
	push	edi
	push	0
	push	eax
	call	[kernel32:MultiByteToWideChar]
	mov	eax,[esi]
	mov	[edi],eax
	jmp	>LN301
	
*/
	
LN322:

	inc	ebx
	
LN323:
	
	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN301
	mov	eax,MAX_PATH * 2
	call	AllocateZeroedMemory
	mov	ebx,eax
	mov	ecx,[FunctionType]
	test	ecx,ecx
	jz	>L404
	cmp	ecx,1
	je	>L405
	cmp	ecx,3
	je	>L404
	push	[Parameter3]
	
L405:

	push	[Parameter2]
	push	[Parameter1]
	
L404:
	
	push	eax
	push	[wParameter]	
	push	esi
	push	[hWindow]
	cmp	ecx,3
	jne	>U100
	push	[Parameter1]
	
U100:
	
	call	[lpProcAddress]
	test	eax,eax
	mov	edi,[lParameter]
	jz	>L324
	xor	ecx,ecx
	inc	eax
	push	ecx
	push	ecx
	push	MAX_PATH * 2
	push	edi
	push	eax
	push	ebx
	push	ecx
	push	ecx
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L324
	dec	eax
	
L501:
	
	jmp	>LN793
	
L324:

	cmp	edi,eax
	je	<L501
	mov	[edi],al
	jmp	<L501
	
LN329:

	inc	ebx
	mov	eax,[hWindow]
	call	CheckWindowStyle
	inc	eax
	jz	>LN301
	mov	ecx,[FunctionType]
	cmp	ecx,ebx
	je	>L437
	cmp	ecx,1
	je	>L438
	cmp	ecx,3
	je	>L437
	push	[Parameter3]
	
L438:

	push	[Parameter2]
	push	[Parameter1]
	
L437:
	
	push	ebx
	push	[wParameter]
	push	LB_GETTEXTLEN
	push	[hWindow]
	cmp	ecx,3
	jne	>U113
	push	[Parameter1]
	
U113:

	call	[lpProcAddress]
	test	eax,eax
	jz	>LN793
	inc	eax
	mov	edi,eax
	add	eax,eax
	call	AllocateZeroedMemory
	mov	ebx,eax
	mov	ecx,[FunctionType]
	test	ecx,ecx
	jz	>L447
	cmp	ecx,1
	je	>L448
	cmp	ecx,3
	je	>L447
	push	[Parameter3]
	
L448:

	push	[Parameter2]
	push	[Parameter1]
	
L447:
	
	push	eax
	push	[wParameter]
	push	LB_GETTEXT
	push	[hWindow]
	cmp	ecx,3
	jne	>U112
	push	[Parameter1]
	jmp	>U112
	
LN327:

	mov	ecx,[FunctionType]
	cmp	ecx,ebx
	je	>L417
	cmp	ecx,1
	je	>L418
	cmp	ecx,3
	je	>L417
	push	[Parameter3]
	
L418:

	push	[Parameter2]
	push	[Parameter1]
	
L417:
	
	push	ebx
	push	ebx
	push	WM_GETTEXTLENGTH
	push	[hWindow]
	cmp	ecx,3
	jne	>U111
	push	[Parameter1]
	
U111:

	call	[lpProcAddress]
	test	eax,eax
	jz	>LN793
	inc	eax
	mov	edi,eax
	add	eax,eax
	call	AllocateZeroedMemory
	mov	ebx,eax
	mov	ecx,[FunctionType]
	test	ecx,ecx
	jz	>L427
	cmp	ecx,1
	je	>L428
	cmp	ecx,3
	je	>L427
	push	[Parameter3]
	
L428:

	push	[Parameter2]
	push	[Parameter1]
	
L427:
	
	push	eax
	push	edi
	push	WM_GETTEXT
	push	[hWindow]
	cmp	ecx,3
	jne	>U112
	push	[Parameter1]
	
U112:

	call	[lpProcAddress]
	xor	eax,eax
	push	eax
	push	eax
	push	eax
	push	eax
	push	-1
	push	ebx
	push	eax
	push	eax
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	je	>L328
	dec	eax
	
L328:
	
	jmp	>LN793
	
LN321:

	mov	edi,[lParameter]
	push	1
	push	edi
	call	[kernel32:IsBadWritePtr]
	test	eax,eax
	jnz	>LN325
	movzx	esi,W[edi]
	inc	esi
	lea	eax,[esi + esi]
	call	AllocateZeroedMemory
	mov	ecx,[FunctionType]
	cmp	ecx,ebx
	je	>L407
	cmp	ecx,1
	je	>L408
	cmp	ecx,3
	je	>L407
	push	[Parameter3]
	
L408:

	push	[Parameter2]
	push	[Parameter1]
	
L407:
	
	push	eax
	push	[wParameter]
	push	[Message]
	push	[hWindow]
	cmp	ecx,3
	jne	>U101
	push	[Parameter1]
	
U101:
	
	mov	[eax],si
	mov	ebx,eax
	call	[lpProcAddress]
	test	eax,eax
	jz	>L326
	xor	ecx,ecx
	dec	esi
	push	ecx
	push	ecx
	push	esi
	push	edi
	push	-1
	push	ebx
	push	ecx
	push	ecx
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L326
	dec	eax
	jmp	>LN793
	
LN320:

	mov	eax,[lParameter]
	test	eax,eax
	jz	>L1001
	push	eax
	call	MultiByteToWideCharInternal
	mov	ebx,eax
	mov	[lParameter],eax
	
L1001:
	
	jmp	>LN301
	
LN310:

	push	1
	push	[lParameter]
	call	[kernel32:IsBadWritePtr]
	test	eax,eax
	jz	>L311

LN325:
	
	xor	eax,eax	
	ret
	
L326:

	mov	[edi],al
	jmp	>LN793
	
L311:

	mov	ecx,[FunctionType]
	test	ecx,ecx
	jz	>L313
	cmp	ecx,1
	je	>L400
	cmp	ecx,3
	je	>L313
	push	[Parameter3]
	
L400:

	push	[Parameter2]
	push	[Parameter1]
	
L313:
	
	push	eax
	push	eax
	push	WM_GETTEXTLENGTH
	push	[hWindow]
	cmp	ecx,3
	jne	>U102
	push	[Parameter1]
	
U102:
	
	call	[lpProcAddress]
	lea	eax,[eax + eax + 2]
	mov	edi,eax
	call	AllocateZeroedMemory
	mov	ecx,[FunctionType]
	cmp	ecx,ebx
	je	>L314
	cmp	ecx,1
	je	>L401
	cmp	ecx,3
	je	>L314
	push	[Parameter3]

L401:

	push	[Parameter2]
	push	[Parameter1]

L314:

	push	eax
	push	[wParameter]
	push	esi
	push	[hWindow]
	cmp	ecx,3
	jne	>U103
	push	[Parameter1]
	
U103:

	mov	ebx,eax
	call	[lpProcAddress]
	xor	eax,eax
	cmp	esi,WM_UNKNOWN
	je	>L315
	mov	edi,[wParameter]
	
L315:

	mov	esi,[lParameter]
	push	eax
	push	eax
	push	edi
	push	esi
	push	-1
	push	ebx
	push	eax
	push	eax
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L316
	dec	eax
	jmp	>L793
	
L316:

	call	[kernel32:GetLastError]
	cmp	eax,ERROR_INSUFFICIENT_BUFFER
	jne	>L317
	mov	eax,[wParameter]
	dec	eax
	mov	B[esi + eax],0
	jmp	>L793
	
L317:
	
	xor	eax,eax
	mov	[esi],al
	jmp	>L793

LN301:
L301:

	mov	ecx,[FunctionType]
	je	>L402
	cmp	ecx,1
	je	>L403
	cmp	ecx,3
	je	>L402
	push	[Parameter3]
	
L403:

	push	[Parameter2]
	push	[Parameter1]
	
L402:
	
	push	[lParameter]
	push	[wParameter]
	push	[Message]
	push	[hWindow]
	cmp	ecx,3
	jne	>U104
	push	[Parameter1]
	
U104:
	
	call	[lpProcAddress]

LN793:
L793:
	
	mov	ecx,[lpUnicodeWindowName]
	push	eax
	test	ecx,ecx
	jz	>L796
	call	FreeStringInternal
	
L796:

	mov	ecx,[lpUnicodeClassName]
	test	ecx,ecx
	jz	>L795
	call	FreeStringInternal
	
L795:
	
	test	ebx,ebx
	jz	>L794
	mov	ecx,ebx
	call	FreeStringInternal

L794:

	pop	eax	
	ret

ENDF

ALIGN 4

NtleaExceptionHandler:

	
	
ALIGN 4

InitUnicodeLayer:

#ifdef	DBG
	int 3
#endif

#ifdef	DBG_ENTRY
	int 3
#endif

#ifdef	TRAP
	mov	eax,ADDR bTrapFlag
#endif

/*

	;LCMapString

	mov	eax,MAX_WINDOW_SUPPORTED * 4 * 2 + (MAX_CLASS_SUPPORTED * 4)
	call	AllocateZeroedMemory
	mov	[lpWindowProcTable],eax
	add	eax,MAX_CLASS_SUPPORTED * 4
	mov	[lpWindowHandleTable],eax
	
*/

	push	ADDR szAlLayer
	call	[kernel32:GetModuleHandleA]
	test	eax,eax
	jz	>F100
	mov	esi,ADDR szMultipleHookNotAllowed
	jmp	>ShowUnhandledExceptionMessage
	
F100:
	
	mov	D[context],CONTEXT_CONTROL
	xor	ebx,ebx
	push	ADDR szRcpFileMap
	push	4
	push	ebx
	push	PAGE_READONLY
	push	ebx
	push	-1
	call	[kernel32:CreateFileMappingA]
	cmp	eax,INVALID_HANDLE_VALUE
	jne	>XX1
	push	OFFSET szErrorFilemapping
	call	[kernel32:OutputDebugStringA]

XX1:

	push	eax
	push	4
	push	ebx
	push	ebx
	push	FILE_MAP_READ
	push	eax
	call	[kernel32:MapViewOfFile]
	cmp	eax,0
	jne >XX2
	push	OFFSET szErrorFilemapview
	call	[kernel32:OutputDebugStringA]

XX2:

	push	eax
	mov	eax,[eax]
	mov	edx,ebx
	mov	[lpParameterAddress],eax
	
C0:
	
	mov	dx,[eax]
	inc	eax
	inc	eax
	cmp	edx,ebx
	jne	<C0
	mov	ecx,eax
	mov	[lpFontFaceName],eax
	
C1:
	
	mov	dl,[ecx]
	inc	ecx
	cmp	edx,ebx
	jne	<C1
	mov	eax,[ecx]
	mov	esi,[ecx + 4]
	test	al,1
	jz	>J0
	mov	[bChinesePath],al
	
J0:

	test	al,2
	jz	>J1
	mov	[bCreateProcNative],al
	
J1:

	test	al,4
	jz	>J2
	mov	[bForceSpecifyFont],al
	
J2:
	
	mov	eax,[ecx + 8]
	mov	edx,[ecx + 12]
	mov	[dwLocaleID],eax
	mov	[dwTimeZone],edx
	mov	eax,[ecx + 16]
	mov	[dwCodePage],esi
	test	eax,eax
	jz	>J3
	mov	[dwFontSizePercent],eax
	
J3:
	
	call	[kernel32:UnmapViewOfFile]
	call	[kernel32:CloseHandle]
	mov	eax,SIZEOF OSVERSIONINFO
	mov	esi,esp
	sub	esp,eax
	mov	[esp + OSVERSIONINFO.dwOSVersionInfoSize],eax
	push	esp
	call	[kernel32:GetVersionExA]
	cmp	D[esp + OSVERSIONINFO.dwMajorVersion],VER_MAJOR_WIN2K
	jb	>EOF
	ja	>E7
	cmp	[esp + OSVERSIONINFO.dwMinorVersion],ebx
	jne	>E3
	mov	al,VER_WIN2K
	jmp	>E0
	
E3:

	cmp	D[esp + OSVERSIONINFO.dwMinorVersion],1
	jne	>E7
	cmp	[esp + OSVERSIONINFO.szCSDVersion],bl
	je	>E6
	cmp	B[esp + OSVERSIONINFO.szCSDVersion + 13],'2'
	jae	>E7
	mov	al,VER_WINXP_SP1
	jmp	>E0
	
E6:
	
	mov	al,VER_WINXP
	
E0:

	mov	[nOSVer],al

E7:

	mov	esp,esi
	mov	edi,ADDR HookDllFunc
	push	ADDR szNtDLLName
	call	[kernel32:LoadLibraryA]
	push	ADDR szNlsMbCodePageTag
	push	eax
	push	ADDR szNlsAnsiCodePage
	push	eax
	push	eax
	push	ADDR HookMultiByteToUnicode
	push	ADDR szMultiByteToUnicode
	cmp	[bChinesePath],bl
	jne	>A7
	push	eax
	push	ADDR HookUnicodeToMultiByte
	push	ADDR szUnicodeToMultiByte
	call	edi
	cmp	B[nOSVer],VER_WIN2K
	jne	>A0
	add	esi,5
	
A0:

	mov	[lpUnitoMBAddress],esi
	
A7:

	call	edi	
	cmp	B[nOSVer],VER_WIN2K
	jne	>A5
	add	esi,5
	
A5:

	mov	[lpMBtoUniAddress],esi
	call	[kernel32:GetProcAddress]
	test	eax,eax
	jz	>E1
	mov	ecx,[dwCodePage]
	mov	[eax],ecx
	
E1:

	call	[kernel32:GetProcAddress]
	test	eax,eax
	jz	>E4
	mov	cl,[eax]
	mov	[MbCodePageTag],cl
	
E4:

	push	ADDR szGdiDLLName
	call	[kernel32:LoadLibraryA]
	push	eax
	push	ADDR HookGdiGetCodePage
	push	ADDR szGdiGetCodePage
	call	edi
	mov	eax,[kernel32:GetACP]
	push	ebx
	push	ADDR HookGetACP
	push	eax
	call	edi	
	mov	eax,[kernel32:GetOEMCP]
	push	ebx
	push	ADDR HookGetACP
	push	eax
	call	edi
	mov	eax,[kernel32:GetCPInfo]
	push	ebx
	push	ADDR HookGetCPInfo
	push	eax
	call	edi
	mov	[lpCPInfoAddress],esi
	
/*

	mov	eax,[kernel32:GetCPInfoExA]
	push	ebx
	push	ADDR HookGetCPInfoEx
	push	eax
	call	edi
	
*/
	
	cmp	[bChinesePath],bl
	je	>A6
	mov	eax,[kernel32:CreateFileA]
	push	ebx
	push	ADDR HookCreateFile
	push	eax
	call	edi
	cmp	[nOSVer],bl
	je	>A3
	inc	esi
	
A3:

	mov	[lpCreateFileAddress],esi
	
A6:

	mov	eax,[kernel32:CompareStringA]
	push	ebx
	push	ADDR HookCompareString
	push	eax
	call	edi
	cmp	[nOSVer],bl
	je	>A8
	add	esi,4
	
A8:
	
	mov	[lpCompareStringAddress],esi
	mov	eax,[kernel32:CreateProcessA]
	push	ebx
	push	ADDR HookCreateProcessA
	push	eax
	call	edi
	cmp	B[nOSVer],VER_WIN2K
	jne	>A9
	inc	esi
	
A9:
	
	mov	[lpCreateProcAnsiAddress],esi
	mov	eax,[kernel32:CreateProcessW]
	push	ebx
	push	ADDR HookCreateProcessW
	push	eax
	call	edi
	cmp	B[nOSVer],VER_WIN2K
	jne	>A9
	inc	esi
	
A9:
	
	mov	[lpCreateProcUniAddress],esi
	mov	eax,[kernel32:GetTimeZoneInformation]
	push	ebx
	push	ADDR HookGetTimeZoneInformation
	push	eax
	call	edi
	cmp	B[nOSVer],VER_WIN2K
	jne	>A10
	add	esi,4
	
A10:
	
	mov	[lpGetTimeZoneInformation],esi
	mov	eax,[version:VerQueryValueA]
	push	ebx
	push	ADDR HookVerQueryValue
	push	eax
	call	edi
	cmp	[nOSVer],bl
	je	>A11
	inc	esi

A11:
	
	mov	[lpVerQueryValueAddress],esi
	mov	eax,[kernel32:GetThreadLocale]
	push	ebx
	push	ADDR HookGetLocaleID
	push	eax
	call	edi
	mov	eax,[kernel32:GetSystemDefaultUILanguage]
	push	ebx
	push	ADDR HookGetLocaleID
	push	eax
	call	edi
	mov	eax,[kernel32:GetUserDefaultUILanguage]
	push	ebx
	push	ADDR HookGetLocaleID
	push	eax
	call	edi
	mov	eax,[kernel32:GetSystemDefaultLCID]
	push	ebx
	push	ADDR HookGetLocaleID
	push	eax
	call	edi
	mov	eax,[kernel32:GetUserDefaultLCID]
	push	ebx
	push	ADDR HookGetLocaleID
	push	eax
	call	edi
	mov	eax,[kernel32:GetSystemDefaultLangID]
	push	ebx
	push	ADDR HookGetLocaleID
	push	eax
	call	edi
	mov	eax,[kernel32:GetUserDefaultLangID]
	push	ebx
	push	ADDR HookGetLocaleID
	push	eax
	call	edi
#ifdef DEPRECATED ; for compatible with WIN7
	mov	eax,[kernel32:GetCommandLineA]
	mov	esi,[eax + 1]
	call	[kernel32:GetCommandLineW]
	xor	ecx,ecx
	mov	esi,[esi]
#else
	call	[kernel32:GetCommandLineA]
	mov	esi,eax
	call	[kernel32:GetCommandLineW]
	xor	ecx,ecx
#endif
	push	ecx
	push	ecx
	push	MAX_PATH * 2
	push	esi
	push	-1
	push	eax
	push	ecx
	push	[dwCodePage]
	call	[kernel32:WideCharToMultiByte] 
	mov	eax,[kernel32:WideCharToMultiByte]
	push	ebx
	push	ADDR HookWCtoMB
	push	eax
	call	edi
	cmp	[nOSVer],bl
	je	>A1
	inc	esi
	inc	esi
	
A1:
	
	mov	[lpWCtoMBAddress],esi
	mov	eax,[kernel32:MultiByteToWideChar]
	push	ebx
	push	ADDR HookMBtoWC
	push	eax
	call	edi
	cmp	[nOSVer],bl
	je	>A2
	inc	esi
	
A2:

	mov	[lpMBtoWCAddress],esi
	
	mov	eax,[user32:DefWindowProcA]
	push	ebx
	push	ADDR HookDefWindowProc
	push	eax
	call	edi

	mov	eax,[user32:DefMDIChildProcA]
	push	ebx
	push	ADDR HookDefMDIChildProc
	push	eax
	call	edi
	
	mov	eax,[user32:DefDlgProcA]
	push	ebx
	push	ADDR HookDefDlgProc
	push	eax
	call	edi
	
	mov	eax,[user32:DefFrameProcA]
	push	ebx
	push	ADDR HookDefFrameProc
	push	eax
	call	edi

#ifdef NoUsed
	
	mov	eax,[user32:RegisterClassA]
	push	ebx
	push	ADDR HookRegisterClass
	push	eax
	call	edi
	
	mov	[lpRegisterClassAddress],esi
	
	mov	eax,[user32:RegisterClassExA]
	push	ebx
	push	ADDR HookRegisterClassEx
	push	eax
	call	edi
	
	mov	[lpRegisterClassExAddress],esi
	
	mov	eax,[user32:SetClassLongA]
	push	ebx
	push	ADDR HookSetClassLong
	push	eax
	call	edi
	
	mov	eax,[user32:GetClassInfoA]
	push	ebx
	push	ADDR HookGetClassInfo
	push	eax
	call	edi
	
	mov	eax,[user32:GetClassInfoExA]
	push	ebx
	push	ADDR HookGetClassInfoEx
	push	eax
	call	edi
	
	mov	eax,[user32:UnregisterClassA]
	push	ebx
	push	ADDR HookUnregisterClass
	push	eax
	call	edi
	
	mov	eax,[user32:GetClassLongA]
	push	ebx
	push	ADDR HookGetClassLong
	push	eax
	call	edi
	
#endif

	mov	eax,[user32:DialogBoxParamA]
	push	ebx
	push	ADDR HookDialogBoxParam
	push	eax
	call	edi
	
	mov	eax,[user32:DialogBoxIndirectParamA]
	push	ebx
	push	ADDR HookDialogBoxIndirectParam
	push	eax
	call	edi
	
	mov	eax,[user32:CreateDialogIndirectParamA]
	push	ebx
	push	ADDR HookCreateDialogIndirectParam
	push	eax
	call	edi
	
	mov	eax,[user32:CreateDialogParamA]
	push	ebx
	push	ADDR HookCreateDialogParam
	push	eax
	call	edi
	
	mov	eax,[user32:CreateWindowExA]
	push	ebx
	push	ADDR HookCreateWindowEx
	push	eax
	call	edi
	
	mov	eax,[user32:CallWindowProcA]
	push	ebx
	push	ADDR HookCallWindowProc
	push	eax
	call	edi
	
	mov	[lpCallWindowProcAddress],esi
	
	mov	eax,[user32:SetWindowTextA]
	push	ebx
	push	ADDR HookSetWindowText
	push	eax
	call	edi
	
	mov	eax,[user32:GetWindowTextA]
	push	ebx
	push	ADDR HookGetWindowText
	push	eax
	call	edi
	
	mov	eax,[user32:SendMessageA]
	push	ebx
	push	ADDR HookSendMessage
	push	eax
	call	edi
	
	mov	eax,[user32:SendMessageCallbackA]
	push	ebx
	push	ADDR HookSendMessageCallback
	push	eax
	call	edi
	
	mov	eax,[user32:SendMessageTimeoutA]
	push	ebx
	push	ADDR HookSendMessageTimeout
	push	eax
	call	edi
	
	mov	eax,[user32:SendNotifyMessageA]
	push	ebx
	push	ADDR HookSendNotifyMessage
	push	eax
	call	edi
	
	mov	eax,[user32:PostMessageA]
	push	ebx
	push	ADDR HookPostMessage
	push	eax
	call	edi
	
	mov	eax,[user32:SetWindowLongA]
	push	ebx
	push	ADDR HookSetWindowLong
	push	eax
	call	edi
	
	mov	eax,[user32:GetWindowLongA]
	mov	ecx,[eax + 3]
	mov	[lpUser32SEHAddress],ecx
	push	ebx
	push	ADDR HookGetWindowLong
	push	eax
	call	edi
	cmp	B[nOSVer],VER_WIN2K
	je	>A12	
	inc	esi
	inc	esi
	
A12:

	mov	[lpGetWindowLongAddress],esi
	
	mov	eax,[user32:GetMenuStringA]
	push	ebx
	push	ADDR HookGetMenuString
	push	eax
	call	edi
	
#ifdef Testing
	
	mov	eax,[user32:GetMenuItemInfoA]
	push	ebx
	push	ADDR HookGetMenuItemInfo
	push	eax
	call	edi
	
	mov	eax,[user32:SetMenuItemInfoA]
	push	ebx
	push	ADDR HookSetMenuItemInfo
	push	eax
	call	edi
	
#endif
	
	mov	eax,[kernel32:IsDBCSLeadByte]
	push	ebx
	push	ADDR HookIsDBCSLeadByte
	push	eax
	call	edi
	
	mov	eax,[user32:CharPrevA]
	push	ebx
	push	ADDR HookCharPrev
	push	eax
	call	edi
	
	mov	eax,[user32:CharNextA]
	push	ebx
	push	ADDR HookCharNext
	push	eax
	call	edi
	
	mov	eax,[gdi32:EnumFontFamiliesExA]
	push	ebx
	push	ADDR HookEnumFontFamiliesEx
	push	eax
	call	edi
	cmp	[nOSVer],bl
	je	>A13
	inc	esi
	
A13:
	
	mov	[lpEnumFontFamiliesEx],esi

	
	mov	eax,[gdi32:CreateFontIndirectA]
	push	ebx
	push	ADDR HookCreateFontIndirect
	push	eax
	call	edi
	
	mov	eax,[ntdll:RtlMultiByteToUnicodeSize]
	push	ebx
	push	ADDR HookMultiByteToUnicodeSize
	push	eax
	call	edi
	
	mov	eax,[ntdll:RtlUnicodeToMultiByteSize]
	push	ebx
	push	ADDR HookUnicodeToMultiByteSize
	push	eax
	call	edi
	
	mov	eax,[ntdll:RtlOemToUnicodeN]
	push	ebx
	push	ADDR HookMultiByteToUnicode
	push	eax
	call	edi
	
	mov	eax,[ntdll:RtlUnicodeToOemN]
	push	ebx
	push	ADDR HookUnicodeToMultiByte
	push	eax
	call	edi

EOF:
E2:

	push	ADDR szRcpEventName
	push	ebx
	push	EVENT_MODIFY_STATE
	call	[kernel32:OpenEventA]
	push	eax
	push	eax
	call	[kernel32:SetEvent]
	call	[kernel32:CloseHandle]

	retn	4
	
/*
	
ALIGN 4
HookGetClassLong:

	cmp	D[esp + 8],GCL_MENUNAME
	jne	>L777
	int 3
	
L777:
	
	jmp	[user32:GetClassLongW]
	
*/
	
ALIGN 4
HookIsDBCSLeadByte:

	push	[esp + 4]
	push	[dwCodePage]
	call	[kernel32:IsDBCSLeadByteEx]
	retn	4

ALIGN 4
HookCharPrev:

	push	0
	push	[esp + 12]
	push	[esp + 12]
	push	[dwCodePage]
	call	[user32:CharPrevExA]
	retn	8

ALIGN 4
HookCharNext:

	push	0
	push	[esp + 8]
	push	[dwCodePage]
	call	[user32:CharNextExA]
	retn	4
	
ALIGN 4
HookCreateFontIndirect    FRAME    lplf

	USES    esi,edi,ebx

	LOCAL   logfont : LOGFONTW
	
	mov	edi,esp
	mov	esi,[lplf]
	push	edi
	push	esi
	push	SIZEOF LOGFONTSIMILAR / 4
	pop	ecx
	rep	movsd
	pop	esi
	pop	edi
	mov	ecx,[dwFontSizePercent]
	mov	eax,[edi + LOGFONTW.lfHeight]
	xor	edx,edx
	mov	ebx,100
	imul	ecx
	idiv	ebx
	add	esi,SIZEOF LOGFONTSIMILAR
	mov	[edi + LOGFONTW.lfHeight],eax
	xor	edx,edx
	mov	eax,[edi + LOGFONTW.lfWidth]
	imul	ecx
	idiv	ebx
	xor	ebx,ebx
	mov	[edi + LOGFONTW.lfWidth],eax
	cmp	[bForceSpecifyFont],bl
	jne	>L102
	
L101:
	
	lea	eax,[edi + SIZEOF LOGFONTSIMILAR]
	push	LF_FACESIZE
	push	eax
	push	-1
	push	esi
	push	0
	push	eax
	call	[kernel32:MultiByteToWideChar]
	push	edi
	call	[gdi32:CreateFontIndirectW]
	test	ebx,ebx
	jnz	>L100
	test	eax,eax
	jnz	>L100
	
L102:
	
	inc	ebx
	mov	esi,[lpFontFaceName]
	jmp	<L101
	
L100:

	ret

ENDF
	
ALIGN 4
HookGetMenuString    FRAME    hMenu, uIDItem, lpString, nMaxCount, uFlag

	LOCAL    StringBuffer[MAX_PATH] : W

	mov	eax,esp
	push	[uFlag]
	push	MAX_PATH
	push	eax
	push	[uIDItem]
	push	[hMenu]
	call	[user32:GetMenuStringW]
	xor	ecx,ecx
	mov	eax,esp
	push	ecx
	push	ecx
	push	[nMaxCount]
	push	[lpString]
	push	-1
	push	eax
	push	ecx
	push	ecx
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L100
	dec	eax
	jmp	>L101
	
L100:

	mov	eax,[nMaxCount]
	mov	edx,[lpString]
	dec	eax
	mov	B[edx + eax],0

L101:
	
	ret

ENDF

ALIGN 4
HookGetMenuItemInfo    FRAME    hMenu, uItem, fByPosition, lpmii

	USES    esi,edi,ebx
	
	LOCAL   cchtmp : D
	LOCAL   miitmp : MENUITEMINFO
	
	mov	edi,esp
	mov	esi,[lpmii]
	push	edi
	mov	ecx,[esi + MENUITEMINFO.cbSize]
	xor	ebx,ebx
	shr	ecx,2
	rep	movsd
	pop	esi
	mov	eax,[esi + MENUITEMINFO.fMask]
	test	al,MIIM_STRING
	jnz	>L100
	test	al,MIIM_TYPE
	jnz	>L101
	test	eax,MIIM_FTYPE
	jz	>L102
	
L101:
	
	cmp	[esi + MENUITEMINFO.fType],ebx
	jne	>L102
	
L100:
	
	mov	eax,[esi + MENUITEMINFO.cch]
	mov	[cchtmp],eax
	lea	eax,[eax + eax + 2]
	call	AllocateZeroedMemory
	mov	ebx,eax
	mov	[esi + MENUITEMINFO.dwTypeData],eax
	
L102:

	push	esi
	push	[fByPosition]
	push	[uItem]
	push	[hMenu]
	call	[user32:GetMenuItemInfoW]
	test	eax,eax
	jz	>L105
	mov	edi,[lpmii]
	mov	ecx,[esi + MENUITEMINFO.cbSize]
	mov	edx,[edi + MENUITEMINFO.dwTypeData]
	shr	ecx,2
	push	eax
	push	edi
	rep	movsd
	test	ebx,ebx
	pop	edi
	jz	>L104
	mov	[edi + MENUITEMINFO.dwTypeData],edx
	xor	eax,eax
	push	eax
	push	eax
	push	[edi + MENUITEMINFO.cch]
	push	edx
	push	-1
	push	ebx
	push	eax
	push	eax
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L105
	dec	eax
	mov	[edi + MENUITEMINFO.cch],eax
	jmp	>L105
	
L105:

	test	ebx,ebx
	jz	>L104
	push	eax
	mov	ecx,ebx
	call	FreeStringInternal
	pop	eax
	mov	edx,[edi + MENUITEMINFO.dwTypeData]
	mov	ecx,[cchtmp]
	dec	ecx
	mov	[edx + ecx],al
	mov	[edi + MENUITEMINFO.cch],ecx
	
L104:
	
	ret

ENDF

ALIGN 4
HookSetMenuItemInfo    FRAME    hMenu, uItem, fByPosition, lpmii

	USES    esi,edi,ebx
	
	xor	ebx,ebx
	mov	esi,[lpmii]
	mov	eax,[esi + MENUITEMINFO.fMask]
	mov	edi,[esi + MENUITEMINFO.dwTypeData]
	test	al,MIIM_STRING
	jnz	>L100
	test	al,MIIM_TYPE
	jnz	>L101
	test	eax,MIIM_FTYPE
	jz	>L102
	
L101:
	
	cmp	[esi + MENUITEMINFO.fType],ebx
	jne	>L102
	
L100:
	
	cmp	edi,ebx
	jz	>L102
	push	edi
	call	MultiByteToWideCharInternal
	mov	ebx,eax
	mov	[esi + MENUITEMINFO.dwTypeData],eax

L102:

	push	esi
	push	[fByPosition]
	push	[uItem]
	push	[hMenu]
	call	[user32:SetMenuItemInfoW]
	mov	[esi + MENUITEMINFO.dwTypeData],edi
	test	ebx,ebx
	jz	>L103
	push	eax
	mov	ecx,ebx
	call	FreeStringInternal
	pop	eax
	
L103:

	ret

ENDF

ALIGN 4
HookEnumFontFamiliesEx    FRAME    hdc, lpLogfont, lpEnumFontFamExProc, lParam, dwFlags

	USES   esi,edi
	
	LOCAL   logfont : LOGFONT

	call	GetTlsValueInternal
	xor	edx,edx
	mov	edi,eax
	mov	[eax + NTLEA_TLS_DATA.IsFontAvailable],edx
	mov	esi,[lpLogfont]
	push	edx
	push	edx
	push	ADDR EnumFontFamExProc
	push	esi
	push	[hdc]
	push	ADDR EnumFontFamiliesExEnd
	
L101:
	
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>L110
	mov	ecx,[esp + 8]
	xor	edx,edx
	jmp	>L111
	
L110:
	
	push	ebp
	mov	ebp,esp
	
L111:
	
	jmp	[lpEnumFontFamiliesEx]
	
ALIGN 4
	
EnumFontFamiliesExEnd:

	cmp	D[edi + NTLEA_TLS_DATA.IsFontAvailable],0
	jne	>L100
	mov	edi,esp
	push	edi
	push	SIZEOF LOGFONT / 4
	pop	ecx
	rep	stosd
	mov	edx,[lpFontFaceName]
	cmp	[edx],cl
	pop	esi
	je	>L100
	
L102:
	
	mov	al,[edx + ecx]
	mov	[esi + LOGFONT.lfFaceName + ecx],al
	inc	ecx
	test	al,al
	jnz	<L102

L100:

	push	0
	push	[lParam]
	push	[lpEnumFontFamExProc]
	push	esi
	push	[hdc]
	push	ADDR EnumFontFamiliesExTerminal
	jmp	<L101
	
EnumFontFamiliesExTerminal:
	
	ret
	
ENDF

ALIGN 4
EnumFontFamExProc:

	call	GetTlsValueInternal
	inc	D[eax + NTLEA_TLS_DATA.IsFontAvailable]
	xor	eax,eax
	retn	16


/*
	
ALIGN 4
HookGetClassInfo    FRAME    hInstance, lpClassName, lpWndClass

	USES    esi

	mov	eax,[lpClassName]
	xor	esi,esi
	test	eax,0xFFFF0000
	jz	>L100
	push	eax
	call	MultiByteToWideCharInternal
	mov	esi,eax
	
L100:
	
	push	[lpWndClass]
	push	eax
	push	[hInstance]
	call	[user32:GetClassInfoW]
	test	esi,esi
	jz	>L101
	push	eax
	mov	ecx,esi
	call	FreeStringInternal
	pop	eax
	
L101:
	
	ret
	
ENDF
	
ALIGN 4
HookGetClassInfoEx    FRAME    hInstance, lpClassName, lpWndClassEx

	USES    esi

	mov	eax,[lpClassName]
	xor	esi,esi
	test	eax,0xFFFF0000
	jz	>L100
	push	eax
	call	MultiByteToWideCharInternal
	mov	esi,eax
	
L100:
	
	push	[lpWndClassEx]
	push	eax
	push	[hInstance]
	call	[user32:GetClassInfoExW]
	test	esi,esi
	jz	>L101
	push	eax
	mov	ecx,esi
	call	FreeStringInternal
	pop	eax
	
L101:
	
	ret
	
ENDF
	
ALIGN 4
HookUnregisterClass    FRAME    lpClassName, hInstance

	USES    esi
	
	mov	eax,[lpClassName]
	xor	esi,esi
	test	eax,0xFFFF0000
	jz	>L100
	push	eax
	call	MultiByteToWideCharInternal
	mov	esi,eax
	
L100:

	push	[hInstance]
	push	eax
	call	[user32:UnregisterClassW]
	test	esi,esi
	jz	>L101
	push	eax
	mov	ecx,esi
	call	FreeStringInternal
	pop	eax
	
L101:
	
	ret

ENDF

*/

ALIGN 4
HookGetWindowText    FRAME    hWindow, lpString, nMaxCount

	USES    esi,edi,ebx

#ifdef	DBG
	int 3
#endif

#ifdef	TRAP
	call	TrapFunc
#endif

	xor	ebx,ebx
	push	ebx
	push	ebx
	push	WM_GETTEXTLENGTH
	push	[hWindow]
	call	[user32:SendMessageW]
	inc	eax
	mov	edi,eax
	add	eax,eax
	call	AllocateZeroedMemory
	mov	esi,eax
	push	edi
	push	eax
	push	[hWindow]
	call	[user32:GetWindowTextW]
	test	eax,eax
	mov	edi,[lpString]
	jz	>L100
	push	ebx
	push	ebx
	push	[nMaxCount]
	push	edi
	push	-1
	push	esi
	push	ebx
	push	ebx
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jz	>L102
	dec	eax
	jmp	>L101
	
L102:

	call	[kernel32:GetLastError]
	cmp	eax,ERROR_INSUFFICIENT_BUFFER
	jne	>L100
	mov	ecx,[nMaxCount]
	dec	ecx
	mov	[edi + ecx],bl
	mov	eax,ecx
	jmp	>L101
	
L100:

	mov	[edi],bl
	xor	eax,eax
	
L101:
	
	push	eax
	mov	ecx,esi
	call	FreeStringInternal
	pop	eax
	ret

ENDF

/*
	
ALIGN 4

HookSetClassLong    FRAME    hWindow, nIndex, dwNewLong

#ifdef	DBG
	int 3
#endif

#ifdef	TRAP
	call	TrapFunc
#endif

	cmp	D[nIndex],GCL_WNDPROC
	je	>L100
	
L101:
	
	push	[dwNewLong]
	push	[nIndex]
	push	[hWindow]
	call	[user32:SetClassLongW]
	ret
	
L100:

	push	[hWindow]
	call	[user32:IsWindow]
	test	eax,eax
	jz	<L101
	push	GCL_WNDPROC
	push	[hWindow]
	call	[user32:GetClassLongW]
	mov	edx,[lpWindowProcTable]
	xor	ecx,ecx
	
L102:
	
	cmp	[edx + ecx * 4],eax
	je	>L103
	inc	ecx
	cmp	cl,MAX_CLASS_SUPPORTED
	jb	<L102
	jmp	<L101
	
L103:

	mov	eax,[dwNewLong]
	mov	[edx + ecx * 4],eax
	jmp	<L101
	
ENDF

*/

ALIGN 4

HookGetWindowLong:

#ifdef	DBG
	int 3
#endif

	cmp	D[nIdx],GWL_WNDPROC
	je	>G200
	;cmp	D[nIdx],DWL_DLGPROC
	;je	>G200
	push	[nIdx]
	push	[hWnd + 4]
	call	[user32:GetWindowLongW]
	jmp	>G203
	
G202:

	dec	D[eax + NTLEA_TLS_DATA.InternalCall]
	
G201:

	cmp	B[nOSVer],VER_WIN2K
	je	>G204
	push	8
	push	[lpUser32SEHAddress]
	jmp	>G205
	
G204:
	
	push	ebp
	mov	ebp,esp
	push	-1
	
G205:
	
	jmp	[lpGetWindowLongAddress]
	
G200:

	call	GetTlsValueInternal
	cmp	D[eax + NTLEA_TLS_DATA.InternalCall],0
	jne	<G202
	push	ADDR szNtleaWndAscData
	push	[hWnd + 4]
	call	[user32:GetPropA]
	test	eax,eax
	jz	<G201
	mov	eax,[eax + NTLEA_WND_ASC_DATA.PrevAnsiWindowProc]
	
G203:
	
	retn	8

ALIGN 4

HookSetWindowLong:

#ifdef	DBG
	int 3
#endif

#ifdef	TRAP
	call	TrapFunc
#endif

	cmp	D[nIdx],GWL_WNDPROC
	je	>G210
	;cmp	D[nIdx],DWL_DLGPROC
	;je	>G210
	
G211:
	
	push	[dwNewValue]
	push	[nIdx + 4]
	push	[hWnd + 8]
	call	[user32:SetWindowLongW]
	jmp	>G213
	
G210:

	push	ADDR szNtleaWndAscData
	push	[hWnd + 4]
	call	[user32:GetPropA]
	test	eax,eax
	jz	<G211
	push	[eax + NTLEA_WND_ASC_DATA.PrevAnsiWindowProc]
	mov	ecx,[dwNewValue + 4]
	mov	[eax + NTLEA_WND_ASC_DATA.PrevAnsiWindowProc],ecx
	pop	eax
	
G213:
	
	retn	12

/*

;G211:

	push	esi
	push	edi
	push	ebx
	call	GetTlsValueInternal
	mov	esi,eax
	mov	edi,[hWnd + 12]
	call	HookWindowProc
	pop	ebx
	pop	edi
	pop	esi
	jmp		<G210
	
*/
	
/*

ALIGN 4
InsertToWindowProcTable:

	mov	cl,1

L080:

	xor	eax,eax
	lock	cmpxchg	[bInternalLockRegClass],cl
	dec	eax
	jz	<L080
	mov	eax,[dwWindowProcCounter]
	cmp	eax,MAX_CLASS_SUPPORTED
	jb	>L081
	mov	esi,ADDR szClassTableFull
	jmp	>S999
	
L081:

	inc	eax
	mov	ecx,[lpWindowProcTable]
	mov	[dwWindowProcCounter],eax
		
L082:

	mov	eax,[ecx]
	add	ecx,4
	test	eax,eax
	jnz	<L082
	mov	[ecx - 4],edx
	lock	dec	B[bInternalLockRegClass]
	retn
	
*/

ShowUnhandledExceptionMessage:
S999:

	push	0
	call	[kernel32:SetUnhandledExceptionFilter]
	
S997:
	
	or	eax,-1
	cmp	[bErrorFlag],al
	jne	>S998
	push	eax
	push	eax
	call	[kernel32:TerminateProcess]
	
S998:
	
	sub	esp,512
	mov	[bErrorFlag],al
	mov	edi,esp
	push	esi
	push	ADDR szUnhandledExceptionText
	push	edi
	call	[user32:wsprintfA]
	push	MB_ICONHAND
	push	ADDR szUnhandledExceptionTitle
	push	edi
	call	[user32:GetForegroundWindow]
	push	eax
	call	[user32:MessageBoxA]
	push	eax
	call	[kernel32:ExitProcess]

ALIGN 4
HookSetWindowText		FRAME		 hWindow,	lpstrText

	USES    esi
	
#ifdef	DBG
	int 3
#endif

#ifdef	TRAP
	call	TrapFunc
#endif

	mov	esi,[lpstrText]
	test	esi,esi
	jz	>L100
	push	esi
	call	MultiByteToWideCharInternal
	mov	esi,eax
	
L100:
	
	push	esi
	push	[hWindow]
	call	[user32:SetWindowTextW]
	test	esi,esi
	jz	>L101
	push	eax
	mov	ecx,esi
	call	FreeStringInternal
	pop	eax
	
L101:
	
	ret

ENDF

/*

ALIGN 4
HookRegisterClass:

	push	ebp
	mov	edx,[esp + 8]
	mov	ebp,esp
	test	edx,edx
	jz	>L500
	mov	edx,[edx + 4]
	test	edx,edx
	jz	>L500
	call	InsertToWindowProcTable
	
L500:
	
	jmp	[lpRegisterClassAddress]

ALIGN 4
HookRegisterClassEx:

	push	ebp
	mov	edx,[esp + 8]
	mov	ebp,esp
	test	edx,edx
	jz	>L510
	mov	edx,[edx + 4]
	test	edx,edx
	jz	>L510
	call	InsertToWindowProcTable
	
L510:
	
	jmp	[lpRegisterClassExAddress]
	
*/

ALIGN 4
HookCreateWindowEx		FRAME		 dwExStyle,	lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam

	LOCAL    PrevCallType
	
	USES    ebx,esi,edi

	xor	esi,esi
	call	GetTlsValueInternal
	mov	ecx,[eax + NTLEA_TLS_DATA.CurrentCallType]
	mov	ebx,eax
	cmp	[lpWindowName],esi
	mov	edi,esi
	mov	D[eax + NTLEA_TLS_DATA.CurrentCallType],CT_CREATE_WINDOW
	mov	[PrevCallType],ecx
	je	>L030
	push	[lpWindowName]
	call	MultiByteToWideCharInternal
	mov	esi,eax
	
L030:
	
	mov	eax,[lpClassName]
	test	eax,0xFFFF0000
	jz	>L033
	push	eax
	call	MultiByteToWideCharInternal
	mov	edi,eax
	
L033:
	
	push	[lpParam]
	push	[hInstance]
	push	[hMenu]
	push	[hWndParent]
	push	[nHeight]
	push	[nWidth]
	push	[y]
	push	[x]
	push	[dwStyle]
	push	esi
	push	eax
	call	InstallCbtHook
	push	[dwExStyle]
	call	[user32:CreateWindowExW]
	push	eax
	call	UninstallCbtHook	
	test	esi,esi
	jz	>L035
	mov	ecx,esi
	call	FreeStringInternal
	
L035:
	
	test	edi,edi
	jz	>L036
	mov	ecx,edi
	call	FreeStringInternal
	
L036:

	mov	ecx,[PrevCallType]
	pop	eax
	mov	[ebx + NTLEA_TLS_DATA.CurrentCallType],ecx
	ret
	
ENDF
	
ALIGN 4
MultiByteToWideCharInternal    FRAME    lpString

	USES    edi,esi
	
	mov	ecx,[lpString]
	
L031:

	mov	al,[ecx]
	inc	ecx
	test	al,al
	jnz	<L031
	sub	ecx,[lpString]
	mov	esi,ecx
	add	ecx,ecx
	call	AllocateHeapInternal
	mov	edi,eax
	xor	eax,eax
	dec	esi
	jz	>L032
	push	esi
	push	edi
	push	esi
	push	[lpString]
	push	eax
	push	eax
	call	[kernel32:MultiByteToWideChar]
	
L032:

	and	W[edi + eax * 2],0
	mov	eax,edi
	ret

ENDF

ALIGN 4
WideCharToMultiByteInternal    FRAME    lpString

	USES    edi,esi
	
	mov	ecx,[lpString]
	xor	eax,eax
	
L031:

	mov	ax,[ecx]
	inc	ecx
	inc	ecx
	test	eax,eax
	jnz	<L031
	sub	ecx,[lpString]
	mov	esi,ecx
	call	AllocateHeapInternal
	mov	edi,eax
	xor	ecx,ecx
	xor	eax,eax
	dec	esi
	dec	esi
	je	>L032
	push	ecx
	push	ecx
	push	esi
	push	edi
	shr	esi,1
	push	esi
	push	[lpString]
	push	ecx
	push	ecx
	call	[kernel32:WideCharToMultiByte]
	
L032:

	mov	B[edi + eax],0
	mov	eax,edi
	ret

ENDF

ALIGN 4
FreeStringInternal:

	push	ecx
	push	0
	push	[hHeap]
	call	[kernel32:HeapFree]
	retn

ALIGN 4
HookDefWindowProc:

	mov	eax,[user32:DefWindowProcW]
	
Y107:
	
	push	[lParam]
	push	[wParam + 4]
	push	[Msg + 8]
	push	0
	push	eax
	push	[hWnd + 20]
	push	eax
	call	DefConversionProc
	retn	16
	
ALIGN 4
HookDefMDIChildProc:

	mov	eax,[user32:DefMDIChildProcW]
	jmp	<Y107
	
ALIGN 4
HookDefDlgProc:

	mov	eax,[user32:DefDlgProcW]
	jmp	<Y107
	
ALIGN 4
HookDefFrameProc:

	mov	eax,[user32:DefFrameProcW]
	push	[lParam + 4]
	push	[wParam + 8]
	push	[Msg + 12]
	push	1
	push	[hWndMDIClient + 16]
	push	[hWnd + 20]
	push	eax
	call	DefConversionProc
	retn	20
	
ALIGN 4

HookVerQueryValue:

	mov	cl,1

L000:

	xor	eax,eax
	lock	cmpxchg	[bInternalLockVQV],cl
	dec	eax
	jz	<L000
	mov	edx,[esp + 8]
	xor	ecx,ecx
	
L010:
	
	mov	eax,[edx + ecx * 4]
	cmp	[szTranslation + ecx * 4],eax
	jnz	>L011
	inc	ecx
	cmp	ecx,6
	jne	<L010
	mov	eax,[esp]
	mov	[lpReturnAddressVQV],eax
	mov	[esp],ADDR VerQueryValueEnd
	jmp	>L015
	
L011:

	lock	dec	B[bInternalLockVQV]

L015:

	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>L012
	push	0
	push	[esp + 0x14]
	jmp	>L014
	
L012:
	
	push	ebp
	mov	ebp,esp
	
L014:

	jmp	[lpVerQueryValueAddress]

ALIGN 4

VerQueryValueEnd:

	test	eax,eax
	jz	>L012
	mov	eax,[esp - 4]
	cmp	D[eax],4
	jb	>L012
	mov	ecx,[esp - 8]
	mov	eax,[dwLocaleID]
	mov	ecx,[ecx]
	mov	[ecx],eax
	
L012:
	
	lock	dec	B[bInternalLockVQV]
	jmp	[lpReturnAddressVQV]

ALIGN 4

HookGetTimeZoneInformation:

	mov	eax,[esp + 4]
	push	eax
	push	ADDR GetTimeZoneInformationEnd
	push	ebp
	mov	ebp,esp
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>H60
	cmp	B[nOSVer],VER_WIN2K
	je	>H61
	lea	ebp,[esp - 0x74]
	
H60:
	
	jmp	[lpGetTimeZoneInformation]
	
H61:

	sub	esp,0xAC
	jmp	<H60
	
ALIGN 4

GetTimeZoneInformationEnd:

	cmp	eax,-1
	je	>L10
	mov	ecx,[dwTimeZone]
	mov	edx,[esp + 4]
	mov	[edx],ecx
	
L10:
	
	retn	4
	
AcquireCreateProcLock:

	mov	cl,1

L001:

	xor	eax,eax
	lock	cmpxchg	[bInternalLockCreateProc],cl
	dec	eax
	jz	<L001
	retn
	
GetApplicationPathA:

	cmp	B[eax],'"'
	jne	>P541
	inc	eax
	
P541:

	mov	edx,eax
	
P54:
	
	cmp	B[eax],0
	je	>P55
	cmp	B[eax],' '
	je	>P55
	cmp	B[eax],'	'
	je	>P55
	cmp	B[eax],'"'
	je	>P55
	inc	eax
	jmp	<P54
	
P55:

	mov	cl,[eax]
	push	eax
	mov	B[eax],0
	push	ecx
	push	edx
	call	CreateProcessBegin
	pop	ecx
	pop	edx
	mov	[edx],cl
	retn
	
GetApplicationPathW:

	cmp	W[eax],'"'
	jne	>P41
	inc	eax
	inc	eax
	
P41:

	mov	edx,eax
	
P4:

	movzx	ecx,W[eax]	
	test	ecx,ecx
	je	>P5
	cmp	ecx,' '
	je	>P5
	cmp	ecx,'	'
	je	>P5
	cmp	ecx,'"'
	je	>P5
	inc	eax
	inc	eax
	jmp	<P4
	
P5:

	mov	cx,[eax]
	push	eax
	and	W[eax],0
	push	ecx
	push	edx
	call	CreateProcessBegin
	pop	ecx
	pop	edx
	mov	[edx],cx
	retn
	
ALIGN 4

HookCreateProcessW:

	call	AcquireCreateProcLock
	inc	B[bCreateProcUniFunc]
	jmp	>L020
	
ALIGN 4

HookCreateProcessA:

	call	AcquireCreateProcLock
	
L020:
	
	mov	eax,[esp]
	mov	ecx,[esp + 40]
	mov	[lpReturnAddress],eax
	mov	[lpProcInfoAddress],ecx
	push	ebp
	mov	ebp,esp
	test	D[ebp	+	28],CREATE_SUSPENDED
	jnz	>M0
	or	D[ebp	+	28],CREATE_SUSPENDED
	jmp	>M1
	
M0:

	inc	B[bSuspendThreadFlag]

M1:
	
	cmp	D[ebp	+	8],0
	je	>P2
	push	[ebp + 8]
	call	CreateProcessBegin
	jmp	>P3
	
P2:

	mov	eax,[ebp + 12]
	test	eax,eax
	jz	>PN6
	cmp	B[bCreateProcUniFunc],0
	jne	>L021
	call	GetApplicationPathA
	jmp	>P3
	
L021:

	call	GetApplicationPathW
	
P3:

	test	eax,eax
	jnz	>P6
	mov	[ebp + 4],ADDR CreateProcessEnd
	
P1:
	
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>Q0
	cmp	B[nOSVer],VER_WIN2K
	je	>Q1
	push	0
	
Q0:

	cmp	B[bCreateProcUniFunc],0
	jne	>Q2
	jmp	[lpCreateProcAnsiAddress]
	
Q2:

	jmp	[lpCreateProcUniAddress]
	
Q1:

	push	[ebp + 0x2C]
	jmp	<Q0

PN6:
P6:

	mov	B[bCreateProcUniFunc],0
	lock	dec	B[bInternalLockCreateProc]
	jmp	<P1

ALIGN 4

CreateProcessBegin    FRAME    lpFileName

	USES    esi,edi

	LOCAL   nNumberOfBytesRead
	LOCAL   FileBuffer[1024] : B

	push	ADDR szAppHelp + 2
	call	[kernel32:GetModuleHandleW]
	test	eax,eax
	jnz	>F101
	lea	esi,[FileBuffer]
	push	512
	push	esi
	call	[kernel32:GetSystemDirectoryW]
	push	ADDR szAppHelp
	push	esi
	call	[kernel32:lstrcatW]
	push	esi
	call	[kernel32:LoadLibraryW]
	test	eax,eax
	jz	>F101
	push	ADDR szAppCheck
	push	eax
	call	[kernel32:GetProcAddress]
	test	eax,eax
	jz	>F101
	lea	ecx,[nNumberOfBytesRead]
	mov	esi,eax
	push	ecx
	push	PAGE_EXECUTE_READWRITE
	push	6
	push	eax
	call	[kernel32:VirtualProtect]
	test	eax,eax
	jz	>F101
	mov	D[esi],0xC240C033
	mov	W[esi + 4],0x20 ;xor eax,eax | inc eax | retn 20
	push	6
	push	esi
	push	-1
	call	[kernel32:FlushInstructionCache]
	
F101:
	
	xor	eax,eax
	movzx	edi,B[bCreateProcUniFunc]
	push	eax
	push	eax
	push	OPEN_EXISTING
	push	eax
	push	FILE_SHARE_READ
	push	GENERIC_READ
	push	[lpFileName]
	test	edi,edi
	jne	>X2
	call	[kernel32:CreateFileA]
	jmp	>X3
	
X2:
	
	call	[kernel32:CreateFileW]
	
X3:
	
	cmp	eax,INVALID_HANDLE_VALUE
	jne	>XN1
	mov	ecx,esp
	push	MAX_PATH
	push	ecx
	test	edi,edi
	jne	>X4
	call	[kernel32:GetSystemDirectoryA]
	jmp	>X5
	
X4:
	
	call	[kernel32:GetSystemDirectoryW]
	
X5:

	test	edi,edi
	mov	ecx,esp
	jne	>X6
	push	ecx
	call	[kernel32:lstrlenA]
	mov	ecx,esp
	mov	W[ecx + eax],'\'
	push	[lpFileName]
	push	ecx
	call	[kernel32:lstrcatA]
	jmp	>X7
	
X6:

	push	ecx
	call	[kernel32:lstrlenW]
	mov	ecx,esp
	mov	D[ecx + eax],'\'
	push	[lpFileName]
	push	ecx
	call	[kernel32:lstrcatW]

X7:

	xor	eax,eax
	mov	ecx,esp
	push	eax
	push	eax
	push	OPEN_EXISTING
	push	eax
	push	FILE_SHARE_READ
	push	GENERIC_READ
	push	ecx
	test	edi,edi
	jne	>X8
	call	[kernel32:CreateFileA]
	jmp	>X9
	
X8:
	
	call	[kernel32:CreateFileW]
	
X9:

	cmp	eax,INVALID_HANDLE_VALUE
	jne	>X1
	
X0:
	
	dec	eax
	ret

XN1:
X1:

	mov	esi,eax
	lea	edx,[nNumberOfBytesRead]
	mov	ecx,esp	; lea	ecx,[FileBuffer]
	push	0
	push	edx
	push	1024
	push	ecx
	push	esi
	call	[kernel32:ReadFile]
	push	esi
	call	[kernel32:CloseHandle]
	mov	eax,[FileBuffer	+	0x3C]
	mov	ecx,[FileBuffer	+	eax	+	0x28]
	add	ecx,[FileBuffer	+	eax	+	0x34]
	xor	esi,esi
	mov	[lpEntryPoint],ecx
	push	ADDR szRcpEvent
	push	esi
	push	esi
	push	esi
	call	[kernel32:CreateEventA]
	test	eax,eax
	mov	[hEvent],eax
	jz	<X0
	push	ADDR szRcpFileMap
	push	4
	push	esi
	push	PAGE_READWRITE
	push	esi
	push	-1
	call	[kernel32:CreateFileMappingA]
	mov	[hFileMapping],eax
	push	4
	push	esi
	push	esi
	push	FILE_MAP_ALL_ACCESS
	push	eax
	call	[kernel32:MapViewOfFile]
	mov	[lpFileMappingAddress],eax
	xor	eax,eax
	ret
	
ENDF


ALIGN 4

CreateProcessEnd:

	test	eax,eax
	jz	>RO0
	push	esi
	push	edi
	push	ebx
	push	ecx
	mov	esi,[lpProcInfoAddress]
	mov	W[esp	+	2],0xFEEB
	mov	ebx,[lpEntryPoint]
	mov	edi,[esi + PROCESS_INFORMATION.hProcess]
	mov	eax,esp
	mov	[hEvent + 4],edi
	push	0
	push	2
	push	eax
	push	ebx
	push	edi
	call	[kernel32:ReadProcessMemory]
	lea	eax,[esp + 2]
	push	0
	push	2
	push	eax
	push	ebx
	push	edi
	call	[kernel32:WriteProcessMemory]
	push	2
	push	ebx
	push	edi
	call	[kernel32:FlushInstructionCache]
	push	[esi + PROCESS_INFORMATION.hThread]
	call	[kernel32:ResumeThread]
	
R3:

	push	100
	call	[kernel32:Sleep]
	push	[esi + PROCESS_INFORMATION.hThread]
	call	[kernel32:SuspendThread]
	push	ADDR context
	push	[esi + PROCESS_INFORMATION.hThread]
	call	[kernel32:GetThreadContext]
	push	[esi + PROCESS_INFORMATION.hThread]
	call	[kernel32:ResumeThread]
	cmp	[context + regEip],ebx
	jne	<R3
	xor	eax,eax
	mov	esi,[lpParameterAddress]
	xor	ecx,ecx
	
R1:
	
	mov	ax,[esi	+	ecx]
	inc	ecx
	inc	ecx
	test	eax,eax
	jnz	<R1
	
R2:
	
	mov	al,[esi + ecx]
	inc	ecx
	test	al,al
	jnz	<R2
	add	ecx,20
	push	ecx
	push	PAGE_EXECUTE_READWRITE
	push	MEM_COMMIT
	push	ecx
	push	0
	push	edi
	call	[kernel32:VirtualAllocEx]
	mov	edx,[lpFileMappingAddress]
	pop	ecx
	mov	[edx],eax
	push	eax
	push	0
	push	ecx
	push	esi
	push	eax
	push	edi
	call	[kernel32:WriteProcessMemory]
	mov	eax,[esp]
	xor	edx,edx
	mov	ecx,[kernel32:LoadLibraryW]
	push	esp
	push	edx
	push	eax
	push	ecx
	push	edx
	push	edx
	push	edi
	call	[kernel32:CreateRemoteThread]
	pop	eax
	push	100000
	push	0
	push	ADDR hEvent
	push	2
	call	[kernel32:WaitForMultipleObjects]
	cmp	B[bSuspendThreadFlag],0
	je	>M2
	mov	ecx,[lpProcInfoAddress]
	push	eax
	push	[ecx + PROCESS_INFORMATION.hThread]
	call	[kernel32:SuspendThread]
	pop	eax
	dec	B[bSuspendThreadFlag]
	
M2:
	
	test	eax,eax
	jnz	>R02
	mov	ecx,esp
	push	0
	push	2
	push	ecx
	push	ebx
	push	edi
	call	[kernel32:WriteProcessMemory]
	
R02:
	
	pop	ecx
	pop	ebx
	pop	edi
	pop	esi
	
RO0:
	
	push	[hEvent]
	call	[kernel32:CloseHandle]
	push	[lpFileMappingAddress]
	call	[kernel32:UnmapViewOfFile]
	push	[hFileMapping]
	call	[kernel32:CloseHandle]
	mov	B[bCreateProcUniFunc],0
	lock	dec	B[bInternalLockCreateProc]
	jmp	[lpReturnAddress]
	
ALIGN 4

HookCompareString:

	push	ebp
	mov	ebp,esp
	and	D[ebp + 12],NORM_IGNORECASE	
	or	D[ebp	+	12],NORM_IGNOREKANATYPE	|	NORM_IGNOREWIDTH
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>H5
	cmp	B[nOSVer],VER_WINXP_SP1
	je	>H51
	cmp	B[nOSVer],VER_WINXP
	je	>H52
	sub	esp,0x21C
	
H5:

	jmp	[lpCompareStringAddress]
	
H51:

	sub	esp,0x208
	jmp	<H5
	
H52:

	sub	esp,0x20C
	jmp	<H5
	

HookCreateFile:

	push	ebp
	call	GetTlsValueInternal
	inc	D[eax + NTLEA_TLS_DATA.IsCreateFileCall]
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	mov	ebp,esp
	je	>H40
	push	[ebp + 8]

H40:
	
	jmp	[lpCreateFileAddress]
	
ALIGN 4

HookMBtoWC:

	push	ebp
	call	GetTlsValueInternal
	xor	ecx,ecx
	cmp	[eax + NTLEA_TLS_DATA.IsCreateFileCall],ecx
	mov	[eax + NTLEA_TLS_DATA.IsCreateFileCall],ecx
	jne	>H111
	mov	ecx,[dwCodePage]
	
H111:
	
	mov	ebp,esp
	mov	[ebp + 8],ecx
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>H1
	cmp	B[nOSVer],VER_WIN2K
	mov	eax,0x14
	je	>H11
	sub	eax,4
	
H11:
	
	sub	esp,eax

H1:
	
	jmp	[lpMBtoWCAddress]
	
ALIGN 4

HookGetLocaleID:

	mov	eax,[dwLocaleID]
	retn
	
ALIGN 4

HookGetCPInfo:

	cmp	B[nOSVer],VER_WINXP_SP1
	mov	eax,[dwCodePage]
	je	>H31
	push	ebp
	mov	ebp,esp
	mov	[ebp + 8],eax
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>H3
	push	ebx
	push	esi
	
H3:

	jmp	[lpCPInfoAddress]
	
H31:

	push	edi
	mov	[esp + 8],eax
	mov	edi,eax
	jmp	<H3
	
ALIGN 4

HookWCtoMB:

	push	ebp
	mov	eax,[dwCodePage]
	mov	ebp,esp
	mov	[ebp + 8],eax
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>H2
	cmp	B[nOSVer],VER_WIN2K
	je	>H21	
	push	edi
	mov	edi,eax
	jmp	>H2
	
H21:

	push	ebx
	mov	ebx,eax
	
H2:
	
	jmp	[lpWCtoMBAddress]
	
ALIGN 4

HookGetACP:

	mov	eax,[dwCodePage]
	retn
	
ALIGN 4

HookGdiGetCodePage:

	mov	eax,[dwCodePage]
	retn	4
	
ALIGN 4

HookUnicodeToMultiByte    FRAME    AnsiBuffer, MultiByteLength,	lpNumberOfBytesConverted,	UnicodeBuffer, WideCharLength

	mov	cl,1

G7:

	xor	eax,eax
	lock	cmpxchg	[bInternalLockWCtoMB],cl
	dec	eax
	jz	>G9
	
G8:

	cmp	B[bInternalCallWCtoMB],0
	je	>G0
	
G20:
	
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>S0
	cmp	B[nOSVer],VER_WIN2K
	je	>S01
	push	ebx
	push	esi
	jmp	>S0
	
G9:

	call	[kernel32:GetCurrentThreadId]
	cmp	[dwThreadIdWCtoMB],eax
	je	<G8
	jmp	<G7
	
S01:

	cmp	B[MbCodePageTag],al
	
S0:

	jmp	[lpUnitoMBAddress]
	
G0:

	call	[kernel32:GetCurrentThreadId]
	mov	[dwThreadIdWCtoMB],eax
	mov	ecx,[MultiByteLength]
	xor	eax,eax
	mov	edx,[AnsiBuffer]
	push	eax
	push	eax
	push	ecx
	push	edx
	mov	ecx,[WideCharLength]
	mov	edx,[UnicodeBuffer]
	shr	ecx,1
	inc	B[bInternalCallWCtoMB]
	push	ecx
	push	edx
	push	eax
	push	eax
	call	[kernel32:WideCharToMultiByte]
	test	eax,eax
	jnz	>G33
	push	eax
	call	[kernel32:SetLastError]
	xor	eax,eax
	
G33:
	
	mov	ecx,[lpNumberOfBytesConverted]
	dec	B[bInternalCallWCtoMB]
	lock	dec	B[bInternalLockWCtoMB]
	test	ecx,ecx
	jz	>G1
	mov	[ecx],eax

G1:

	xor	eax,eax
	ret
	
ENDF

ALIGN 4

HookUnicodeToMultiByteSize:

	mov	eax,[esp + 12]
	xor	edx,edx
	mov	ecx,[esp + 8]
	shr	eax,1
	push	edx
	push	edx
	push	edx
	push	edx
	push	eax
	push	ecx
	push	edx
	push	edx
	call	[kernel32:WideCharToMultiByte]
	
H121:
	
	mov	ecx,[esp + 4]
	test	ecx,ecx
	jz	>H120
	mov	[ecx],eax
	
H120:

	xor	eax,eax
	retn	12
	
ALIGN 4

HookMultiByteToUnicodeSize:

	xor	edx,edx
	mov	eax,[esp + 12]
	mov	ecx,[esp + 8]
	push	edx
	push	edx
	push	eax
	push	ecx
	push	edx
	push	edx
	call	[kernel32:MultiByteToWideChar]
	add	eax,eax
	jmp	<H121

HookMultiByteToUnicode    FRAME    UnicodeBuffer,	WideCharLength,	lpNumberOfBytesConverted,	AnsiBuffer,	MultiByteLength

	mov	cl,1

L7:

	xor	eax,eax
	lock	cmpxchg	[bInternalLock],cl
	dec	eax
	jz	>L9
	
L8:
	
	cmp	B[bInternalCall],0
	je	>L002
	cmp	B[nOSVer],VER_WINXP_SP2_OR_ABOVE
	je	>H0
	cmp	B[nOSVer],VER_WIN2K
	je	>H01
	push	ebx
	push	esi
	jmp	>H0
	
L9:

	call	[kernel32:GetCurrentThreadId]
	cmp	[dwThreadId],eax
	je	<L8
	jmp	<L7
	
H01:


	cmp	B[MbCodePageTag],al
	
H0:

	jmp	[lpMBtoUniAddress]
	
L002:

	call	[kernel32:GetCurrentThreadId]
	mov	[dwThreadId],eax
	mov	ecx,[WideCharLength]
	mov	edx,[UnicodeBuffer]
	push	ecx
	push	edx
	mov	ecx,[MultiByteLength]
	mov	edx,[AnsiBuffer]
	push	ecx
	push	edx
	push	0
	push	0
	inc	B[bInternalCall]
	call	[kernel32:MultiByteToWideChar]
	mov	ecx,[lpNumberOfBytesConverted]
	add	eax,eax
	dec	B[bInternalCall]
	lock	dec	B[bInternalLock]
	test	ecx,ecx
	jz	>L1
	mov	[ecx],eax

L1:
	
	xor	eax,eax
	ret
	
ENDF

ALIGN 4

InstallCbtHook:

	call	[kernel32:GetCurrentThreadId]
	push	eax
	push	0
	push	ADDR CbtHookProcW
	push	WH_CBT
	push	eax
	push	0
	push	ADDR CbtHookProcA
	push	WH_CBT
	call	[user32:SetWindowsHookExA]
	mov	[ebx + NTLEA_TLS_DATA.hWindowCbtHookAnsi],eax
	call	[user32:SetWindowsHookExW]
	mov	[ebx + NTLEA_TLS_DATA.hWindowCbtHookUnicode],eax
	retn

ALIGN 4

UninstallCbtHook:

	push	[ebx + NTLEA_TLS_DATA.hWindowCbtHookAnsi]
	call	[user32:UnhookWindowsHookEx]
	push	[ebx + NTLEA_TLS_DATA.hWindowCbtHookUnicode]
	call	[user32:UnhookWindowsHookEx]
	retn

HookDllFunc    FRAME    lpszFuncName, lpHookAddress, hDLL

	mov	ecx,[hDLL]
	mov	eax,[lpszFuncName]
	test	ecx,ecx
	jz	>L003
	push	eax
	push	ecx
	call	[kernel32:GetProcAddress]
	
L003:
	
	mov	esi,eax
	push	ecx
	push	esp
	push	PAGE_EXECUTE_READWRITE
	push	5
	push	eax
	call	[kernel32:VirtualProtect]
	mov	eax,[lpHookAddress]
	mov	B[esi],0xE9
	sub	eax,esi
	push	5
	pop	[esp]
	sub	eax,5
	push	esi
	push	-1
	mov	[esi + 1],eax
	call	[kernel32:FlushInstructionCache]
	add	esi,5
	ret
	
ENDF


#include <Windows.h>

#include "ntleai.h"		// require that struct for jump address 

extern Addresses addresses;
#pragma warning(disable: 4100 4731 4740)

// --------------- hook head + tail ----------------- //
BOOL /*__declspec(naked)*/ __stdcall CreateProcessInternalAorW(DWORD lpAcGenralType, LPCVOID lpApplicationName,
	LPCVOID lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPCVOID lpEnvironment/*ebp+0x20*/, LPCVOID lpCurrentDirectory,
	LPCVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation/**/)
{///PASS 
	// NOTE: a stack usage (int ret) prevent using nakedcall here !! 
	//		 but, the compiler still use call not jump here even no stack usage at all, why M$C??
	__asm push	[lpProcessInformation]
	__asm push	[lpStartupInfo]
	__asm push	[lpCurrentDirectory]
	__asm push	[lpEnvironment]
	__asm push	[dwCreationFlags]
	__asm push	[bInheritHandles]
	__asm push	[lpThreadAttributes]
	__asm push	[lpProcessAttributes]
	__asm push	[lpCommandLine]
	__asm push	[lpApplicationName]
	__asm push	LCreateProcessEnd	// return place !! 

///	switch (addresses.nOSVer) { // 
///	case VER_WIN2K:
///		__asm push	[lpProcessInformation] // [ebp + 0x2C]
///		break;
///	case VER_WINXP_SP2_OR_ABOVE:
///		break;
///	default: // WINXP 
///		__asm push	0 // [ebp + 0x2C]
///		break;
///	}
	if (lpAcGenralType) {
		__asm push	10h
		__asm mov	eax, [lpAcGenralType]
	} else {
		__asm push	ebp
		__asm mov	ebp, esp
	}

	__asm jmp	[addresses.lpCreateProcAddress]
	// ---- 
LCreateProcessEnd:
//	__asm add	esp, 40
//	__asm ret	40
	__asm nop
}

BOOL /*__declspec(naked)*/ __stdcall CreateProcessInternalA(LPCVOID lpApplicationName, LPCVOID lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCVOID lpCurrentDirectory,
	LPCVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	addresses.lpCreateProcAddress = addresses.lpCreateProcAnsiAddress;
	return CreateProcessInternalAorW(addresses.lpAcGenralAnsiType, 
		lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}
BOOL /*__declspec(naked)*/ __stdcall CreateProcessInternalW(LPCVOID lpApplicationName, LPCVOID lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCVOID lpCurrentDirectory,
	LPCVOID lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	addresses.lpCreateProcAddress = addresses.lpCreateProcUniAddress;
	return CreateProcessInternalAorW(addresses.lpAcGenralUniType,
		lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

int /*__declspec(naked)*/ __stdcall EnumFontFamiliesExInternalA(HDC hdc, LPLOGFONTA lpLogfont,
	FONTENUMPROCA lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags)
{
	__asm push	[dwFlags]
	__asm push	[lParam]
	__asm push	[lpEnumFontFamExProc]
	__asm push	[lpLogfont] // esi 
	__asm push	[hdc]
	__asm push	EnumFontFamiliesExEnd
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
	__asm push	ebp
	__asm mov	ebp, esp
///	}
///	else { // FASTCALL ?? 
///	__asm mov	ecx, [esp + 8]
///	__asm xor	edx, edx
///	}
	__asm jmp	[addresses.lpEnumFontFamiliesExA]
	// ---- 
EnumFontFamiliesExEnd:
//	__asm add	esp, 20
//	__asm ret	20
	__asm nop
}
int /*__declspec(naked)*/ __stdcall EnumFontFamiliesExInternalW(HDC hdc, LPLOGFONTW lpLogfont,
	FONTENUMPROCW lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags)
{
	__asm push	[dwFlags]
	__asm push	[lParam]
	__asm push	[lpEnumFontFamExProc]
	__asm push	[lpLogfont]
	__asm push	[hdc]
	__asm push	EnumFontFamiliesExEnd
	/// --- 
	__asm push	ebp
	__asm mov	ebp, esp
	__asm jmp	[addresses.lpEnumFontFamiliesExW]
	/// --- 
EnumFontFamiliesExEnd:
//	__asm ret	20
	__asm nop
}

int /*__declspec(naked)*/ __stdcall EnumFontsInternalA(HDC hdc, LPCSTR lpszFamily, FONTENUMPROCA lpEnumFontFamProc, LPARAM lParam)
{
	__asm push	[lParam]
	__asm push	[lpEnumFontFamProc]
	__asm push	[lpszFamily]
	__asm push	[hdc]
	__asm push	EnumFontFontsEnd
	/// --- 
	__asm push	ebp
	__asm mov	ebp, esp
	__asm jmp	[addresses.lpEnumFontsA]
	/// --- 
EnumFontFontsEnd:
//	__asm ret	16
	__asm nop
}

int /*__declspec(naked)*/ __stdcall EnumFontsInternalW(HDC hdc, LPCWSTR lpszFamily, FONTENUMPROCW lpEnumFontFamProc, LPARAM lParam)
{
	__asm push	[lParam]
	__asm push	[lpEnumFontFamProc]
	__asm push	[lpszFamily]
	__asm push	[hdc]
	__asm push	EnumFontFontsEnd
	/// --- 
	__asm push	ebp
	__asm mov	ebp, esp
	__asm jmp	[addresses.lpEnumFontsW]
	/// --- 
EnumFontFontsEnd:
//	__asm ret	16
	__asm nop
}

BOOL __declspec(naked) __stdcall VerQueryValueInternal(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{///PASS
	__asm push	[puLen]
	__asm push	[lplpBuffer]
	__asm push	[lpSubBlock]
	__asm push	[pBlock]
	__asm push	VerQueryValueEnd
	// ------------------- 
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
		__asm push	ebp
		__asm mov	ebp, esp
///	}
///	else { // other
///		__asm push	ebp
///		__asm mov	ebp, esp
///		__asm push	0
///		__asm push	[esp + 0x14]
///	}
	__asm jmp	[addresses.lpVerQueryValueAddress]
	// ---- 
VerQueryValueEnd:
//	__asm add	esp, 16
	__asm ret	16
}

DWORD __declspec(naked) __stdcall GetTimeZoneInformationInternal(LPTIME_ZONE_INFORMATION lpTimeZoneInformation)
{///PASS
	__asm push	[lpTimeZoneInformation]
	__asm push	GetTimeZoneInformationEnd
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
		__asm push	ebp
		__asm mov	ebp, esp
///	}
///	else if (addresses.nOSVer == VER_WIN2K) {
///		__asm push	ebp
///		__asm mov	ebp, esp
///		__asm sub	esp, 0xAC
///	}
///	else { // WINXP
///		__asm push	ebp
///		__asm mov	ebp, esp
///		__asm lea	ebp, [esp - 0x74]
///	}
	__asm jmp	[addresses.lpGetTimeZoneInformation]
	// ---- 
GetTimeZoneInformationEnd:
//	__asm add	esp, 4
	__asm ret	4
}
/*
HWND __declspec(naked) __stdcall CreateWindowExInternal(
	DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{///PASS
	__asm push	[lpParam]
	__asm push	[hInstance]
	__asm push	[hMenu]
	__asm push	[hWndParent]
	__asm push	[nHeight]
	__asm push	[nWidth]
	__asm push	[y]
	__asm push	[x]
	__asm push	[dwStyle]
	__asm push	[lpWindowName]
	__asm push	[lpClassName]
	__asm push	[dwExStyle]
	__asm push	CreateWindowExInternalEnd
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
		__asm push	ebp
		__asm mov	ebp, esp
///}
	__asm jmp	[addresses.lpCreateWindowExWddress]
	// ---- 
CreateWindowExInternalEnd:
//	__asm add	esp, 48
	__asm ret	48
} // the return address inside the hooked function !
*/
// --------------- Jump ---------- 
LONG_PTR __declspec(naked) __stdcall SetWindowLongPtrJ(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{///PASS
	__asm push	ebp
	__asm mov	ebp, esp
	__asm jmp	[addresses.lpSetWindowLongAddress]
} // the return address inside the hooked function !

LRESULT __declspec(naked) __stdcall CallWindowProcJ(WNDPROC PrevWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{///PASS
	//	case (i >= 14) 
	// C202
	__asm push	ebp
	__asm mov	ebp, esp
///	if (addresses.nOSVer != VER_WINXP_SP2_OR_ABOVE) {
///		__asm push	1
///	}
	__asm jmp	[addresses.lpCallWindowProcAddress]
} // the return address inside the hooked function !

int __declspec(naked) __stdcall UnicodeToMultiByteJ(LPSTR AnsiBuffer, DWORD MultiByteLength,
	LPDWORD lpNumberOfBytesConverted, LPCWSTR UnicodeBuffer, DWORD WideCharLength)
{///PASS
	// see: http://msdn.microsoft.com/en-us/library/windows/hardware/ff553261(v=vs.85).aspx
/*	typedef NTSTATUS (*fnRtlUnicodeToMultiByteN)(
		_Out_      PCHAR MultiByteString,
		_In_       ULONG MaxBytesInMultiByteString,
		_Out_opt_  PULONG BytesInMultiByteString,
		_In_       PCWCH UnicodeString,
		_In_       ULONG BytesInUnicodeString
	);
	*/
	__asm push	ebp
	__asm mov	ebp, esp
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) { // check ntdll.dll, no need recover !!
///		__asm nop
///	}
///	else if (addresses.nOSVer == VER_WIN2K && addresses.MbCodePageTag) { // WIN2K: 
///		__asm cmp byte ptr[addresses.MbCodePageTag], 0
///	}
///	else { // XP or XPSP1: push ebx, push esi
///		__asm push	ebx
///		__asm push	esi
///	}
	__asm jmp	[addresses.lpUnitoMBAddress]
	// no return !!
}

int __declspec(naked) __stdcall MultiByteToUnicodeJ(LPWSTR UnicodeBuffer, DWORD WideCharLength,
	LPDWORD lpNumberOfBytesConverted, LPCSTR AnsiBuffer, DWORD MultiByteLength)
{///PASS
	// see : http://msdn.microsoft.com/en-us/library/windows/hardware/ff553113(v=vs.85).aspx
/*	typedef NTSTATUS (*fnRtlMultiByteToUnicodeN)(
	_Out_      PWCH UnicodeString,
	_In_       ULONG MaxBytesInUnicodeString,
	_Out_opt_  PULONG BytesInUnicodeString,
	_In_       const CHAR *MultiByteString,
	_In_       ULONG BytesInMultiByteString
	);
	*/
	__asm push	ebp
	__asm mov	ebp, esp
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
///		__asm nop
///	} 
///	else if (addresses.nOSVer == VER_WIN2K && addresses.MbCodePageTag) { // WIN2K: 
///		__asm cmp byte ptr[addresses.MbCodePageTag], 0
///	}
///	else { // XP or XPSP1 : 
///		__asm push	ebx
///		__asm push	esi
///	}
	__asm jmp	[addresses.lpMBtoUniAddress]
	// no return !!
}

LONG __declspec(naked) __stdcall GetWindowLongPtrJ(HWND hWnd, int nIndex)
{
	__asm jmp	[addresses.lpGetWindowLongPtrSelect]
}
LONG __declspec(naked) __stdcall GetWindowLongPtrJ0(HWND hWnd, int nIndex)
{///PASS
///	if (addresses.nOSVer == VER_WIN2K) {
		// G204:
///		__asm push	ebp
///		__asm mov	ebp, esp
///		__asm push	-1
///	}
///	else { // WINXP ABOVE
		__asm push	8
		__asm push	[addresses.lpUser32SEHAddress]
///	}
	__asm jmp	[addresses.lpGetWindowLongPtrAddress]
}
LONG __declspec(naked) __stdcall GetWindowLongPtrJ8(HWND hWnd, int nIndex)
{///PASS
	__asm push	ebp
	__asm mov	ebp, esp
	__asm push	-2
	__asm jmp	[addresses.lpGetWindowLongPtrAddress]
}

DWORD __declspec(naked) __stdcall VerQueryValueJ(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{///PASS
	__asm push	ebp
	__asm mov	ebp, esp
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
///		__asm nop
///	}
///	else {
///		__asm push	0
///		__asm push	[esp + 0x14]
///	}
	__asm jmp	[addresses.lpVerQueryValueAddress]
	// no return actually : 
}

int __declspec(naked) __stdcall CompareStringJ(LCID Locale, DWORD dwCmpFlags, LPCSTR lpString1, int cchCount1, LPCSTR lpString2, int cchCount2)
{
	__asm push	ebp
	__asm mov	ebp, esp	
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
///		__asm nop
///	}
///	else if (addresses.nOSVer == VER_WINXP_SP1) {
///		__asm sub	esp, 0x208
///	}
///	else if (addresses.nOSVer == VER_WINXP) {
///		__asm sub	esp, 0x20C
///	}
///	else { // WIN2K
///		__asm sub	esp, 0x21C
///	}
	__asm jmp	[addresses.lpCompareStringAddress]
	// no return actually : 
}

BOOL __declspec(naked) __stdcall GetCPInfoJ(UINT CodePage, LPCPINFO lpCPInfo)
{
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
	//	__asm mov	eax, [addresses.dwCodePage]
		__asm push	ebp
	//	__asm mov	[ebp + 8], eax
		__asm mov	ebp, esp
///	}
///	else if (addresses.nOSVer == VER_WINXP_SP1) {
///		__asm mov	eax, [addresses.dwCodePage]
///		__asm push	edi
///		__asm mov	[esp + 8], eax
///		__asm mov	edi, eax
///	}
///	else { // WIN2K 
///		__asm mov	eax, [addresses.dwCodePage]
///		__asm push	ebp
///		__asm mov	[ebp + 8], eax
///		__asm mov	ebp, esp
///		__asm push	ebx
///		__asm push	esi
///	}
	__asm jmp	[addresses.lpCPInfoAddress]
	// no return actually : 
}

HANDLE __declspec(naked) __stdcall CreateFileJ(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{///PASS
	__asm push	ebp
	__asm mov	ebp, esp
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
///		__asm nop
///	}
///	else {
///		__asm push	[ebp + 8]
///	}
	__asm jmp	[addresses.lpCreateFileAddress]
	// no return actually :
}

HINSTANCE __declspec(naked) __stdcall ShellExecuteJ(HWND hwnd,
	LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	__asm push	ebp
	__asm mov	ebp, esp
	__asm jmp	[addresses.lpShellExecuteAddress]
}

int __declspec(naked) __stdcall MBtoWCJ(UINT CodePage, DWORD dwFlags,
	LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{///PASS
	__asm push	ebp
	__asm mov	ebp, esp
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
///		__asm nop
///	}
///	else if (addresses.nOSVer == VER_WIN2K) {
///		__asm sub	esp, 0x14
///	}
///	else {
///		__asm sub	esp, 0x10
///	}
	__asm jmp	[addresses.lpMBtoWCAddress]
	// no return actually :
}

int __declspec(naked) __stdcall WCtoMBJ(UINT CodePage, DWORD dwFlags,
	LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{///PASS
	__asm push	ebp
	__asm mov	ebp, esp
///	if (addresses.nOSVer == VER_WINXP_SP2_OR_ABOVE) {
///		__asm nop
///	}
///	else if (addresses.nOSVer == VER_WIN2K) {
///		__asm push	ebx
///		__asm mov	ebx, eax
///	}
///	else { // WINXPSP1
///		__asm push	edi
///		__asm mov	edi, eax
///	}
	__asm jmp	[addresses.lpWCtoMBAddress]
	// no return actually :
}

HGDIOBJ __declspec(naked) __stdcall GetStockObjectJ(int fnObject)
{
	__asm push	ebp
	__asm mov	ebp, esp
	__asm jmp	[addresses.lpGetStockObjectAddress]
}

#pragma warning(default: 4100 4740)

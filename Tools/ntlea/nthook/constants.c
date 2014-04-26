
#include <crtdefs.h>

// more controls see : http://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx
char const* SystemClassNameA[] = {
	"BUTTON", "COMBOBOX", "ComboLBox", /*"#32770",*/ "EDIT", "LISTBOX", "MDICLIENT", "RichEdit", "RICHEDIT_CLASS", 
	"SCROLLBAR", "STATIC", "SysTreeView32", "SysListView32", "SysAnimate32", "SysHeader32", "tooltips_class32",
//	"SysTabControl32", "ToolbarWindow32", "ComboBoxEx32", "SysDateTimePick32", "SysMonthCal32", "ReBarWindow32", 
//	"msctls_progress32", "msctls_trackbar32", "msctls_statusbar32", "msctls_updown32", "msctls_hotkey32",
	/*NULL, */
};
wchar_t const* SystemClassNameW[] = {
	L"BUTTON", L"COMBOBOX", L"ComboLBox", /*L"32770",*/ L"EDIT", L"LISTBOX", L"MDICLIENT", L"RichEdit", L"RICHEDIT_CLASS", 
	L"SCROLLBAR", L"STATIC", L"SysTreeView32", L"SysListView32", L"SysAnimate32", L"SysHeader32", L"tooltips_class32",
//	L"SysTabControl32", L"ToolbarWindow32", L"ComboBoxEx32", L"SysDateTimePick32", L"SysMonthCal32", L"ReBarWindow32",
//	L"msctls_progress32", L"msctls_trackbar32", L"msctls_statusbar32", L"msctls_updown32", L"msctls_hotkey32",
	/*NULL, */
};

char const* szMultipleHookNotAllowed = "The Microsoft Applocale has been detected, NTLEA is not compatible with it.\r\n"
"Please unload it from memory and run the application again.";
char const* szUnhandledExceptionText = "Sorry, NTLEA got a fatal exception during the execution, the current application will be terminated from its context immediately.\r\n\r\n"
"Exception description:\r\n\r\n%s";
char const* szAppCallDefConvProc = "The executing application submitted a call to one of the DefWindowProc/DefFrameProc/DefDlgProc/DefMDIChildProc functions for a non-unicode window with incorrect timing.";
char const* szAppCallRefUnmatch = "The Install/UninstallCBTHook reference count is mismatch.";
char const* szNtleaiDllMissing = "The Injection DLL could not find in Windows PATH Environment.";
char const* szNtleaiExeFailed = "The Specified EXE creation was failed.";
char const* szNtleaiExeUnsupport = "The Specified EXE creation was not support yet.";


#include "ntleasInc.h"
#include "ntleasWinApp.h"

// see : http://msdn.microsoft.com/en-us/library/windows/desktop/bb773175(v=vs.85).aspx
#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		// Start Win32++
		CNtleasWinApp theApp;

		// Run the application
		return theApp.Run();
	}

	// catch all exceptions inherited from std::exception
	catch (std::exception &e)
	{
		// Process the exception and quit 
		e.what();
		return -1;
	}
}

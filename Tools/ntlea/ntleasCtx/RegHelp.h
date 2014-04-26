
#ifndef __REGHELP_H__
#define __REGHELP_H__

#include <Windows.h>

BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey);
BOOL RegDelnode(HKEY hKeyRoot, LPTSTR lpSubKey);

#endif // __REGHELP_H__

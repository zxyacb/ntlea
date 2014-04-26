
#ifndef __SHORTCUT_H__
#define __SHORTCUT_H__

// see : http://msdn.microsoft.com/en-us/library/aa969393.aspx#Shellink_Creating_Shortcut

#include <Windows.h>

// CreateLink - Uses the Shell's IShellLink and IPersistFile interfaces 
//              to create and store a shortcut to the specified object. 
//
// Returns the result of calling the member functions of the interfaces. 
//
// Parameters:
// lpszPathObj  - Address of a buffer that contains the path of the object,
//                including the file name.
// lpszArgsObj	- Address of a buffer that contains the whole arguments,
//				  append at the tail of the pathobj. <can be NULL>
// lpszDirObj	- Address of a buffer that contains the initialize directory.
//				  <can be NULL>
// lpszPathLnk  - Address of a buffer that contains the path where the 
//                Shell link is to be stored, including the file name.
// lpszDesc     - Address of a buffer that contains a description of the 
//                Shell link, stored in the Comment field of the link
//                properties. <can be NULL>

HRESULT CreateFileLink(LPCWSTR lpszPathObj, LPCWSTR lpszArgsObj, LPCWSTR lpszDirObj, 
	LPCWSTR lpszIconPath, LPCWSTR lpszPathLnk, LPCWSTR lpszDesc);

#endif // __SHORTCUT_H__


#ifndef __NTLEA_H__
#define __NTLEA_H__

#define MAX_APPLICATION_SUPPORTED	(2)
#define NTLEA_PARAMETERS_LENGTH		(LF_FACESIZE + 20)

///#define VER_WINXP_SP2_OR_ABOVE		0
///#define VER_WINXP_SP1				1
///#define VER_WIN2K					2
///#define VER_WINXP					3
///#define VER_MAJOR_WIN2K				5

#define szRcpIntMtx					"RcpInternalMutex"
#define szTranslation				"\\VarFileInfo\\Translation"
#define szRcpEvent					"RcpEvent000"
#define szRcpFileMap				"RcpFileMap000"
#define szRshFileMap				"RshFileMap000"
#define szCaption					"NT Locale Emulator Advance"

#if defined(DEBUG) || defined(_DEBUG)
#include <stdio.h>
#include <assert.h>
#include <ntdebug.h>
#else
#define fprintf(...)				(0)
#define assert(...)					(0)
#define ntprintfA(...)				(0)
#define getchar(...)				Sleep(0)
#endif

#if defined(_AMD64_)
#define RegIP						Rip
#define IMAGE_FILE_MACHINE_VALID	IMAGE_FILE_MACHINE_AMD64
#define IMAGE_FILE_MACHINE_INVALID	IMAGE_FILE_MACHINE_I386
#define IMAGE_FILE_MACHINE_ESTRING	L"x86"
#define WM_CLASSMASK				0xFFFFFFFF00000000
#elif defined(_X86_)
#define RegIP						Eip
#define IMAGE_FILE_MACHINE_VALID	IMAGE_FILE_MACHINE_I386
#define IMAGE_FILE_MACHINE_INVALID	IMAGE_FILE_MACHINE_AMD64
#define IMAGE_FILE_MACHINE_ESTRING	L"x64"
#define WM_CLASSMASK				0xFFFF0000
#endif

#endif // __NTLEA_H__

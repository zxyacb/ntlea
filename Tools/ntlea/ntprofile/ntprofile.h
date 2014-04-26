
#ifndef __NTPROFILE_H__
#define __NTPROFILE_H__

#include <wchar.h>

typedef enum {
	PROFILE_CONFIGNAME,
	PROFILE_CODEPAGECODE,	// 
	PROFILE_CODEPAGE,		// code-string <=> readable-string
	PROFILE_LOCALECODE,		// 
	PROFILE_LOCALE,			// code-string <=> readable-string
	PROFILE_TIMEZONEDESC,	// 
	PROFILE_TIMEZONE,		// code-string <=> readable-string
	PROFILE_FONTCODE,		// not used currently ... 
	PROFILE_FONT,			// ?? may or may not ?? 
	PROFILE_ARGUMENT,
	PROFILE_COMPONENT,
	PROFILE_DUMPERR,
	PROFILE_SPECPATH,

	PROFILE_COUNT,
} NtEntryType;

typedef struct NtProfile {
	unsigned int bitfields;
	wchar_t const* entrys[PROFILE_COUNT]; // I know here should be 'entries', but ... 
} NtProfile;

#ifdef __cplusplus
extern "C" {
#endif // 

int InitNtProfiles(void * profilestream, size_t length, int(*fnparse)(NtProfile* ntprofiles, void* ctx), void* callback);
int CreateProfiles(NtProfile* ntprofile);
int UpdateProfiles(NtProfile* ntprofile, NtEntryType entry, wchar_t const* newvalue);
int DeleteProfiles(NtProfile* ntprofile, int clear, int reuse);
int IsDeletedProfile(NtProfile* profile);
int FreeNtProfiles(NtProfile* ntprofiles, int count, int(*fnwrite)(wchar_t const* buf, void* ctx), void* callback);

#ifdef __cplusplus
}
#endif // 

#endif // __NTPROFILE_H__

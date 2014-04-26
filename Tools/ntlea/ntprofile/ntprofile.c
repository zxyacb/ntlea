
#include "ntprofile.h"
#include <Windows.h>

int InitNtProfiles(void * profilestream, size_t length, int(*fnparse)(NtProfile* ntprofiles, void* ctx), void* callback)
{
	int i, ret = 1, count = 0;
	wchar_t const* p = (wchar_t const*)((unsigned char*)profilestream + 0);
	wchar_t const* e = (wchar_t const*)((unsigned char*)profilestream + length);
	while (p < e && ret && fnparse) {
		NtProfile profile = { 0 };
		for (i = 0; i < PROFILE_COUNT; ++i) {
			profile.entrys[i] = p;
			while (*p && p < e) ++p;
			if (*p++ != L'\0') return -1; // broken ... 
		}
		if (i != PROFILE_COUNT) return -2; // broken ... 
		ret = fnparse(&profile, callback); ++count;
	}
	return (count);
}

int CreateProfiles(NtProfile* ntprofile)
{
	for (int i = 0; i < PROFILE_COUNT; ++i) {
		ntprofile->entrys[i] = L""; // as empty string 
	}
	ntprofile->bitfields = 0;
	return (0);
}

int UpdateProfiles(NtProfile* ntprofile, NtEntryType entry, wchar_t const* newvalue)
{
	if (entry >= PROFILE_COUNT) return -3;
	ntprofile->bitfields |= 1 << entry;
	ntprofile->entrys[entry] = /*wcsdup*/lstrcpyW((wchar_t*)LocalAlloc(0, (lstrlenW(newvalue)+1)*sizeof(wchar_t)), newvalue);
	return (0);
}

int DeleteProfiles(NtProfile* ntprofile, int clear, int reuse)
{
	if (clear)
		ntprofile->bitfields |= (1 << 31); // set MSB
	else
		ntprofile->bitfields &= ~(1 << 31); // clear MSB
	// if not reuse, clean buffer allocated ... 
	if (!reuse) {
		for (int j = 0; j < PROFILE_COUNT; ++j) {
			if (ntprofile->bitfields & (1 << j)) LocalFree((void*)ntprofile->entrys[j]);
		}
	}
	return (0);
}

int FreeNtProfiles(NtProfile* ntprofiles, int count, int(*fnwrite)(wchar_t const* buf, void* ctx), void* callback)
{
	for (int i = 0; i < count; ++i) { // ------- 
		NtProfile* p = &ntprofiles[i];
		for (int j = 0; j < PROFILE_COUNT; ++j) {
			if (!IsDeletedProfile(p) && fnwrite) fnwrite(p->entrys[j], callback);
			// if tag as new, free memory : 
			if (p->bitfields & (1 << j)) /*free*/LocalFree((void*)p->entrys[j]);
		}
	}
	return (0);
}

int IsDeletedProfile(NtProfile* profile)
{
	return profile->bitfields & (1 << 31); /*check MSB*/
}

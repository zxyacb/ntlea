
#if !defined(_DEBUG) && !defined(DEBUG)
#include <crtdefs.h>
//#pragma comment(linker, "/ALIGN:16")// Merge sections
// see also : http://stackoverflow.com/questions/2938966/how-to-use-vc-intrinsic-functions-w-o-run-time-library 
#pragma function(memset)
void* memset(void* dest, int val, size_t len) {
	register unsigned char *ptr = (unsigned char*)dest;
	while (len-- > 0) { *ptr++ = (unsigned char)val; } return dest;
}
//#pragma function(memmove)
void* memmove(void * ds_, const void * sr_, size_t len)
{
	if (ds_ < sr_) {
		register char* dst = (char *)ds_;
		register char* src = (char *)sr_;
		while (len--)
			*dst++ = *src++;
	}
	else {
		register char *lastd = (char *)ds_ + (len - 1);
		register char *lasts = (char *)sr_ + (len - 1);
		while (len--)
			*(char *)lastd-- = *(char *)lasts--;
	}
	return ds_;
}
#pragma function(memcpy)
void* memcpy(void* ds_, const void* sr_, size_t len)
{
	return memmove(ds_, sr_, len);
}
#else
int ___memory___;
#endif

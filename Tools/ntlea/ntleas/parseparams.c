
#include <wchar.h>

__inline int isdivide(int c) {
	return c == L' ' || c == L'\r' || c == L'\n' || c == L'\v' || c == L'\t';
}
__inline wchar_t* skipdivide(wchar_t* t) {
	while (isdivide(*t)) *t++ = L'\0'; return t;
}

int ParseParameter(wchar_t* commandline, wchar_t const* argv[/*maxargs*/], int maxargs)
{
	int argc = 0;
	int ignore_token = 0, tryfind = 1;
	wchar_t* t = skipdivide(commandline);
	while (L'\0' != *t) {
		register int c = *t;
		if (c == L'\"') {
			ignore_token = 1 - ignore_token; // toggle
			*t++ = L'\0';
		}
		else if (ignore_token == 0 && isdivide(c)) {
			t = skipdivide(t); tryfind = 1;
			// ptr to the buffer 
			if (argc >= maxargs) {
			//	fprintf(stderr, "Params Too much! max support is: %u \n", maxargs);
				return -1;
			}
		}
		else if (tryfind == 1) {
			tryfind = 0;
			argv[++argc] = t; // no needs check MAXARGUMENTS ! 
			++t;
		}
		else ++t;
	}
	return argc;
}

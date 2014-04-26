
#if defined(DEBUG) || defined(_DEBUG)
#define SUFFIX L"_D"
#else
#define SUFFIX L""
#endif

static DWORD NtleasGetArch(LPCWSTR AppPath, int* binary =NULL) {
	DWORD dwtype, dwread; char FileBuffer[1024];
	if (binary) *binary = 0; // clear binary tag first .
	if (GetBinaryTypeW(AppPath, &dwtype)) {
		HANDLE hfile = CreateFileW(AppPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hfile && ReadFile(hfile, FileBuffer, sizeof(FileBuffer), &dwread, NULL) && (*(WORD*)FileBuffer == 0x5A4D)) {
			CloseHandle(hfile);
			if (binary) *binary = 1; // set is binary !
			// now parse the entrypoint : 
			IMAGE_DOS_HEADER* pdoshead = (IMAGE_DOS_HEADER*)(FileBuffer);
			IMAGE_NT_HEADERS* pntheads = (IMAGE_NT_HEADERS*)(FileBuffer + pdoshead->e_lfanew);
			// check x86 or x64 or ... ??? 
			return pntheads->FileHeader.Machine;
		}
	}
	else { // else not executable file, using system version ?? 
		SYSTEM_INFO info; GetSystemInfo(&info);
		if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
			return IMAGE_FILE_MACHINE_AMD64;
		}
		else if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
			return IMAGE_FILE_MACHINE_I386;
		}
	}
	return IMAGE_FILE_MACHINE_UNKNOWN;
}

static BOOL NtleasRunApp(DWORD arch, LPWSTR approot, LPWSTR appname, LPCWSTR parameter)
{
	// -------------------- 
	LPCWSTR archstr;
	if (arch == IMAGE_FILE_MACHINE_I386) {
		archstr = L"x86";
	}
	else if (arch == IMAGE_FILE_MACHINE_AMD64) {
		archstr = L"x64";
	}
	else {
		return FALSE;
	}
	// -------------------- 
	auto fnPathToDir = [](LPWSTR exepath) {
		LPWSTR p = exepath + lstrlenW(exepath);
		while (p > exepath && *p != L'\\') --p;
		if (*p == L'\\') {
			exepath[p - exepath] = L'\0'; // make as tail 
		}
		return exepath;
	};
	// -------------------- 
	LPWSTR appdir = fnPathToDir(approot);
	int strbuff = (32 + lstrlenW(appdir) + lstrlenW(archstr) + lstrlenW(appname));
	int strsize = (strbuff + lstrlenW(parameter));
	LPWSTR param = (LPWSTR)LocalAlloc(0, sizeof(WCHAR)*strsize); param[0] = L'\"';
	// reference : http://blog.csdn.net/guowenyan001/article/details/17259173
	auto FindFileExists = [](LPCWSTR p) {
		WIN32_FIND_DATAW f; auto n = FindFirstFileW(p, &f); return(n == INVALID_HANDLE_VALUE) ? 0 : (FindClose(n), 1); };
	int pathsize = wsprintfW(param + 1, L"%s\\%s\\ntleas" SUFFIX L".exe", appdir, archstr);
	if (!FindFileExists(param + 1))
		pathsize = wsprintfW(param + 1, L"%s\\ntleas" SUFFIX L".exe", appdir); // missing, check same dir 
	if (!FindFileExists(param + 1)) {
		return FALSE;
	}
	else {
		// we know parameter always reserved a space, so here is no needs ... 
		wsprintfW(param + 1 + pathsize, L"\" \"%s\"%s", appname, parameter);
		// -------------- 
		LPWSTR exedir = fnPathToDir(appname); // 
		PROCESS_INFORMATION proinfo = { NULL, NULL, };
		STARTUPINFOW stinfo = { sizeof(STARTUPINFOW), };
	//	OutputDebugStringW(param);
		BOOL ret = CreateProcessW(NULL, param, NULL, NULL, FALSE, 0, NULL, exedir, &stinfo, &proinfo);
		CloseHandle(proinfo.hThread);
		CloseHandle(proinfo.hProcess);
		exedir[lstrlenW(exedir)] = L'\\'; // 
		appdir[lstrlenW(appdir)] = L'\\'; // restore
		LocalFree(param);
		return ret;
	}
}

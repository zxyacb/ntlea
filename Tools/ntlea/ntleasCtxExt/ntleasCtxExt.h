
#ifndef __NTLEASCTXEXT_H__
#define __NTLEASCTXEXT_H__

#include <Windows.h>
#include <Shlobj.h>

typedef struct NtleasContextExtend NtleasContextExtend;

#ifdef DynamicLoadModule 
#define __fnDef(__name)		_##__name
#else
#define __fnDef(__name)		__declspec(dllexport) __name
#endif

#ifdef __cplusplus
extern "C" {
#endif //

	int __fnDef(Ntleas_SizeCtxExt)(void);
	int __fnDef(Ntleas_InitCtxExt)(NtleasContextExtend*);
	int __fnDef(Ntleas_FreeCtxExt)(NtleasContextExtend*);
	
	int __fnDef(Ntleas_QueryContextMenu)(NtleasContextExtend*, HMENU, UINT i, UINT s, UINT e, UINT uFlags);
	int __fnDef(Ntleas_InvokeCommand   )(NtleasContextExtend*, LPCMINVOKECOMMANDINFO pici, LPCWSTR seletedFilePath);
	int __fnDef(Ntleas_GetCommandString)(NtleasContextExtend*, UINT_PTR idCommand, UINT uFlags, UINT*, LPSTR pszName, UINT cchMax);

#ifdef DynamicLoadModule
	// another method is using inline func + jmp Ntleas_DllCtl, but thus required assembly code ... 
#define Ntleas_SizeCtxExt			reinterpret_cast<decltype(_Ntleas_SizeCtxExt)		*>(Ntleas_DllCtl(0))
#define Ntleas_InitCtxExt			reinterpret_cast<decltype(_Ntleas_InitCtxExt)		*>(Ntleas_DllCtl(1))
#define Ntleas_FreeCtxExt			reinterpret_cast<decltype(_Ntleas_FreeCtxExt)		*>(Ntleas_DllCtl(2))
#define Ntleas_QueryContextMenu		reinterpret_cast<decltype(_Ntleas_QueryContextMenu)	*>(Ntleas_DllCtl(3))
#define Ntleas_InvokeCommand		reinterpret_cast<decltype(_Ntleas_InvokeCommand)	*>(Ntleas_DllCtl(4))
#define Ntleas_GetCommandString		reinterpret_cast<decltype(_Ntleas_GetCommandString)	*>(Ntleas_DllCtl(5))
	// -------------------------- 
	inline void* Ntleas_DllCtl(int index, HMODULE newhmod=0) { static FARPROC dllfunc[6]; 
	if (newhmod) { FARPROC dllload[] = { 
			GetProcAddress(newhmod, "Ntleas_SizeCtxExt"), GetProcAddress(newhmod, "Ntleas_InitCtxExt"),
			GetProcAddress(newhmod, "Ntleas_FreeCtxExt"), GetProcAddress(newhmod, "Ntleas_QueryContextMenu"),
			GetProcAddress(newhmod, "Ntleas_InvokeCommand"), GetProcAddress(newhmod, "Ntleas_GetCommandString"), };
		static_assert(sizeof(dllfunc)==sizeof(dllload), "sizeof(dllfunc)!=sizeof(dllload)");
		memcpy(dllfunc,dllload,sizeof(dllload)); return (newhmod);
	} else return (dllfunc[index]);
	}
#else
	inline void* Ntleas_DllCtl(int, HMODULE newhmod) { return newhmod; }
#endif //

#ifdef __cplusplus
}
#endif // 

#endif // __NTLEASCTXEXT_H__


#ifndef __MINIDUMP_H__
#define __MINIDUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

	void InitMiniDump(void const* basename, void const* dumpinst);
	void FreeMiniDump(void);

#ifdef __cplusplus
}
#endif

#endif // __MINIDUMP_H__

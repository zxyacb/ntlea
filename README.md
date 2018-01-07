# ntlea
migrate from http://ntlea.codeplex.com/

A fully rewrite using C source instead of original assembly source, named as the successor of its senior, inherited all input parameters as ntlea. with some new features below:

- Support From WinXP-SP2 to Current Windows 10（including: XP/Vista/7/8/8.1/10）
- Support Random BaseAddress Application Hook, and Some special non-standard Window Message Handling Hook (while ntlea may crash there)
- Support a lot of RMXP/RMVX applications, and more compatible under X64 system to run X86 applications
- Support Hook TabCtrl Message-Handler, to fix garbled-strings special in windows 8 / 8.1

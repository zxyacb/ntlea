;Create Desktop Shortcut & Get Windows & Desktop Dir==========================
;;PB4.00
;20061127, now works with unicode executables
;=============================================================================
;EnableExplicit

Declare createShellLink(obj.s, lnk.s, arg.s, desc.s, dir.s, icon.s, index)
Declare.s getSpecialFolder(id)

Procedure.s getSpecialFolder(id)
  Protected path.s, *ItemId.ITEMIDLIST
 
  *itemId = #Null
  If SHGetSpecialFolderLocation_(0, id, @*ItemId) = #NOERROR
    path = Space(#MAX_PATH)
    If SHGetPathFromIDList_(*itemId, @path)
      If Right(path, 1) <> "\"
        path + "\"
      EndIf
      ProcedureReturn path
    EndIf
  EndIf
  ProcedureReturn ""
EndProcedure

Procedure createShellLink(obj.s, lnk.s, arg.s, desc.s, dir.s, icon.s, index)
  ;obj - path to the exe that is linked to, lnk - link name, dir - working
  ;directory, icon - path to the icon file, index - icon index in iconfile
  Protected hRes.l, mem.s, ppf.IPersistFile
  CompilerIf #PB_Compiler_Unicode
    Protected psl.IShellLinkW
  CompilerElse
    Protected psl.IShellLinkA
  CompilerEndIf

  ;make shure COM is active
  CoInitialize_(0)
  hRes = CoCreateInstance_(?CLSID_ShellLink, 0, 1, ?IID_IShellLink, @psl)

  If hRes = 0
    psl\SetPath(Obj)
    psl\SetArguments(arg)
    psl\SetDescription(desc)
    psl\SetWorkingDirectory(dir)
    psl\SetIconLocation(icon, index)
    ;query IShellLink for the IPersistFile interface for saving the
    ;link in persistent storage
    hRes = psl\QueryInterface(?IID_IPersistFile, @ppf)

    If hRes = 0
      ;CompilerIf #PB_Compiler_Unicode
        ;save the link
        hRes = ppf\Save(lnk, #True)
;       CompilerElse
;         ;ensure that the string is ansi unicode
;         mem = Space(#MAX_PATH)
;         MultiByteToWideChar_(#CP_ACP, 0, lnk, -1, mem, #MAX_PATH)
;         ;save the link
;         hRes = ppf\Save(mem, #True)
;       CompilerEndIf
      ppf\Release()
    EndIf
    psl\Release()
  EndIf

  ;shut down COM
  CoUninitialize_()

  DataSection
    CLSID_ShellLink:
    Data.l $00021401
    Data.w $0000,$0000
    Data.b $C0,$00,$00,$00,$00,$00,$00,$46
    IID_IShellLink:
    CompilerIf #PB_Compiler_Unicode
      Data.l $000214F9
    CompilerElse
      Data.l $000214EE
    CompilerEndIf
    Data.w $0000,$0000
    Data.b $C0,$00,$00,$00,$00,$00,$00,$46
    IID_IPersistFile:
    Data.l $0000010b
    Data.w $0000,$0000
    Data.b $C0,$00,$00,$00,$00,$00,$00,$46
  EndDataSection
  ProcedureReturn hRes
EndProcedure

#CSIDL_WINDOWS = $24
#CSIDL_DESKTOPDIRECTORY = $10
;=============================================================================

;======================================================================
; Module:          Registry.pbi
;
; Author:          Thomas (ts-soft) Schulz
; Date:            Sep 02, 2013
; Version:         1.4.1
; Target Compiler: PureBasic 5.2+
; Target OS:       Windows
; License:         Free, unrestricted, no warranty whatsoever
;                  Use at your own risk
;======================================================================

; History:
; Version 1.4.1, Sep 02, 2013
; fixed XP_DeleteTree()

; Version 1.4, Sep 02, 2013
; fixed Clear Resultstructure
; + compatibility to WinXP

; Version 1.3.3, Sep 01, 2013
; + Clear Resultstructure

; Version 1.3.2, Aug 31, 2013
; fixed a Bug with WriteValue and Unicode

; Version 1.3.1, Aug 30, 2013
; + DeleteTree() ; Deletes the subkeys and values of the specified key recursively.

; Version 1.3, Aug 30, 2013
; + ErrorString to RegValue Structure
; + RegValue to all Functions
; RegValue holds Errornumber and Errorstring!
; Renamed CountValues to CountSubValues

; Version 1.2.1, Aug 25, 2013
; source length reduced with macros

; Version 1.2, Aug 25, 2013
; + CountSubKeys()
; + CountValues()
; + ListSubKey()
; + ListSubValue()
; + updated example
;
; Version 1.1, Aug 25, 2013
; + ReadValue for #REG_BINARY returns a comma separate string with hexvalues (limited to 2096 bytes)
; + small example

DeclareModule Registry
  Structure RegValue
    TYPE.l      ; like: #REG_BINARY, #REG_DWORD ...
    SIZE.l
    ERROR.l
    ERRORSTR.s 
    DWORD.l     ; #REG_DWORD
    QWORD.q     ; #REG_QWORD
    *BINARY     ; #REG_BINARY
    STRING.s    ; #REG_EXPAND_SZ, #REG_MULTI_SZ, #REG_SZ
  EndStructure
  
  Enumeration -1 Step -1
    #REG_ERR_ALLOCATE_MEMORY
    #REG_ERR_BINARYPOINTER_MISSING
    #REG_ERR_REGVALUE_VAR_MISSING
  EndEnumeration
  
  Declare.i ReadType(topKey,                  ; like #HKEY_LOCAL_MACHINE, #HKEY_CURRENT_USER, #HKEY_CLASSES_ROOT ...
                     KeyName.s,               ; KeyName without topKey
                     ValueName.s = "",        ; ValueName, "" for Default 
                     WOW64 = #False,          ; If #TRUE, uses the 'Wow6432Node' path for Key
                     *Ret.RegValue = 0)          
  ; result 0 = error or #REG_NONE (not supported)
  
  Declare.s ReadValue(topKey, 
                      KeyName.s, 
                      ValueName.s = "", 
                      WOW64 = #False, 
                      *Ret.RegValue = 0)
  ; result "" = error
  
  Declare.i WriteValue(topKey, 
                       KeyName.s, 
                       ValueName.s, 
                       Value.s,              ; Value as string
                       Type.l,               ; Type like: #REG_DWORD, #REG_EXPAND_SZ, #REG_SZ
                       WOW64 = #False, 
                       *Ret.RegValue = 0)    ; to return more infos, is required for #REG_BINARY!
  ; result 0 = error, > 0 = successfull (1 = key created, 2 = key opened)
  
  Declare.i DeleteTree(topKey, KeyName.s, WOW64 = #False, *Ret.RegValue = 0)
  ; Deletes the subkeys and values of the specified key recursively.
  ; result 0 = error
  
  Declare.i DeleteKey(topKey, KeyName.s, WOW64 = #False, *Ret.RegValue = 0)
  ; result 0 = error
  
  Declare.i DeleteValue(topKey, KeyName.s, ValueName.s, WOW64 = #False, *Ret.RegValue = 0)
  ; result 0 = error
  
  Declare.i CountSubKeys(topKey, KeyName.s, WOW64 = #False, *Ret.RegValue = 0)
  
  Declare.i CountSubValues(topKey, KeyName.s, WOW64 = #False, *Ret.RegValue = 0)
  
  Declare.s ListSubKey(topKey, KeyName.s, index, WOW64 = #False, *Ret.RegValue = 0) ; the index is 0-based!
  
  Declare.s ListSubValue(topKey, KeyName.s, index, WOW64 = #False, *Ret.RegValue = 0)
  
EndDeclareModule

Module Registry
  EnableExplicit
  
  Prototype RegDeleteKey(hKey.i, lpSubKey.p-Unicode, samDesired.l, Reserved.l = 0)
  Prototype RegSetValue(hKey.i, lpValueName.p-Unicode, Reserved.l, dwType.l, *lpData, cbData.l)
  Prototype RegDeleteTree(hKey.i, lpSubKey.p-Unicode = 0)
  
  Global RegDeleteKey.RegDeleteKey
  Global RegSetValue.RegSetValue
  Global RegDeleteTree.RegDeleteTree
  
  Define dll.i
  
  dll = OpenLibrary(#PB_Any, "Advapi32.dll")
  If dll
    RegDeleteKey = GetFunction(dll, "RegDeleteKeyExW")
    RegSetValue = GetFunction(dll, "RegSetValueExW")
    RegDeleteTree = GetFunction(dll, "RegDeleteTreeW")
  EndIf
  
  #KEY_WOW64_64KEY = $100
  #KEY_WOW64_32KEY = $200
  
  Macro OpenKey()
    If WOW64
      CompilerIf #PB_Compiler_Processor = #PB_Processor_x86
        samDesired | #KEY_WOW64_64KEY
      CompilerElse
        samDesired | #KEY_WOW64_32KEY
      CompilerEndIf
    EndIf
    
    If Left(KeyName, 1) = "\"  : KeyName = Right(KeyName, Len(KeyName) - 1) : EndIf
    If Right(KeyName, 1) = "\" : KeyName = Left(KeyName, Len(KeyName) - 1)  : EndIf
    
    If *Ret <> 0
      ClearStructure(*Ret, RegValue)
    EndIf
    
    error = RegOpenKeyEx_(topKey, KeyName, 0, samDesired, @hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      If hKey
        RegCloseKey_(hKey)
      EndIf
      ProcedureReturn #False
    EndIf
  EndMacro
  
  Macro OpenKeyS()
    If WOW64
      CompilerIf #PB_Compiler_Processor = #PB_Processor_x86
        samDesired | #KEY_WOW64_64KEY
      CompilerElse
        samDesired | #KEY_WOW64_32KEY
      CompilerEndIf
    EndIf
    
    If Left(KeyName, 1) = "\"  : KeyName = Right(KeyName, Len(KeyName) - 1) : EndIf
    If Right(KeyName, 1) = "\" : KeyName = Left(KeyName, Len(KeyName) - 1)  : EndIf
    
    If *Ret <> 0
      ClearStructure(*Ret, RegValue)
    EndIf
    
    error = RegOpenKeyEx_(topKey, KeyName, 0, samDesired, @hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      If hKey
        RegCloseKey_(hKey)
      EndIf
      ProcedureReturn ""
    EndIf
  EndMacro
  
  Procedure.s GetLastErrorStr(error)
    Protected Buffer.i, result.s
    
    If FormatMessage_(#FORMAT_MESSAGE_ALLOCATE_BUFFER | #FORMAT_MESSAGE_FROM_SYSTEM, 0, error, 0, @Buffer, 0, 0)
      result = PeekS(Buffer)
      LocalFree_(Buffer)
      
      ProcedureReturn result
    EndIf
  EndProcedure
  
  Procedure.i XP_DeleteTree(topKey, KeyName.s, *Ret.RegValue = 0)
    Protected hKey, error, dwSize.l, sBuf.s = Space(260)
    
    If Left(KeyName, 1) = "\"  : KeyName = Right(KeyName, Len(KeyName) - 1) : EndIf
    If Right(KeyName, 1) = "\" : KeyName = Left(KeyName, Len(KeyName) - 1)  : EndIf
    
    If *Ret <> 0
      ClearStructure(*Ret, RegValue)
    EndIf
    
    error = RegOpenKeyEx_(topKey, KeyName, 0, #KEY_ENUMERATE_SUB_KEYS, @hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      If hKey
        RegCloseKey_(hKey)
      EndIf
      ProcedureReturn #False
    EndIf
    
    Repeat
      dwSize.l = 260
      error = RegEnumKeyEx_(hKey, 0, @sBuf, @dwSize, 0, 0, 0, 0)
      If Not error
        XP_DeleteTree(hKey, sBuf)
      EndIf
    Until error
    RegCloseKey_(hKey)
    error = RegDeleteKey_(topKey, KeyName)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn #False
    EndIf
    
    ProcedureReturn #True    
  EndProcedure
  
  Procedure.i DeleteTree(topKey, KeyName.s, WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_ALL_ACCESS
    Protected hKey
    
    If RegDeleteTree = 0
      ProcedureReturn XP_DeleteTree(topKey, KeyName, *Ret)
    EndIf
    
    OpenKey()
    
    error = RegDeleteTree(hKey)
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn #False
    EndIf
    ProcedureReturn #True   
  EndProcedure
  
  Procedure.i DeleteKey(topKey, KeyName.s, WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_WRITE
    
    If WOW64
      CompilerIf #PB_Compiler_Processor = #PB_Processor_x86
        samDesired | #KEY_WOW64_64KEY
      CompilerElse
        samDesired | #KEY_WOW64_32KEY
      CompilerEndIf
    EndIf
    
    If Left(KeyName, 1) = "\"  : KeyName = Right(KeyName, Len(KeyName) - 1) : EndIf
    If Right(KeyName, 1) = "\" : KeyName = Left(KeyName, Len(KeyName) - 1)  : EndIf
    
    If RegDeleteKey
      error = RegDeleteKey(topKey, KeyName, samDesired)
    Else
      error = RegDeleteKey_(topKey, KeyName)
    EndIf
    If error
      If *Ret <> 0
        ClearStructure(*Ret, RegValue)
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn #False
    EndIf
    ProcedureReturn #True
  EndProcedure
  
  Procedure.i DeleteValue(topKey, KeyName.s, ValueName.s, WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_WRITE
    Protected hKey
    
    OpenKey()
    
    error = RegDeleteValue_(hKey, ValueName)
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn #False
    EndIf
    ProcedureReturn #True         
  EndProcedure
  
  Procedure.i CountSubKeys(topKey, KeyName.s, WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_READ
    Protected hKey, count
    
    OpenKey()
    
    error = RegQueryInfoKey_(hKey, 0, 0, 0, @count, 0, 0, 0, 0, 0, 0, 0)
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn #False
    EndIf
    ProcedureReturn count
  EndProcedure
  
  Procedure.s ListSubKey(topKey, KeyName.s, index, WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_READ
    Protected hKey, size, result.s
    
    OpenKeyS()
    
    error = RegQueryInfoKey_(hKey, 0, 0, 0, 0, @size, 0, 0, 0, 0, 0, 0)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      RegCloseKey_(hKey)
      ProcedureReturn ""
    EndIf
    size + 1
    result = Space(size)
    error = RegEnumKeyEx_(hKey, index, @result, @size, 0, 0, 0, 0)
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn ""
    EndIf
    ProcedureReturn result    
  EndProcedure
  
  Procedure.i CountSubValues(topKey, KeyName.s, WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_READ
    Protected hKey, count
    
    OpenKey()
    
    error = RegQueryInfoKey_(hKey, 0, 0, 0, 0, 0, 0, @count, 0, 0, 0, 0)
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn #False
    EndIf
    ProcedureReturn count
  EndProcedure
  
  Procedure.s ListSubValue(topKey, KeyName.s, index, WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_READ
    Protected hKey, size, result.s
    
    OpenKeyS()
    
    error = RegQueryInfoKey_(hKey, 0, 0, 0, 0, 0, 0, 0, @size, 0, 0, 0)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      RegCloseKey_(hKey)
      ProcedureReturn ""
    EndIf
    size + 1
    result = Space(size)
    error = RegEnumValue_(hKey, index, @result, @size, 0, 0, 0, 0)
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn ""
    EndIf
    ProcedureReturn result    
  EndProcedure
  
  Procedure.i ReadType(topKey, KeyName.s, ValueName.s = "", WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_READ
    Protected hKey, lpType
    
    OpenKey()
    
    error = RegQueryValueEx_(hKey, ValueName, 0, @lpType, 0, 0)
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn #False
    EndIf
    ProcedureReturn lpType         
  EndProcedure
  
  Procedure.s ReadValue(topKey, KeyName.s, ValueName.s = "", WOW64 = #False, *Ret.RegValue = 0)
    Protected error, result.s, samDesired = #KEY_READ
    Protected hKey, lpType.l, *lpData, lpcbData.l, ExSZlength, *ExSZMem, i
    
    OpenKeyS()
    
    error = RegQueryValueEx_(hKey, ValueName, 0, 0, 0, @lpcbData)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      RegCloseKey_(hKey)
      ProcedureReturn ""
    EndIf
    
    If lpcbData
      *lpData = AllocateMemory(lpcbData)
      If *lpData = 0
        If *Ret <> 0
          *Ret\ERROR = #REG_ERR_ALLOCATE_MEMORY
          *Ret\ERRORSTR = "Error: Can't allocate memory"
        EndIf
        Debug "Error: Can't allocate memory"
        RegCloseKey_(hKey)
        ProcedureReturn ""
      EndIf
    EndIf
    
    error = RegQueryValueEx_(hKey, ValueName, 0, @lpType, *lpData, @lpcbData)
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      FreeMemory(*lpData)
      ProcedureReturn ""
    EndIf    
    
    If *Ret <> 0
      *Ret\TYPE = lpType
    EndIf
    
    Select lpType
      Case #REG_BINARY
        If lpcbData <= 2096
          For i = 0 To lpcbData - 1
            result + "$" + RSet(Hex(PeekA(*lpData + i)), 2, "0") + ","
          Next
        Else
          For i = 0 To 2095
            result + "$" + RSet(Hex(PeekA(*lpData + i)), 2, "0") + ","
          Next
        EndIf
        result = Left(result, Len(result) - 1) 
        If *Ret <> 0
          *Ret\BINARY = *lpData
          *Ret\SIZE = lpcbData
        EndIf
        ProcedureReturn result ; we don't free the memory! 
        
      Case #REG_DWORD
        If *Ret <> 0
          *Ret\DWORD = PeekL(*lpData)
          *Ret\SIZE = SizeOf(Long)
        EndIf
        result = Str(PeekL(*lpData))
        
      Case #REG_EXPAND_SZ
        ExSZlength = ExpandEnvironmentStrings_(*lpData, 0, 0)
        If ExSZlength > 0
          *ExSZMem = AllocateMemory(ExSZlength)
          If *ExSZMem
            If ExpandEnvironmentStrings_(*lpData, *ExSZMem, ExSZlength)
              result = PeekS(*ExSZMem)
              If *Ret <> 0
                *Ret\STRING = result
                *Ret\SIZE = Len(result)
              EndIf
            EndIf
            FreeMemory(*ExSZMem)
          EndIf
        Else
          Debug "Error: Can't allocate memory"
        EndIf
        
      Case #REG_MULTI_SZ
        While i < lpcbData
          If PeekS(*lpData + i, 1) = ""
            result + #LF$
          Else
            result + PeekS(*lpData + i, 1)
          EndIf
          i + SizeOf(Character)
        Wend
        If Right(result, 1) = #LF$
          result = Left(result, Len(result) - 1)
        EndIf
        If *Ret <> 0
          *Ret\STRING = result
          *Ret\SIZE = Len(result)
        EndIf
        
      Case #REG_QWORD
        If *Ret <> 0
          *Ret\QWORD = PeekQ(*lpData)
          *Ret\SIZE = SizeOf(Quad)
        EndIf
        result = Str(PeekQ(*lpData))
        
      Case #REG_SZ
        result = PeekS(*lpData)
        If *Ret <> 0
          *Ret\STRING = result
          *Ret\SIZE = Len(result)
        EndIf
    EndSelect
    
    FreeMemory(*lpData)
    
    ProcedureReturn result
  EndProcedure
  
  Procedure.i WriteValue(topKey, KeyName.s, ValueName.s, Value.s, Type.l, WOW64 = #False, *Ret.RegValue = 0)
    Protected error, samDesired = #KEY_WRITE
    Protected hKey, *lpData, lpcbData.q, count, create, i, tmp.s, pos
    
    If WOW64
      CompilerIf #PB_Compiler_Processor = #PB_Processor_x86
        samDesired | #KEY_WOW64_64KEY
      CompilerElse
        samDesired | #KEY_WOW64_32KEY
      CompilerEndIf
    EndIf
    
    If Left(KeyName, 1) = "\"  : KeyName = Right(KeyName, Len(KeyName) - 1) : EndIf
    If Right(KeyName, 1) = "\" : KeyName = Left(KeyName, Len(KeyName) - 1)  : EndIf
    
    If *Ret <> 0
      If Type <> #REG_BINARY
        ClearStructure(*Ret, RegValue)
      Else
        *Ret\TYPE = 0
        *Ret\ERROR = 0
        *Ret\ERRORSTR = ""
        *Ret\DWORD = 0
        *Ret\QWORD = 0
        *Ret\STRING = ""
      EndIf
    EndIf
    
    error = RegCreateKeyEx_(topKey, KeyName, 0, 0, 0, samDesired, 0, @hKey, @create)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      If hKey
        RegCloseKey_(hKey)
      EndIf
      ProcedureReturn #False
    EndIf
    
    Select Type
      Case #REG_BINARY
        If *Ret = 0
          If *Ret <> 0
            *Ret\ERROR = #REG_ERR_REGVALUE_VAR_MISSING
            *Ret\ERRORSTR = "Error: Required *Ret.RegValue parameter not found!"
          EndIf
          Debug "Error: Required *Ret.RegValue parameter not found!"
          RegCloseKey_(hKey)
          ProcedureReturn #False
        EndIf
        lpcbData = *Ret\SIZE
        *lpData = *Ret\BINARY
        If *lpData = 0
          If *Ret <> 0
            *Ret\ERROR = #REG_ERR_BINARYPOINTER_MISSING
            *Ret\ERRORSTR = "Error: No Pointer to BINARY defined!"
          EndIf
          Debug "Error: No Pointer to BINARY defined!"
          RegCloseKey_(hKey)
          ProcedureReturn #False          
        EndIf
        If lpcbData = 0
          lpcbData = MemorySize(*lpData)
        EndIf
        error = RegSetValueEx_(hKey, ValueName, 0, #REG_BINARY, *lpData, lpcbData)
        
      Case #REG_DWORD
        error = RegSetValueEx_(hKey, ValueName, 0, #REG_DWORD, Val(Value), 4)
        
      Case #REG_QWORD
        error = RegSetValueEx_(hKey, ValueName, 0, #REG_QWORD, Val(Value), 8)
        
      Case #REG_EXPAND_SZ, #REG_SZ
        error = RegSetValueEx_(hKey, ValueName, 0, Type, @Value, StringByteLength(Value) + SizeOf(Character))
        
      Case #REG_MULTI_SZ
        count = CountString(Value, #LF$)
        For i = 0 To count
          tmp = StringField(Value, i + 1, #LF$)
          lpcbData + StringByteLength(tmp, #PB_Unicode) + 2
        Next
        If lpcbData
          *lpData = AllocateMemory(lpcbData)
          If *lpData
            For i = 0 To count
              tmp = StringField(Value, i + 1, #LF$)
              PokeS(*lpData + pos, tmp, -1, #PB_Unicode)
              pos + StringByteLength(tmp, #PB_Unicode) + 2
            Next
            error = RegSetValue(hKey, ValueName, 0, Type, *lpData, lpcbData)
            FreeMemory(*lpData)
          Else
            If *Ret <> 0
              *Ret\ERROR = #REG_ERR_ALLOCATE_MEMORY
              *Ret\ERRORSTR = "Error: Can't allocate memory"
            EndIf
            Debug "Error: Can't allocate memory"
            RegCloseKey_(hKey)
            ProcedureReturn #False            
          EndIf
        EndIf
        
    EndSelect
    
    RegCloseKey_(hKey)
    If error
      If *Ret <> 0
        *Ret\ERROR = error
        *Ret\ERRORSTR = GetLastErrorStr(error)
      EndIf
      Debug GetLastErrorStr(error)
      ProcedureReturn #False
    EndIf
    
    ProcedureReturn create         
  EndProcedure
  
EndModule
; IDE Options = PureBasic 5.21 LTS (Windows - x86)
; CursorPosition = 778
; FirstLine = 721
; Folding = ----
; EnableUnicode
; EnableXP
; EnableAdmin
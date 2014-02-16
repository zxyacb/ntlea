;Initialize Basic Parameters
curPath.s = GetPathPart(ProgramFilename()) ;With Slash (\)
;winPath.s = GetEnvironmentVariable("windir") ;Without Slash

Select ProgramParameter(0)
  Case "/cs"
    RunProgram("ntleas.exe", Chr(34)+ProgramParameter(1)+Chr(34)+" "+Chr(34)+"C936"+Chr(34)+" "+Chr(34)+"L2052"+Chr(34)+" "+Chr(34)+"T-480"+Chr(34), GetPathPart(ProgramParameter(1)))
    End
  Case "/ct"
    RunProgram("ntleas.exe", Chr(34)+ProgramParameter(1)+Chr(34)+" "+Chr(34)+"C950"+Chr(34)+" "+Chr(34)+"L1028"+Chr(34)+" "+Chr(34)+"T-480"+Chr(34), GetPathPart(ProgramParameter(1)))
    End
  Case "/jp"
    RunProgram("ntleas.exe", Chr(34)+ProgramParameter(1)+Chr(34)+" "+Chr(34)+"C932"+Chr(34)+" "+Chr(34)+"L1041"+Chr(34)+" "+Chr(34)+"T-540"+Chr(34), GetPathPart(ProgramParameter(1)))
    End
  Case "/kr"
    RunProgram("ntleas.exe", Chr(34)+ProgramParameter(1)+Chr(34)+" "+Chr(34)+"C949"+Chr(34)+" "+Chr(34)+"L1042"+Chr(34)+" "+Chr(34)+"T-540"+Chr(34), GetPathPart(ProgramParameter(1)))
    End
EndSelect

IncludeFile "ntlea8.pbi"
winPath.s = getSpecialFolder(#CSIDL_WINDOWS)
deskPath.s = getSpecialFolder(#CSIDL_DESKTOPDIRECTORY)
UseModule Registry

IncludeFile "ntlea8.pbf"

; Open the window and the gadgets/menu/toolbar created in form.pbf 
OpenwndNT8()
wndStretch.b =0
ResizeWindow(wndNT8, #PB_Ignore, #PB_Ignore, 390, #PB_Ignore)
SetGadgetText(txtEXE, "程序")
SetGadgetText(txtARG, "參數")
SetGadgetText(txtRUN, "指令")
SetGadgetText(optSimplified, "简体字")
SetGadgetText(optTraditional, "繁體字")
SetGadgetText(optJapanese, "日本語")
SetGadgetText(optKorean, "韓國語")
SetGadgetText(chkSetFont, "指定字體")
SetGadgetText(btnFontPick, "字體")
SetGadgetText(chkScale, "縮放")
SetGadgetText(btnCreateLNK, "建立.lnk")
SetGadgetText(btnExportBAT, "輸出.bat")
SetGadgetText(btnRUN, "開始運行")
SetGadgetText(txtRCSupp, "添加右鍵EXE選單")
SetGadgetText(btnSimplifiedReg, "NT简体字运行")
SetGadgetText(btnTraditionalReg, "NT繁體字運行")
SetGadgetText(btnJapaneseReg, "NT日本語實行")
SetGadgetText(btnKoreanReg, "NT韓國語運行")
SetGadgetText(txtDesktopDroplet, "建立拖放捷徑")
SetGadgetText(btnDropSimplified, "简体字")
SetGadgetText(btnDropTraditional, "繁體字")
SetGadgetText(btnDropJapanese, "日本語")
SetGadgetText(btnDropKorean, "韓國語")
SetGadgetText(chkCreateErrorDump, "建立除錯DUMP")
SetGadgetText(chkMBDir, "中文路徑兼容")
SetGadgetText(cmbFont, "MS Gothic")
SetGadgetState(spnScale, 100)
SetGadgetState(optJapanese, 1)

;Font Enumeration Start==================================================
Global NewList    FaceName.s() 

Procedure EnumFontFamExProc(*lpelfe.ENUMLOGFONTEX,*lpntme.NEWTEXTMETRICEX,FontType,lParam) 
   FaceName$ = PeekS(@*lpelfe\elfFullName) 
   Select FontType 
      Case #TRUETYPE_FONTTYPE 
      ;Type$ + " [TT]" 
   EndSelect 
    
   ForEach FaceName() 
      If FaceName() = FaceName$ + Type$ : Goto SkipFont : EndIf 
   Next 
    
   AddElement(FaceName())    
   FaceName() = FaceName$ + Type$ : Type$ = "" 
   SkipFont: 

   ProcedureReturn #True 
EndProcedure 
lf.LOGFONT 
lf\lfCharset = #DEFAULT_CHARSET 

hdc = GetDC_(WindowID(wndNT8)) 
EnumFontFamiliesEx_(hdc,lf,@EnumFontFamExProc(),0,0) 
ReleaseDC_(WindowID(wndNT8),hdc) 

ForEach FaceName() 
   SendMessage_(GadgetID(cmbFont),#CB_ADDSTRING,0,FaceName()) 
Next 
;Font Enumeration End=====================================================


Procedure updCommand()
  ntArg.s = ""
  If GetGadgetText(strARG) <> ""
  ntArg.s = Chr(34)+"A"+GetGadgetText(strARG)+Chr(34)
  EndIf
  ntLng.s = ""
  If GetGadgetState(optSimplified) = 1 : ntLng.s = Chr(34)+"C936"+Chr(34)+" "+Chr(34)+"L2052"+Chr(34)+" "+Chr(34)+"T-480"+Chr(34) : EndIf
  If GetGadgetState(optTraditional) = 1 : ntLng.s = Chr(34)+"C950"+Chr(34)+" "+Chr(34)+"L1028"+Chr(34)+" "+Chr(34)+"T-480"+Chr(34) : EndIf
  If GetGadgetState(optJapanese) = 1 : ntLng.s = Chr(34)+"C932"+Chr(34)+" "+Chr(34)+"L1041"+Chr(34)+" "+Chr(34)+"T-540"+Chr(34) : EndIf
  If GetGadgetState(optKorean) = 1 : ntLng.s = Chr(34)+"C949"+Chr(34)+" "+Chr(34)+"L1042"+Chr(34)+" "+Chr(34)+"T-540"+Chr(34) : EndIf
  ntFont.s = ""
  If GetGadgetState(chkSetFont) = #PB_Checkbox_Checked
    If GetGadgetState(chkScale) = #PB_Checkbox_Unchecked
    ntFont.s = Chr(34)+"F"+GetGadgetText(cmbFont)+Chr(34)
    ElseIf GetGadgetState(chkScale) = #PB_Checkbox_Checked
    ntFont.s = Chr(34)+"F"+GetGadgetText(cmbFont)+Chr(34)+" "+Chr(34)+"S"+GetGadgetText(spnScale)+Chr(34)
    EndIf
  EndIf
  ntFlag.i = 0
  If GetGadgetState(chkMBDir) = #PB_Checkbox_Checked : ntFlag.i = ntFlag.i+1 :EndIf
  If GetGadgetState(chkSetFont) = #PB_Checkbox_Checked : ntFlag.i = ntFlag.i+4 :EndIf
  If GetGadgetState(chkCreateErrorDump) = #PB_Checkbox_Checked : ntFlag.i = ntFlag.i+8 :EndIf
  SetGadgetText(strRUNCommand, Chr(34)+"P"+ntFlag.i+Chr(34)+" "+ntLng.s+" "+ntFont.s+" "+ntArg.s) 
EndProcedure

updCommand()


Repeat
  ;Main Event Lopp
  Event = WaitWindowEvent()
  If Event = #WM_LBUTTONDOWN : SendMessage_(WindowID(wndNT8), #WM_NCLBUTTONDOWN, #HTCAPTION , 0) : EndIf
  Select Event
  Case #PB_Event_Gadget
    Select EventGadget()
      Case btnClose
        If EventType() = #PB_EventType_LeftClick : Event = #PB_Event_CloseWindow : EndIf
      Case btnExpand
        If EventType() = #PB_EventType_LeftClick And wndStretch.b =0: ResizeWindow(wndNT8, #PB_Ignore, #PB_Ignore, 540, #PB_Ignore) : wndStretch.b =1 : SetGadgetText(btnExpand, "<<")
          ElseIf EventType() = #PB_EventType_LeftClick And wndStretch.b =1 : ResizeWindow(wndNT8, #PB_Ignore, #PB_Ignore, 390, #PB_Ignore) : wndStretch.b =0 : SetGadgetText(btnExpand, ">>"): EndIf
      Case btnEXEPick
        If EventType() = #PB_EventType_LeftClick : PickedEXE.s = OpenFileRequester("選擇可執行文件", "", "Executables (*.exe)|*.exe|All files (*.*)|*.*" , 0)
        SetGadgetText(strEXELoc, PickedEXE.s) : EndIf
      Case btnCreateLNK
        ;Create Desktop Link
        If GetGadgetText(strEXELoc) <> "" And EventType() = #PB_EventType_LeftClick
          targetEXEpath.s = GetGadgetText(strEXELoc)
          If createShellLink(curPath.s+"ntleas.exe", deskPath.s+GetFilePart(targetEXEpath.s, #PB_FileSystem_NoExtension)+"(NT).lnk", Chr(34)+targetEXEpath.s+Chr(34)+" "+GetGadgetText(strRUNCommand), "以NTLEA運行"+GetFilePart(targetEXEpath.s, #PB_FileSystem_NoExtension), GetPathPart(targetEXEpath.s), targetEXEpath.s, 0) = 0
          MessageRequester("捷徑已建立", "對應的捷徑已建立在桌面上", #PB_MessageRequester_Ok)
          EndIf
        EndIf
      Case btnExportBAT
        targetEXEpath.s = GetGadgetText(strEXELoc)
        CreateFile(expBat,GetPathPart(targetEXEpath.s)+GetFilePart(targetEXEpath.s, #PB_FileSystem_NoExtension)+"(NT).bat")
        WriteString(expBat,"ntleas.exe "+Chr(34)+GetFilePart(targetEXEpath.s)+Chr(34)+" "+GetGadgetText(strRUNCommand))
        CloseFile(expBat)
        CopyFile(curPath.s+"ntleas.exe", GetPathPart(targetEXEpath.s)+"ntleas.exe")
        CopyFile(curPath.s+"ntleai.dll", GetPathPart(targetEXEpath.s)+"ntleai.dll")
        MessageRequester("批處理已建立", "已建立批處理文件與複製批處理組件至目標路徑，將為您開啟目標", #PB_MessageRequester_Ok)
        RunProgram(winPath.s+"explorer.exe", Chr(34)+GetPathPart(targetEXEpath.s)+Chr(34), winPath.s)
      Case btnRUN
        targetEXEpath.s = GetGadgetText(strEXELoc)
        RunProgram(curPath.s+"ntleas.exe", Chr(34)+targetEXEpath.s+Chr(34)+" "+GetGadgetText(strRUNCommand), GetPathPart(targetEXEpath.s))
      Case btnDropSimplified  
        If createShellLink(curPath.s+"NTLEA8.exe", deskPath.s+"以简体字運行(NT).lnk", "/cs", "Run EXE in 简体字", curPath.s,  curPath.s+"NTLEA8.exe", 0) = 0
        ;MessageRequester("捷徑已建立", "對應的捷徑已建立在桌面上，拖曳EXE到捷徑上執行", #PB_MessageRequester_Ok)
        SetGadgetText(txtRegState, "简体桌面拖放捷徑OK")
        EndIf
      Case btnDropTraditional  
        If createShellLink(curPath.s+"NTLEA8.exe", deskPath.s+"以繁體字運行(NT).lnk", "/ct", "Run EXE in 繁體字", curPath.s,  curPath.s+"NTLEA8.exe", 0) = 0
        ;MessageRequester("捷徑已建立", "對應的捷徑已建立在桌面上，拖曳EXE到捷徑上執行", #PB_MessageRequester_Ok)
        SetGadgetText(txtRegState, "繁體桌面拖放捷徑OK")
        EndIf
      Case btnDropJapanese  
        If createShellLink(curPath.s+"NTLEA8.exe", deskPath.s+"以日本語運行(NT).lnk", "/jp", "Run EXE in 日本語", curPath.s,  curPath.s+"NTLEA8.exe", 0) = 0
        ;MessageRequester("捷徑已建立", "對應的捷徑已建立在桌面上，拖曳EXE到捷徑上執行", #PB_MessageRequester_Ok)
        SetGadgetText(txtRegState, "日語桌面拖放捷徑OK")
        EndIf
      Case btnDropKorean  
        If createShellLink(curPath.s+"NTLEA8.exe", deskPath.s+"以韓國語運行(NT).lnk", "/kr", "Run EXE in 韓國語", curPath.s,  curPath.s+"NTLEA8.exe", 0) = 0
        ;MessageRequester("捷徑已建立", "對應的捷徑已建立在桌面上，拖曳EXE到捷徑上執行", #PB_MessageRequester_Ok)
        SetGadgetText(txtRegState, "韓語桌面拖放捷徑OK")
        EndIf
      Case btnSimplifiedReg
        WriteValue(#HKEY_CLASSES_ROOT, "exefile\shell\NT简体字运行\command", "", curPath.s+"NTLEA8.exe /cs "+Chr(34)+"%1"+Chr(34), #REG_SZ)
        SetGadgetText(txtRegState, "添加了NT简体字运行")
      Case btnSimplifiedUnreg
        DeleteTree(#HKEY_CLASSES_ROOT, "exefile\shell\NT简体字运行")
        DeleteKey(#HKEY_CLASSES_ROOT, "exefile\shell\NT简体字运行")
        SetGadgetText(txtRegState, "移除了NT简体字运行")
      Case btnTraditionalReg
        WriteValue(#HKEY_CLASSES_ROOT, "exefile\shell\NT繁體字運行\command", "", curPath.s+"NTLEA8.exe /ct "+Chr(34)+"%1"+Chr(34), #REG_SZ)
        SetGadgetText(txtRegState, "添加了NT繁體字運行")
      Case btnTraditionalUnreg
        DeleteTree(#HKEY_CLASSES_ROOT, "exefile\shell\NT繁體字運行")
        DeleteKey(#HKEY_CLASSES_ROOT, "exefile\shell\NT繁體字運行")
        SetGadgetText(txtRegState, "移除了NT繁體字運行")
      Case btnJapaneseReg
        WriteValue(#HKEY_CLASSES_ROOT, "exefile\shell\NT日本語實行\command", "", curPath.s+"NTLEA8.exe /jp "+Chr(34)+"%1"+Chr(34), #REG_SZ)
        SetGadgetText(txtRegState, "添加了NT日本語實行")
      Case btnJapaneseUnreg
        DeleteTree(#HKEY_CLASSES_ROOT, "exefile\shell\NT日本語實行")
        DeleteKey(#HKEY_CLASSES_ROOT, "exefile\shell\NT日本語實行")
        SetGadgetText(txtRegState, "移除了NT日本語實行")
      Case btnKoreanReg
        WriteValue(#HKEY_CLASSES_ROOT, "exefile\shell\NT韓國語運行\command", "", curPath.s+"NTLEA8.exe /kr "+Chr(34)+"%1"+Chr(34), #REG_SZ)
        SetGadgetText(txtRegState, "添加了NT韓國語運行")
      Case btnKoreanUnreg
        DeleteTree(#HKEY_CLASSES_ROOT, "exefile\shell\NT韓國語運行")
        DeleteKey(#HKEY_CLASSES_ROOT, "exefile\shell\NT韓國語運行")
        SetGadgetText(txtRegState, "移除了NT韓國語運行")
    EndSelect
    updCommand()
  EndSelect

  ;Call the procedure generated in form.pbf, which handles all events for the window. It will call EventButtonOK().
  wndNT8_Events(event)

Until Event = #PB_Event_CloseWindow
End

; IDE Options = PureBasic 5.21 LTS (Windows - x86)
; CursorPosition = 142
; FirstLine = 125
; Folding = -
; EnableUnicode
; EnableXP
; EnableAdmin
; UseIcon = ntlea8.ico
; Executable = ..\NTLEA8.exe
; DisableDebugger
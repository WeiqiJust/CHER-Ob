!include "FileAssociation.nsh"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "RTIViewer"
!define PRODUCT_VERSION "1.1.0"
!define PRODUCT_PUBLISHER "VCG - ISTI - CNR"
!define PRODUCT_WEB_SITE "http://culturalheritageimaging.org/What_We_Offer/Downloads/View/index.html"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define QT_BASE "C:\Qt\4.8.4_x64"
!define DIR_INST_NAME "RTIViewer-1.1.0"
!define APPL_INST_NAME "RTIViewer-1.1.0"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\doc\gpl.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_RUN_TEXT "Run RTIViewer"
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "RTIViewerSetup-1.1.0-x64.exe"
InstallDir "$PROGRAMFILES64\${DIR_INST_NAME}"

RequestExecutionLevel admin

ShowInstDetails show
ShowUnInstDetails show
Section "MainSection" SEC01
  SetShellVarContext all
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "..\src\bin\RTIViewer-1.1.0.exe"
  File "..\..\rtiwebmaker\src\bin\rtiwebmaker.exe"
  File "..\doc\manual.pdf"
  
  CreateDirectory "$SMPROGRAMS\${DIR_INST_NAME}"
  CreateShortCut "$SMPROGRAMS\${DIR_INST_NAME}\${APPL_INST_NAME}.lnk" "$INSTDIR\RTIViewer-1.1.0.exe"
  CreateShortCut "$DESKTOP\${APPL_INST_NAME}.lnk" "$INSTDIR\RTIViewer-1.1.0.exe"
  
  ;Let's delete all the dangerous stuff from previous releases.
  Delete "$INSTDIR\qt*.dll"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\imageformats\*.dll"
  
  SetOutPath "$INSTDIR\imageformats"
  File ${QT_BASE}\plugins\imageformats\qjpeg4.dll
  File ${QT_BASE}\plugins\imageformats\qgif4.dll
  SetOutPath "$INSTDIR"
  File "${QT_BASE}\bin\QtCore4.dll"
  File "${QT_BASE}\bin\QtGui4.dll"
  File "${QT_BASE}\bin\QtOpenGL4.dll"
  File "${QT_BASE}\bin\QtXml4.dll"
  File "${QT_BASE}\bin\QtNetwork4.dll"
  
  File "..\doc\readme.txt"
  File "..\doc\gpl.txt"
  File "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\redist\x64\Microsoft.VC100.OpenMP\vcomp100.dll"
  File "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\redist\x64\Microsoft.VC100.CRT\msvcp100.dll"
   File "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\redist\x64\Microsoft.VC100.CRT\msvcr100.dll"
  
  File "..\doc\readme.txt"
  File "..\doc\gpl.txt"
  File "..\doc\thirdpartycode.txt"
    
SectionEnd


Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\RTIViewer-1.1.0.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
  ${registerExtension} "$INSTDIR\RTIViewer-1.1.0.exe" ".ptm" "Polynomial Texture Map"
  ${registerExtension} "$INSTDIR\RTIViewer-1.1.0.exe" ".rti" "RTI Image"
  ${registerExtension} "$INSTDIR\RTIViewer-1.1.0.exe" ".hsh" "Hemispherical Harmonics"
  ${registerExtension} "$INSTDIR\RTIViewer-1.1.0.exe" ".mview" "Multi View RTI"
SectionEnd


Function LaunchLink
  ExecShell "" "$INSTDIR\RTIViewer-1.1.0.exe"
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  SetShellVarContext all
  Delete "$INSTDIR\qt*.dll"
  Delete "$INSTDIR\*.txt"
  Delete "$INSTDIR\*.pdf"
  Delete "$INSTDIR\ming*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\imageformats\*.dll"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.manifest"
  
  Delete "$DESKTOP\${APPL_INST_NAME}.lnk"
  Delete "$SMPROGRAMS\${DIR_INST_NAME}\${APPL_INST_NAME}.lnk"

  RMDir "$SMPROGRAMS\${DIR_INST_NAME}"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR"

  ${unregisterExtension} ".ptm" "Polynomial Texture Map"
  ${unregisterExtension} ".rti" "RTI Image"
  ${unregisterExtension} ".hsh" "Hemispherical Harmonics"
  ${unregisterExtension} ".mview" "Multi View RTI"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey "HKCU" "Software\VCG\RTIViewer"
  SetAutoClose true
SectionEnd

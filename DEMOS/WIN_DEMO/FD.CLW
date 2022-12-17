; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFdView
LastTemplate=generic CWnd
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "fd.h"
VbxHeaderFile=fd.h
VbxImplementationFile=fd.cpp

ClassCount=8
Class1=CFdApp
Class2=CFdDoc
Class3=CFdView
Class4=CMainFrame
Class5=CAboutDlg

ResourceCount=3
Resource1=IDD_VOLUME_SET
Class6=CFdStatus
Resource2=IDR_MAINFRAME
Class7=CVolDlg
Class8=CFdMap
Resource3=IDD_ABOUTBOX

[CLS:CFdApp]
Type=0
HeaderFile=fd.h
ImplementationFile=fd.cpp
Filter=N
LastObject=CFdApp

[CLS:CFdDoc]
Type=0
HeaderFile=fddoc.h
ImplementationFile=fddoc.cpp
Filter=N
LastObject=CFdDoc

[CLS:CFdView]
Type=0
HeaderFile=fdview.h
ImplementationFile=fdview.cpp
Filter=C
LastObject=FD_VIEW_LOW_RES

[CLS:CMainFrame]
Type=0
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp
Filter=T
LastObject=CMainFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=fd.cpp
ImplementationFile=fd.cpp
Filter=D
LastObject=CAboutDlg

[CLS:CFdStatus]
Type=0
HeaderFile=fdstatus.h
ImplementationFile=fdstatus.cpp
Filter=Child Window
LastObject=CFdStatus

[CLS:CVolDlg]
Type=0
HeaderFile=voldlg.h
ImplementationFile=fdmm.cpp
Filter=D
LastObject=CVolDlg

[DLG:IDD_VOLUME_SET]
Type=1
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_SOUNDFX_VOLUME,scrollbar,1342242816
Control3=IDC_MUSIC_VOLUME,scrollbar,1342242816
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308353
Control6=IDC_STATIC,static,1342177283
Class=CVolDlg

[CLS:CFdMap]
Type=0
HeaderFile=fdmap.h
ImplementationFile=fdmap.cpp
Filter=C
LastObject=CFdMap

[MNU:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=FD_FILE_QUIT
Command8=FD_VIEW_FORWARD
Command9=FD_VIEW_LOW_RES
Command10=FD_OPTIONS_SOUND
Command11=FD_OPTIONS_MUSIC
Command12=FD_OPTIONS_VOLUME
Command13=FD_OPTIONS_AUTOMATIC
Command14=FD_OPTIONS_STATUS
Command15=FD_OPTIONS_MAP
Command16=FD_OPTIONS_CROSSHAIRS
Command17=ID_APP_ABOUT
CommandCount=17
Class=CFdView

[ACL:IDR_MAINFRAME]
Type=1
Command1=ID_FILE_SAVE_AS
Command2=FD_OPTIONS_CROSSHAIRS
Command3=FD_OPTIONS_VOLUME
Command4=ID_FILE_OPEN
Command5=FD_OPTIONS_MUSIC
Command6=ID_FILE_NEW
Command7=FD_OPTIONS_MAP
Command8=FD_VIEW_LOW_RES
Command9=ID_FILE_SAVE
Command10=FD_OPTIONS_STATUS
Command11=FD_OPTIONS_AUTOMATIC
Command12=ID_APP_ABOUT
Command13=FD_FILE_QUIT
Command14=FD_VIEW_FORWARD
Command15=FD_OPTIONS_SOUND
CommandCount=15
Class=CFdView

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=17
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308353
Control3=IDC_STATIC,static,1342308353
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Class=CAboutDlg


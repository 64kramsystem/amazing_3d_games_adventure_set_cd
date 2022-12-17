#ifndef __WINGTD_H__
#define __WINGTD_H__

//---------------------------------------------------------------------------
//
//	File:			WINGTD.H
//	Programmer: 	Bob Provencher
//	Create Date:	11/6/94
//	Last Mod Date:	1/2/95
//	Description:	WinGTopDown main module header file
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#ifndef __PRECOMP_H__
#include "precomp.h"
#endif

#define GOLD_BARS	0
#define MONSTERS    1
#define TILE_TIME   2
#define BLIT_TIME   3

extern HINSTANCE	hInst;
extern LPCSTR		lpszMainWndClass;
extern LPCSTR		lpszWinGTopDown;
extern HWND			hWndMain;
extern DWORD		wnd_value[ 4 ];

int PASCAL WinMain( HINSTANCE, HINSTANCE, LPSTR, int );
BOOL InitApplication( HINSTANCE );
BOOL InitInstance( HINSTANCE, int );
BOOL AppIdle( void );
LRESULT CALLBACK _export MainWndProc( HWND, UINT, WPARAM, LPARAM );

#endif


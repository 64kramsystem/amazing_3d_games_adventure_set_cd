#ifndef __WINGWND_H__
#define __WINGWND_H__

//---------------------------------------------------------------------------
//
//	File:			WINGWND.H
//	Programmer: 	Bob Provencher
//	Create Date:	11/6/94
//	Last Mod Date:	1/2/95
//	Description:	WinG window module header file
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#ifndef __PRECOMP_H__
#include "precomp.h"
#endif

#define NUM_TILES   5

extern LPCSTR	lpszWinGWndClass;
extern HWND		hWndWinG;

extern DWORD    tile_time;
extern DWORD    blit_time;

LRESULT CALLBACK _export WinGWndProc( HWND, UINT, WPARAM, LPARAM );
void GetGold( void );
void KillMonster( void );
void OnWindowSize( WORD w, WORD h );
void TileWnd( void );
void SetView( void );
void Move( int x, int y );
void MoveX( int x );
void MoveY( int y );

#endif



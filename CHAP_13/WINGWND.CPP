//---------------------------------------------------------------------------
//
//	File:			WINGWND.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	11/6/94
//	Last Mod Date:	1/2/95
//	Description:	Main WinG Window module
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "wingwnd.h"
#include "wingdc.h"
#include "wingbmp.h"
#include "palette.h"
#include "dibitmap.h"
#include "map.h"

#include "debug.h"

#include "wingtd.h"
#include "wingtd.rh"

LPCSTR			lpszWinGWndClass	= "WinGWndClass";

WinGDC			dcWinG;
WinGBitmap		bmpWinG;
LogPalette		logPalGame;
Palette			palGame;

LogPalette		logPalClear;        // used for clearing the system palette
Palette    		palClear;           // 

DIBitmap		dibFloor;
DIBitmap		dibPlayer;
DIBitmap		dibWall;
DIBitmap		dibGold;
DIBitmap		dibMonster;

DIBitmap		dibTiles;

const DIBitmap* dibs[ NUM_TILES ] =
{
	&dibFloor,
	&dibPlayer,
	&dibWall,
	&dibGold,
	&dibMonster
};

BitmapInfo		recInfo;
Map				map;

HWND			hWndWinG;

WORD            wnd_width;
WORD            wnd_height;
WORD			wnd_width_tiles;
WORD			wnd_height_tiles;
WORD			wnd_start_x;
WORD			wnd_start_y;

#define MIN_DELAY	67

LPARAM CALLBACK _export WinGWndProc( HWND hWnd, UINT msg,
									 WPARAM wParam, LPARAM lParam )
{

#ifdef _DEBUG
	static dbugstream debug;
#endif

	switch ( msg )
	{

		case WM_CREATE:

			HDC hDCScreen;
			HPALETTE hPal;
			int i, w, h;

			hDCScreen = GetDC( NULL );

			//
			//	C++ version of ClearSystemPalette()
			//

			logPalClear.ResizeTo( 256 );
			logPalClear.SetEntryFlags( 0, 256, PC_NOCOLLAPSE );
			palClear.Create( logPalClear );

			hPal = SelectPalette( hDCScreen, palClear, FALSE );
			RealizePalette( hDCScreen );
			SelectPalette( hDCScreen, hPal, FALSE );
			palClear.Delete();

			ReleaseDC( NULL, hDCScreen );

			dibFloor.Load( IDB_FLOOR );
			dibWall.Load( IDB_WALL );
			dibGold.Load( IDB_GOLD );
			dibMonster.Load( IDB_MONSTER );
			dibPlayer.Load( IDB_PLAYER );

			logPalGame.CopyColors( dibFloor );

			logPalGame.GetSystemEntries(   0, 10 );
			logPalGame.GetSystemEntries( 246, 10 );

			logPalGame.SetEntryFlags(   0,  10, 0 );
			logPalGame.SetEntryFlags(  10, 246, PC_NOCOLLAPSE );
			logPalGame.SetEntryFlags( 246,  10, 0 );

			palGame.Create( logPalGame );

			recInfo.ResizeTo( 256 );
			recInfo.WinGRecommendFormat();
			recInfo.CopyColors( logPalGame );

			map.Load( IDMAP_WINGTD );

			dibTiles.ResizeTo( (UINT)dibFloor.Width(),
							   (UINT)( dibFloor.Height() * NUM_TILES ) );

			for ( i = 0; i < NUM_TILES; i++ )
				_hmemcpy( (BYTE huge*)dibTiles.Bits() + ( (DWORD)i * dibs[ i ]->SizeImage() ),
						  dibs[ i ]->Bits(), dibs[ i ]->SizeImage() );

			w = GetSystemMetrics( SM_CXSCREEN );
			h = GetSystemMetrics( SM_CYSCREEN );

			//
			//	align on tile size
			//

			wnd_width_tiles  = ( ( w - 1 ) >> LINE_SHIFT ) + 1;
			wnd_height_tiles = ( ( h - 1 ) >> LINE_SHIFT ) + 1;
			wnd_width  = wnd_width_tiles  << LINE_SHIFT;
			wnd_height = wnd_height_tiles << LINE_SHIFT;
            SetView();

			recInfo.SetWidth( wnd_width );
			recInfo.SetHeight( recInfo.Orientation() * (long)wnd_height );

			bmpWinG.Delete();
			bmpWinG.Create( dcWinG, recInfo );

			dcWinG.Select( bmpWinG );

			TileWnd();

			return 0;

		case WM_QUERYNEWPALETTE:

			HDC hDC;

			hDC = GetDC( hWnd );

			if ( palGame.IsCreated() )
				SelectPalette( hDC, palGame, FALSE );

			RealizePalette( hDC );

			ReleaseDC( hWnd, hDC );

			InvalidateRect( hWnd, NULL, TRUE );

			return TRUE;

		case WM_SIZE:

			int old_w, old_h;

			old_w = wnd_width;
			old_h = wnd_height;

			OnWindowSize( LOWORD( lParam ), HIWORD( lParam ) );

			if ( old_w < wnd_width || old_h < wnd_height )
				TileWnd();

			return 0;

		case WM_PAINT:

			PAINTSTRUCT ps;
			DWORD start, blit;
			int left, top, width, height;

			hDC = BeginPaint( hWnd, &ps );

			left   = ps.rcPaint.left;
			top    = ps.rcPaint.top;
			width  = ps.rcPaint.right  - left;
			height = ps.rcPaint.bottom - top;

			if ( palGame.IsCreated() )
				SelectPalette( hDC, palGame, FALSE );

			RealizePalette( hDC );

			start = timeGetTime();

			dcWinG.BitBlt( hDC, left, top, width, height, left, top );

			blit = timeGetTime();

			wnd_value[ BLIT_TIME ] = blit - start;

#ifdef _DEBUG
			debug << "\r\nTile: " << wnd_value[ TILE_TIME ] << " milliseconds.\r\n"
				  << "Blit: "     << wnd_value[ BLIT_TIME ] << " milliseconds.\r\n";
#endif

			EndPaint( hWnd, &ps );

			return 0;

		case WM_LBUTTONDOWN:

			DWORD delay;

			delay = 0;

			do
			{
				RECT rc;
				POINT p;
				POINT p1, p2;
				double ratio;

				if ( timeGetTime() - delay < MIN_DELAY )
                	continue;

				GetCursorPos( &p );
				ScreenToClient( hWnd, &p );
				GetClientRect( hWnd, &rc );
				p1.x = ( (int)map.PlayerX() - (int)wnd_start_x ) * 32 + 16;
				p1.y = ( (int)map.PlayerY() - (int)wnd_start_y ) * 32 + 16;
				p2.x = p.x - p1.x;
				p2.y = p.y - p1.y;
				ratio = 0.0;
				if ( p2.x )
					ratio = abs( p2.y ) / abs( p2.x );
				if ( ratio >= 2.0 )
					p2.x = 0;
				else if ( ratio <= 0.5 )
					p2.y = 0;
				p2.x = p2.x ? ( p2.x > 0 ? 1 : -1 ) : 0;
				p2.y = p2.y ? ( p2.y > 0 ? 1 : -1 ) : 0;
				Move( p2.x, p2.y );
				TileWnd();
				InvalidateRect( hWnd, NULL, FALSE );
				UpdateWindow( hWnd );
                delay = timeGetTime();
			}
			while ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 );

      	return 0;

		case WM_KEYDOWN:

			int x_move, y_move;

			x_move = 0;
			y_move = 0;

			switch ( wParam )
			{

				case VK_NUMPAD6:
				case VK_RIGHT:

					x_move = 1;
					break;

				case VK_NUMPAD4:
				case VK_LEFT:

					x_move = -1;
					break;

				case VK_NUMPAD8:
				case VK_UP:

					y_move = -1;
					break;

				case VK_NUMPAD2:
				case VK_DOWN:

					y_move = 1;
					break;

				case VK_NUMPAD7:

					x_move = y_move = -1;
					break;

				case VK_NUMPAD9:

					x_move = 1;
					y_move = -1;
					break;

				case VK_NUMPAD3:

					x_move = 1;
					y_move = 1;
					break;

				case VK_NUMPAD1:

					x_move = -1;
					y_move = 1;
					break;

			}

			if ( x_move || y_move )
			{
				delay = 0;
				do
				{
					if ( timeGetTime() - delay < MIN_DELAY )
						continue;
					Move( x_move, y_move );
					TileWnd();
					InvalidateRect( hWnd, NULL, FALSE );
					UpdateWindow( hWnd );
                    delay = timeGetTime();
				}
				while ( GetAsyncKeyState( wParam ) && 0x8000 );
			}
			return 0;

		case WM_DESTROY:

			return 0;

	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

void OnWindowSize( WORD w, WORD h )
{
	wnd_width_tiles  = ( ( w - 1 ) >> LINE_SHIFT ) + 1;
	wnd_height_tiles = ( ( h - 1 ) >> LINE_SHIFT ) + 1;
	wnd_width  = w;
	wnd_height = h;
    SetView();
}

void SetView( void )
{
	wnd_start_x = max( 0, (int)map.PlayerX() - ( (int)wnd_width_tiles / 2 ) );
	wnd_start_x = min( (int)wnd_start_x, (int)map.Width() - (int)wnd_width_tiles );
	wnd_start_y = max( 0, (int)map.PlayerY() - ( (int)wnd_height_tiles / 2 ) );
	wnd_start_y = min( (int)wnd_start_y, (int)map.Height() - (int)wnd_height_tiles );
}

void TileWnd( void )
{
	DWORD start;
	start = timeGetTime();
	bmpWinG.Tile( map, dibTiles, wnd_start_x, wnd_start_y,
				  wnd_width_tiles, wnd_height_tiles );
	wnd_value[ TILE_TIME ] = timeGetTime() - start;
}

void Move( int x, int y )
{
	BYTE tile;
	if ( map.Tile( (int)map.PlayerX() + x, (int)map.PlayerY() + y, tile ) )
	{
		if ( tile == TILE_GOLD )
			GetGold();
		else if ( tile == TILE_MONSTER )
        	KillMonster();
		map.Move( x, y );
		SetView();
	}
}

void GetGold( void )
{
	wnd_value[ GOLD_BARS ] += random( 99 ) + 1;
}

void KillMonster( void )
{
	wnd_value[ MONSTERS ]++;
}

void MoveX( int x )
{
	Move( x, 0 );
}

void MoveY( int y )
{
	Move( 0, y );
}


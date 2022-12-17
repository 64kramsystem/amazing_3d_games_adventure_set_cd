//---------------------------------------------------------------------------
//
//	File:			WINGTD.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	11/6/94
//	Last Mod Date:	1/2/95
//	Description:	Main WinGTopDown module
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "wingtd.h"
#include "wingwnd.h"
#include "about.h"
#include "gen.h"

#include "wingtd.rh"

#define TEXT_WND_HEIGHT		20

HINSTANCE	hInst;
LPCSTR		lpszMainWndClass	= "MainWndClass";
LPCSTR		lpszWinGTopDown	= "WinG: From the Top Down";
HWND		hWndMain			= 0;
HWND		hWndText[ 4 ]		= { 0, 0, 0, 0 };
DWORD		wnd_value[ 4 ] 		= { 0, 0, 0, 0 };
char* 		wnd_text[ 4 ]		=
{
	"Gold Bars: %lu",
	"Monsters: %lu",
	"Tile: %lu ms",
	"Blit: %lu ms"
};

BOOL		fActive;

void Message( char* msg )
{
	MessageBox( NULL, msg, "WinG: From the Top Down", MB_OK | MB_ICONINFORMATION );
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR, int nCmdShow )
{

	hInst = hInstance;

	MSG msg;

    randomize();

	if ( !hPrevInstance )
		if ( !InitApplication( hInst ) )
			return FALSE;

	if ( !InitInstance( hInst, nCmdShow ) )
		return FALSE;

	do		
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message != WM_QUIT )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else
		{
			if ( AppIdle() )
				WaitMessage();
		}
	}
	while ( msg.message != WM_QUIT );

	return msg.wParam;

}

BOOL AppIdle()
{
	if ( fActive )
	{
		char buffer[ 64 ];
		for ( int i = 0; i < 4; i++ )
		{
			sprintf( buffer, wnd_text[ i ], wnd_value[ i ] );
			SetWindowText( hWndText[ i ], buffer );
		}
	}
	return TRUE;	
}

BOOL InitApplication( HINSTANCE hInstance )
{

	WNDCLASS  wc;

	wc.style			= CS_BYTEALIGNCLIENT;
	wc.lpfnWndProc		= MainWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= MAKEINTRESOURCE( IDM_DEMO );
	wc.lpszClassName	= lpszMainWndClass;

	if ( !RegisterClass( &wc ) )
		return FALSE;

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
	wc.lpfnWndProc		= WinGWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= lpszWinGWndClass;

	return RegisterClass( &wc );

}

BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{

	hInst = hInstance;

	hWndMain = CreateWindow( lpszMainWndClass, lpszWinGTopDown,
							 WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT, CW_USEDEFAULT,
							 CW_USEDEFAULT, CW_USEDEFAULT,
							 NULL, NULL, hInstance, NULL );

	if ( !hWndMain )
		return FALSE;

	ShowWindow( hWndMain, nCmdShow );
	UpdateWindow( hWndMain );

    return TRUE;                   

}

LPARAM CALLBACK _export MainWndProc( HWND hWnd, UINT message, WPARAM wParam,
									 LPARAM lParam )
{

	switch ( message )
	{

		case WM_CREATE:

			int i;

			for ( i = 0; i < 4; i++ )
			{
				hWndText[ i ] = CreateWindow( "STATIC", "",
										 WS_CHILD | WS_VISIBLE |
										  SS_CENTER | WS_BORDER,
										 0, 0, 0, 0,
										 hWnd, NULL, hInst, NULL );
			}

			hWndWinG = CreateWindow( lpszWinGWndClass, "",
									 WS_VISIBLE | WS_CHILD,
									 0, 0, 0, 0,
									 hWnd, NULL, hInst, NULL );

			return 0;

		case WM_ACTIVATEAPP:

			fActive = wParam;

            return 0;

		case WM_SIZE:

			int w, h;

			w = LOWORD( lParam );
			h = HIWORD( lParam );

			for ( i = 0; i < 4; i++ )
			{
				SetWindowPos( hWndText[ i ], NULL,
							  i * ( w / 4 ), h - TEXT_WND_HEIGHT,
							  w / 4, TEXT_WND_HEIGHT,
							  SWP_NOZORDER );
			}

			SetWindowPos( hWndWinG, NULL,
						  0, 0, w, h - TEXT_WND_HEIGHT,
						  SWP_NOMOVE | SWP_NOZORDER );

			return 0;

		case WM_COMMAND:

			if ( wParam == IDM_ABOUT )
			{
				DLGPROC dlgProc =
					(DLGPROC)MakeProcInstance( (FARPROC)About, hInst );
				DialogBox( hInst, MAKEINTRESOURCE( IDD_ABOUT ), hWnd, dlgProc );
				FreeProcInstance( (FARPROC)dlgProc );
				return 0;
			}
			break;

		case WM_SETFOCUS:

			SetFocus( hWndWinG );

            return 0;

		case WM_DESTROY:

			PostQuitMessage( 0 );

			return 0;

	}

	return DefWindowProc( hWnd, message, wParam, lParam );

}


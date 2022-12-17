#define APPNAME "StaticApp"
// NOTE: We couldn't use "STATIC" here, because that is already the name
// of a type of window in the system, and it would cause problems if we
// re-defined this.

/*
 *    Static.C
 *
 *    (C) Copyright Microsoft Corp. 1993.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */

#include <windows.h>	// Defines bulk of Windows functions and such...
#include <mmsystem.h>	// Defines additional Multi-Media functions...

#if !defined(WIN32)
	#include <wing.h>	// Defines WinG functions. Not use on Win32
#endif

#include <stdlib.h>		// Since I am using 'rand()'

#include "static.h"		// Local header

//  ==========================================================================
//  GLOBAL VARIABLES ---------------------------------------------------------
//  ==========================================================================

char    szAppName[]= APPNAME;	// A handy string to identify this app
char    szTitle[] = APPNAME ": Sample App"; // For the title bar

HINSTANCE hInstApp;		// A handle that identifies this 'process'
HWND      hwndApp;		// A handle that identifies the main window
HPALETTE  hpalApp;		// A handle that identifies the main palette
BOOL      fAppActive;	// A boolean that refers to this app being 'foreground'

// Define a structure that we will be using for keeping information about the
// image we are currently drawing into.
typedef struct _IMAGE {
	BITMAPINFOHEADER bi;  // Bitmap header information.
	RGBQUAD aColors[256]; // Palette color table
	union { // Now for the pointer to the data buffer we can whack on:
		LPVOID  lpvData; // This is the type that WinG likes to deal with
		LPBYTE  lpIndex; // This is just to make it easier for us to access it
	};
} _IMAGE;
_IMAGE image; // Contains most necessary information about the image to display


// Define a structure for holding our palette data. This is the color palette
// that will be assigned to both the above image, and be selected into the
// 'Device Context' for the screen so we will have an 'Identity' palette which
// will make for much faster screen updating.
typedef struct _PALETTE
{
	WORD Version;
	WORD NumberOfEntries;
	PALETTEENTRY aEntries[256];
} _PALETTE;
_PALETTE LogicalPalette = {0x300, 256}; // The 'logical' palette we will use
	// "0x300" = Windows 3.0 or later
	// "256" = Number of colors

long Orientation = 1; // Bitmap Orientation: TopDown=1, BottomUp=-1
HDC hdcImage = NULL;  // A handle to the Device Context for our image

HBITMAP gbmOldMonoBitmap = 0;	// Storage for the 'original' bitmap from our
								// Device Context, we need to restore it
								// later on, so we need to save it here.


//  ==========================================================================
//  FUNCTION DEFINITIONS -----------------------------------------------------
//  ==========================================================================

// Forward declarations for all functions.
// Listed in order they appear in this source listing

BOOL AppInit(HINSTANCE hInst,HINSTANCE hPrev,int sw,LPSTR szCmdLine);
BOOL AppIdle(void);
LONG FAR PASCAL AppWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL AppPaint (HWND hwnd, HDC hdc);
LONG AppCommand (HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL AppAbout(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
void AppExit(void);


/*----------------------------------------------------------------------------*\
|   WinMain( hInst, hPrev, lpszCmdLine, cmdShow )                              |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the App.  After initializing, it just goes      |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hInst           instance handle of this instance of the app            |
|       hPrev           instance handle of previous instance, NULL if first    |
|       szCmdLine       ->null-terminated command line                         |
|       cmdShow         specifies how the window is initially displayed        |
|                                                                              |
|   Returns:                                                                   |
|       The exit code as specified in the WM_QUIT message.                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
    MSG     msg;

	// NOTE: On Win32, hPrev will -always- be NULL

    // Do application initialization stuff
    if (!AppInit(hInst,hPrev,sw,szCmdLine)) {
		return 0; // Something failed to initialize
	}
	
	// Poll for messages from event queue.
	// loop continues until WM_QUIT is encountered    
    for (;;) {
		if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// When WM_QUIT comes through, we're DONE!
				break;
			}
			TranslateMessage(&msg); // Does some very necessary stuff
			DispatchMessage(&msg);  // Does some very necessary stuff
		} else {
			if (AppIdle()) {
				WaitMessage();	// Defer to all other apps until a message
								// gets put into our message queue
			}
		}
    }
    
    AppExit();	// Do application exiting stuff

    return msg.wParam;
}

/*----------------------------------------------------------------------------*\
|   AppInit( hInst, hPrev)                                                     |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into               |
|       memory.  It performs all initialization that doesn't need to be done   |
|       once per instance.                                                     |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of current instance                    |
|       hPrev           instance handle of previous instance                   |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL AppInit(HINSTANCE hInst,HINSTANCE hPrev,int sw,LPSTR szCmdLine)
{
    WNDCLASS cls;

    // Save instance handle for DialogBoxs as a global variable
    hInstApp = hInst;

    if (!hPrev) {
		// We don't already have a version running, so we need to
		// register our window class with the system
		cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
		cls.hIcon          = LoadIcon(hInst,szAppName);
		cls.lpszMenuName   = szAppName;
		cls.lpszClassName  = szAppName;
		cls.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
		cls.hInstance      = hInst;
		cls.style          = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		cls.lpfnWndProc    = (WNDPROC)AppWndProc;
		cls.cbWndExtra     = 0;
		cls.cbClsExtra     = 0;

		if (!RegisterClass(&cls)) {
			return FALSE; // Failed to register class. No need to continue.
	    }
    }

    hwndApp = CreateWindow (szAppName,  // Class name
			    szTitle,                // Caption
			    WS_OVERLAPPEDWINDOW,    // Style bits
			    CW_USEDEFAULT, 0,       // Position (x,y)
			    320, 200,               // Size (w,h)
			    (HWND)NULL,             // Parent window (no parent)
			    (HMENU)NULL,            // use class menu
			    hInst,                  // handle to current process instance
			    (LPSTR)NULL             // no params to pass on
			   );

	if (hwndApp) {
		ShowWindow(hwndApp,sw); // Time to display the window.
		return TRUE;
    } else {
		return FALSE; // Failed to create window. No need to continue
    }

}

/*----------------------------------------------------------------------------*\
|   AppIdle()                                                                  |
|                                                                              |
|   Description:                                                               |
|       place to do idle time stuff.                                           |
|                                                                              |
|   Returns:                                                                   |
|       RETURN TRUE IF YOU HAVE NOTHING TO DO OTHERWISE YOUR APP WILL BE A     |
|       CPU PIG!                                                               |
\*----------------------------------------------------------------------------*/
BOOL AppIdle()
{
    if (fAppActive) { // we are running in the foreground
		return TRUE;        // We currently aren't doing anything at idle time
    } else { // we are running in the background
		return TRUE;        // We currently aren't doing anything at idle time
    }

	// You will want to return FALSE here if any processing was done in this
	// function that might have caused some windows messages, or will require
	// the window to be repainted. If all you do is calculate and store
	// some values to be used later, you can return TRUE.
}


/*----------------------------------------------------------------------------*\
|   AppWndProc( hwnd, uiMessage, wParam, lParam )                              |
|                                                                              |
|   Description:                                                               |
|       The window proc for the app's main (tiled) window.  This processes all |
|       of the parent window's messages.                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/
LONG FAR PASCAL AppWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    static int XOffset, YOffset;
    static int dxClient, dyClient; // The 'Client' size. (drawing area of window)
    
    PAINTSTRUCT ps;
    HDC hdc;
	UINT uMappedColors;
    
    switch (msg) {
		case WM_CREATE:
			// This occures during 'CreateWindow' time. Here is where we can
			// easily initialize some things for this window.
			srand ((int)GetTickCount()); // initialize the random seed
			SetTimer(hwnd, 1, 1, NULL);  // start up the WM_TIMER messages
										 // We will force a recalc/redraw of
										 // our main window on each WM_TIMER
		break;
	    
		case WM_TIMER:
			// This comes to us because we called 'SetTimer(...)'.
			InvalidateRect (hwnd, NULL, FALSE); // Just force a redraw
			break;

		case WM_ACTIVATEAPP:
			// The application z-ordering has changed. If we are now the
			// foreground application wParm will be TRUE.
			fAppActive = (BOOL)wParam;
			break;

		case WM_ERASEBKGND:
			return TRUE;
			break;

	    case WM_SIZE:
			// This message comes to us because the window size has been
			// changed. It also comes to us when the application is first
			// being executed.
		    dxClient = LOWORD(lParam); // The 'new' width of our window
		    dyClient = HIWORD(lParam); // The 'new' height of our window

			if(hdcImage)     {
				// Our image Device Context has already been created, so
				// we can just use it. But we need to create a new bitmap
				// to draw into. We also need to make sure we delete the
				// previous bitmap -afterwards-.
				HBITMAP hbm;

				//  Create a new 8-bit WinGBitmap with the new size
				image.bi.biWidth = dxClient;
				image.bi.biHeight = dyClient * Orientation;
#if defined(WIN32)
				hbm = CreateDIBSection (hdcImage, (BITMAPINFO far *)&image.bi,
					DIB_PAL_COLORS, &image.lpvData, NULL, 0);
#else
				hbm = WinGCreateBitmap(hdcImage, (BITMAPINFO far *)&image.bi,
					&image.lpvData);
#endif
				if (!hbm) {
					//MessageBox (GetFocus(), "Failed to create Bitmap", szAppName, 0);
				} else {
					// Make sure that 'biSizeImage' reflects the
					// size of the bitmap data.
					image.bi.biSizeImage = (image.bi.biWidth * image.bi.biHeight);
					image.bi.biSizeImage *= Orientation;
					//  Select it in and delete the old one
					hbm = (HBITMAP)SelectObject(hdcImage, hbm);
					DeleteObject(hbm); // Make sure you delete the previous one!
				}
			} else {
				//  Create a new WinGDC and 8-bit WinGBitmap

				HBITMAP hbm;
			    int Counter;
			    HDC Screen;
			    //RGBQUAD far *pColorTable;

				//  Get WinG to recommend the fastest DIB format
#if defined(WIN32)
				if (FALSE) {
#else
				if(WinGRecommendDIBFormat((BITMAPINFO far *)&image.bi))	{
#endif
					//  make sure it's 8bpp and remember the orientation
					image.bi.biBitCount = 8;
					image.bi.biCompression = BI_RGB;
					Orientation = image.bi.biHeight;
				} else {
					//  set it up ourselves
					image.bi.biSize = sizeof(BITMAPINFOHEADER);
					image.bi.biPlanes = 1;
					image.bi.biBitCount = 8;
					image.bi.biCompression = BI_RGB;
					image.bi.biSizeImage = 0;
					image.bi.biClrUsed = 0;
					image.bi.biClrImportant = 0;
				}

				image.bi.biWidth = LOWORD(lParam);
				image.bi.biHeight = HIWORD(lParam) * Orientation;

				//  create an identity palette from the DIB's color table

				// Get the Device Context of the screen
			    Screen = GetDC(HWND_DESKTOP);

				// Get the 20 system colors as PALETTEENTRIES
			    GetSystemPaletteEntries(Screen,0,10,LogicalPalette.aEntries);
			    GetSystemPaletteEntries(Screen,246,10,LogicalPalette.aEntries + 246);
                
                // Only a few DCs available, free this up so we aren't a hog
				ReleaseDC(0,Screen);

				// Initialize the logical palette and DIB color table
				// Note that we are doing this as double entries. Making
				// sure that we keep both tables -identical- this is to
				// make sure that we can end up with an 'identity palette'
				// which means that both the colortable assigned to the DIB
				// and the palette entries associated with the palette
				// that is selected into the Device Context are identical.

			    for(Counter = 0; Counter < 10; Counter++) {
				// copy the system colors into the DIB header
				// WinG will do this in WinGRecommendDIBFormat,
				// but it may have failed above so do it here anyway

					// The low end colors...				
					image.aColors[Counter].rgbRed =
						LogicalPalette.aEntries[Counter].peRed;
					image.aColors[Counter].rgbGreen =
						LogicalPalette.aEntries[Counter].peGreen;
					image.aColors[Counter].rgbBlue =
						LogicalPalette.aEntries[Counter].peBlue;
					image.aColors[Counter].rgbReserved = 0;
					LogicalPalette.aEntries[Counter].peFlags = 0;
                                            
					// And the high end colors...
					image.aColors[Counter + 246].rgbRed =
						LogicalPalette.aEntries[Counter + 246].peRed;
					image.aColors[Counter + 246].rgbGreen =
						LogicalPalette.aEntries[Counter + 246].peGreen;
					image.aColors[Counter + 246].rgbBlue =
						LogicalPalette.aEntries[Counter + 246].peBlue;
					image.aColors[Counter + 246].rgbReserved = 0;
					LogicalPalette.aEntries[Counter + 246].peFlags = 0;
				}

			    // Now fill in all of the colors in the middle to reflect
			    // the colors that we are wanting. Here, we are just
			    // setting random values.
			    for(Counter = 10;Counter < 246;Counter++) {
					image.aColors[Counter].rgbRed =
						LogicalPalette.aEntries[Counter].peRed = rand()%255;
					image.aColors[Counter].rgbGreen =
						LogicalPalette.aEntries[Counter].peGreen = rand()%255;
					image.aColors[Counter].rgbBlue =
						LogicalPalette.aEntries[Counter].peBlue = rand()%255;
					image.aColors[Counter].rgbReserved = 0;
					// In order for this to be an identity palette, it is
					// important that we not only get this color, but that
					// we get it in THIS location. Using PC_NOCOLLAPSE tells
					// the system not to 'collapse' this entry to another
					// palette entry that already has this color.
					LogicalPalette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
			    }

			    // The logical palette table is fully initialized.
			    // All we have to do now, is create it.
			    hpalApp = CreatePalette((LOGPALETTE far *)&LogicalPalette);
			    
				//  Create a WinGDC and Bitmap, then select away
#if defined (WIN32)
				hdcImage = CreateCompatibleDC (NULL); // Create a DC compatible with current screen
#else
				hdcImage = WinGCreateDC();
#endif
				image.bi.biWidth = LOWORD(lParam);
				image.bi.biHeight = HIWORD(lParam) * Orientation;

#if defined (WIN32)
				hbm = CreateDIBSection (hdcImage, (BITMAPINFO far *)&image.bi, DIB_PAL_COLORS, &image.lpvData, NULL, 0);
#else
				hbm = WinGCreateBitmap(hdcImage,(BITMAPINFO far *)&image.bi, &image.lpvData);
#endif
				// Make sure that 'biSizeImage' reflects the
				// size of the bitmap data.
				image.bi.biSizeImage = (image.bi.biWidth * image.bi.biHeight);
				image.bi.biSizeImage *= Orientation;
				//  Store the old hbitmap to select back in before deleting
				gbmOldMonoBitmap = (HBITMAP)SelectObject(hdcImage, hbm);
			}

			PatBlt(hdcImage, 0,0,dxClient,dyClient, BLACKNESS);
	    break;

	case WM_COMMAND:
	    return AppCommand(hwnd, msg, wParam, lParam);

	case WM_PALETTECHANGED:
	    if ((HWND)wParam == hwnd) {
			break;
		}

	    // fall through to WM_QUERYNEWPALETTE

	case WM_QUERYNEWPALETTE:
	    hdc = GetDC(hwnd);

	    if (hpalApp) {
			SelectPalette(hdc, hpalApp, FALSE);
		}

	    uMappedColors = RealizePalette(hdc);
	    ReleaseDC(hwnd,hdc);

	    if (uMappedColors>0) {
			InvalidateRect(hwnd,NULL,TRUE);
			return TRUE;
		} else {
			return FALSE;
		}
		break;


	case WM_PAINT:
		hdc = BeginPaint(hwnd,&ps);
		SelectPalette(hdc, hpalApp, FALSE);
		RealizePalette(hdc);
		AppPaint (hwnd,hdc);
		EndPaint(hwnd,&ps);
		return 0L;
    
	case WM_DESTROY:
	    PostQuitMessage(0);
	    break;
    }
    
    return DefWindowProc(hwnd,msg,wParam,lParam);
}


/*----------------------------------------------------------------------------*\
|   AppPaint(hwnd, hdc)                                                        |
|                                                                              |
|   Description:                                                               |
|       The paint function.  Right now this does nothing.                      |
|                                                                              |
|   Arguments:                                                                 |
|       hwnd             window painting into                                  |
|       hdc              display context to paint to                           |
|                                                                              |
|   Returns:                                                                   |
|       nothing                                                                |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL AppPaint (HWND hwnd, HDC hdc)
{
	DWORD l;
	RECT rc;
	unsigned char ch=0;
#if defined (WIN32)
	BYTE *lpData;
#else
	BYTE __huge *lpData;
#endif

	// The data in our bitmap image is in 8bit chunks. Each element is an
	// index value to the colortable that is associated with the bitmap,
	// which, since we took pains to create an 'identity palette', is also
	// the same as the palette currently selected into this device context.

	if (image.lpIndex) {
		lpData = image.lpIndex;
		ch = rand()%256;
		for (l=0; l<image.bi.biSizeImage; l++) {
			// Now's the time to draw in some random static.
			//*lpData++ = rand()%256; // a random value [0..255]
			*lpData++ = ch++;
		}
	
		GetClientRect (hwnd, &rc); // Get the size of the window on the screen
		// And slam the image onto the screen:
#if defined (WIN32)
		BitBlt(hdc, 0, 0, rc.right-rc.left, rc.bottom-rc.top, hdcImage, 0, 0, SRCCOPY);
#else
		WinGBitBlt(hdc, 0, 0, rc.right-rc.left, rc.bottom-rc.top, hdcImage, 0, 0);
#endif
	}
	return TRUE;
}


/*----------------------------------------------------------------------------*\
|   AppCommand(hwnd, msg, wParam, lParam )                                     |
|                                                                              |
|   Description:                                                               |
|       handles WM_COMMAND messages for the main window (hwndApp)              |
|       of the parent window's messages.                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/
LONG AppCommand (HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	FARPROC fpAbout;
	int idItem;
	HWND hwndCtl;
	WORD wNotifyCode;

#if defined(WIN32)
	idItem = LOWORD(wParam);         // WIN32: control or menu item identifier
	hwndCtl = (HWND)lParam;          // WIN32: handle of control
	wNotifyCode = HIWORD(wParam);    // WIN32: notification message
#else
	idItem = wParam;                 // WIN16: control or menu item identifier
	hwndCtl = (HWND) LOWORD(lParam); // WIN16: handle of control
	wNotifyCode = HIWORD(lParam);    // WIN16: notification message
#endif		
	
    switch(idItem) {
		case MENU_ABOUT:
			fpAbout = MakeProcInstance ((FARPROC)AppAbout, hInstApp);
			DialogBox(hInstApp, szAppName, hwnd, (DLGPROC)fpAbout);
			FreeProcInstance (fpAbout);
			break;

		case MENU_EXIT:
		    PostMessage(hwnd,WM_CLOSE,0,0L);
		break;
    }
    return 0L; // returning '0' means we processed this message.
}


/*----------------------------------------------------------------------------*\
|   AppAbout( hDlg, uiMessage, wParam, lParam )                                |
|                                                                              |
|   Description:                                                               |
|       This function handles messages belonging to the "About" dialog box.    |
|       The only message that it looks for is WM_COMMAND, indicating the use   |
|       has pressed the "OK" button.  When this happens, it takes down         |
|       the dialog box.                                                        |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of about dialog window                   |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if message has been processed, else FALSE                         |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL FAR PASCAL AppAbout(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int idItem;
	HWND hwndCtl;
	WORD wNotifyCode;
	
    switch (msg) {
		case WM_INITDIALOG:
			// Here is where you could move the dialog around, change
			// some of the text that is going to be displayed, or other
			// things that you want to have happen right before the 
			// dialog is brought up.
			return TRUE;
		
		case WM_COMMAND:
			// A 'command' has been recieved by the dialog. Very few are
			// actually possible in this dialog. Usually just the 'OK' button
#if defined(WIN32)
			idItem = LOWORD(wParam);         // WIN32: control or menu item identifier
			hwndCtl = (HWND)lParam;          // WIN32: handle of control
			wNotifyCode = HIWORD(wParam);    // WIN32: notification message
#else
			idItem = wParam;                 // WIN16: control or menu item identifier
			hwndCtl = (HWND) LOWORD(lParam); // WIN16: handle of control
			wNotifyCode = HIWORD(lParam);    // WIN16: notification message
#endif		
		if (idItem == IDOK) {
				EndDialog(hwnd,TRUE);
		}
		break;

    }
    return FALSE;
}



/*----------------------------------------------------------------------------*\
|   AppExit()                                                                  |
|                                                                              |
|   Description:                                                               |
|       app is just about to exit, cleanup                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
void AppExit()
{
	// Clean up after ourselves...
	
	if (hdcImage) {
		// Remove our Device Context that we got from WinG:
		HBITMAP hbm;

		// Its not nice to delete a bitmap that is selected into a Device
		// Context, so lets swap in the original bitmap, which will return
		// to us our custom bitmap. You -did- remember to save the original
		// bitmap didn't you?
		hbm = (HBITMAP)SelectObject(hdcImage, gbmOldMonoBitmap);

		// Now we can delete the bitmap...
		DeleteObject(hbm);

		// ...and the Device Context
		DeleteDC(hdcImage);
	}

	if(hpalApp) {
		// And finally remove our Palette:
		DeleteObject(hpalApp);
	}
}

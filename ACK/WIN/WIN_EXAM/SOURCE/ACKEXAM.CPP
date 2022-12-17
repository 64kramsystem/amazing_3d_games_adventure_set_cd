// Example source for using ACK-3D in Windows
// (c) 1995 ACK Software (Lary Myers)
#define  STRICT
#include <windows.h>
#pragma hdrstop
#include <stdlib.h>
#include <string.h>
#include "wing.h"
#include "wingdll.h"
#include "ackwin.h"
#include "ackexam.h"

#define IDM_RES1    1000
#define IDM_RES2    1001
#define IDM_RES3    1002

extern "C" {
#include "keys.h"
#include "ack3d.h"
};

// Some globals to use
	 ACKENG      *ae = 0;        // Interface structure
	 HWND        hwndApp;
	 UCHAR       kbPressed[128];

	 WinGdll     WinG;

    header      BufferHeader;
    void        *pAckBuffer = 0;
    HDC         AckBuffer = 0;
    HPALETTE    hpalApp;
    RGBQUAD     ColorTable[256];
    pal         LogicalPalette = {0x300,256};
    HBITMAP     gbmOldMonoBitmap = 0;
    int         StretchFactor = 1;
    int         nBitmapW;
    int         nBitmapH;			//WinG bitmap dimensions.
    int         nWindowW;
    int         nWindowH;
    short       nMoveSpeed;
    short       nTurnSpeed;

    HINSTANCE	hInstGlobal;
    BOOL        bNoShow;        // Do not display if TRUE

//****************************************************************************
//
//****************************************************************************
char *FindParm(char *s)
{

while (*s)
    {
    if (*s == ' ' || *s == '\t')
        {
        while (*s == ' ' || *s == '\t')
            s++;
        return(s);
        }
    s++;
    }

return(NULL);
}

//****************************************************************************
//
//****************************************************************************
#pragma argsused
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
		    int nCmdShow )
{
    int     result;
	HBITMAP hbm;
    char    *szParms;

// Register our Window class if this is the first time in
if (!hPrevInstance)
    if (!Register(hInstance))
        return(-1);

// Prime the ACK-3D engine with our startup parameters
if (InitACKEngine())
    return(-1);

// Open our default resource file and load bitmaps, etc.
szParms = NULL;
if (*lpszCmdLine)
    szParms = FindParm(lpszCmdLine);

if (szParms != NULL)
    {
    if (*szParms)
        AckOpenResource(szParms);
    else
        AckOpenResource("pics.dtf");
    }
else
    AckOpenResource("pics.dtf");

ProcessInfoFile();
LoadBackDrop();
AckCloseResource();

// Create the actual window to display the rendered view
if (!CreateAll(hInstance))
    return(-1);

// Here we loop until the application closes
result = MessageLoop();

// Free up our buffers we used from WinG
if (AckBuffer)
    {
	hbm = (HBITMAP)SelectObject(AckBuffer, gbmOldMonoBitmap);
	DeleteObject(hbm);
	DeleteDC(AckBuffer);
	}

if(hpalApp)
	DeleteObject(hpalApp);

// Shutdown WinG DLL
WinG.Free();

return(result);
}

//****************************************************************************
//
//****************************************************************************
BOOL Register(HINSTANCE hInst)
{
    WNDCLASS wndclass;

wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
wndclass.lpfnWndProc   = AckWndProc;
wndclass.cbClsExtra    = 0;
wndclass.cbWndExtra    = 0;
wndclass.hInstance     = hInst;
wndclass.hIcon	       = LoadIcon(hInst,"ackicon");
wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
wndclass.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
wndclass.lpszMenuName  = NULL;
wndclass.lpszClassName = "ACK3DVIEW";

if (!RegisterClass(&wndclass))
    return(FALSE);

return(TRUE);
}

//****************************************************************************
//
//****************************************************************************
BOOL CreateAll(HINSTANCE hInst)
{
	int     windx, windy;
    HMENU   hMenu;

if (!WinG.Load())
    {
	MessageBox(0,"Can't find WING32.DLL","Error!",MB_OK);
	return(FALSE);
    }


nBitmapW = 319;			//default size (window client size to use).
nBitmapH = 199;

windx = windy = GetSystemMetrics(SM_CXBORDER)*2;
windx += nBitmapW;
windy += nBitmapH + GetSystemMetrics(SM_CYCAPTION) - 1;
nWindowW = windx;
nWindowH = windy;

// Note that in this example we're not allowing any resizing borders
// This helps to keep things alittle more constrained with the current
// ACK-3D engine.
hwndApp = CreateWindow ("ACK3DVIEW",    // Class name
		"ACK-3D Example",               // Caption
		WS_POPUP|WS_BORDER|WS_CAPTION|WS_VISIBLE|WS_SYSMENU,
		0,                      // Position
		0,
		windx,windy,		    // Size
		(HWND)NULL,             // Parent window
		(HMENU)NULL,            // use class menu
		hInst,                  // handle to window instance
	    (LPSTR)NULL );          // no params to pass on

if (!hwndApp)
    {
    MessageBox(NULL,"Unable to create 3D window","ERROR",MB_OK);
    return(FALSE);
    }

// Now we'll display the window and setup the WinG buffer when we
// receive the WM_SIZE message.
ShowWindow(hwndApp,SW_NORMAL);
hMenu = GetSystemMenu(hwndApp,FALSE);
AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
AppendMenu(hMenu,MF_STRING,IDM_RES1,"320 x 200");
AppendMenu(hMenu,MF_STRING,IDM_RES2,"480 x 300");
AppendMenu(hMenu,MF_STRING,IDM_RES3,"640 x 400");
return(TRUE);
}

//****************************************************************************
// Here is where we loop until the application terminates.
//****************************************************************************
int MessageLoop(void)
{
    MSG msg;

for (;;)
    {
    if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
        {
    	if (msg.message == WM_QUIT)
    	    break;

    	TranslateMessage(&msg);
    	DispatchMessage(&msg);
    	}
    else
    	{
	    DoCycle();  // Display the 3D window
    	}
    }

return msg.wParam;
}


//****************************************************************************
//
//****************************************************************************
#pragma argsused
void AckPaint(HWND hWnd,HDC hdc)
{

if (ae != NULL)
    {
    AckCheckObjectMovement();   // Check object animation
    AckBuildView();             // Render the 3D view
    }

// Let WinG display the buffer into our window. We can use a StretchFactor
// to display larger windows if so desired, at the cost of speed. The
// fastest way is to simply BitBlt the buffer into our window.
if (StretchFactor != 1)
	WinG.pStretchBlt(hdc,0,0,nWindowW,nWindowH, // Use the current window size
						AckBuffer,0,0,nBitmapW,nBitmapH);

else
	WinG.pBitBlt(hdc,0,0,nBitmapW,nBitmapH,AckBuffer,0,0);

}

//****************************************************************************
// Adjust the actual window size to account for borders and a caption bar
//****************************************************************************
void SetNewWindowSize(HWND hWnd)
{
    int     windx,windy;

windx = windy = GetSystemMetrics(SM_CXBORDER)*2;
nWindowW += windx;
nWindowH += windy + GetSystemMetrics(SM_CYCAPTION) - 1;
SetWindowPos(hWnd,NULL,0,0,nWindowW,nWindowH,SWP_NOMOVE|SWP_NOZORDER|SWP_NOCOPYBITS);
}

//****************************************************************************
// Message handler for the 3D window
//****************************************************************************
LRESULT CALLBACK _export AckWndProc(HWND hWnd,UINT iMessage,
                                    WPARAM wParam,LPARAM lParam)
{
	HDC	    hDC;
	PAINTSTRUCT ps;

switch (iMessage)
    {
    case WM_CREATE:
    	break;

    case WM_DESTROY:
    	PostQuitMessage(0);
    	break;

    case WM_SIZE:
    	AckDoSize(hWnd,lParam);
        break;

    case WM_PAINT:
    	BeginPaint(hWnd,&ps);
    	AckPaint(hWnd,ps.hdc);
    	EndPaint(hWnd,&ps);
    	break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    	ProcessKeys(iMessage,wParam);
    	break;

    case WM_RBUTTONDOWN:
    	AckCheckDoorOpen(ae->xPlayer,ae->yPlayer,ae->PlayerAngle);
    	break;

    case WM_LBUTTONDOWN:
    	ProcessMouse(hWnd,1,LOWORD(lParam),HIWORD(lParam));
    	SetCapture(hWnd);
    	break;

    case WM_MOUSEMOVE:
    	if (wParam & MK_LBUTTON)
    	    ProcessMouse(hWnd,1,LOWORD(lParam),HIWORD(lParam));
    	break;

    case WM_LBUTTONUP:
    	ProcessMouse(hWnd,0,LOWORD(lParam),HIWORD(lParam));
    	ReleaseCapture();
    	break;

	case WM_PALETTECHANGED:
	    if (hWnd == (HWND)wParam)
    		break;
	    // Fall through here
	case WM_QUERYNEWPALETTE:
	    hDC = GetDC(hWnd);
   	    if (hpalApp)
    		SelectPalette(hDC,hpalApp,FALSE);

	    RealizePalette(hDC);
	    ReleaseDC(hWnd,hDC);
	    return FALSE;

    case WM_SYSCOMMAND:
        switch (wParam)
            {
            case IDM_RES1:
                if (StretchFactor != 1)
                    {
                    StretchFactor = 1;
                    nWindowW = 320;
                    nWindowH = 200;
                    SetNewWindowSize(hWnd);
                    }
                break;

            case IDM_RES2:
                if (StretchFactor != 2)
                    {
                    StretchFactor = 2;
                    nWindowW = 480;
                    nWindowH = 300;
                    SetNewWindowSize(hWnd);
                    }
                break;

            case IDM_RES3:
                if (StretchFactor != 3)
                    {
                    StretchFactor = 3;
                    nWindowW = 640;
                    nWindowH = 400;
                    SetNewWindowSize(hWnd);
                    }
                break;

            default:
                break;
            }
    	return DefWindowProc( hWnd, iMessage, wParam, lParam );

    default:
    	return DefWindowProc( hWnd, iMessage, wParam, lParam );
    }

return 0;
}

//**** End of Source ****


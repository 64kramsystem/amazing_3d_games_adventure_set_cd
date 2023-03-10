//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// 3D Construction Kit
// Started: 01/02/94
//  Author: Lary Myers
//  Module: MOUSE.C
// (c) CopyRight 1994 All Rights Reserved
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ

#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

#ifdef __BORLANDC__
#define ACKREGS union		REGS
#define ACKINT(n,r)		int386(n,r,r)
#else
#define ACKREGS union		REGPACK
#define ACKINT(n,r)		intr(n,r)
#endif

	    short   MouseModifier;  // Used to adjust to 320x200 graphics

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Check if mouse is installed, returns -1 if it IS installed
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
int MouseInstalled(void)
{
    int	    yesno;
	 ACKREGS regs;


MouseModifier = 2;
memset(&regs,0,sizeof(ACKREGS));
ACKINT(0x33,&regs);
yesno = regs.w.ax;

return(yesno);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Show the mouse cursor
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void ShowMouse(void)
{
    ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 1;
ACKINT(0x33,&regs);

}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Hide the mouse cursor
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void HideMouse(void)
{
    ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 2;
ACKINT(0x33,&regs);

}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Returns button status, mouse row and column
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
int ReadMouseCursor(int *mrow,int *mcol)
{
    int	    bstatus;
    ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 3;
ACKINT(0x33,&regs);
bstatus = regs.w.bx;
*mrow = regs.w.dx;
*mcol = regs.w.cx >> 1;

return(bstatus);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Returns just the mouse button status
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
int ReadMouseButtons(void)
{
    int	    bstatus;
    ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 3;
ACKINT(0x33,&regs);
bstatus = regs.w.bx;

return(bstatus);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Set mouse cursor to desired row and column
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void SetMouseCursor(int mrow,int mcol)
{
    ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 4;
regs.w.dx = mrow;
regs.w.cx = mcol * MouseModifier;
ACKINT(0x33,&regs);

}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Defines left and right columns for mouse travel
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void SetMouseMinMaxColumns(int mincol,int maxcol)
{
    ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 7;
regs.w.cx = mincol * MouseModifier;
regs.w.dx = maxcol * MouseModifier;
ACKINT(0x33,&regs);

}


//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Defines top and bottom rows for mouse travel
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void SetMouseMinMaxRows(int minrow,int maxrow)
{
    ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 8;
regs.w.cx = minrow;
regs.w.dx = maxrow;
ACKINT(0x33,&regs);

}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Set shape of mouse cursor. 8 byte mask, hotspot row,col
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
#if !__BORLANDC__
void SetMouseShape(int hsrow,int hscol,char *mask)
{
	 ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 9;
regs.w.dx = FP_OFF(mask);
regs.w.es = FP_SEG(mask);
regs.w.bx = hscol;
regs.w.cx = hsrow;
ACKINT(0x33,&regs);
}
#endif

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Wait for the mouse button to be released
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void MouseReleased(void)
{

while (ReadMouseButtons());

}


//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
//
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
#if !__BORLANDC__
void MouseSetFunction(unsigned char mFlag,void *Func)
{
	 ACKREGS regs;

memset(&regs,0,sizeof(ACKREGS));
regs.w.ax = 12;
regs.w.dx = FP_OFF(Func);
regs.w.es = FP_SEG(Func);
regs.w.cx = mFlag;
ACKINT(0x33,&regs);
}
#endif


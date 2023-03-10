#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <mem.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys\stat.h>

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

typedef struct {
	int	sel;
	int	off;
	} SELOFF;

void AckGetIntVector(int VecNum,int *sel,int *off);
void AckSetIntVector(int VecNum,int sel,void *VecOff);
void AckKbdInt(void);
void AckTimerHandler(void);
void AckSetTextMode(void);

	long	AckMemUsed;
	short	AckDisplayErrors;
	SELOFF	OldKeybdInt;
	char	AckKeyboardSetup;
	SELOFF	OldTimerInt;
	char	AckTimerSetup;

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Establish a hook into interrupt 9 for keyboard handling
// The application can access which key is pressed by looking at the
// AckKeys global array
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
#ifndef _MSC_VER
void AckSetupKeyboard(void)
{
AckGetIntVector(9,&OldKeybdInt.sel,&OldKeybdInt.off);
AckSetIntVector(9,_CS,AckKbdInt);
AckKeyboardSetup = 1;
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Establish a hook into the user timer interrupt
// The application can access a counter by looking at the AckTimerCounter
// global variable.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void AckSetupTimer(void)
{
AckGetIntVector(0x1C,&OldTimerInt.sel,&OldTimerInt.off);
AckSetIntVector(0x1C,_CS,AckKbdInt);
AckTimerSetup = 1;
}
#endif

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Utility routine used to track memory usage by the ACK engine and
// applications.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void *AckMalloc(size_t mSize)
{
    UCHAR   *mBlock;

mSize += sizeof(long);
mSize++;
mBlock = malloc(mSize);

if (mBlock == NULL)
    {
    if (AckDisplayErrors)
	{
	AckSetTextMode();
	printf("\n\nOut of memory on call to AckMalloc.\n");
	printf("Memory used: %ld bytes.\n",AckMemUsed);
	}
    return(mBlock);
    }

(*(UCHAR *)mBlock) = 0xF2;
mBlock += 1;
(*(long *)mBlock) = mSize;
mBlock += sizeof(long);
AckMemUsed += mSize;

return(mBlock);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Matching routine for AckMalloc(). This routine MUST be used to free
// memory if AckMalloc() is used to allocate memory.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void AckFree(void *m)
{
    UCHAR   *mBlock;
    long    mSize;

mBlock = (UCHAR *)m;
mBlock -= sizeof(long);
mBlock -= 1;
if ((*(UCHAR *)mBlock) != 0xF2)
    {
    if (AckDisplayErrors)
	{
	AckSetTextMode();
	printf("\n\nCorrupt memory block in AckFree.\n");
	printf("Mem ptr: %p",mBlock);
	return;
	}
    }

mBlock += 1;
mSize = (*(long *)mBlock);
mBlock -= 1;
AckMemUsed -= mSize;
free(mBlock);

}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Read a palette from a file and immediately set it into the VGA regs.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
short AckLoadAndSetPalette(char *PalName)
{
    short     handle,ErrCode;
    char    *buf;

buf = AckMalloc(800);
if (buf == NULL)
    return(ERR_NOMEMORY);

ErrCode = 0;
if (!rsHandle)
    handle = _lopen(PalName,O_RDWR|O_BINARY);
else
    {
    handle = rsHandle;
    _llseek(handle,rbaTable[(ULONG)PalName],SEEK_SET);
    }

if (handle > 0)
    {
    read(handle,buf,768);
    if (!rsHandle)
	_lclose(handle);

    memset(buf,0,3);	    // Make sure color 0 is always black
    AckSetPalette(buf);
    }
else
    ErrCode = ERR_BADFILE;

AckFree(buf);
return(ErrCode);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Set up the palette range for light shading. The incoming ranges are in
// a 16 by 256 array where there are 16 different distance levels each
// having a full color tranlation table for light shading.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
void AckSetupPalRanges(ACKENG *ae,ColorRange *ranges)
{
	short	i,j,k,found;
	short	rangenos;
	UCHAR	plotcolor;

if (ae->LightFlag == SHADING_OFF)
    {
    for ( i = 0;i<16;i++)
	{
	for (j=0;j<256;j++)
	    {
	    ae->PalTable[j+(i*256)] = j;
	    }
	}
    return;
    }

for (rangenos = 0; rangenos < 64; rangenos++)
    {
    if (ranges[rangenos].start == 0)
	break;
    }

for ( i = 0;i<16;i++)
    {
    for (j=0;j<256;j++)
	{
	found = 0;
	// find the range the color is in.
	for ( k = 0; k < rangenos; k++ )
	    {
	    if (j >= ranges[k].start && j < ranges[k].start+ranges[k].length)
		{
		found = 1;
		break;
		}
	    }
	if (found)
	    {
//=============================================================================
//	 add color + i;
//	 if color + i > color+range then plot color = 0;
//	 otherwise plotcolor = color+i
//=============================================================================
	    if (j+i >= ranges[k].start+ranges[k].length)
		   plotcolor = 0;
	    else
		   plotcolor = j+i;
	    }
	else
	    {
	    plotcolor = j;
	    }
	ae->PalTable[j+(i*256)] = plotcolor;
	}
    }


}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Returns the index of the last object hit by the POV.
// The variable LastObjectHit can also be accessed globally.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
short AckGetObjectHit(void)
{
return(LastObjectHit);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Returns the map location of the last wall hit. LastMapPosn is a global
// variable.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
short AckGetWallHit(void)
{
return(LastMapPosn);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Sets the object to inactive. The memory used by the object is NOT
// freed by this routine.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
short AckDeleteObject(ACKENG *ae,short ObjIndex)
{

if (ae->ObjList[ObjIndex] == NULL)
    return(-1);

if (!ae->ObjList[ObjIndex]->Active)
    return(-1);

ae->ObjList[ObjIndex]->Active = 0;

return(0);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Sets a new wall or object index into the map array specified.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
short AckSetNewBitmap(short index,UCHAR **Maps,UCHAR *NewBitmap)
{
    UCHAR   *bPtr;

bPtr = Maps[index];
Maps[index] = NewBitmap;

if (bPtr != NULL)
    AckFree(bPtr);

return(0);
}

//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Obsolete routine for real mode. Flat model memory can be freed by the
// application. Real mode required XMS memory to be handled. This routine
// is maintained for backward compatability with the older versions.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
short AckFreeBitmap(UCHAR *bmType)
{
	short	  i;
	UCHAR *Bmp;

if (bmType != NULL)
    AckFree(bmType);

return(0);
}

// **** End of Source ****


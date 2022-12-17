/******************* ( Animation Construction Kit 3D ) ***********************/
/*			    Utility Routines				     */
/* CopyRight (c) 1993	   Author: Lary Myers				     */
/*****************************************************************************/

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

	long	AckMemUsed;
	short	AckDisplayErrors;

//=============================================================================
//
//=============================================================================
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

//=============================================================================
//
//=============================================================================
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

//=============================================================================
// This function reads a palette and sets the pal registers.
//=============================================================================
short AckLoadAndSetPalette(char *PalName)
{
    short     handle,ErrCode;
    char    *buf;

buf = AckMalloc(800);
if (buf == NULL)
    return(ERR_NOMEMORY);

ErrCode = 0;
if (!rsHandle)
    handle = open(PalName,O_RDWR|O_BINARY);
else
    {
    handle = rsHandle;
    lseek(handle,rbaTable[(ULONG)PalName],SEEK_SET);
    }

if (handle > 0)
    {
    read(handle,buf,768);
    if (!rsHandle)
	close(handle);

    memset(buf,0,3);	    // Make sure color 0 is always black
    AckSetPalette(buf);
    }
else
    ErrCode = ERR_BADFILE;

AckFree(buf);
return(ErrCode);
}


//=============================================================================
// This routine sets up a range of palette values that ACK will use for
// light shading.
//=============================================================================
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
	    // add color + i;
	    // if color + i > color+range then plot color = 0;
	    // otherwise plotcolor = color+i
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

//=============================================================================
// Returns the last object hit. LastObjectHit can also be referenced as a
// global variable.
//=============================================================================
short AckGetObjectHit(void)
{

return(LastObjectHit);
}

//=============================================================================
// Returns the last wall hit. LastMapPosn can also be referenced as a global
//=============================================================================
short AckGetWallHit(void)
{

return(LastMapPosn);
}


//=============================================================================
// This function merely sets an object to inactive. It does NOT actually
// delete the object from memory.
//=============================================================================
short AckDeleteObject(ACKENG *ae,short ObjIndex)
{

if (ae->ObjList[ObjIndex] == NULL)
    return(-1);

if (!ae->ObjList[ObjIndex]->Active)
    return(-1);

ae->ObjList[ObjIndex]->Active = 0;

return(0);
}

//=============================================================================
// This function sets a new wall or object bitmap and frees the old one.
//=============================================================================
short AckSetNewBitmap(short index,UCHAR **Maps,UCHAR *NewBitmap)
{
    UCHAR   *bPtr;

bPtr = Maps[index];
Maps[index] = NewBitmap;

if (bPtr != NULL)
    AckFree(bPtr);

return(0);
}

//=============================================================================
// This function frees the memory used by a bitmap. It does NOT remove the
// bitmap from the wall or object arrays.
// Note: This function is obsolete with flat model, the memory can just be
//	 freed by the application.
//=============================================================================
short AckFreeBitmap(UCHAR *bmType)
{
	short	  i;
	UCHAR *Bmp;

if (bmType != NULL)
    AckFree(bmType);

return(0);
}

//====== End of Code===========================================================


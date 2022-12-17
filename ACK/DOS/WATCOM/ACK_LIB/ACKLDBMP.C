/******************* ( Animation Construction Kit 3D ) ***********************/
/*			  Load Bitmap Routines				     */
/* CopyRight (c) 1993,94   Author: Lary Myers				     */
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

char *GetExtent(char *s);
UCHAR *AckReadiff(char *s);
short BlankSlice(short,UCHAR *);

//=============================================================================
// This function determines if the column of the bitmap contains all transparent
// colors or not. If it does it can be skipped during the drawing phase.
//=============================================================================
short BlankSlice(short col,UCHAR *bmp)
{
    short     i,pos;

pos = col * 64;
for (i = 0; i < 64; i++)
    {
    if (bmp[pos++])
	return(1);
    }

return(0);
}


//=============================================================================
// This function loads a bitmap image (wall or object) from either a separate
// file or a resource file. Currently only BBM and IMG formats are supported.
// Changes need to be made to allow GIF image support.
//=============================================================================
short AckLoadBitmap(ACKENG *ae,short BitmapNumber,short BitmapType,char *BitmapName)
{
    short    handle,bFlag;
    short    x,y,bLen;
    short    sPos,dPos;
    UCHAR    ch;
    UCHAR    *buf;
    UCHAR    *bmp;
    UCHAR    *bmpFlags;

bFlag = 0;

bLen = BITMAP_SIZE + BITMAP_WIDTH;
buf = NULL;

if (rsHandle || !(stricmp(GetExtent(BitmapName),"BBM")))
    {
    if (ae->bmLoadType == BMLOAD_BBM)
	buf = AckReadiff(BitmapName);

    if (ae->bmLoadType == BMLOAD_GIF)
	buf = AckReadgif(BitmapName);

    if (buf == NULL)
	return(ERR_LOADINGBITMAP);

    x = (*(short *)buf);
    y = (*(short *)&buf[2]);
    if ((x*y) != BITMAP_SIZE)
	{
	AckFree(buf);
	return(ERR_INVALIDFORM);
	}

    memmove(buf,&buf[4],BITMAP_SIZE);
    bFlag = 1;
    }
else
    {
    buf = AckMalloc(BITMAP_SIZE);
    if (buf == NULL)
	return(ERR_NOMEMORY);
    }

bmp = AckMalloc(bLen);
if (bmp == NULL)
    {
    AckFree(buf);
    return(ERR_NOMEMORY);
    }

if (BitmapType == TYPE_WALL)
    {
    ae->bMaps[BitmapNumber] = bmp;
    }

if (BitmapType == TYPE_OBJECT)
    {
    ae->oMaps[BitmapNumber] = bmp;
    }

if (!bFlag)
    {
    handle = open(BitmapName,O_RDWR|O_BINARY);
    if (handle < 1)
	{
	AckFree(buf);
	AckFree(bmp);
	return(ERR_BADFILE);
	}

    read(handle,buf,4);		/* Skip width and height for now */
    read(handle,buf,BITMAP_SIZE);
    close(handle);
    }

for (y = 0; y < BITMAP_HEIGHT; y++)
    {
    sPos = y;
    dPos = y * BITMAP_WIDTH;
    for (x = 0; x < BITMAP_WIDTH; x++)
	{
	ch = buf[sPos];
	bmp[dPos++] = ch;
	sPos += BITMAP_WIDTH;
	}
    }


bmpFlags = &bmp[BITMAP_SIZE];
memset(bmpFlags,0,BITMAP_WIDTH);

for (x = 0; x < BITMAP_WIDTH; x++)
    {
    if (!BlankSlice(x,bmp))
	bmpFlags[x] = 1;

    }

AckFree(buf);
return(0);
}


//=============================================================================
// This function returns a pointer to the extent portion of a filename.
//=============================================================================
char *GetExtent(char *s)
{
    char    *e;

e = strchr(s,'.');
if (e == NULL)
    return(s);
e++;

return(e);
}

//=============================================================================
// This function loads a wall type of bitmap
//=============================================================================
short AckLoadWall(ACKENG *ae,short WallNumber,char *bmFileName)
{

return( AckLoadBitmap(ae,WallNumber,TYPE_WALL,bmFileName) );
}

//=============================================================================
// This function loads an object type of bitmap
//=============================================================================
short AckLoadObject(ACKENG *ae,short BmpNumber,char *bmFileName)
{

return( AckLoadBitmap(ae,BmpNumber,TYPE_OBJECT,bmFileName) );
}

//=============================================================================
// This function creates an object structure prior to loading any bitmaps for
// the object.
//=============================================================================
short AckCreateObject(ACKENG *ae,short ObjNumber)
{

if (ae->ObjList[ObjNumber] == NULL)
    {
    ae->ObjList[ObjNumber] = (NEWOBJECT *)AckMalloc(sizeof(NEWOBJECT));

    if (ae->ObjList[ObjNumber] == NULL)
	return(ERR_NOMEMORY);

    memset(ae->ObjList[ObjNumber],0,sizeof(NEWOBJECT));
    }

if (ObjNumber >= ae->MaxObjects)
    ae->MaxObjects = ObjNumber + 1;

return(0);
}

//=============================================================================
// This function sets up an object for one of the predefined types of phases
// that an object may have. Moveable objects are also placed into a special
// list for later use by the drawing phase.
//=============================================================================
short AckSetObjectType(ACKENG *ae,short oNum,short oType)
{
    short   i,j,result = 0;
    OBJSEQ  *os;


switch (oType)
    {
    case NO_CREATE:
	os = &ae->ObjList[oNum]->Create;
	break;

    case NO_DESTROY:
	os = &ae->ObjList[oNum]->Destroy;
	break;

    case NO_WALK:
	os = &ae->ObjList[oNum]->Walk;
	break;

    case NO_ATTACK:
	os = &ae->ObjList[oNum]->Attack;
	break;

    case NO_INTERACT:
	os = &ae->ObjList[oNum]->Interact;
	break;

    default:
	result = ERR_BADOBJTYPE;
	break;
    }

if (!result)
    {
    ae->ObjList[oNum]->CurrentBitmaps = &os->bitmaps;
    ae->ObjList[oNum]->Flags = os->flags;
    ae->ObjList[oNum]->Sides = os->bmSides;
    ae->ObjList[oNum]->BitmapsPerView = os->bmBitmapsPerView;
    ae->ObjList[oNum]->CurrentBm = 0;
    ae->ObjList[oNum]->Maxbm = os->MaxBitmaps;
    ae->ObjList[oNum]->CurrentType = oType;
    ae->ObjList[oNum]->aFactor = os->AngleFactor;
    }

if (ae->ObjList[oNum]->Flags & OF_MOVEABLE)
    {
    j = 0;
    for (i = 0; i < MoveObjectCount; i++)
	{
	if (MoveObjectList[i] == oNum)
	    {
	    j = 1;
	    break;
	    }
	}

    if (!j)
	MoveObjectList[MoveObjectCount++] = oNum;

    i = (ae->ObjList[oNum]->y & 0xFFC0) + (ae->ObjList[oNum]->x >> 6);
    ObjGrid[i] = 0;
    }

return(result);
}

//=============================================================================
// This function passes the information in the OBJect SEQuence structure from
// the application to the object structure created in AckCreateObject().
//=============================================================================
short AckSetupObject(ACKENG *ae,short oNum,short oType,OBJSEQ *os)
{
    short   result = 0;

if (ae->ObjList[oNum] == NULL)
    return(ERR_BADOBJECTNUM);

if (os->flags & OF_MULTIVIEW)
    {
    os->AngleFactor = INT_ANGLE_360 / os->bmSides;
    }

switch (oType)
    {
    case NO_CREATE:
	memmove(&ae->ObjList[oNum]->Create,os,sizeof(OBJSEQ));
	break;

    case NO_DESTROY:
	memmove(&ae->ObjList[oNum]->Destroy,os,sizeof(OBJSEQ));
	break;

    case NO_WALK:
	memmove(&ae->ObjList[oNum]->Walk,os,sizeof(OBJSEQ));
	break;

    case NO_ATTACK:
	memmove(&ae->ObjList[oNum]->Attack,os,sizeof(OBJSEQ));
	break;

    case NO_INTERACT:
	memmove(&ae->ObjList[oNum]->Interact,os,sizeof(OBJSEQ));
	break;

    default:
	result = ERR_BADOBJTYPE;
	break;
    }

if (!result && ae->ObjList[oNum]->CurrentBitmaps == NULL)
    result = AckSetObjectType(ae,oNum,oType);

return(result);
}

//====== End of Code ==========================================================


/******************* ( Animation Construction Kit 3D ) ***********************/
/*			 Initialization Routines			     */
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


    short   *LowerTable[2048];
    short   tmpLowerValue[400];
    short   LowerLen[2048];

    long FloorCosTable[VIEW_WIDTH+1];

short AckBuildTables(ACKENG *ae);
void AckBuildHeightTables(ACKENG *ae);
void AckBuildGrid(ACKENG *ae);
void SetupFloors(ACKENG *ae);

//=============================================================================
// This function MUST be called before AckBuildView() and AckDisplayScreen().
// It allocates the required buffers and reads in the trig tables from a
// predefined resource file.
//=============================================================================
short AckInitialize(ACKENG *ae)
{
	short	  i,result = 0;
	short	  j;
	UCHAR	topcolor;

if (!ae->WinEndY ||
    !ae->WinEndX ||
    (ae->WinEndY - ae->WinStartY) < 10 ||
    (ae->WinEndX - ae->WinStartX) < 10)
    {
    return(ERR_BADWINDOWSIZE);
    }


result = AckBuildTables(ae);	// Read in TRIG.DAT and allocate tables
if (result)
    return(result);

ae->CenterRow	   = ae->WinStartY + ((ae->WinEndY - ae->WinStartY) / 2);
ae->CenterOffset   = ae->CenterRow * BYTES_PER_ROW;
ae->WinStartOffset = ae->WinStartY * BYTES_PER_ROW;
ae->WinLength	   = ((ae->WinEndY - ae->WinStartY)+1) * DWORDS_PER_ROW;
ae->WinWidth	   = (ae->WinEndX - ae->WinStartX) + 1;
ae->WinHeight	   = (ae->WinEndY - ae->WinStartY) + 1;

SetupFloors(ae);

AckBuildHeightTables(ae);   /* Build height and adjustment tables */

topcolor = ae->TopColor;
BackDropRows = 100;

for (i = 0; i < 640; i++)
    {
    BackArray[i] = AckMalloc(BackDropRows+1);

    if (BackArray[i] == NULL)
	return(ERR_NOMEMORY);

    memset(BackArray[i],topcolor,BackDropRows);
    }

return(result);
}

//=============================================================================
// This function opens a resource file specified by the application. The
// resource file can then be used to load bitmaps, ASCII files, etc.
//=============================================================================
short AckOpenResource(char *fName)
{
    ULONG   hLen;

if (rsHandle)
    close(rsHandle);

rsHandle = open(fName,O_RDONLY|O_BINARY);
if (rsHandle < 1)
    {
    rsHandle = 0;
    return(ERR_BADFILE);
    }

hLen = MAX_RBA * sizeof(long);

if (rbaTable == NULL)
    rbaTable = (ULONG *)AckMalloc(hLen);

if (rbaTable == NULL)
    {
    close(rsHandle);
    rsHandle = 0;
    return(ERR_NOMEMORY);
    }

if (read(rsHandle,(ULONG *)rbaTable,hLen) != hLen)
    {
    close(rsHandle);
    rsHandle = 0;
    AckFree(rbaTable);
    return(ERR_BADFILE);
    }

return(0);
}

//=============================================================================
// This function closes down a previously opened resource. Only one resource
// file can be open at a time.
//=============================================================================
void AckCloseResource(void)
{
if (rsHandle)
    close(rsHandle);

if (rbaTable != NULL)
    {
    AckFree(rbaTable);
    rbaTable = NULL;
    }

rsHandle = 0;
}

//=============================================================================
// This is an internal function called by AckBuildTables() to predefine height
// tables used by the wall drawing code.
//=============================================================================
void BuildWallDstTables(void)
{
    short	i,j,dst,row,HiValue;
    long	ldst,value,LowValue,len;
    short	*lp;

for (ldst = 10;ldst < 2048; ldst++)
    {
    HiValue = value = 0;
    row = 0;

    while (HiValue < 64 && row < 100)
	{
	HiValue = (value >> 8) & 0xFF;
	tmpLowerValue[row] = HiValue;
	row++;
	value += ldst;
	}

    LowerLen[ldst] = row;

    len = row * 2;
    j = 1;

    if (row == LowerLen[ldst-1])
	{
	j = 0;
	lp = LowerTable[ldst-1];

	for (i = 0; i < row; i++)
	    {
	    if (tmpLowerValue[i] != lp[i])
		{
		j = 1;
		break;
		}

	    }
	}

    if (j)
	{
	lp = AckMalloc(len);
	if (lp == NULL)
	    {
	    return;
	    }

	LowerTable[ldst] = lp;
	for (i = 0; i < row; i++)
	    lp[i] = tmpLowerValue[i];

	}
    else
	{
	LowerTable[ldst] = LowerTable[ldst-1];
	}
    }


}

//=============================================================================
// Internal function called by AckInitialize() to read in the trig tables
// and allocate memory for the various buffers.
//=============================================================================
short AckBuildTables(ACKENG *ae)
{
	short	  handle,len,ca,na;
	int	  c,s,ang;
	long	fAng,tu,tv;
	WALLARRAY   *wa,*waNext;
	SLICE	*sa,*saNext;

BuildWallDstTables();

if (!rsHandle)
    {
    handle = open("trig.dat",O_RDWR|O_BINARY);	// Does file exist
    if (handle < 1)
	return(ERR_BADFILE);
    }
else
    {
    handle = rsHandle;
    lseek(handle,rbaTable[0],SEEK_SET);
    }

LongTanTable	 = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
LongInvTanTable	 = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
CosTable	 = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
SinTable	 = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
LongCosTable	 = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
xNextTable	 = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
yNextTable	 = (long *)AckMalloc(sizeof(long) * INT_ANGLE_360);
ViewCosTable	 = (long *)AckMalloc(sizeof(long) * VIEW_WIDTH);
Grid		 = (unsigned short *)AckMalloc((GRID_MAX * 2)+1);
ObjGrid		 = (unsigned short *)AckMalloc((GRID_MAX * 2)+1);
AdjustTable	 = (long *)AckMalloc((MAX_DISTANCE+1) * sizeof(long));
ae->ScreenBuffer = (UCHAR *)AckMalloc(SCREEN_SIZE+640);
ae->BkgdBuffer	 = (UCHAR *)AckMalloc(SCREEN_SIZE+640);

if (LongTanTable     == NULL ||
    LongInvTanTable  == NULL ||
    CosTable	     == NULL ||
    SinTable	     == NULL ||
    LongCosTable     == NULL ||
    xNextTable	     == NULL ||
    yNextTable	     == NULL ||
    Grid	     == NULL ||
    ObjGrid	     == NULL ||
    AdjustTable	     == NULL ||
    ae->ScreenBuffer == NULL ||
    ae->BkgdBuffer   == NULL ||
    ViewCosTable     == NULL)
    {
    if (!rsHandle)
	close(handle);
    return(ERR_NOMEMORY);
    }

len = sizeof(long) * INT_ANGLE_360;

read(handle,SinTable,len);
read(handle,CosTable,len);
read(handle,LongTanTable,len);
read(handle,LongInvTanTable,len);
read(handle,InvCosTable,len);
read(handle,InvSinTable,len);
read(handle,LongCosTable,len);

if (!rsHandle)
    close(handle);

ca = INT_ANGLE_32;
na = -1;

for (len = 0; len < VIEW_WIDTH; len++)
    {
    ViewCosTable[len] = LongCosTable[ca];
    FloorCosTable[len] = InvCosTable[ca] >> 6;
    ca += na;
    if (ca <= 0)
	{
	ca = -ca;
	na = -na;
	}
    }

LongTanTable[INT_ANGLE_90] = LongTanTable[INT_ANGLE_90+1];
LongInvTanTable[INT_ANGLE_90] = LongInvTanTable[INT_ANGLE_90+1];
LongTanTable[INT_ANGLE_180] = LongTanTable[INT_ANGLE_180+1];
LongInvTanTable[INT_ANGLE_180] = LongInvTanTable[INT_ANGLE_180+1];
LongTanTable[INT_ANGLE_270] = LongTanTable[INT_ANGLE_270+1];
LongInvTanTable[INT_ANGLE_270] = LongInvTanTable[INT_ANGLE_270+1];


for (len = 0; len < INT_ANGLE_360; len++)
    {
    yNextTable[len] = (long)BITMAP_WIDTH * LongTanTable[len];
    xNextTable[len] = (long)BITMAP_WIDTH * LongInvTanTable[len];

    InvCosTable[len] = InvCosTable[len] >> 4;
    InvSinTable[len] = InvSinTable[len] >> 6;

    }

if (!rsHandle)
    close(handle);

for (len = 0; len < VIEW_WIDTH; len++)
    {
    sa = &Slice[len];
    memset(sa,0,sizeof(SLICE));

    for (ca = 0; ca < 8; ca++)
	{
	saNext = AckMalloc(sizeof(SLICE));
	if (saNext == NULL)
	    return(ERR_NOMEMORY);

	memset(saNext,0,sizeof(SLICE));
	sa->Next = saNext;
	saNext->Prev = sa;
	sa = saNext;
	}
    }

return(0);
}

#if 0
//=============================================================================
// This function loads in a map file and processes the grid.
//=============================================================================
short AckReadMapFile(ACKENG *ae,char *fName)
{
    short     len,handle,rdlen;

if (!rsHandle)
    {
    handle = open(fName,O_RDWR|O_BINARY);
    if (handle < 1)
	return(ERR_BADMAPFILE);
    }
else
    {
    handle = rsHandle;
    lseek(handle,rbaTable[(ULONG)fName],SEEK_SET);
    }

rdlen = GRID_MAX * 2;

len = read(handle,Grid,rdlen);

if (len == rdlen)
    len = read(handle,ObjGrid,rdlen);

read(handle,FloorMap,rdlen);
read(handle,CeilMap,rdlen);

if (!rsHandle)
    close(handle);

if (len != rdlen)
    return(ERR_READINGMAP);

AckBuildGrid(ae);	    // Build wall and object XY grids

return(0);
}
#endif

//=============================================================================
// This function loads in a map file and processes the grid.
//=============================================================================
short AckReadMapFile(ACKENG *ae,char *fName)
{
    short   len,handle,rdlen,count,i,pos;
    int	    mLen,aLen;
    UCHAR   buf[MAX_MULTI+2];
    UCHAR   *mPtr;

if (!rsHandle)
    {
    handle = open(fName,O_RDWR|O_BINARY);
    if (handle < 1)
	return(ERR_BADMAPFILE);
    }
else
    {
    handle = rsHandle;
    lseek(handle,rbaTable[(ULONG)fName],SEEK_SET);
    }

aLen = GRID_ARRAY * 2;
mLen = GRID_MAX * 2;

if (read(handle,Grid,mLen) != mLen)
    {
    if (!rsHandle)
	close(handle);

    return(ERR_READINGMAP);
    }

if (read(handle,ObjGrid,mLen) != mLen)
    {
    if (!rsHandle)
	close(handle);
    return(ERR_READINGMAP);
    }

if (read(handle,ae->xGrid,aLen) != aLen)
    {
    if (!rsHandle)
	close(handle);
    return(ERR_READINGMAP);
    }

if (read(handle,ae->yGrid,aLen) != aLen)
    {
    if (!rsHandle)
	close(handle);
    return(ERR_READINGMAP);
    }

if (read(handle,FloorMap,mLen) != mLen)
    {
    if (!rsHandle)
	close(handle);
    return(ERR_READINGMAP);
    }

if (read(handle,CeilMap,mLen) != mLen)
    {
    if (!rsHandle)
	close(handle);
    return(ERR_READINGMAP);
    }


read(handle,&count,2);

if (count)
    {
    for (i = 0; i < count;i++)
	{
	read(handle,&pos,2);
	mPtr = (UCHAR *)AckMalloc(MAX_MULTI+1);
	if (mPtr == NULL)
	    {
	    if (!rsHandle)
		close(handle);
	    return(ERR_NOMEMORY);
	    }

	ae->mxGrid[pos] = mPtr;
	ae->myGrid[pos] = mPtr;
	ae->mxGrid[pos+1] = mPtr;
	ae->myGrid[pos+GRID_WIDTH] = mPtr;
	read(handle,buf,MAX_MULTI);
	buf[MAX_MULTI] = '\0';
	len = strlen(buf);
	if (len > MAX_MULTI) len = MAX_MULTI;
	*mPtr = len;
	if (len)
	    memmove(&mPtr[1],buf,len);
	}
    }

if (!rsHandle)
    close(handle);

AckBuildGrid(ae);	    // Build object lists

return(0);
}

//=============================================================================
// Internal function called by AckInitialize().
// This function creates the height and distance tables used by objects for
// display purposes.
//=============================================================================
void AckBuildHeightTables(ACKENG *ae)
{
    short     i,x;
    short     result;
    long    height;

//height = BITMAP_WIDTH * 141L;
height = BITMAP_WIDTH * 128L;

DistanceTable[0] = MAX_HEIGHT;

//************* 64 * 65536 ************
AdjustTable[0] = 4194304L / height;

for (i = 1; i < MAX_DISTANCE; i++)
    {
    DistanceTable[i] = height / i;
    if (height - (DistanceTable[i] * i) > (i / 2))
	DistanceTable[i]++;

    if (DistanceTable[i] < MIN_HEIGHT)
	DistanceTable[i] = MIN_HEIGHT;

    if (DistanceTable[i] > MAX_HEIGHT)
	DistanceTable[i] = MAX_HEIGHT;

    AdjustTable[i] = 2097152L / DistanceTable[i];
    }

}

//=============================================================================
// Internal function called by AckReadMapFile().
// Processes the map file into the X and Y grids. Checks objects for MOVEABLE
// or not, etc.
//=============================================================================
void AckBuildGrid(ACKENG *ae)
{
	    short     i,j,CurIndex,pos,x1,y1;
	    UINT    MapCode,MapHiCode;

for (i = 0; i < MAX_DOORS; i++)
    {
    ae->Door[i].ColOffset = 0;
    ae->Door[i].mPos = ae->Door[i].mPos1 = -1;
    }


ae->SysFlags |= SYS_NO_WALLS;		// Assume no floating walls

#if 0
i = (GRID_WIDTH+1) * (GRID_HEIGHT+1);
memset(ae->xGrid,0,i);
memset(ae->yGrid,0,i);
#endif

CurIndex     = 1;
TotalSpecial = 0;
TotalSecret  = 0;

for (i = 0; i < GRID_HEIGHT; i++)
    {
    for (j = 0; j < GRID_WIDTH; j++)
	{
	pos	= (i * GRID_WIDTH) + j;



    #if 0
	MapCode = Grid[pos];

	if (MapCode == 0xF0 ||
	    MapCode == 0xF1)	    // Special turn right and left squares
	    continue;

	if (MapCode == MAP_STARTCODE)
	    {
	    ae->yPlayer = i * BITMAP_WIDTH;
	    ae->xPlayer = j * BITMAP_WIDTH;
	    ae->yPlayer += (BITMAP_WIDTH/2);
	    ae->xPlayer += (BITMAP_WIDTH/2);
	    continue;
	    }

	if (MapCode == MAP_UPCODE ||
	    MapCode == MAP_DOWNCODE ||
	    MapCode == MAP_GOALCODE)
	    {
	    SpecialCodes[TotalSpecial].mPos = pos;
	    SpecialCodes[TotalSpecial++].mCode = MapCode;
	    continue;
	    }

	if (MapCode)		    // Something is in map
	    {

	    if (MapCode & WALL_TYPE_UPPER)
		ae->SysFlags &= ~SYS_NO_WALLS;	    // There are floating walls

	    if ((MapCode & 0xFF) != DOOR_YCODE)
		{
		if (ae->xGrid[pos] != DOOR_SIDECODE)
		    ae->xGrid[pos] = MapCode;

		ae->xGrid[pos+1] = MapCode;
		}
	    else
		{
		MapHiCode = (MapCode & 0x0F00) | DOOR_SIDECODE;
		ae->xGrid[pos]	 = MapHiCode;
		ae->xGrid[pos+1] = MapHiCode;
		}

	    if ((MapCode & 0xFF) != DOOR_XCODE)
		{
		if (ae->yGrid[pos] != DOOR_SIDECODE)
		    ae->yGrid[pos] = MapCode;
		ae->yGrid[pos+GRID_WIDTH] = MapCode;
		}
	    else
		{
		MapHiCode = (MapCode & 0x0F00) | DOOR_SIDECODE;
		ae->yGrid[pos]		  = MapHiCode;
		ae->yGrid[pos+GRID_WIDTH] = MapHiCode;
		}
	    }
    #endif


	MapCode = ObjGrid[pos];
	if (MapCode)
	    {
	    CurIndex = MapCode & 0xFF;
	    if (CurIndex < MAX_OBJECTS)
		{
		if (ae->ObjList[CurIndex] == NULL)
		    {
		    ae->ObjList[CurIndex] = (NEWOBJECT *)AckMalloc(sizeof(NEWOBJECT));
		    if (ae->ObjList[CurIndex] != NULL)
			memset(ae->ObjList[CurIndex],0,sizeof(NEWOBJECT));
		    }

		if (ae->ObjList[CurIndex] != NULL)
		    {
		    x1 = (j * BITMAP_WIDTH) + (BITMAP_WIDTH/2);
		    y1 = (i * BITMAP_WIDTH) + (BITMAP_WIDTH/2);
		    ae->ObjList[CurIndex]->x	  = x1;
		    ae->ObjList[CurIndex]->y	  = y1;
		    ae->ObjList[CurIndex]->mPos	  = pos;
		    ae->ObjList[CurIndex]->Active = 1;
		    }
		}
	    }
	}
    }

}

//====== End of Code ==========================================================


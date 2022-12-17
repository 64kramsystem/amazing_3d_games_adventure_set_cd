/******************* ( Animation Construction Kit 3D ) ***********************/
/*			 Build Current Viewport				     */
/* CopyRight (c) 1993	   Author: Lary Myers				     */
/*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys\stat.h>
#include <limits.h>
//#include <math.h>

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

extern	long FloorCosTable[];

    void    (*FloorCeilRtn)(void);
    void    (*WallRtn)(void);
    void    (*WallMaskRtn)(void);

    short   tuFactor = 0;
    short   tvFactor = 4;
    long    kduFactor = -8192;
    long    kdvFactor = 0;
    long    kxFactor = 0;
    long    kyFactor = 0;

    short   gWinFullWidth;
    long    gWinDWORDS;
    long    gWinStartOffset;
    short   gWinStartX;
    short   gWinStartY;
    short   gWinEndX;
    short   gWinEndY;
    short   gWinHeight;
    short   gWinHalfHeight;
    short   gWinWidth;
    short   gCenterRow;
    short   gCenterOff;
    long    gBottomOff;
    UCHAR   *gScrnBufferCenter;
    UCHAR   *gScrnBuffer;
    UCHAR   *gBkgdBuffer;
    UCHAR   *gPalTable;
    short   gMultiWalls;

    UCHAR   **mxGridGlobal;
    UCHAR   **myGridGlobal;

    UCHAR   gTopColor;
    UCHAR   gBottomColor;

    UCHAR   *scVid;
    UCHAR   *scWall;
    UCHAR   *scPal;
    short   scdst;
    short   scwht;
    short   scmulti;
    short   sctopht;
    short   scbotht;
    short   scsavwht;
    short   scmulcnt;
    UCHAR   *scsavVid;
    UINT    scbNum;
    UCHAR   *scMulData;
    UCHAR   *scColumn;

    UCHAR   *gPtr;
    UCHAR   *gmPtr;
    short   gBitmapNumber;
    short   gBitmapColumn;
    short   gyBitmapNumber;
    short   gyBitmapColumn;
    long    gWallDistance;
    short   gmPos;
    DOORS   *gDoor;
    DOORS   *gDoorPosn;
    short   wFound;
    UCHAR   *mgPtr;

    short   BegX,EndX;

extern	long	    x_xPos;
extern	long	    x_yPos;
extern	long	    x_xNext;
extern	long	    x_yNext;
extern	long	    y_xPos;
extern	long	    y_yPos;
extern	long	    y_xNext;
extern	long	    y_yNext;

#define MAX_F_VIEWHALFHEIGHT   150

    short   LastVht;
    long    zdTable[VIEW_WIDTH][200];
    long    WallDistTable[VIEW_WIDTH];

void ShowTheWalls(void);
void FloorLoop(void);
void CeilLoop(void);
void DrawSolidCeiling(SOLIDS *s);
void DrawSolidFloor(SOLIDS *s);
void BuildSlice(void);
void BuildSliceMulti(void);
void DrawSlices(void);
void FindObject(void);
void xRaySetup(void);
UINT xRayCast(void);
void yRaySetup(void);
UINT yRayCast(void);
UINT xRayCastMulti(void);
UINT yRayCastMulti(void);
void CheckDoors(void);
short FindDoor(short MapPosn);
short BlankSlice(UINT col,UCHAR *bmp);
void AckDrawFloor(void);
void AckDrawFloorOnly(void);
void AckDrawCeilingOnly(void);
void AckDrawFloorNS(void);
void AckDrawFloorOnlyNS(void);
void AckDrawCeilingOnlyNS(void);
void AckDrawFloorHz(void);
void AckDrawOneFloor(void);
void ShowCol(void);
void ShowColMask(void);
void ShowColNS(void);
void ShowColMaskNS(void);
void ShowColLow(void);
void ShowColMaskLow(void);
void BuildSliceAsm(void);
void DrawFloorCeiling(void);

void BuildUpView(void);

void DrawSolidCeilAndFloor(void);
void DrawSolidCeilAndFloorNS(void);
void DrawSolidFloorAndCeil(void);
void DrawSolidFloorAndCeilNS(void);
void DrawSolidCeilSolidFloor(void);

int lines_intersect(long,long,long,long,long,long,long,long,long *,long *);

extern	UCHAR	*bmWall;

	long	NewHtTable[MAX_DISTANCE+1];



//=============================================================================
//
//=============================================================================
void zzzShowColNS(void)
{
    short   ht,x;
    long    pf,nVal;
    UCHAR   *pWall = scWall;
    UCHAR   *pVid = scVid;

ht = NewHtTable[bmDistance];
pf = bmDistance;
nVal = 0;

if (ht > 199)
    {
    nVal = pf * (ht - 100);
    ht = 199;
    }

x = ht >> 1;
pVid += ((x << 8) + (x << 6));
//pVid += (320 * (ht >> 1));

while (ht-- > 0)
    {
    *pVid = pWall[64 - (nVal >> 8)];
    pVid -= 320;
    nVal += pf;
    }
}

//=============================================================================
// This is a NEW function that sets up various global variables for the routines
// to use. Call this routine ONCE before going into the main display loop and
// then each time a new ACKENG structure is used, or the viewing window size
// changes or new floor\ceiling colors change, etc.
//=============================================================================
void AckRegisterStructure(ACKENG *ae)
{
    int	    mode,i;


for (i = 1; i < MAX_DISTANCE; i++)
    NewHtTable[i] = 16384 / i;

aeGlobal    = ae;
xGridGlobal = ae->xGrid;
yGridGlobal = ae->yGrid;
mxGridGlobal = ae->mxGrid;
myGridGlobal = ae->myGrid;
WallbMaps   = ae->bMaps;
gWinStartX  = ae->WinStartX;
gWinStartY  = ae->WinStartY;
gWinEndX    = ae->WinEndX;
gWinEndY    = ae->WinEndY;
gWinHeight  = ae->WinHeight;
gWinWidth   = ae->WinWidth;
gWinHalfHeight = (gWinEndY - (gWinHeight >> 1)) + 1;
gCenterRow  = ae->CenterRow;
gCenterOff  = ae->CenterOffset;
gScrnBuffer = ae->ScreenBuffer + 320;
gScrnBufferCenter = gScrnBuffer + gCenterOff;
gBkgdBuffer = ae->BkgdBuffer + 320;
gPalTable   = ae->PalTable;
gDoor	    = &ae->Door[0];
gTopColor   = ae->TopColor;
gBottomColor = ae->BottomColor;
LightFlag   = ae->LightFlag;

SysFlags    = ae->SysFlags;

mode = 0;				// Draw both floor and ceiling

if (SysFlags & SYS_SOLID_CEIL)
    {
    mode = 1;				// Draw Floor only
    if (SysFlags & SYS_SOLID_FLOOR)
	mode = 2;			// Draw solid floor and ceiling
    }

if (SysFlags & SYS_SOLID_FLOOR)
    {
    if (!mode)
	mode = 3;			// Draw Ceiling only

    }



if (!LightFlag)
    {
    WallRtn = ShowColNS;
    WallMaskRtn = ShowColMaskNS;
    switch (mode)
	{
	case 0:	    // Draw both floor and ceiling
	    if (ae->SysFlags & SYS_SINGLE_BMP)
		FloorCeilRtn = AckDrawOneFloor;
	    else
		FloorCeilRtn = AckDrawFloorHz;
	    break;
	case 1:	    // Draw solid ceiling and texture floor
	    FloorCeilRtn = DrawSolidCeilAndFloorNS;
	    break;
	case 2:	    // Draw both solid floor and solid ceiling
	    FloorCeilRtn = DrawSolidCeilSolidFloor;
	    break;
	case 3:	    // Draw solid floor and texture ceiling
	    FloorCeilRtn = DrawSolidFloorAndCeilNS;
	    break;
	default:
	    break;
	}
    }
else
    {
    WallRtn = ShowCol;
    WallMaskRtn = ShowColMask;
    switch (mode)
	{
	case 0:	    // Draw both floor and ceiling
	    FloorCeilRtn = AckDrawFloor;
	    break;
	case 1:	    // Draw solid ceiling and texture floor
	    FloorCeilRtn = DrawSolidCeilAndFloor;
	    break;
	case 2:	    // Draw both solid floor and solid ceiling
	    FloorCeilRtn = DrawSolidCeilSolidFloor;
	    break;
	case 3:	    // Draw solid floor and texture ceiling
	    FloorCeilRtn = DrawSolidFloorAndCeil;
	    break;
	default:
	    break;
	}
    }

gWinStartOffset = ae->WinStartOffset;
gBottomOff  = (gWinEndY * 320) + 319;

gWinFullWidth = 0;

if (gWinStartX == 0 && gWinEndX == 319)
    {
    gWinFullWidth = 1;
    gWinDWORDS = (gWinEndY - gWinStartY) * 80;
    }

gMultiWalls = 0;
for (i = 0; i < GRID_MAX; i++)
    {
    if ((xGridGlobal[i] & WALL_TYPE_MULTI) || (yGridGlobal[i] & WALL_TYPE_MULTI))
	{
	gMultiWalls = 1;
	break;
	}
    }

}


//=============================================================================
// This function builds the floor, ceiling, backdrop and walls into the
// screen buffer. Use AckDisplayScreen() to copy the buffer to the video.
//=============================================================================
void AckBuildView(void)
{

xPglobal    = aeGlobal->xPlayer;
xBegGlobal  = xPglobal & GRID_MASK;
xPglobalHI  = ((long)xPglobal << FP_SHIFT);
yPglobal    = aeGlobal->yPlayer;
yBegGlobal  = yPglobal & GRID_MASK;
yPglobalHI  = ((long)yPglobal << FP_SHIFT);
PlayerAngle = aeGlobal->PlayerAngle;
SysFlags    = aeGlobal->SysFlags;

BuildUpView();	// Assembly routine

}


//=============================================================================
//  Dummy proceedure when no wall is to be shown.
//=============================================================================
void ShowNone(void)
{
return;
}


//=============================================================================
// Internal routine called by AckBuildView()
// Casts the X and Y rays and builds the slice structure for each column of
// the viewing window.
//=============================================================================
void BuildSlice(void)
{
    short   j,index,wFound;
    long    xBitmap,yBitmap,BitmapNumber;
    short   DoorOpenColumn;
    ULONG   xDistance,yDistance;
    ULONG   xd,yd,mf;
    long    WallDistance,UnAdjustDist;
    long    distance,LightAdj;
    UINT    BitmapColumn,yBitmapColumn;
    long    OldMapPosn,OldMapPosn1;
    long    HoldAngle,HoldX,HoldY,xp1,yp1;
    long    offset;
    long    mPos;

    UINT    *gPtr;
    UCHAR   *mgPtr;
    SLICE   *spNext;

WallDistance = 3000000;	    /* Set to a ridiculous distance */
sPtr->Distance = 200;
wFound = 0;
sPtr->Fnc = ShowNone;

//if (ViewAngle != INT_ANGLE_90 && ViewAngle != INT_ANGLE_270)
//    {
    if ((BitmapNumber = xRayCast()) != 0)
	{
	wFound = 1;
	// Use the Y intercept to determine the wall column
	BitmapColumn = (LastY1 >> FP_SHIFT) & (BITMAP_WIDTH-1);

	// Keep the orientation the same no matter which side we're on
	if ((short)iLastX < xPglobal)
	    BitmapColumn = (BITMAP_WIDTH-1) - BitmapColumn;

	// Did we strike a door?
    //	if ((BitmapNumber & 0xFF) == DOOR_XCODE)
	if ((BitmapNumber & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT)))
	    {
	    index = FindDoor(xMapPosn);
	    if (index >= 0)			// This is a valid door
		{
		j = aeGlobal->Door[index].ColOffset;  // Get its current pos
		offset = 0;

	    if (BitmapNumber & DOOR_TYPE_SLIDE)
		{
		DoorOpenColumn = BITMAP_WIDTH-1;
		if ((short)iLastX > xPglobal)	    // Handle orientation
		    j = -j;
		BitmapColumn += j;		    // Adjust column to show
		}

	    if (BitmapNumber & DOOR_TYPE_SPLIT)
		{
		DoorOpenColumn = (BITMAP_WIDTH/2)-1;
		if (BitmapColumn < (BITMAP_WIDTH/2))
		    {
		    BitmapColumn += j;
		    if (BitmapColumn > (BITMAP_WIDTH/2)-1)
			offset = 1;
		    }
		else
		    {
		    BitmapColumn -= j;
		    if (BitmapColumn < (BITMAP_WIDTH/2))
			offset = 1;
		    }

		}


		if (offset == 1 || BitmapColumn > (BITMAP_WIDTH-1))
		    {
		    // Get the grid coordinates for this door
		    OldMapPosn = aeGlobal->Door[index].mPos;
		    OldMapPosn1 = aeGlobal->Door[index].mPos1;

		    // Fake the engine into thinking no door is there
		    xGridGlobal[OldMapPosn] = 0;
		    xGridGlobal[OldMapPosn1] = 0;

		    // Cast the ray to get walls beyond the door
		    BitmapNumber = xRayCast();

		    // Put back the door codes if not fully open
		    if (aeGlobal->Door[index].ColOffset < DoorOpenColumn)
			{
			xGridGlobal[OldMapPosn] = aeGlobal->Door[index].mCode;
			xGridGlobal[OldMapPosn1] = aeGlobal->Door[index].mCode1;
			}

		    // Calc the new bitmap column of wall behind door
		    BitmapColumn = (LastY1 >> FP_SHIFT) & (BITMAP_WIDTH-1);
		    if ((short)iLastX < xPglobal)
			BitmapColumn = (BITMAP_WIDTH-1) - BitmapColumn;
		    }
		}
	    }

	// Calculate the distance to the wall. Since the X position was
	// fixed to move 64 or -64 we can use it to determine the actual
	// wall distance. The InvCosTable values were stored with a fixed
	// point of 20 decimal places. At this time we'll knock off 14 of
	// them so we can later multiply with a fixed point value of 16

	xd = iLastX - xPglobal;
	mf = InvCosTable[ViewAngle];
	WallDistance = (xd * mf) >> 10;

	if (WallDistance > 33554432L)
	    WallDistance = 1200000L;

	gPtr = xGridGlobal;
	mgPtr = mxGridGlobal[xMapPosn];
	mPos = xMapPosn;

	}

//    }

// Don't cast a Y ray if its impossible to intercept any Y walls
//if (ViewAngle != 0 && ViewAngle != INT_ANGLE_180)
//    {
    if ((yBitmap = yRayCast()) != 0)
	{

	// Use the X intercept to determine the column of the bitmap
	yBitmapColumn = (LastX1 >> FP_SHIFT) & (BITMAP_WIDTH-1);

	// Handle orientation from either side of the wall
	if ((short)iLastY > yPglobal)
	    yBitmapColumn = (BITMAP_WIDTH-1) - yBitmapColumn;

	// Did we strike a door?
    //	if ((yBitmap & 0xFF) == DOOR_YCODE)
	if ((yBitmap & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT)))
	    {
	    index = FindDoor(yMapPosn);
	    if (index >= 0)		    // This is a valid door
		{
		// Get the current door column offset
		j = aeGlobal->Door[index].ColOffset;
		offset = 0;

	    // Deal with orientation
	    if (yBitmap & DOOR_TYPE_SLIDE)
		{
		DoorOpenColumn = BITMAP_WIDTH-1;
		if ((short)iLastY < yPglobal)
		    j = -j;
		yBitmapColumn += j;
		}

	    if (yBitmap & DOOR_TYPE_SPLIT)
		{
		DoorOpenColumn = (BITMAP_WIDTH/2)-1;
		if (yBitmapColumn < (BITMAP_WIDTH/2))
		    {
		    yBitmapColumn += j;
		    if (yBitmapColumn > (BITMAP_WIDTH/2)-1)
			offset = 1;
		    }
		else
		    {
		    yBitmapColumn -= j;
		    if (yBitmapColumn < (BITMAP_WIDTH/2))
			offset = 1;
		    }
		}

		// If beyond width of bitmap than cast again
		if (offset == 1 || yBitmapColumn > (BITMAP_WIDTH-1))
		    {

		    // Get the yGrid coordinates for this door
		    OldMapPosn = aeGlobal->Door[index].mPos;
		    OldMapPosn1 = aeGlobal->Door[index].mPos1;

		    // Fool the engine into thinking no door is there
		    yGridGlobal[OldMapPosn] = 0;
		    yGridGlobal[OldMapPosn1] = 0;

		    // Cast again for walls beyond the door
		    yBitmap = yRayCast();

		    // Put door code back if not fully open
		    if (aeGlobal->Door[index].ColOffset < DoorOpenColumn)
			{
			yGridGlobal[OldMapPosn] = aeGlobal->Door[index].mCode;
			yGridGlobal[OldMapPosn1] = aeGlobal->Door[index].mCode1;
			}

		    // Get the bitmap column of wall beyond door
		    yBitmapColumn = (LastX1 >> FP_SHIFT) & (BITMAP_WIDTH-1);
		    if ((short)iLastY > yPglobal)
			yBitmapColumn = (BITMAP_WIDTH-1) - yBitmapColumn;

		    }
		}
	    }


	// Calculate the distance to the wall. Since the Y position was
	// fixed to move 64 or -64 we can use it to determine the actual
	// wall distance. The InvSinTable values were stored with a fixed
	// point of 20 decimal places. At this time we'll knock off 14 of
	// them so we can later multiply with a fixed point value of 16

	yd = iLastY - yPglobal;
	mf = InvSinTable[ViewAngle];
	yDistance = (yd * mf) >> 8;

	if (yDistance > 33554432L)
	    yDistance = 120000L;

	// At this point check the distance to the Y wall against the X
	// wall to see which one is closer. The closer one is the one
	// we'll draw at this column of the screen.
	if (yDistance < WallDistance)
	    {
	    wFound = 1;
	    WallDistance = yDistance;
	    BitmapNumber = yBitmap;
	    BitmapColumn = yBitmapColumn;
	    gPtr = yGridGlobal;
	    mPos = yMapPosn;
	    mgPtr = myGridGlobal[mPos];

	    }
	}
//    }

if (wFound)
    {
    // To avoid a fishbowl affect we need to adjust the distance so
    // it appears perpendicular to the center point of the display
    // which is relative angle 0 from the players current angle. We
    // started at -30 degrees for the first screen column and will
    // cycle from -30 down to 0 then back up to +30 degrees. This
    // cosine value was pre-calculated and placed in ViewCosTable.
    UnAdjustDist = WallDistance >> 5;
    yd = ViewCosTable[ViewColumn] >> 3;
    WallDistance *= yd;

    // Now we strip off somemore decimal points and check round-up
    xd = WallDistance >> 12;
    if (WallDistance - (xd << 12) >= 2048)
	xd++;

    // The last decimal points from the multiplication after the X and
    // Y rays is stripped off and checked for round-up.
    WallDistance = xd >> 5;
    if (xd - (WallDistance << 5) >= 16)
	WallDistance++;

    // Don't really need to, but put it into an integer for fast compare
    distance = WallDistance;

    // This is an arbitrary minimum distance to look for
    if (distance < 10)
	distance = 10;

    // Don't want it to go outside our table boundaries
    if (distance >= MAX_DISTANCE)
	distance = MAX_DISTANCE - 1;

    // Save the wall data to display when done with entire screen
    sPtr->Distance = distance;
    sPtr->bNumber  = BitmapNumber;
    sPtr->bColumn  = BitmapColumn;
    sPtr->bMap	   = WallbMaps;
    sPtr->Active   = 0;
    sPtr->Type	   = ST_WALL;
    sPtr->Fnc	   = WallRtn;
    sPtr->mPos	   = mPos;
    sPtr->mPtr	   = mgPtr;
    spNext	   = sPtr->Next;

    if (distance > MaxDistance)
	MaxDistance = distance;

    if (CeilMap[mPos])
	LastWallHeight = 9000;

    if (((BitmapNumber & WALL_TYPE_UPPER) ||
	(BitmapNumber & WALL_TYPE_TRANS)) &&
	spNext != NULL)
	{
	BitmapColumn = gPtr[mPos];
	gPtr[mPos]   = 0;
	sPtr->Active = 1;

	if (BitmapNumber & WALL_TYPE_TRANS)
	    {
	    sPtr->Type = ST_OBJECT;
	    sPtr->Fnc  = WallMaskRtn;
	    }

	sPtr		= spNext;
	spNext->Active	= 0;
	spNext->bNumber = 0;

	BuildSlice();

	gPtr[mPos] = BitmapColumn;

	if (!sPtr->bNumber)
	    {
	    spNext = sPtr->Prev;
	    if (spNext != NULL)
		spNext->Active = 0;
	    }
	}
    else
	{
	if (UnAdjustDist < WallDistTable[ViewColumn])
	    WallDistTable[ViewColumn] = UnAdjustDist;
	}

    }
}

//=============================================================================
// Internal function called by AckBuildView().
// Continues the ray cast to determine if any multi-height walls are beyond
// the normal height walls and adds them to the wall slices. This will allow
// higher walls to appear over the top of normal walls.
//=============================================================================
void BuildSliceMulti(void)
{
    short   j,index,wFound;
    UINT    yHeight;
    UINT    xBitmap,yBitmap,BitmapNumber;
    short   DoorOpenColumn;
    short   xWallHeight,yWallHeight;
    ULONG   xDistance,yDistance;
    ULONG   xd,yd,mf;

    long    WallDistance;
    UINT    distance,LightAdj;
    UINT    BitmapColumn,yBitmapColumn;
    short   OldMapPosn,OldMapPosn1;
    short   HoldAngle,HoldX,HoldY,xp1,yp1;
    UINT    offset;
    short   mPos;
    UINT    *gPtr;
    UCHAR   *mgPtr;
    SLICE   *spNext;

WallDistance = 3000000;	    // Set to a ridiculous distance
wFound = 0;

if (ViewAngle != INT_ANGLE_90 && ViewAngle != INT_ANGLE_270)
    {
    if ((BitmapNumber = xRayCastMulti()) != 0)
	{
	xBitmap = BitmapNumber & 0xFF;
	if (((BitmapNumber & WALL_TYPE_MULTI) ||
	    (BitmapNumber & WALL_TYPE_UPPER)) &&
	    (xBitmap > LastWallHeight))
	    {
	//  LastWallHeight = xBitmap;
	    wFound = 1;
	    // Use the Y intercept to determine the wall column
	    BitmapColumn = (LastY1 >> FP_SHIFT) & (BITMAP_WIDTH-1);

	    // Keep the orientation the same no matter which side we're on
	    if ((short)iLastX < xPglobal)
		BitmapColumn = (BITMAP_WIDTH-1) - BitmapColumn;

	    xd = iLastX - xPglobal;
	    mf = InvCosTable[ViewAngle];
	    WallDistance = (xd * mf) >> 10;

	    if (WallDistance > 33554432L)
		WallDistance = 1200000L;

	    gPtr = xGridGlobal;
	    mPos = xMapPosn;
	    mgPtr = mxGridGlobal[mPos];
	    }
	}

    }

// Don't cast a Y ray if its impossible to intercept any Y walls
if (ViewAngle != 0 && ViewAngle != INT_ANGLE_180)
    {
    if ((yBitmap = yRayCastMulti()) != 0)
	{
	yHeight = yBitmap & 0xFF;
	if (((yBitmap & WALL_TYPE_MULTI) ||
	    (yBitmap & WALL_TYPE_UPPER)) &&
	    (yHeight > LastWallHeight))
	    {
	    yWallHeight = yHeight;

	    // Use the X intercept to determine the column of the bitmap
	    yBitmapColumn = (LastX1 >> FP_SHIFT) & (BITMAP_WIDTH-1);

	    // Handle orientation from either side of the wall
	    if ((short)iLastY > yPglobal)
		yBitmapColumn = (BITMAP_WIDTH-1) - yBitmapColumn;

	    yd = iLastY - yPglobal;
	    mf = InvSinTable[ViewAngle];
	    yDistance = (yd * mf) >> 8;

	    if (yDistance > 33554432L)
		yDistance = 120000L;

	    // At this point check the distance to the Y wall against the X
	    // wall to see which one is closer. The closer one is the one
	    // we'll draw at this column of the screen.
	    if (yDistance < WallDistance)
		{
		wFound = 1;
		WallDistance = yDistance;
		BitmapNumber = yBitmap;
		BitmapColumn = yBitmapColumn;
		gPtr = yGridGlobal;
		mPos = yMapPosn;
		xBitmap = yHeight;
		mgPtr = myGridGlobal[mPos];
		}
	    }
	}

    }

if (wFound)
    {
    LastWallHeight = xBitmap;
    yd = ViewCosTable[ViewColumn] >> 2;
    WallDistance *= yd;

    // Now we strip off somemore decimal points and check round-up
    xd = WallDistance >> 12;
    if (WallDistance - (xd << 12) >= 2048)
	xd++;

    // The last decimal points from the multiplication after the X and
    // Y rays is stripped off and checked for round-up.
    WallDistance = xd >> 5;
    if (xd - (WallDistance << 5) >= 16)
	WallDistance++;

    // Don't really need to, but put it into an integer for fast compare
    distance = WallDistance;

    // This is an arbitrary minimum distance to look for
    if (distance < 10)
	distance = 10;

    // Don't want it to go outside our table boundaries
    if (distance >= MAX_DISTANCE)
	distance = MAX_DISTANCE - 1;

    // Save the wall data to display when done with entire screen
    sPtr->Active = 1;
    sPtr	 = sPtr->Next;
    if (sPtr == NULL)
	return;

    sPtr->Distance = distance;
    sPtr->bNumber  = BitmapNumber;
    sPtr->bColumn  = BitmapColumn;
    sPtr->bMap	   = WallbMaps;
    sPtr->Active   = 0;
    sPtr->Type	   = ST_WALL;
    sPtr->Fnc	   = WallRtn;
    sPtr->mPos	   = mPos;
    sPtr->mPtr	   = mgPtr;
    spNext	   = sPtr->Next;

    if (spNext != NULL)
	{
//	BitmapColumn = gPtr[mPos];
//	gPtr[mPos]   = 0;

	BuildSliceMulti();

//	gPtr[mPos]   = BitmapColumn;
	}
    }

}


#if 0
/* return values */
#define DONT_INTERSECT 0
#define DO_INTERSECT   1
#define COLLINEAR      2
//=============================================================================
//
//=============================================================================
void BuildWallSlice(void)
{
    long    xBias,yBias,lx,ly,lx1,ly1,xp,yp,xp1,yp1;
    long    mx,my,mPos,xDist,yDist,x_xp,x_yp,y_xp,y_yp;
    long    wFound,xd,yd,xCol,yCol;
    UINT    mCodeX,mCodeY,BitmapNumber;
    short   mPosX,mPosY;
    UINT    *gPtr,*gPtrY;
    UCHAR   *mgPtr,*mgPtrY;
    SLICE   *spNext;

xp = xPglobal;
yp = yPglobal;
lx = xBegGlobal;
ly = yBegGlobal;
sPtr->Fnc = ShowNone;

if (ViewAngle > INT_ANGLE_270 || ViewAngle < INT_ANGLE_90)
    {
    xBias = 64;
    lx += 64;
    }
else
    xBias = -64;

if (ViewAngle < INT_ANGLE_180)
    {
    yBias = 64;
    ly += 64;
    }
else
    yBias = -64;

wFound = mCodeX = mCodeY = 0;
xDist = yDist = 30000000;

xp1 = xp + (MAX_DISTANCE * CosTable[ViewAngle]) >> 16;
yp1 = yp + (MAX_DISTANCE * SinTable[ViewAngle]) >> 16;

while (lx >= 0 && lx < 4097 &&
    lines_intersect(xp,yp,xp1,yp1,lx,0,lx,4096,&mx,&my) == DO_INTERSECT)
    {
    mPos = (my & 0xFFC0) + (mx >> 6);
    mCodeX = xGridGlobal[mPos];
    if (mCodeX)
	{
	xDist = ((mx-xp) * InvCosTable[ViewAngle]) >> 14;
	x_xp = mx;
	x_yp = my;
	xCol = my & 63;
	mPosX = mPos;
	gPtr = xGridGlobal;
	mgPtr = mxGridGlobal[mPos];
	wFound = 1;
	break;
	}
    lx += xBias;
    }

while (ly >= 0 && ly < 4097 &&
    lines_intersect(xp,yp,xp1,yp1,0,ly,4096,ly,&mx,&my) == DO_INTERSECT)
    {
    mPos = (my & 0xFFC0) + (mx >> 6);
    mCodeY = yGridGlobal[mPos];
    if (mCodeY)
	{
	yDist = ((my-yp) * InvSinTable[ViewAngle]) >> 8;
	y_xp = mx;
	y_yp = my;
	yCol = mx & 63;
	mPosY = mPos;
	gPtrY = yGridGlobal;
	mgPtrY = myGridGlobal[mPos];
	break;
	}
    ly += yBias;
    }


if (yDist < xDist)
    {
    xDist = yDist;
    x_xp = y_xp;
    x_yp = y_yp;
    mCodeX = mCodeY;
    xCol = yCol;
    mPosX = mPosY;
    gPtr = gPtrY;
    mgPtr = mgPtrY;
    wFound = 1;
    }

if (wFound)
    {
    // To avoid a fishbowl affect we need to adjust the distance so
    // it appears perpendicular to the center point of the display
    // which is relative angle 0 from the players current angle. We
    // started at -30 degrees for the first screen column and will
    // cycle from -30 down to 0 then back up to +30 degrees. This
    // cosine value was pre-calculated and placed in ViewCosTable.
    yd = ViewCosTable[ViewColumn] >> 2;
    xDist *= yd;

    // Now we strip off somemore decimal points and check round-up
    xd = xDist >> 12;
    if (xDist - (xd << 12) >= 2048)
	xd++;

    // The last decimal points from the multiplication after the X and
    // Y rays is stripped off and checked for round-up.
    xDist = xd >> 5;
    if (xd - (xDist << 5) >= 16)
	xDist++;

    // This is an arbitrary minimum distance to look for
    if (xDist < 10)
	xDist = 10;

    // Don't want it to go outside our table boundaries
    if (xDist >= MAX_DISTANCE)
	xDist = MAX_DISTANCE - 1;

    // Save the wall data to display when done with entire screen
    sPtr->Distance = xDist;
    sPtr->bNumber  = mCodeX;
    sPtr->bColumn  = xCol;
    sPtr->bMap	   = WallbMaps;
    sPtr->Active   = 0;
    sPtr->Type	   = ST_WALL;
    sPtr->Fnc	   = WallRtn;
    sPtr->mPos	   = mPosX;
    sPtr->mPtr	   = mgPtr;
    spNext	   = sPtr->Next;

    if (xDist > MaxDistance)
	MaxDistance = xDist;

    if (CeilMap[mPosX])
	LastWallHeight = 9000;

#if 0
    if (((BitmapNumber & WALL_TYPE_UPPER) ||
	(BitmapNumber & WALL_TYPE_TRANS)) &&
	spNext != NULL)
	{
	BitmapColumn = gPtr[mPosX];
	gPtr[mPos]   = 0;
	sPtr->Active = 1;

	if (BitmapNumber & WALL_TYPE_TRANS)
	    {
	    sPtr->Type = ST_OBJECT;
	    sPtr->Fnc  = WallMaskRtn;
	    }

	sPtr		= spNext;
	spNext->Active	= 0;
	spNext->bNumber = 0;

	BuildSlice();

	gPtr[mPos] = BitmapColumn;

	if (!sPtr->bNumber)
	    {
	    spNext = sPtr->Prev;
	    if (spNext != NULL)
		spNext->Active = 0;
	    }
	}
#endif

    }
}

//=============================================================================
//
//=============================================================================
void AckBuildView(void)
{
    int	    col;

xPglobal    = aeGlobal->xPlayer;
xBegGlobal  = xPglobal & GRID_MASK;
xPglobalHI  = ((long)xPglobal << FP_SHIFT);
yPglobal    = aeGlobal->yPlayer;
yBegGlobal  = yPglobal & GRID_MASK;
yPglobalHI  = ((long)yPglobal << FP_SHIFT);
PlayerAngle = aeGlobal->PlayerAngle;
SysFlags    = aeGlobal->SysFlags;

MaxDistance = 0;
CheckDoors();

ViewAngle = PlayerAngle - INT_ANGLE_32;
if (ViewAngle < 0)
    ViewAngle += INT_ANGLE_360;

for (ViewColumn = 0; ViewColumn < 320; ViewColumn++)
    {
    sPtr = &Slice[ViewColumn];

    BuildWallSlice();

    if (++ViewAngle >= INT_ANGLE_360)
	ViewAngle -= INT_ANGLE_360;
    }


FindObject();
AckDrawFloor();
DrawWalls();

}
#endif


    long    mFactor;
    long    dFactor;


//=============================================================================
// Internal function called by AckInitialize().
// Sets up the shading pointers for the floor\ceiling routines to use.
//=============================================================================
void SetupFloors(ACKENG *ae)
{
    short   i,a;
    int	    ht,scanline,ht1;
    int	    Scale_Fac;
    long    scan1,scan2,f;
    long    x,y,dist;
    long    Lastx,Lasty;

    for ( i=0; i<12;i++ ) scantables[i] = ae->PalTable + (7*256);
    for ( i=12;i<24;i++ ) scantables[i] = ae->PalTable + (6*256);
    for ( i=24;i<36;i++ ) scantables[i] = ae->PalTable + (5*256);
    for ( i=36;i<48;i++ ) scantables[i] = ae->PalTable + (4*256);
    for ( i=48;i<60;i++ ) scantables[i] = ae->PalTable + (3*256);
    for ( i=60;i<72;i++ ) scantables[i] = ae->PalTable + (2*256);
    for ( i=72;i<84;i++ ) scantables[i] = ae->PalTable + (1*256);
    for ( i=84;i<96;i++ ) scantables[i] = ae->PalTable;

Scale_Fac = (89 - ViewHeight) * 5;   // 295;

ht = 89 - ViewHeight;

ht *= Scale_Fac;

for (i = 0; i < VIEW_WIDTH; i++)
    {
    f = FloorCosTable[i];
    zdTable[i][0] = 0;
    for (scanline = 1; scanline < MAX_F_VIEWHALFHEIGHT; scanline++)
	{
	scan2 = ht / scanline;
	zdTable[i][scanline] = (f * scan2) >> 15;
//	if ((zdTable[i][scanline]-zdTable[i][scanline-1]) > 60)
//	    zdTable[i][scanline] = zdTable[i][scanline-1] + 60;
	if (zdTable[i][scanline-1] < zdTable[i][scanline])
	    zdTable[i][scanline-1] = zdTable[i][scanline];

	}
    }

//mFactor = 274944;
mFactor = 10368;
dFactor = 160;

}

#define DRAW_BACK	1
#define USE_SYMMETRY	0

#if USE_SYMMETRY
	long	xyPosns[300];
#endif

//=============================================================================
//
//=============================================================================
void AckDrawFloorHz(void)
{
    int	    i,col,row,ht,Rcol,EndCol,BegCol;
    UCHAR   *scr,*fscr,*bmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1;
    short   va,va1;
    long    LastDist;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    UCHAR   ch;
    UINT    bCode;

#if !(DRAW_BACK)
DrawBackDrop();
#endif

BegCol = gWinStartX;

#if USE_SYMMETRY
EndCol = gWinEndX >> 1;
#else
EndCol = gWinEndX;
#endif

scr = gScrnBufferCenter + BegCol;
va = PlayerAngle - INT_ANGLE_32;
if (va < 0) va += INT_ANGLE_360;

va += BegCol;
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

bcol = va % 640;

#if USE_SYMMETRY
Rbcol = bcol + 319;
if (Rbcol > 639) Rbcol -= 640;
#endif

ht = gWinHalfHeight; // (gWinEndY - (gWinHeight >> 1)) + 1;

scrCeil = scr - 1600;
scr += 1920;

#if USE_SYMMETRY
Rscr = scr + 318;
RscrCeil = scrCeil + 318;
cv = CosTable[PlayerAngle];
sv = SinTable[PlayerAngle];
zd = &zdTable[160][6];		// Get the distance to the center column

col = 0;			// Use this as an index into our table

// Calculate all the coordinates for the center column of the screen
// This will allow us to use symmetry later on.
for (row = 6; row <= ht; row++)
    {
    dist = *zd++;
    xyPosns[col++] = xPglobal + ((dist * cv) >> 16);
    xyPosns[col++] = yPglobal + ((dist * sv) >> 16);
    }
#endif

Rcol = 319;
wPtr = &WallDistTable[BegCol];		// Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];

for (col = BegCol; col < EndCol; col += 2)
    {
    cv = CosTable[va];
    sv = SinTable[va];
    fscr = scr;
    cscr = scrCeil;
    scr += 2;
    scrCeil += 2;

#if USE_SYMMETRY
    Rfscr = Rscr;
    Rcscr = RscrCeil;
    Rscr -= 2;
    RscrCeil -= 2;
    Rwdist = *RwPtr;
    RwPtr -= 2;
#endif

    zd = &zdTable[col][6];
    wdist = *wPtr;
    wPtr += 2;

#if DRAW_BACK
    ba = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba1 = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba += ht;
    ba1 += ht;

#if USE_SYMMETRY
    Rba = BackArray[Rbcol--];
    if (Rbcol < 0) Rbcol = 639;
    Rba1 = BackArray[Rbcol--];
    if (Rbcol < 0) Rbcol = 639;
    Rba += ht;
    Rba1 += ht;
    xyPtr = &xyPosns[0];
#endif

#endif

    LastDist = -1;

    for (row = 6; row <= ht; row++)
	{
	dist = *zd++;

#if USE_SYMMETRY
	xp = *xyPtr++;	    // Pick up the center column coordinates that
	yp = *xyPtr++;	    // we calculated above.
	if (dist < wdist || dist < Rwdist)
#else
	if (dist < wdist)
#endif
	    {
	    if (dist != LastDist)
		{
		x = xPglobal + ((cv * dist) >> 16);
		y = yPglobal + ((sv * dist) >> 16);
		LastDist = dist;
		}
	    mPos = (y & 0xFC0) + (x >> 6);	// Calc the Map Posn
	    bPos = (y & 63) + ((x & 63)<<6);	// Calc the Bitmap Pixel
    //	    bmp = WallbMaps[FloorMap[mPos]];	// Get the bitmap for the floor
    //	    ch = bmp[bPos];			// Get the pixel from the bitmap

    //	    ch = *(WallbMaps[FloorMap[mPos]]+bPos);

	    bCode = FloorMap[mPos];
	    bmp = WallbMaps[bCode];
	    ch = bmp[bPos];

	    *fscr = ch;				// Put it into two locations
	    fscr[1] = ch;			//  for low resolution

	    if ((bCode = CeilMap[mPos]))
		{
		bmp = WallbMaps[bCode];
		ch = bmp[bPos];
		*cscr = ch;
		cscr[1] = ch;
		}
#if DRAW_BACK
	    else
		{
		*cscr = *ba;
		cscr[1] = *ba1;
		}
#endif

#if USE_SYMMETRY
// Here is where we get the symmetrical coordinates for the other side of
// the screen. The two equations below add the inverse of the delta-x and
// delta-y to the center column coordinates to get the new coordinates. This
// avoids having to do 2 multiplies and shifts for the actual coordinates.

	    x = xp - (x - xp);
	    y = yp - (y - yp);


	    mPos = (y & 0xFC0) + (x >> 6);
	    bPos = (y & 63) + ((x & 63)<<6);
	    bmp = WallbMaps[FloorMap[mPos]];
	    ch = bmp[bPos];
	    *Rfscr = ch;
	    Rfscr[1] = ch;

	    bCode = CeilMap[mPos];
	    if (bCode)
		{
		bmp = WallbMaps[bCode];
		ch = bmp[bPos];
		*Rcscr = ch;
		Rcscr[1] = ch;
		}
    #if DRAW_BACK
	    else
		{
		*Rcscr = *Rba1;
		Rcscr[1] = *Rba;
		}
    #endif
#endif

	    }

	fscr += 320;	    // Advance our screen position for the floor
	cscr -= 320;	    // and the ceiling

#if USE_SYMMETRY
	Rfscr += 320;	    // Do the same for the right side of the screen
	Rcscr -= 320;
#endif

#if DRAW_BACK
	ba--;
	ba1--;

    #if USE_SYMMETRY
	Rba--;
	Rba1--;
    #endif

#endif
	}


#if USE_SYMMETRY
    Rcol -= 2;
#endif

    va += 2;
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}


//=============================================================================
//
//=============================================================================
void AckDrawOneFloor(void)
{
    int	    i,col,row,ht,Rcol,EndCol,BegCol;
    UCHAR   *scr,*fscr,*bmp,*scrCeil,*cscr;
    UCHAR   *Rscr,*Rfscr,*RscrCeil,*Rcscr;
    UCHAR   *ba,*ba1,*Rba,*Rba1,*cBmp;
    short   va,va1;
    long    LastDist;
    long    cv,sv,dist,x,y,bx,by,mPos,bPos,wdist,bcol;
    long    Rbcol,Rwdist,xp,yp;
    long    *zd,*wPtr,*RwPtr,*xyPtr;
    UCHAR   ch;
    UINT    bCode;

BegCol = gWinStartX;

EndCol = gWinEndX;

scr = gScrnBufferCenter + BegCol;
va = PlayerAngle - INT_ANGLE_32;
if (va < 0) va += INT_ANGLE_360;

va += BegCol;
if (va >= INT_ANGLE_360)
    va -= INT_ANGLE_360;

bcol = va % 640;

ht = gWinHalfHeight; // (gWinEndY - (gWinHeight >> 1)) + 1;

scrCeil = scr - 1600;
scr += 1920;

Rcol = 319;
wPtr = &WallDistTable[BegCol];		// Get pointers to avoid indexing
RwPtr = &WallDistTable[Rcol];
cBmp = WallbMaps[aeGlobal->CeilBitmap];
bmp = WallbMaps[aeGlobal->FloorBitmap];

for (col = BegCol; col < EndCol; col += 2)
    {
    cv = CosTable[va];
    sv = SinTable[va];
    fscr = scr;
    cscr = scrCeil;
    scr += 2;
    scrCeil += 2;

    zd = &zdTable[col][6];
    wdist = *wPtr;
    wPtr += 2;

    ba = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba1 = BackArray[bcol++];
    if (bcol > 639) bcol = 0;
    ba += ht;
    ba1 += ht;

    LastDist = -1;

    for (row = 6; row <= ht; row++)
	{
	dist = *zd++;

	if (dist < wdist)
	    {
	    if (dist != LastDist)
		{
		x = xPglobal + ((cv * dist) >> 16);
		y = yPglobal + ((sv * dist) >> 16);
		LastDist = dist;
		}
	    mPos = (y & 0xFC0) + (x >> 6);	// Calc the Map Posn
	    bPos = (y & 63) + ((x & 63)<<6);	// Calc the Bitmap Pixel
	//  bCode = FloorMap[mPos];
	//  bmp = WallbMaps[bCode];
	    ch = bmp[bPos];

	    *fscr = ch;				// Put it into two locations
	    fscr[1] = ch;			//  for low resolution


	    ch = cBmp[bPos];
	    *cscr = ch;
	    cscr[1] = ch;


	#if 0
	    if ((bCode = CeilMap[mPos]))
		{
		bmp = WallbMaps[bCode];
		ch = bmp[bPos];
		*cscr = ch;
		cscr[1] = ch;
		}
	    else
		{
		*cscr = *ba;
		cscr[1] = *ba1;
		}
	#endif

	    }

	fscr += 320;	    // Advance our screen position for the floor
	cscr -= 320;	    // and the ceiling
	ba--;
	ba1--;
	}

    va += 2;
    if (va >= INT_ANGLE_360) va -= INT_ANGLE_360;
    }

}


//=============================================================================
// Internal function called by FindObject(). The application may also use this
// function if it needs to find the angle between two points. DX and DY are the
// deltas between the two points. (ie. DX = X1 - X and DY = Y1 - Y)
//
//	Quadrants
//	  2 | 3		If the object is in quadrants 0 or 2 then we need
//	 ---+---	to add the resulting angle to the quad value less
//	  1 | 0		than the resulting angle. If the object is in
//			quadrants 1 or 3 then we need to subtract the
//			resulting angle from the next higher quadrant
//			value. This is because quads 1 and 3 are negative
//			values returned from arctan, while 0 and 2 are
//			positive.
//=============================================================================
short AckGetObjectAngle(long dx,long dy)
{
    short     i,quadrant,objAngle;
    short     Beg;
    long    avalue;

if (dx == 0 || dy == 0)
    {
    if (dx == 0)
	{
	if (dy < 0)
	    return(INT_ANGLE_270);
	return(INT_ANGLE_90);
	}

    if (dx < 0)
	return(INT_ANGLE_180);

    return(0);
    }

quadrant = 0;

if (dx < 0 && dy > 0)
    quadrant = INT_ANGLE_180;
else
    {
    if (dx < 0 && dy < 0)
	quadrant = INT_ANGLE_270;
    else
	{
	if (dx > 0 && dy < 0)
	    quadrant = INT_ANGLE_360;
	}
    }

// Get the absolute values to use for our ratio

if (dy < 0)
    dy = -dy;

if (dx < 0)
    dx = -dx;

//	The first thing is to get the delta Y into
// the same fixed point size as our tangent table, then divide by the
// delta X to get the objects ratio so we can determine the angle that
// the object is located. We then use the ratio to search the tan table
// and use the index into the table as our base angle (0-90 degrees).
// This angle is then adjusted based on what quadrant we are in.

dy = dy << FP_SHIFT;	// Make the dividend the same fixed point as the table

avalue = dy / dx;	// Get our ratio to search for

if (LongTanTable[INT_ANGLE_90-1] <= avalue)
    return(INT_ANGLE_90-1);

objAngle = 0;

// This is where a binary lookup may be faster. Currently searching the list
// sequentially to find the first value higher than our ratio.

Beg = 0;

if (LongTanTable[INT_ANGLE_45] < avalue)
    {
    if (LongTanTable[360] < avalue)	// Midpoint between 45 and 90
	Beg = 360;
    else
	Beg = INT_ANGLE_45;
    }

for (i = Beg; i < INT_ANGLE_90; i++)
    {
    if (LongTanTable[i] > avalue)
	{
	objAngle = i - 1;
	break;
	}

    }


if (objAngle < 0)
    objAngle = 0;


// Now we adjust the resulting angle based on the quadrant. If we are in
// quadrant 0 we do nothing, quadrants 1 and 3 we subtract the angle from
// the next higher quadrant angle, quadrant 2 we add the angle to the next
// lower quadrant angle to get the actual angle (0-1920) of the object.
if (quadrant)
    {
    if (quadrant != INT_ANGLE_270)
	objAngle = quadrant - objAngle;
    else
	objAngle += INT_ANGLE_180;
    }

return(objAngle);
}

//=============================================================================
// Internal function which returns a short square root of a long value passed
//=============================================================================
short near long_sqrt(long v)
{
    short	i;
    unsigned	short result,tmp;
    unsigned	long low,high;

if (v <= 1L) return((unsigned)v);

low = v;
high = 0L;
result = 0;

for (i = 0; i < 16; i++)
    {
    result += result;
    high = (high << 2) | ((low >>30) & 0x3);
    low <<= 2;

    tmp = result + result + 1;
    if (high >= tmp)
	{
	result++;
	high -= tmp;
	}
    }

return(result);
}

//=============================================================================
// Internal function called by AckBuildView().
// Checks the list of objects and combines them into the wall slice structures
// created in the BuildSlice() function above.
//=============================================================================
void FindObject(void)
{
    short   i,j,StartX,EndX;
    short   oCount;
    short   minAngle,maxAngle,objAngle;
    short   objColumn;
    UINT    distance;
    long    dx,dy;
    short   count,SaveCenter;
    short   ObjNum,oQuad,pQuad,numsides,afact;
    short   NewX,NewY,LightFlag;
    short   MaxOpp,Column,ColBeg,ColEnd;
    short   wt,ObjIndex;
    short   vidwt,vidht,hoff;
    short   MaxObjs;
    short   SliceLen;
    UINT    BmpColumn;
    long    xp,yp;
    short   wht;
    UCHAR   *wall,*ScreenBuffer;
    UCHAR   *pTable;
    UCHAR   **omaps;
    SLICE   *sa,*sa2,*saNext;
    UCHAR   *bmpFlags;
    NEWOBJECT **oList;
    NEWOBJECT	*oPtr;

if (FoundObjectCount)
    {
    oList = &aeGlobal->ObjList;

    StartX = gWinStartX;
    EndX = gWinEndX;

    minAngle = PlayerAngle - (INT_ANGLE_32 + 10);
    if (minAngle < 0)
	minAngle += INT_ANGLE_360;

    maxAngle = PlayerAngle + (INT_ANGLE_32 + 10);
    if (maxAngle >= INT_ANGLE_360)
	maxAngle -= INT_ANGLE_360;

    TotalObjects = 0;

    SliceLen = sizeof(SLICE) - 9;

    for (oCount = 0; oCount < FoundObjectCount; oCount++)
	{
	i = ObjectsSeen[oCount];
	oPtr = oList[i];

	if (oPtr == NULL)
	    continue;

	if (!oPtr->Active)
	    continue;

	dx = oPtr->x - xPglobal;
	dy = oPtr->y - yPglobal;

	if ((objAngle = AckGetObjectAngle(dx,dy)) < 0)
	    continue;

	if (minAngle > maxAngle)
	    {
	    if (objAngle >= minAngle)
		objColumn = objAngle - minAngle;
	    else
		objColumn = (objAngle+INT_ANGLE_360) - minAngle;
	    }
	else
	    {
	    objColumn = objAngle - minAngle;
	    }

	distance = long_sqrt((dx * dx) + (dy * dy));

#if 0
	if (objColumn >= 0 && objColumn < 320)
	    {
	    yp = ViewCosTable[objColumn] >> 2;
	    xp = distance * yp;

	    // Now we strip off somemore decimal points and check round-up
	    yp = xp >> 12;
	    if (xp - (yp << 12) >= 4096)
		yp++;

	    if (yp > 32L)
		distance = yp;
	    }
#endif

	if (distance >= MaxDistance)
	    continue;

    // Place the objects in the correct order so further ones are behind
	j = TotalObjects;
	if (j)
	    {
	    for (count = 0; count < TotalObjects; count++)
		{
		if (distance <= ObjRelDist[count])
		    {
		    for (j = TotalObjects; j > count; j--)
			{
			ObjRelDist[j] = ObjRelDist[j-1];
			ObjNumber[j]  = ObjNumber[j-1];
			ObjColumn[j]  = ObjColumn[j-1];
			ObjAngle[j]   = ObjAngle[j-1];
			}

		    j = count;
		    count = TotalObjects;
		    }
		}
	    }

    // Hold onto relavant data for the object found
	ObjNumber[j]  = i;
	ObjRelDist[j] = distance;
	ObjColumn[j]  = objColumn;
	ObjAngle[j] = objAngle;
	TotalObjects++;
	ObjRelDist[TotalObjects] = 0L;
	}

    // Didn't find any objects on the above pass, so we're done
    if (!TotalObjects)
	return;

    omaps = &aeGlobal->oMaps[0];
    pQuad = PlayerAngle / INT_ANGLE_45;

    for (i = 0; i < TotalObjects; i++)
	{
	ObjIndex = ObjNumber[i];
	oPtr = oList[ObjIndex];
	ObjNum = oPtr->CurrentBitmaps[oPtr->CurrentBm];

	distance = ObjRelDist[i];

	if (distance >= (MAX_DISTANCE - 10))
	    distance = MAX_DISTANCE-11;

	wt = DistanceTable[distance];

    /* Keep the width of the object reasonable */

	if (wt > 300)
	    continue;

	if (wt < 6) wt = 6;

	yp = AdjustTable[distance];
	xp = 0;			    // First col of the object to display

	NewX = ObjColumn[i];

	if (oPtr->Flags & OF_MULTIVIEW)
	    {
	    afact = oPtr->aFactor;
	    numsides = oPtr->Sides;
	    pQuad = ObjAngle[i] / afact;
	    oQuad = oPtr->Dir / afact;
	    j = (pQuad - oQuad) + (numsides >> 1);

	    if (j >= numsides)
		j -= numsides;

	    if (j < 0)
		j += numsides;

	    j *= oPtr->BitmapsPerView;
	    j += oPtr->CurrentBm;

	    ObjNum = oPtr->CurrentBitmaps[j];
	    }

	ColEnd = NewX + wt;
	wall = omaps[ObjNum];
	bmpFlags = &wall[BITMAP_SIZE];
    //	j = distance - 32;
	j = distance;

	for (Column = NewX - wt; Column < ColEnd; Column++)
	    {
	    if (Column >= StartX && Column <= EndX)
		{
		BmpColumn = xp >> FP_SHIFT;

		if (bmpFlags[BmpColumn])    // If transparent column
		    goto keepgoing;

		j = distance;
		dy = ViewCosTable[Column] >> 2;
		dx = distance * dy;

		// Now we strip off somemore decimal points and check round-up
		dy = dx >> 12;
		if (dx - (dy << 12) >= 4096)
		    dy++;

		if (dy > 32L)
		    j = dy;



		sa = &Slice[Column];

		if (sa->Active)
		    {
		    while (sa != NULL)
			{
			if (j <= sa->Distance)
			    {
			    sa2 = sa;
			    while (sa2->Next != NULL)
				sa2 = sa2->Next;

			    saNext = sa2->Prev;
			    while (sa2 != sa)
				{
				memmove(sa2,saNext,sizeof(SLICE)-9);
				sa2->Active = saNext->Active;
				sa2 = sa2->Prev;
				saNext = saNext->Prev;
				}

			    sa->Distance = distance;
			    sa->bNumber	 = ObjNum;
			    sa->bColumn	 = BmpColumn;
			    sa->bMap	 = omaps;
			    sa->Active	 = 1;
			    sa->Type	 = ST_OBJECT;
			    sa->Fnc	 = WallMaskRtn;
			    break;
			    }

			if (!sa->Active)
			    break;

			sa = sa->Next;
			}
		    }
		else
		    {
		    if (j <= sa->Distance)
			{
			sa->Active = 1;
			saNext = sa->Next;
			memmove(saNext,sa,sizeof(SLICE)-9);
			sa->Distance = distance;
			sa->bColumn  = BmpColumn;
			sa->bNumber  = ObjNum;
			sa->bMap     = omaps;
			sa->Type     = ST_OBJECT;
			sa->Fnc	     = WallMaskRtn;
			saNext->Active = 0;
			}
		    }
		}

    keepgoing:
	    xp += yp;	/* Advance the next column to display (scaling) */

	    }
	}
    }

}

//====== End of Code ==========================================================


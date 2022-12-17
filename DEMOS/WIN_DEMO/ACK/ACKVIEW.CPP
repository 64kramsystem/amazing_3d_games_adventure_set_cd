/******************* ( Animation Construction Kit 3D ) ******************/
/*			 Build Current View						     															*/
/* CopyRight (c) 1993	   Author: Lary Myers				     									*/
/* CopyRight (c) 1994/5	 John Lundy								     									*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"
#include "ackray.cpp"		//	Inline functions
#include "acksqr.cpp"		//	inline functions

extern int			ViewLowRes, ViewUpDown, ViewUDCenter;

//	Assembly language routines
void	draw_walls(void far *Walls[],
			UCHAR far *bMaps[],
			void far *pBmScrn[],
			UCHAR far *ScreenBuffer,
			UCHAR far *PalTable,
			int width,
			int height,
			int UpDown,
			int LightFlag);
int		draw_object(void far *Walls[],
			UCHAR far *bMap,
			char far *pBmScrn,
			UINT Offset,
			int Length,
			UCHAR far *ScreenBuffer,
			UCHAR far *PalTable,
			int width,
			int height,
			int UpDown,
			int LightFlag,
			int	LightAdj,
			int dist,
			int	first,
			int	last,
			long whole,
			long delta);
void	FloorSlice(int xPlayer,int yPlayer,int Column,
			FLOORARRAY far *xFloors,FLOORARRAY far *yFloors,
			long Cos,long Sin,long View,UCHAR far *ScreenBuffer,
			int width,int height,int UpDown,int UDCenter,int far *BS_Floors);
void	AckCopyBackground(UCHAR far *scrn,UCHAR far *bkgd,int wt,int ht,int len,int res);
long	long_divide(long denom,long divisor);

//	C Language routines
void	FloorSlice(ACKENG *ae,int xPlayer,int yPlayer,int ViewAngle,int Column);
void	OrderObjects(ACKENG *ae,int xPlayer,int yPlayer,int PlayerAngle);
void	FindObject(ACKENG *ae,int xPlayer,int yPlayer,int PlayerAngle);
int		FindDoor(ACKENG *ae,int MapPosn);
void	CheckDoors(ACKENG *ae);

/****************************************************************************
* This is the main routine the application should call to create the 3D			*
* view of the current POV. This routine will build all the walls and				*
* objects in the current view into the ScreenBuffer. No display to the			*
*	video is done by this routine. The application can determine when and			*
*	how actual display is to be done.																					*
****************************************************************************/
int AckBuildView(ACKENG *ae)
{
union
{	struct
	{	UCHAR	BitmapNumberLow;
		UCHAR	BitmapNumberHigh;
	};
	UINT	BitmapNumber;
};
union
{	struct
	{	UCHAR	yBitmapLow;
		UCHAR	yBitmapHigh;
	};
	UINT	yBitmap;
};
int	    i,j,index;
int			xPlayer,yPlayer,PlayerAngle;
int	    ViewAngle;
int	    DoorOpenColumn;
int			LightAdj;
int	    OldMapPosn,OldMapPosn1;

UINT    BitmapColumn,yBitmapColumn;
UINT    offset;

union
{	struct
	{	int		yDistanceLow;
		int		yDistanceHigh;
	};
	long    yDistance;
};
union
{	struct
	{	int		WallDistanceLow;
		int		WallDistanceHigh;
	};
	long    WallDistance;
};
long    xd,yd;

CheckDoors(ae);

/* Put in background */
AckCopyBackground(ScreenBuffer,BkgdBuffer,ViewWidth,ViewHeight,DEF_VIEW_WIDTH,ViewLowRes);

xPlayer = ae->ObjList[PLAYER_OBJECT].x;
yPlayer = ae->ObjList[PLAYER_OBJECT].y;
PlayerAngle = ae->ObjList[PLAYER_OBJECT].Dir;

/* Begin looking 30 degrees to the left of our current angle */
ViewAngle = PlayerAngle - INT_ANGLE_30;

if (ViewAngle < 0) ViewAngle += INT_ANGLE_360;

MaxDistance = 0;

/* Cast two rays for each column of the video display */
for (i = 0; i < ViewWidth; i++)
{
  WallDistance = 0x7fffffff;	/* Set to a ridiculous distance */

	xTiles = 0;
	/* Don't even cast an X ray if impossible to intersect the X walls */
  if (ViewAngle != INT_ANGLE_90 && ViewAngle != INT_ANGLE_270)
	{
		BitmapNumber = xVRay(ae->xGrid,xPlayer,yPlayer,ViewAngle);
		if (BitmapNumber)	/* A wall was found */
		{
    	/* Use the Y intercept to determine the wall column */
    	BitmapColumn = LastY1 & 0x3F;

    	/* Keep the orientation the same no matter which side we're on */
    	if (iLastX < xPlayer) BitmapColumn = 63 - BitmapColumn;

    	/* Did we strike a door? */
    	if (BitmapNumberLow >= DOOR_XCODE || BitmapNumber & WALL_SECRET_DOOR)
			{
				index = FindDoor(ae,xMapPosn);
				if (index >= 0)						/* This is a valid door */
	    	{
	    		j = ae->Door[index].ColOffset;  /* Get its current pos  */
	    		offset = 0;

					/* Handle orientation   */
					if (BitmapNumber & DOOR_TYPE_SLIDE || BitmapNumber & WALL_SECRET_DOOR)
	    		{
	    			DoorOpenColumn = 63;
	    			if (iLastX > xPlayer) j = -j;
	    			BitmapColumn += j;
	    		}
					else	//	DOOR_TYPE_SPLIT
	    		{
	    			DoorOpenColumn = 31;
	    			if (BitmapColumn < 32)
						{
							BitmapColumn += j;
							if (BitmapColumn > 31) offset = 1;
						}
	    			else
						{
							BitmapColumn -= j;
							if (BitmapColumn < 32) offset = 1;
						}
	    		}

	    		if (offset == 1 || BitmapColumn > 63)
					{
						/* Get the grid coordinates for this door */
						OldMapPosn = ae->Door[index].mPos;
						OldMapPosn1 = ae->Door[index].mPos1;

						/* Fake the engine into thinking no door is there */
						ae->xGrid[OldMapPosn] = 0;
						ae->xGrid[OldMapPosn1] = 0;

						xTiles = 0;
						/* Cast the ray to get walls beyond the door */
						BitmapNumber = xVRay(ae->xGrid,xPlayer,yPlayer,ViewAngle);

						/* Put back the door codes if not fully open */
						if (ae->Door[index].ColOffset < DoorOpenColumn)
		    		{
		    			ae->xGrid[OldMapPosn] = ae->Door[index].mCode;
		    			ae->xGrid[OldMapPosn1] = ae->Door[index].mCode1;
		    		}

						/* Calc the new bitmap column of wall behind door */
						BitmapColumn = LastY1 & 0x3F;
						if (iLastX < xPlayer) BitmapColumn = 63 - BitmapColumn;
					}
	    	}
			}

    	/* Calculate the distance to the wall. Since the X position was		*/
    	/* fixed to move 64 or -64 we can use it to determine the actual	*/
    	/* wall distance.																									*/
    	xd = iLastX - xPlayer;
    	WallDistance = xd * InvCosTable[ViewAngle];

			//	Adjust the light for some walls
    	if (ViewAngle < INT_ANGLE_90 || ViewAngle > INT_ANGLE_270)
   			LightAdj = ae->LightMap[xMapPosn-1];
   		else
   			LightAdj = ae->LightMap[xMapPosn];
    }
	}

	yTiles = 0;
	/* Don't cast a Y ray if its impossible to intercept any Y walls */
  if (ViewAngle != 0 && ViewAngle != INT_ANGLE_180)
	{
		yBitmap = yVRay(ae->yGrid,xPlayer,yPlayer,ViewAngle);
		if (yBitmap)	    /* A wall was found */
    {
    	/* Use the X intercept to determine the column of the bitmap */
    	yBitmapColumn = LastX1 & 0x3F;

    	/* Handle orientation from either side of the wall */
    	if (iLastY > yPlayer) yBitmapColumn = 63 - yBitmapColumn;

    	/* Did we strike a door? */
    	if (yBitmapLow >= DOOR_YCODE || yBitmap & WALL_SECRET_DOOR)
			{
				index = FindDoor(ae,yMapPosn);
				if (index >= 0)			/* This is a valid door */
	    	{
	    		/* Get the current door column offset */
	    		j = ae->Door[index].ColOffset;
	    		offset = 0;

	    		/* Deal with orientation */
					if (yBitmap & DOOR_TYPE_SLIDE || yBitmap & WALL_SECRET_DOOR)
	    		{
	    			DoorOpenColumn = 63;
	    			if (iLastY < yPlayer) j = -j;
	    			yBitmapColumn += j;
	    		}
					else	//	DOOR_TYPE_SPLIT
	    		{
	    			DoorOpenColumn = 31;
	    			if (yBitmapColumn < 32)
						{
							yBitmapColumn += j;
							if (yBitmapColumn > 31) offset = 1;
						}
	    			else
						{
							yBitmapColumn -= j;
							if (yBitmapColumn < 32) offset = 1;
						}
	    		}

	    		/* If beyond width of bitmap then cast again */
	    		if (offset == 1 || yBitmapColumn > 63)
					{
						/* Get the yGrid coordinates for this door */
						OldMapPosn = ae->Door[index].mPos;
						OldMapPosn1 = ae->Door[index].mPos1;

						/* Fool the engine into thinking no door is there */
						ae->yGrid[OldMapPosn] = 0;
						ae->yGrid[OldMapPosn1] = 0;

						yTiles = 0;
						/* Cast again for walls beyond the door */
						yBitmap = yVRay(ae->yGrid,xPlayer,yPlayer,ViewAngle);

						/* Put door code back if not fully open */
						if (ae->Door[index].ColOffset < DoorOpenColumn)
		    		{
		    			ae->yGrid[OldMapPosn] = ae->Door[index].mCode;
		    			ae->yGrid[OldMapPosn1] = ae->Door[index].mCode1;
		    		}

						/* Get the bitmap column of wall beyond door */
						yBitmapColumn = LastX1 & 0x3F;

			    	/* Handle orientation from either side of the wall */
						if (iLastY > yPlayer) yBitmapColumn = 63 - yBitmapColumn;
					}
	    	}
			}

    	/* Calculate the distance to the wall. Since the Y position was		*/
    	/* fixed to move 64 or -64 we can use it to determine the actual	*/
    	/* wall distance.																									*/
    	yd = iLastY - yPlayer;
    	yDistance = yd * InvSinTable[ViewAngle];

    	/* At this point check the distance to the Y wall against the X */
    	/* wall to see which one is closer. The closer one is the one   */
    	/* we'll draw at this column of the screen.                     */

    	/* When the two walls are at the same distance,	this causes a		*/
    	/* problem.  At this time, we choose the Y wall to display.			*/
    	if ((yDistanceHigh < WallDistanceHigh) ||
    			(yDistanceHigh == WallDistanceHigh &&
					yBitmapLow >= DOOR_SIDECODE))
			{
				WallDistance = yDistance;
				BitmapNumber = yBitmap;
				BitmapColumn = yBitmapColumn;

				//	Adjust the light for some walls
		    if (ViewAngle < INT_ANGLE_180)
					LightAdj = ae->LightMap[yMapPosn-GRID_WIDTH];
				else
					LightAdj = ae->LightMap[yMapPosn];
				ae->yGrid[yMapPosn] |= WALL_WAS_SEEN;	//	Update map
			}
			else
			{
				if (yDistanceHigh != WallDistanceHigh)
					ae->xGrid[xMapPosn] |= WALL_WAS_SEEN;	//	Update map
			}
    }
		else
			ae->xGrid[xMapPosn] |= WALL_WAS_SEEN;	//	Update map
	}
	else
		ae->xGrid[xMapPosn] |= WALL_WAS_SEEN;	//	Update map

	//	if (BitmapColumn < 64)	//	Will always be TRUE!	jcl
	{
		/* This is an arbitrary minimum distance to look for */
		if (WallDistanceHigh < 16) WallDistance = 1048576L;	//	16 << FP_SHIFT
		else
		/* Don't want it to go outside our table boundaries */
		if (WallDistanceHigh >= MAX_DISTANCE) WallDistanceHigh = MAX_DISTANCE - 1;

		if (WallDistanceHigh > MaxDistance) MaxDistance = WallDistanceHigh;

		/* To avoid a fishbowl affect we need to adjust the distance so */
		/* it appears perpendicular to the center point of the display	*/
		/* which is relative angle 0 from the players current angle. We */
		/* started at -30 degrees for the first screen column and will	*/
		/* cycle from -30 down to 0 then up to +30 degrees. This cosine	*/
		/* value was pre-calculated and placed in ViewCosTable.					*/
		WallDistance = WallDistanceHigh * ViewCosTable[i];

		/* Save the wall data to display when done with entire screen */
		Walls[i].Distance = WallDistanceHigh;
		Walls[i].Number = BitmapNumberLow;
		Walls[i].Column = BitmapColumn;
		Walls[i].LightAdj = LightAdj;
	}

	/* Draw a slice of the floor at this column.  At present, only an		*/
	/* outline of the floor tile is drawn.  Hopefully, later, bitmaps		*/
	/* will be drawn.  Gosh, I hope so!	 Time is at a premium, already.	*/

	FloorSlice(xPlayer,yPlayer,i,xFloors,yFloors,
		InvCosTable[ViewAngle],InvSinTable[ViewAngle],ViewCosTable[i],
		ScreenBuffer,BytesPerRow,ViewHeight,ViewUpDown,ViewUDCenter,BS_Floor);
/*
	FloorSlice(ae,xPlayer,yPlayer,ViewAngle,i);
*/
  ViewAngle += ViewLowRes + 1;
  if (ViewAngle >= INT_ANGLE_360) ViewAngle -= INT_ANGLE_360;
}

/* Draw the whole screen in assembly */
draw_walls((void far **)Walls,
			bMaps,
			(void far **)pBmScrn,
			ScreenBuffer,
			PalTable,
			BytesPerRow,
			ViewHeight,
			ViewUpDown,
			ae->LightFlag);

/* Now we look at any objects that may be closer than the walls */

/* Order the objects by distance */
OrderObjects(ae,xPlayer,yPlayer,PlayerAngle);

/* Display the objects that should be seen */
FindObject(ae,xPlayer,yPlayer,PlayerAngle);

return(0);
}

/****************************************************************************
*	Internal routine used to order objects by distance in the POV							*
****************************************************************************/
void OrderObjects(ACKENG *ae,int xPlayer,int yPlayer,int PlayerAngle)
{
int	    i,j,count;
int	    NewX,NewY;
union
{	struct
	{	int		xpLow;
		int	    xpHigh;
	};
	long	xp;
};
union
{	struct
	{	int		ypLow;
		int	    ypHigh;
	};
	long	yp;
};
union
{	struct
	{	int		distanceLow;
		int		distanceHigh;
	};
	long		distance;
};
long    SinValue,CosValue;

TotalObjects = 0;
ObjectOrder[0].Distance = 0L;
SinValue = SinTable[PlayerAngle];
CosValue = CosTable[PlayerAngle];

/* First thing we'll do is check all the objects to see which ones may be */
/* completely out of view, and to get some initial values for later...	  */
for (i = 1; i < ae->MaxObjects; i++)
{
  if (!ae->ObjList[i].Active) continue;

	/* Translate the object coordinates to make relative to the POV */
  NewX = ae->ObjList[i].x - xPlayer;
  NewY = ae->ObjList[i].y - yPlayer;

	/* Rotate coordinates to current player angle */
  xp = (NewX * CosValue) + (NewY * SinValue);
  if (xpHigh <= 0) continue;

  yp = (NewY * CosValue) - (NewX * SinValue);

	distance = long_sqrt(((long)xpHigh * (long)xpHigh) + ((long)ypHigh * (long)ypHigh));

	//	Must be closer than the farthest visible wall
	if (distance > MaxDistance) continue;	//	MAX_DISTANCE) continue;

	if (distance < 16L) distance = 16L;		//	arbitrary minimum distance

	/* Place the objects in the correct order; farther to nearer */
  j = TotalObjects;
  if (j)
	{
		for (count = 0 ; count < TotalObjects ; count++)
		{
	   	if (distance > ObjectOrder[count].Distance)
			{
				for (j = TotalObjects ; j > count ; j--)
		   		ObjectOrder[j] = ObjectOrder[j-1];
				break;
			}
		}
	}
	/* Hold onto relevent data for the object found */
	ObjectOrder[j].Number  = i;
	ObjectOrder[j].Distance = distance;
	ObjectOrder[j].DeltaX = xpHigh;
	ObjectOrder[j].DeltaY = ypHigh;
	TotalObjects++;
	ObjectOrder[TotalObjects].Distance = 0L;
}
}

/****************************************************************************
** Internal routine used to display objects that are visible in the POV			*
****************************************************************************/
void FindObject(ACKENG *ae,int xPlayer,int yPlayer,int PlayerAngle)
{
int	    i;
int	    ObjY,ObjNum;
int	    NewY;
int	    ColBeg,Column,ColEnd;
int	    wt,CenterColumn;
long    deltay,Tangent;
union
{	struct
	{	int		MaxOppLow;
		int		MaxOpp;
	};
	long	MaxOppLong;
};
union
{	struct
	{	int		xpLow;
		int		xpHigh;
	};
	long	xp;
};
union
{	struct
	{	int		ypLow;
		int		ypHigh;
	};
	long	yp;
};
union
{	struct
	{	int		distanceLow;
		int		distanceHigh;
	};
	long		distance;
};
OBJECTS far		*ObjectPtr;

/* Didn't find any objects on the above pass, so we're done */
if (!TotalObjects) return;

CenterColumn = ae->WinStartX + (ae->WinWidth / 2);
Tangent = LongTanTable[INT_ANGLE_30];

for (i = 0; i < TotalObjects; i++)
{
	ObjectPtr = &ae->ObjList[ObjectOrder[i].Number];

  ObjY = ObjectPtr->y;
  NewY = ObjY - yPlayer;

  yp = deltay = ObjectOrder[i].DeltaY;

  MaxOppLong = Tangent * ObjectOrder[i].DeltaX;

  if (NewY < ObjY)
	{
		MaxOppLong = -MaxOppLong;
		yp = -yp;
	}

  if ((yp + 32) < MaxOpp) continue;

  if (MaxOpp)
   	Column = CenterColumn - (int)long_divide(deltay * (long)CenterColumn,MaxOpp);
  else
   	Column = CenterColumn;

	if (Column < 0)
   	yp = ViewCosTable[0];
  else
  if (Column >= ViewWidth)
  	yp = ViewCosTable[ViewWidth - 1];
  else
   	yp = ViewCosTable[Column];

  distance = ObjectOrder[i].Distance * yp;
//	CAN NOT USE, BECAUSE MaxDistance HAS NOT BEEN CONVERTED WITH ViewCosTable!
//	if (distanceHigh >= MaxDistance) continue;	//	MAX_DISTANCE) continue;

  wt = DistanceTable[distanceHigh];
  if (!wt) continue;	//	(wt < 4) wt = 4;	//	16

  ColBeg = Column - wt;	//	First col of the object to display
  if (ColBeg >= ViewWidth) continue;

  yp = AdjustTable[distanceHigh];

	if (ColBeg < ae->WinStartX)
	{
		xp = (ae->WinStartX - ColBeg) * yp;
		if (xpHigh < 0 || xpHigh > 63) continue;
		ColBeg = ae->WinStartX;
	}
	else
		xp = 0;

  ColEnd = Column + wt;	//	Last col of the object to display
	if (ColEnd > ae->WinEndX) ColEnd = ae->WinEndX;
         
  if (ObjList[ObjectOrder[i].Number]->Sides)
		ObjNum = ObjList[ObjectOrder[i].Number]->bmNum[((PlayerAngle / ObjList[ObjectOrder[i].Number]->Sides) & 7)];
	else
   	ObjNum = ObjList[ObjectOrder[i].Number]->bmNum[ObjList[ObjectOrder[i].Number]->CurNum];

	/* raw whole bitmap in assembly */
	if (draw_object((void far **)Walls,
						oMaps[ObjNum],
						pBmScrn[distanceHigh].BS_Pointer,
						pBmScrn[distanceHigh].BS_Start,
						pBmScrn[distanceHigh].BS_Length,
						ScreenBuffer,
						PalTable,
						BytesPerRow,
						ViewHeight,
						ViewUpDown,
						(int)ae->LightFlag,
						(int)ae->LightMap[ObjectPtr->mPos],
						distanceHigh,
						ColBeg,
						ColEnd,
						xp,
						yp)) ObjectPtr->Flags |= OF_WAS_SEEN;

}
}

/****************************************************************************
* Draw a floor slice for the present column based upon data returned				*
* by the ray cast routines in the AckView routine														*
* Viewer angle from center of screen to bottom row is 18.75 degrees or 100	*
* screen units in high resolution mode (50 units in low resolution mode)		*
****************************************************************************/
void	FloorSlice(ACKENG *ae,int xPlayer,int yPlayer,int ViewAngle,int Column)
{
int			i, j;
UINT		offset;
long		xd, yd;
union
{	struct
	{	int		DistanceLow;
		int		DistanceHigh;
	};
	long	Distance;
};

for (i=0 ; i < xTiles ; i++)
{
	xd = xFloors[i].xPos - xPlayer;
	Distance = xd * InvCosTable[ViewAngle];
	if (DistanceHigh >= MAX_DISTANCE)
	{
		xTiles = i;
		break;	//	Everything else is farther away
	}
	if (DistanceHigh < 16) DistanceHigh = 16;
	Distance = DistanceHigh * ViewCosTable[Column];
	offset = BS_Floor[DistanceHigh];
	offset += (ViewUDCenter - ViewUpDown) / DistanceHigh;
	if ((int)offset >= ViewHeight) continue;
	offset = offset * BytesPerRow + Column;
	ScreenBuffer[offset] = 244;
}

for (j=0 ; j < yTiles; j++)
{
	yd = yFloors[j].yPos - yPlayer;
	Distance = yd * InvSinTable[ViewAngle];
	if (DistanceHigh >= MAX_DISTANCE)
	{
		yTiles = j;
		break;	//	Everything else is farther away
	}
	if (DistanceHigh < 16) DistanceHigh = 16;
	Distance = DistanceHigh * ViewCosTable[Column];
	offset = BS_Floor[DistanceHigh];
	offset += (ViewUDCenter - ViewUpDown) / DistanceHigh;
	if ((int)offset >= ViewHeight) continue;
	offset = offset * BytesPerRow + Column;
	ScreenBuffer[offset] = 244;
}
}

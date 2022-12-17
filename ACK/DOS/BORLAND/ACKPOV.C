
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

UINT xRay(void);
UINT yRay(void);
void xRaySetup(void);
void yRaySetup(void);
UINT xRayCast(void);
UINT yRayCast(void);
UINT xRayAsm(void);
UINT yRayAsm(void);

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Internal function called by AckMovePOV(). Checks the passed X and Y
// coordinates against the object coordinates.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckCheckObjPosn(short xPlayer,short yPlayer,short PlayerAngle,short oIndex)
{
    short     i,mPos,result,maxObj;
    short     j,count,SaveCenter;
    short     ObjX,ObjY,ObjNum;
    short     NewX,NewY,MapPosn;
    short     MaxOpp,Column,ColBeg,ColEnd;
    long    deltax,deltay;
    long    xp,yp,MinDistance,distance;
    long    SinValue,CosValue;
    NEWOBJECT **oList;
    NEWOBJECT	*oPtr;

result = POV_NOTHING;
MapPosn = (yPlayer & 0xFFC0) + (xPlayer >> 6);
maxObj = aeGlobal->MaxObjects;
oList = &aeGlobal->ObjList[0];

for (i = 0; i < maxObj; i++)
    {
    oPtr = oList[i];
    if (oPtr == NULL)
	continue;

    if (!oPtr->Active || oPtr->Flags & OF_PASSABLE)
	continue;

    if (MapPosn == oPtr->mPos && i != oIndex)
	{
	LastObjectHit = i;
	return(POV_OBJECT);
	}
    }

return(result);
}

/* return values */
#define DONT_INTERSECT 0
#define DO_INTERSECT   1
#define COLLINEAR      2


/* The SAME_SIGNS macro assumes arithmetic where the exclusive-or    */
/* operation will work on sign bits.  This works for twos-complement,*/
/* and most other machine arithmetic.				     */
#define SAME_SIGNS( a, b ) \
	(((long) ((unsigned long) a ^ (unsigned long) b)) >= 0 )


/* The use of some short working variables allows this code to run   */
/* faster on 16-bit computers, but is not essential.  It should not  */
/* affect operation on 32-bit computers.  The short working variables*/
/* to not restrict the range of valid input values, as these were    */
/* constrained in any case, due to algorithm restrictions.           */


int lines_intersect(x1,y1,x2,y2,x3,y3,x4,y4,x,y)
long x1,y1,x2,y2,x3,y3,x4,y4,*x,*y;
{

long Ax,Bx,Cx,Ay,By,Cy,d,e,f,temp,num,offset;
short x1lo,x1hi,x3lo,x3hi,y1lo,y1hi,y3lo,y3hi;

Ax = x2-x1;
Bx = x3-x4;

if(Ax<0) {                                              /* X bound box test*/
  x1lo=(short)x2; x1hi=(short)x1;
  } else {
  x1hi=(short)x2; x1lo=(short)x1;
  }
if(Bx>0) {
  if(x1hi < (short)x4 || (short)x3 < x1lo) return DONT_INTERSECT;
  } else {
  if(x1hi < (short)x3 || (short)x4 < x1lo) return DONT_INTERSECT;
  }

Ay = y2-y1;
By = y3-y4;

if(Ay<0) {						/* Y bound box test*/
  y1lo=(short)y2; y1hi=(short)y1;
  } else {
  y1hi=(short)y2; y1lo=(short)y1;
  }
if(By>0) {
  if(y1hi < (short)y4 || (short)y3 < y1lo) return DONT_INTERSECT;
  } else {
  if(y1hi < (short)y3 || (short)y4 < y1lo) return DONT_INTERSECT;
  }


Cx = x1-x3;
Cy = y1-y3;
d = By*Cx - Bx*Cy;					/* alpha numerator*/
f = Ay*Bx - Ax*By;                                      /* both denominator*/
if(f>0) {                                               /* alpha tests*/
  if(d<0 || d>f) return DONT_INTERSECT;
  } else {
  if(d>0 || d<f) return DONT_INTERSECT;
  }

e = Ax*Cy - Ay*Cx;					/* beta numerator*/
if(f>0) {						/* beta tests*/
  if(e<0 || e>f) return DONT_INTERSECT;
  } else {
  if(e>0 || e<f) return DONT_INTERSECT;
  }

/*compute intersection coordinates*/

if(f==0) return COLLINEAR;
num = d*Ax;                                             /* numerator */
offset = SAME_SIGNS(num,f) ? f/2 : -f/2;                /* round direction*/
*x = x1 + (num+offset) / f;                             /* intersection x */

num = d*Ay;
offset = SAME_SIGNS(num,f) ? f/2 : -f/2;
*y = y1 + (num+offset) / f;				/* intersection y */

return DO_INTERSECT;
}




UINT CheckMapPosn(short x,short y,short x1,short y1)
{
    long    bx,by,bx1,by1,dx,dy;
    short   mPos;
    UINT    result = 0;

bx = x & 0xFFC0;
by = y & 0xFFC0;
bx1 = bx + 64;
by1 = by + 64;

mPos = by + (x >> 6);

if (lines_intersect(x,y,x1,y1,bx,by,bx1,by,&dx,&dy) == DO_INTERSECT)
    result = yGridGlobal[mPos];
else
    if (lines_intersect(x,y,x1,y1,bx,by1,bx1,by1,&dx,&dy) == DO_INTERSECT)
	result = yGridGlobal[mPos+GRID_WIDTH];
    else
	if (lines_intersect(x,y,x1,y1,bx,by,bx,by1,&dx,&dy) == DO_INTERSECT)
	    result = xGridGlobal[mPos];
	else
	    if (lines_intersect(x,y,x1,y1,bx1,by,bx1,by1,&dx,&dy) == DO_INTERSECT)
		result = xGridGlobal[mPos+1];


if (result & (WALL_TYPE_UPPER+WALL_TYPE_PASS))
    result = 0;

return(result);
}

#define INT_ANGLE_15	(INT_ANGLE_1 * 15)

typedef struct {
    short   wdx;
    short   wdy;
    short   wdx1;
    short   wdy1;
    } WDIST;


void Dist2Wall(short x,short y,WDIST *w)
{
    short   mPos,xBeg,yBeg;
    UINT    mCode;

w->wdx = w->wdy = w->wdx1 = w->wdy1 = 4096;
xBeg = x & 0xFFC0;
yBeg = y & 0xFFC0;
mPos = yBeg + (x >> 6);

mCode = xGridGlobal[mPos];
if (mCode & WALL_TYPE_PASS) mCode = 0;
if (mCode)
    w->wdx = x - xBeg;

mCode = xGridGlobal[mPos+1];
if (mCode & WALL_TYPE_PASS) mCode = 0;
if (mCode)
    w->wdx1 = (xBeg+64) - x;

mCode = yGridGlobal[mPos];
if (mCode & WALL_TYPE_PASS) mCode = 0;
if (mCode)
    w->wdy = y - yBeg;

mCode = yGridGlobal[mPos+GRID_WIDTH];
if (mCode & WALL_TYPE_PASS) mCode = 0;
if (mCode)
    w->wdy1 = (yBeg+64) - y;

}

short HitWall(short xp,short yp,short xm,short ym)
{
    short   result = 0;
    short   mPos,xDist,yDist;
    UINT    mCode;

xDist = yDist = 9000;
ym &= 0xFFC0;
mPos = ym + (xm >> 6);
xm &= 0xFFC0;
mCode = xGridGlobal[mPos];
if (mCode & WALL_TYPE_PASS) mCode = 0;
if (mCode) xDist = abs(xm - xp);

mCode = yGridGlobal[mPos];
if (mCode & WALL_TYPE_PASS) mCode = 0;
if (mCode) yDist = abs(ym - yp);

if (xDist < 30)
    result = 1;
if (yDist < 30)
    result |= 2;

return(result);
}

UINT GetWallX(short mPos)
{
    UINT    mCode;

mCode = xGridGlobal[mPos];
if (mCode & WALL_TYPE_PASS)
    mCode = 0;

return(mCode);
}

UINT GetWallY(short mPos)
{
    UINT    mCode;

mCode = yGridGlobal[mPos];
if (mCode & WALL_TYPE_PASS)
    mCode = 0;

return(mCode);
}



//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Moves the POV based on Angle for Amount. After moving but prior to
// returning the position of the POV is check for collisions.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckMovePOV(short Angle,short Amount)
{
    short   x1,y1,HitResult;
    short   xp,yp;
    short   xLeft,xRight,yTop,yBottom;
    short   lx,ly,lx1,ly1,yAngle,xAngle;
    short   xKeep,yKeep;
    short   mPos;
    UINT    mCodeX,mCodeY,mCodeX1,mCodeY1;

HitResult = POV_NOTHING;
xp = aeGlobal->xPlayer;
yp = aeGlobal->yPlayer;

xLeft = xp & 0xFFC0;
yTop = yp & 0xFFC0;
xRight = xLeft + GRID_SIZE;
yBottom = yTop + GRID_SIZE;

x1 = xp + (long)((CosTable[Angle] * Amount) >> FP_SHIFT);
y1 = yp + (long)((SinTable[Angle] * Amount) >> FP_SHIFT);

mPos = yTop + (xp >> 6); /* Current Map Posn */

if (x1 < xp)				    /* Moving left */
    {
    if (GetWallX(mPos))			    /* Wall in current square */
	{
	if (x1 < xLeft || abs(x1-xLeft) < 28)	/* Crossed it or too close */
	    {
	    x1 = xp;
	    HitResult = POV_SLIDEX;
	    }
	}
    }

if (x1 > xp)				    /* Moving right */
    {
    if (GetWallX(mPos+1))		    /* Wall in current square */
	{
	if (x1 > xRight || abs(xRight-x1) < 28) /* Crossed it or too close */
	    {
	    x1 = xp;
	    HitResult = POV_SLIDEX;
	    }
	}
    }

if (y1 < yp)				    /* Moving up */
    {
    if (GetWallY(mPos))			    /*  Wall in current square */
	{
	if (y1 < yTop || abs(y1-yTop) < 28) /* Crossed it or too close */
	    {
	    y1 = yp;
	    HitResult = POV_SLIDEY;
	    }
	}
    }

if (y1 > yp)				    /* Moving down */
    {
    if (GetWallY(mPos+GRID_WIDTH))	    /* Wall in current square */
	{
	if (y1 > yBottom || abs(yBottom-y1) < 28) /* Crossed or too close */
	    {
	    y1 = yp;
	    HitResult = POV_SLIDEY;
	    }
	}
    }


if (!HitResult)				/* Nothing hit yet, look further */
    {
    if (y1 < (yTop+32))		       /* We are above upper half of square */
	{
	if (x1 < (xLeft+32))	       /* and on the left half of square */
	    {
	    mCodeX = GetWallX(mPos-GRID_WIDTH);
	    mCodeY = GetWallY(mPos-1);

	    if (mCodeX && y1 < (yTop+28))
		{
		if (x1 < (xLeft+28))
		    {
		    if (xp > (xLeft+27))
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    else
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    }
		}

	    if (mCodeY && x1 < (xLeft+28))
		{
		if (y1 < (yTop+28))
		    {
		    if (yp > (yTop+27))
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    else
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    }
		}
	    }

	if (x1 > (xRight-32) && !HitResult)	/* on right side of square */
	    {
	    mCodeX = GetWallX(mPos+1-GRID_WIDTH);
	    mCodeY = GetWallY(mPos+1);

	    if (mCodeX && y1 < (yTop+28))
		{
		if (x1 > (xRight-28))
		    {
		    if (xp < (xRight-27))
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    else
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    }
		}

	    if (mCodeY && x1 > (xRight-28))
		{
		if (y1 < (yTop+28))
		    {
		    if (yp > (yTop+27))
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    else
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    }
		}
	    }
	}

    if (y1 > (yTop+32) && !HitResult)	/* We are below upper half of square */
	{
	if (x1 < (xLeft+32))	       /* and on the left half of square */
	    {
	    mCodeX = GetWallX(mPos+GRID_WIDTH);
	    mCodeY = GetWallY(mPos-1+GRID_WIDTH);

	    if (mCodeX && y1 > (yBottom-28))
		{
		if (x1 < (xLeft+28))
		    {
		    if (xp > (xLeft+27))
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    else
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    }
		}

	    if (mCodeY && x1 < (xLeft+28))
		{
		if (y1 > (yBottom-28))
		    {
		    if (yp < (yBottom-27))
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    else
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    }
		}
	    }

	if (x1 > (xRight-32) && !HitResult)	/* on right side of square */
	    {
	    mCodeX = GetWallX(mPos+1+GRID_WIDTH);
	    mCodeY = GetWallY(mPos+1+GRID_WIDTH);

	    if (mCodeX && y1 > (yBottom-28))
		{
		if (x1 > (xRight-28))
		    {
		    if (xp < (xRight-27))
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    else
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    }
		}

	    if (mCodeY && x1 > (xRight-28))
		{
		if (y1 > (yBottom-28))
		    {
		    if (yp < (yBottom-27))
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    else
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    }
		}
	    }
	}
    }

if (AckCheckObjPosn(x1,y1,Angle,0))
    return(POV_OBJECT);

if (HitResult == POV_SLIDEX && y1 == yp)
    HitResult = POV_XWALL;

if (HitResult == POV_SLIDEY && x1 == xp)
    HitResult = POV_YWALL;


aeGlobal->xPlayer = x1;
aeGlobal->yPlayer = y1;

return(HitResult);
}

/*
   This function moves the current POV in direction Angle for Amount units.
*/
short xxxAckMovePOV(short Angle,short Amount)
{
    short   x1,y1,HitResult,NewAngle;
    short   xp,yp,xc,yc,CheckDist;
    UINT    mCode;
    UCHAR   gCode;
    short   MapPosn;

xp = aeGlobal->xPlayer;
yp = aeGlobal->yPlayer;
x1 = xp + (short)((CosTable[Angle] * Amount) >> FP_SHIFT);
y1 = yp + (short)((SinTable[Angle] * Amount) >> FP_SHIFT);

CheckDist = 48;
if (Amount > CheckDist) CheckDist = Amount;
xc = xp + (short)((CosTable[Angle] * CheckDist) >> FP_SHIFT);
yc = yp + (short)((SinTable[Angle] * CheckDist) >> FP_SHIFT);


HitResult = AckCheckHit(xp,yp,Angle);


if (!HitResult)
    {
    MapPosn = (y1 & 0xFFC0) + (x1 >> 6);

    if (AckCheckObjPosn(xc,yc,Angle,0))
	return(POV_OBJECT);

    mCode = CheckMapPosn(xp,yp,xc,yc);
    if (mCode > 0 && !(mCode & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT)))
	return(POV_XWALL);

    aeGlobal->xPlayer = x1;
    aeGlobal->yPlayer = y1;
    return(HitResult);
    }

if (HitResult == POV_OBJECT)
    return(HitResult);


if (HitResult == POV_XWALL)
    {
    xc = x1 = aeGlobal->xPlayer;
    if (Angle < INT_ANGLE_180)
	NewAngle = INT_ANGLE_90;
    else
	NewAngle = INT_ANGLE_270;

    HitResult = POV_SLIDEX;
    }
else
    {
    yc = y1 = aeGlobal->yPlayer;
    if (Angle > INT_ANGLE_270 || Angle < INT_ANGLE_90)
	NewAngle = 0;
    else
	NewAngle = INT_ANGLE_180;

    HitResult = POV_SLIDEY;
    }


if (!AckCheckHit(aeGlobal->xPlayer,aeGlobal->yPlayer,NewAngle))
    {
    MapPosn = (y1 & 0xFFC0) + (x1 >> 6);

    if (AckCheckObjPosn(xc,yc,Angle,0))
	return(POV_OBJECT);

    mCode = CheckMapPosn(xp,yp,xc,yc);
    if (mCode > 0 && !(mCode & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT)))
	return(POV_XWALL);

    aeGlobal->xPlayer = x1;
    aeGlobal->yPlayer = y1;
    return(POV_NOTHING);
    }

return(HitResult);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Moves an object based on Angle and Amount then checks for collision
// with other objects AND the POV.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckMoveObjectPOV(short ObjIndex,short Angle,short Amount)
{
    short   xp,yp,x1,y1,HitResult,NewAngle,oNum;
    UINT    mCodeX,mCodeY;
    short   xLeft,xRight,yTop,yBottom,mPos;
    short   MapPosn,PlayerPosn;
    NEWOBJECT **oList;
    NEWOBJECT	*oPtr;

oList = &aeGlobal->ObjList[0];
oPtr = oList[ObjIndex];

if (oPtr == NULL)
    return(0);

xp = oPtr->x;
yp = oPtr->y;
x1 = xp + (short)((CosTable[Angle] * Amount) >> FP_SHIFT);
y1 = yp + (short)((SinTable[Angle] * Amount) >> FP_SHIFT);

xLeft = xp & 0xFFC0;
xRight = xLeft + GRID_SIZE - 1;
yTop = yp & 0xFFC0;
yBottom = yTop + GRID_SIZE - 1;
mPos = yTop + (xp >> 6);

MapPosn = (y1 & 0xFFC0) + (x1 >> 6);

oNum = AckCheckObjPosn(x1,y1,Angle,ObjIndex);
if (oNum > 0)
    return(POV_OBJECT);

HitResult = POV_NOTHING;

if (x1 < xp)				    /* Moving left */
    {
    if (GetWallX(mPos))			    /* Wall in current square */
	{
	if (x1 < xLeft || abs(x1-xLeft) < 28)	/* Crossed it or too close */
	    {
	    x1 = xp;
	    HitResult = POV_SLIDEX;
	    }
	}
    }

if (x1 > xp)				    /* Moving right */
    {
    if (GetWallX(mPos+1))		    /* Wall in current square */
	{
	if (x1 > xRight || abs(xRight-x1) < 28) /* Crossed it or too close */
	    {
	    x1 = xp;
	    HitResult = POV_SLIDEX;
	    }
	}
    }

if (y1 < yp)				    /* Moving up */
    {
    if (GetWallY(mPos))			    /* Wall in current square */
	{
	if (y1 < yTop || abs(y1-yTop) < 28) /* Crossed it or too close */
	    {
	    y1 = yp;
	    HitResult = POV_SLIDEY;
	    }
	}
    }

if (y1 > yp)				    /* Moving down */
    {
    if (GetWallY(mPos+GRID_WIDTH))	    /* Wall in current square */
	{
	if (y1 > yBottom || abs(yBottom-y1) < 28) /* Crossed or too close */
	    {
	    y1 = yp;
	    HitResult = POV_SLIDEY;
	    }
	}
    }


if (!HitResult)				/* Nothing hit yet, look further */
    {
    if (y1 < (yTop+32))		       /* We are above upper half of square */
	{
	if (x1 < (xLeft+32))	       /* and on the left half of square */
	    {
	    mCodeX = GetWallX(mPos-GRID_WIDTH);
	    mCodeY = GetWallY(mPos-1);

	    if (mCodeX && y1 < (yTop+28))
		{
		if (x1 < (xLeft+28))
		    {
		    if (xp > (xLeft+27))
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    else
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    }
		}

	    if (mCodeY && x1 < (xLeft+28))
		{
		if (y1 < (yTop+28))
		    {
		    if (yp > (yTop+27))
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    else
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    }
		}
	    }

	if (x1 > (xRight-32) && !HitResult)	/* on right side of square */
	    {
	    mCodeX = GetWallX(mPos+1-GRID_WIDTH);
	    mCodeY = GetWallY(mPos+1);

	    if (mCodeX && y1 < (yTop+28))
		{
		if (x1 > (xRight-28))
		    {
		    if (xp < (xRight-27))
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    else
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    }
		}

	    if (mCodeY && x1 > (xRight-28))
		{
		if (y1 < (yTop+28))
		    {
		    if (yp > (yTop+27))
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    else
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    }
		}
	    }
	}

    if (y1 > (yTop+32) && !HitResult)	/* We are below upper half of square */
	{
	if (x1 < (xLeft+32))	       /* and on the left half of square */
	    {
	    mCodeX = GetWallX(mPos+GRID_WIDTH);
	    mCodeY = GetWallY(mPos-1+GRID_WIDTH);

	    if (mCodeX && y1 > (yBottom-28))
		{
		if (x1 < (xLeft+28))
		    {
		    if (xp > (xLeft+27))
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    else
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    }
		}

	    if (mCodeY && x1 < (xLeft+28))
		{
		if (y1 > (yBottom-28))
		    {
		    if (yp < (yBottom-27))
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    else
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    }
		}
	    }

	if (x1 > (xRight-32) && !HitResult)	/* on right side of square */
	    {
	    mCodeX = GetWallX(mPos+1+GRID_WIDTH);
	    mCodeY = GetWallY(mPos+1+GRID_WIDTH);

	    if (mCodeX && y1 > (yBottom-28))
		{
		if (x1 > (xRight-28))
		    {
		    if (xp < (xRight-27))
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    else
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    }
		}

	    if (mCodeY && x1 > (xRight-28))
		{
		if (y1 > (yBottom-28))
		    {
		    if (yp < (yBottom-27))
			{
			y1 = yp;
			HitResult = POV_SLIDEY;
			}
		    else
			{
			x1 = xp;
			HitResult = POV_SLIDEX;
			}
		    }
		}
	    }
	}
    }



#if 0
if (GetWallX(mPos))
    {
    if (x1 < xLeft || abs(x1-xLeft) < 31)
	HitResult = POV_XWALL;
    }

if (GetWallX(mPos+1))
    {
    if (x1 > xRight || abs(xRight-x1) < 31)
	HitResult = POV_XWALL;
    }

if (!HitResult)
    {
    if (GetWallY(mPos))
	{
	if (y1 < yTop || abs(y1-yTop) < 31)
	    HitResult = POV_YWALL;
	}

    if (GetWallY(mPos+GRID_WIDTH))
	{
	if (y1 > yBottom || abs(yBottom-y1) < 31)
	    HitResult = POV_YWALL;
	}
    }


xLeft = x1 & 0xFFC0;
xRight = xLeft + GRID_SIZE - 1;
yTop = y1 & 0xFFC0;
yBottom = yTop + GRID_SIZE - 1;
mPos = yTop + (x1 >> 6);

if (!HitResult)
    {
    if (GetWallX(mPos))
	{
	if (x1 < xLeft || abs(x1-xLeft) < 31)
	    HitResult = POV_XWALL;
	}

    if (GetWallX(mPos+1))
	{
	if (x1 > xRight || abs(xRight-x1) < 31)
	    HitResult = POV_XWALL;
	}

    if (!HitResult)
	{
	if (GetWallY(mPos))
	    {
	    if (y1 < yTop || abs(y1-yTop) < 31)
		HitResult = POV_YWALL;
	    }

	if (GetWallY(mPos+GRID_WIDTH))
	    {
	    if (y1 > yBottom || abs(yBottom-y1) < 31)
		HitResult = POV_YWALL;
	    }
	}
    }
#endif


oPtr->x = x1;
oPtr->y = y1;
oPtr->mPos = MapPosn;

PlayerPosn = (aeGlobal->yPlayer & 0xFFC0) + (aeGlobal->xPlayer >> 6);
if (MapPosn == PlayerPosn)
    return(POV_PLAYER);

return(HitResult);
}

/*
   Similiar to the AckMovePOV() above except ignores collision with the
   same object being moved, and also checks for a collision with the
   player. Angle should be the direction to move the object, Amount is the
   map unit distance the object is to be moved.
*/
short xxxAckMoveObjectPOV(short ObjIndex,short Angle,short Amount)
{
    short   ox,oy,x1,y1,HitResult,NewAngle,oNum;
    UINT    mCode;
    UCHAR   gCode;
    short   xc,yc;
    short   MapPosn,PlayerPosn,CheckDist;
    NEWOBJECT **oList;
    NEWOBJECT	*oPtr;

oList = &aeGlobal->ObjList[0];
oPtr = oList[ObjIndex];

if (oPtr == NULL)
    return(0);

ox = oPtr->x;
oy = oPtr->y;
x1 = ox + (short)((CosTable[Angle] * Amount) >> FP_SHIFT);
y1 = oy + (short)((SinTable[Angle] * Amount) >> FP_SHIFT);

CheckDist = 48;
if (Amount > CheckDist) CheckDist = Amount;
xc = ox + (short)((CosTable[Angle] * CheckDist) >> FP_SHIFT);
yc = oy + (short)((SinTable[Angle] * CheckDist) >> FP_SHIFT);


HitResult = AckCheckHit(ox,oy,Angle);

if (!HitResult)
    {
    MapPosn = (y1 & 0xFFC0) + (x1 >> 6);

    oNum = AckCheckObjPosn(xc,yc,Angle,ObjIndex);
    if (oNum > 0 && LastObjectHit != ObjIndex)
	return(POV_OBJECT);

    mCode = CheckMapPosn(ox,oy,xc,yc);
    if (mCode > 0 && !(mCode & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT)))
	return(POV_XWALL);

    oPtr->x = x1;
    oPtr->y = y1;
    oPtr->mPos = MapPosn;

    PlayerPosn = (aeGlobal->yPlayer & 0xFFC0) + (aeGlobal->xPlayer >> 6);
    if (MapPosn == PlayerPosn)
	return(POV_PLAYER);

    }

return(HitResult);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Internal function called by AckMovePOV() and AckMoveObjectPOV() to
// check for a collision with a wall within a certain distance.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
short AckCheckHit(short xPlayer,short yPlayer,short vAngle)
{
    short     BitmapColumn,BitmapNumber,yBitmap,distance;
    short     i,WallCode;
    ULONG   WallDistance;
    ULONG   xd,yd,mf,yDistance;
    long    CheckDist;

WallDistance = 3000000;	    /* Set to a ridiculous value */
WallCode     = POV_NOTHING;
CheckDist    = 56L;	    /* (was 48) Initial minimum distance to look for */
BitmapNumber = 0;	    /* Initialize to no bitmap found */

xPglobal = xPlayer;
xBegGlobal = xPglobal & GRID_MASK;
xPglobalHI = ((long)xPglobal << FP_SHIFT);
yPglobal = yPlayer;
yBegGlobal = yPglobal & GRID_MASK;
yPglobalHI = ((long)yPglobal << FP_SHIFT);

ViewAngle = vAngle;

if (MoveObjectCount)
    memmove(ObjectsSeen,MoveObjectList,MoveObjectCount);

FoundObjectCount = MoveObjectCount;

/*
   Don't allow one of these angles, causes either the X or Y ray to not be
   cast which gives a false reading about an obstacle.
*/
if (ViewAngle == INT_ANGLE_45 ||
    ViewAngle == INT_ANGLE_135 ||
    ViewAngle == INT_ANGLE_225 ||
    ViewAngle == INT_ANGLE_315)
    ViewAngle++;

    xRaySetup();
    BitmapNumber = xRayCast();

    if (BitmapNumber & (WALL_TYPE_UPPER+WALL_TYPE_PASS))
	BitmapNumber = 0;

    if (BitmapNumber)
	{
	xd = iLastX - xPlayer;

	mf = InvCosTable[ViewAngle];
	WallDistance = (xd * mf) >> 10;

	if (WallDistance > 33554432L)
	    WallDistance = 1200000L;

	/* Set the wall struck code to an X wall */
	WallCode = POV_XWALL;
	LastMapPosn = xMapPosn;
	}

    yRaySetup();
    yBitmap = yRayCast();

    if (yBitmap & (WALL_TYPE_UPPER+WALL_TYPE_PASS))
	yBitmap = 0;

    if (yBitmap)
	{
	yd = iLastY - yPlayer;

	mf = InvSinTable[ViewAngle];
	yDistance = (yd * mf) >> 8;

	if (yDistance > 33554432L)
	    yDistance = 120000L;

	/* If Y wall closer than X wall then use Y wall data */
	if (yDistance < WallDistance)
	    {
	    WallDistance = yDistance;

	    /* Indicate the wall struck was a Y wall */
	    WallCode = POV_YWALL;
	    BitmapNumber = yBitmap;
	    LastMapPosn = yMapPosn;
	    }

	}


/*
   Since doors appear in the middle of the wall, adjust the minimum distance
   to it. This handles walking up close to a door.
*/
if (BitmapNumber & (DOOR_TYPE_SLIDE+DOOR_TYPE_SPLIT))
    CheckDist += 64L;

BitmapNumber &= 0xFF;

if (WallCode)
    {
    yd = ViewCosTable[160] >> 3;
    WallDistance *= yd;

    /* Now we strip off somemore decimal points and check round-up */
    xd = WallDistance >> 12;
    if (WallDistance - (xd << 12) >= 2048)
	xd++;

    /*
      The last decimal points from the multiplication after the X and
      Y rays is stripped off and checked for round-up.
    */
    WallDistance = xd >> 5;
    if (xd - (WallDistance << 5) >= 16)
	WallDistance++;


   /*
      If the wall or object is further than the minimum distance, we can
       continue moving in this direction.
   */
    if (WallDistance > CheckDist)
	WallCode = POV_NOTHING;
    }

return(WallCode);
}

//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
// Runs the list of objects and checks for any animation that should occur
// for that object. The name of the function is left over from the older
// version which also moved the object based on an index value.
//北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
void AckCheckObjectMovement(void)
{
    short     i,speed,Pos,maxObj;
    short     dx,dy,dir,oNum;
    short     NewDx,NewDy;
    NEWOBJECT **oList;
    NEWOBJECT	*oPtr;

maxObj = aeGlobal->MaxObjects;
oList = &aeGlobal->ObjList[0];

for (i = 1; i < maxObj; i++)
    {
    oPtr = oList[i];
    if (oPtr == NULL)
	continue;

    if (!oPtr->Active)
	continue;

    if (!(speed = oPtr->Speed))
	continue;

    if (!(oPtr->Flags & OF_ANIMATE))
	continue;

    dx = oPtr->CurrentBm + 1;
    if (dx >= oPtr->Maxbm)
	{
	if (oPtr->Flags & OF_ANIMONCE)
	    {
	    oPtr->Flags &= ~OF_ANIMATE;
	    oPtr->Flags |= OF_ANIMDONE;
	    dx = oPtr->CurrentBm;
	    }
	else
	    dx = 0;
	}

    oPtr->CurrentBm = dx;
    }

}


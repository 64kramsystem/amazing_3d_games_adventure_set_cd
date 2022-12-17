// Support routines for ACK applications
// (c) 1995 ACK Software (Lary Myers)
#define  STRICT
#include <windows.h>
#pragma hdrstop
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <mem.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <string.h>
#include "wing.h"
#include "wingdll.h"

// Uncomment next line to use normal windows colors
//#define _USE_SYSTEM_PALETTE 1


extern "C" {
#include "keys.h"
#include "ack3d.h"
extern	short	rsHandle;
extern	ULONG	*rbaTable;
extern	short	Resolution;
extern	UCHAR	colordat[];
extern	UCHAR	*BackArray[];
extern	unsigned short FloorMap[];
extern	unsigned short CeilMap[];
extern	unsigned short *ObjGrid;
extern	unsigned short *Grid;
};


extern ACKENG	*ae;

// Globals
#if 0
    char    LineBuffer[200];
    short   LastObjectIndex;
    int     MapResource;
    int     PalResource;
    int	    ResScreenBack;
    int     ResScrollBack;
#endif

    extern int	    MapResource;
    extern int	    PalResource;
    extern int	    ResScreenBack;
    extern int	    ResScrollBack;



//***************************************************************************
// Creates the correct arrays for displaying a rotating background
//***************************************************************************
void ProcessBackDrop(UCHAR *bPtr)
{
	int     i,j,pos;
	UCHAR   *aPtr;

for (i = 0; i < 320; i++)
    {
    pos = i + 4;
    aPtr = BackArray[i];
    for (j = 0; j < 100; j++)
   		{
      	*aPtr++ = bPtr[pos];
        pos += 320;
        }
    }

for (i = 320; i < 640; i++)
    {
    pos = (i - 320) + 32004;
    aPtr = BackArray[i];
    for (j = 0; j < 100; j++)
        {
   		*aPtr++ = bPtr[pos];
		pos += 320;
   		}
    }

}


//***************************************************************************
// Loads a background image (mountains) and processes the image into the
// separate slices for use at display time. Currently a background image
// can be 640 columns wide. This number can be made dynamic if needbe and would
// need to be changed in the routine below and in the DrawBackground routine
// in the ACK engine.
//***************************************************************************
int LoadBackDrop(void)
{
	UCHAR	*bPtr;

if (ResScrollBack)
    {
    bPtr = AckReadiff((char *)ResScrollBack);

    if (bPtr == NULL)
        return(8);

    ProcessBackDrop(bPtr);
    AckFree(bPtr);
    }

return(0);
}

#if 0
//***************************************************************************
// Reads a text line from the resource file
//***************************************************************************
int ReadLine(void)
{
    int	    len;
    char    ch;

len = 0;
while (len < 200)
    {
    if (_lread(rsHandle,&LineBuffer[len],1) != 1)
    	break;

    ch = LineBuffer[len];
    if (ch == 10)
	continue;

    if (ch == 13)
    	break;

    len++;
    }

LineBuffer[len] = '\0';

return(len);
}

//***************************************************************************
// Skips to the next parameter in a text line
//***************************************************************************
char *GetNextParm(char *s)
{
    char    ch;

while (*s)
    {
    ch = *s++;
    if (ch == ',')
	{
    	while (*s)
    	    {
    	    ch = *s;
    	    if (ch != ',' && ch != ' ' && ch != '\t')
        		return(s);
    	    s++;
    	    }
    	return(NULL);
    	}
    }

return(NULL);
}

//***************************************************************************
// Loads a wall bitmap specified in info file
//***************************************************************************
int LoadWall(void)
{
    int     wnum,rnum,result;
    long    pos;
    char    *lb;


lb = LineBuffer;	    // Info file buffer
wnum = atoi(lb);	    // Wall number to load into

lb = GetNextParm(lb);

if (lb == NULL)
    return(-1);

rnum = atoi(lb);	    // Resource number

pos = _llseek(rsHandle,0L,SEEK_CUR);
result = AckLoadWall(ae,(short)wnum,(char *)rnum);
_llseek(rsHandle,pos,SEEK_SET);

return(result);
}


//***************************************************************************
// Loads an object bitmap specified in info file
//***************************************************************************
int LoadObject(void)
{
    int	    onum,rnum,result;
    long    pos;
    char    *lb;

lb = LineBuffer;
onum = atoi(lb);	    // Object bitmap number

lb = GetNextParm(lb);

if (lb == NULL)
    return(-2);

rnum = atoi(lb);	    // Resource number
pos = _llseek(rsHandle,0L,SEEK_CUR);
result = AckLoadObject(ae,(short)onum,(char *)rnum);
_llseek(rsHandle,pos,SEEK_SET);

return(result);
}


//***************************************************************************
// Skip any leading spaces in the string
// NOTE: Actually modifies the string passed!
//***************************************************************************
char *SkipSpaces(char *s)
{

while (*s == ' ' || *s == '\t' || *s == ',')
    strcpy(s,&s[1]);

return(s);
}


//***************************************************************************
// Creates and object of the desired style
//***************************************************************************
int CreateObject(void)
{
    int     onum,vnum;
    short   result,oType;
    short   NumViews,bmPerView;
    USHORT  flags;
    char    *lb;
    OBJSEQ  os;

lb = LineBuffer;

if (!strnicmp(lb,"NUMBER:",7))
    {
    lb = &lb[7];
    onum = atoi(lb);
    if (onum < 1 || onum > MAX_OBJECTS)
    	return(-3);

    result = AckCreateObject(ae,(short)onum);
    if (result)
    	return(result);

    LastObjectIndex = (short)onum;
    lb = GetNextParm(lb);
    if (lb == NULL)
    	return(-4);

    ae->ObjList[onum]->Speed = (short)atoi(lb);
    return(0);
    }

onum = LastObjectIndex;		// Object number

oType = 0;

if (!strnicmp(lb,"CREATE:",7))
    {
    oType = NO_CREATE;
    lb = &lb[7];
    }

if (!strnicmp(lb,"DESTROY:",8))
    {
    oType = NO_DESTROY;
    lb = &lb[8];
    }

if (!strnicmp(lb,"WALK:",5))
    {
    oType = NO_WALK;
    lb = &lb[5];
    }

if (!strnicmp(lb,"ATTACK:",7))
    {
    oType = NO_ATTACK;
    lb = &lb[7];
    }

if (!strnicmp(lb,"INTERACT:",9))
    {
    oType = NO_INTERACT;
    lb = &lb[9];
    }

if (!oType)
    return(-5);

lb = SkipSpaces(lb);
if (lb == NULL)
    return(-6);

flags = 0;
strupr(lb);

if (strstr(lb,"ANIMATE") != NULL)
    flags |= OF_ANIMATE;

if (strstr(lb,"MOVEABLE") != NULL)
    flags |= OF_MOVEABLE;

if (strstr(lb,"PASSABLE") != NULL)
    flags |= OF_PASSABLE;

if (strstr(lb,"MULTIVIEW") != NULL)
    flags |= OF_MULTIVIEW;

if (strstr(lb,"SHOWONCE") != NULL)
    flags |= OF_ANIMONCE;

lb = GetNextParm(lb);
if (lb == NULL)
    return(-5);

NumViews = (short)atoi(lb);
if (NumViews < 1)
    return(-6);

lb = GetNextParm(lb);
if (lb == NULL)
    return(-7);

bmPerView = (short)atoi(lb);
if (bmPerView < 1)
    return(-7);

vnum = NumViews * bmPerView;
if (vnum > MAX_OBJ_BITMAPS)
    return(-8);

lb = GetNextParm(lb);
if (lb == NULL)
    return(-9);

vnum = 0;

while (lb != NULL && vnum < MAX_OBJ_BITMAPS)
    {
    os.bitmaps[vnum++] = (short)atoi(lb);
    lb = GetNextParm(lb);
    }

os.bmBitmapsPerView = bmPerView;
os.flags = flags;
os.MaxBitmaps = bmPerView;
os.bmSides = NumViews;

result = (short)AckSetupObject(ae,(short)onum,oType,&os);

return(result);
}

//***************************************************************************
// Reads the ASCII info file and processes the commands.
//***************************************************************************
#pragma argsused
int ProcessInfoFile(short QuietFlag)
{
    int	    result;
    int	    mode;
    long    pos;


// Position to start of info file within resource file
_llseek(rsHandle,rbaTable[0],SEEK_SET);

mode = result = 0;

while (!result)
    {
    if (!ReadLine())
    	continue;

    if (*LineBuffer == ';')
	continue;

    if (!strnicmp(LineBuffer,"END:",4))
    	break;

    switch (mode)
    	{
    	case 1:		// Read walls
    	    if (!strnicmp(LineBuffer,"LOADTYPE:",9))
    		{
    		ae->bmLoadType = (short)atoi(&LineBuffer[9]);	// Sets for GIF or BBM
    		break;
    		}

    	    if (!strnicmp(LineBuffer,"ENDBITMAPS:",11))
    			mode = 4;
       	    else
        		result = LoadWall();
    	    break;

    	case 2:		// Object bitmaps
    	    if (!strnicmp(LineBuffer,"LOADTYPE:",9))	// Sets for GIF or BBM
        		{
        		ae->bmLoadType = (short)atoi(&LineBuffer[9]);
			break;
        		}
    	    if (!strnicmp(LineBuffer,"ENDBITMAPS:",11))
        		mode = 5;
    	    else
        		result = LoadObject();
    	    break;

    	case 3:		// Create Object
    	    if (!strnicmp(LineBuffer,"ENDDESC:",8))
        		mode = 5;
    	    else
        		result = CreateObject();
    	    break;

    	case 4:		// Walls topic
    	    if (!strnicmp(LineBuffer,"BITMAPS:",8))
        		mode = 1;

    	    if (!strnicmp(LineBuffer,"ENDWALLS:",9))
        		mode = 0;
    	    break;


	case 5:		// Objects topic
    	    if (!strnicmp(LineBuffer,"BITMAPS:",8))
        		mode = 2;

       	    if (!strnicmp(LineBuffer,"OBJDESC:",8))
        		mode = 3;

    	    if (!strnicmp(LineBuffer,"ENDOBJECTS:",11))
        		mode = 0;
    	    break;


    	default:
    	    if (!strnicmp(LineBuffer,"WALLS:",6))
        		{
    			mode = 4;
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"OBJECTS:",8))
        		{
        		mode = 5;
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"MAPFILE:",8))
        		{
        		MapResource = atoi(&LineBuffer[8]);
        		pos = _llseek(rsHandle,0L,SEEK_CUR);
        		result = AckReadMapFile(ae,(char *)MapResource);
        		_llseek(rsHandle,pos,SEEK_SET);
        		break;
        		}

       	    if (!strnicmp(LineBuffer,"PALFILE:",8))
        		{
        		PalResource = atoi(&LineBuffer[8]);
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"XPLAYER:",8))
        		{
        		ae->xPlayer = (short)atoi(&LineBuffer[8]);
        		break;
        		}

       	    if (!strnicmp(LineBuffer,"YPLAYER:",8))
        		{
			ae->yPlayer = (short)atoi(&LineBuffer[8]);
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"PLAYERANGLE:",12))
        		{
        		ae->PlayerAngle = (short)atoi(&LineBuffer[12]);
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"SCREENBACK:",11))
        		{
        		ResScreenBack = (short)atoi(&LineBuffer[11]);
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"SCROLLBACK:",11))
        		{
        		ResScrollBack = (short)atoi(&LineBuffer[11]);
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"TOPCOLOR:",9))
        		{
			ae->TopColor = (short)atoi(&LineBuffer[9]);
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"BOTTOMCOLOR:",12))
        		{
        		ae->BottomColor = (short)atoi(&LineBuffer[12]);
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"SHADING:",8))
        		{
        		strupr(LineBuffer);
        		if (strstr(&LineBuffer[8],"OFF") != NULL)
        		    ae->LightFlag = SHADING_OFF;
    			else
        		    ae->LightFlag = SHADING_ON;
        		break;
        		}

    	    if (!strnicmp(LineBuffer,"FLOORS:",7))
        		{
        		strupr(LineBuffer);
        		if (strstr(&LineBuffer[7],"OFF") != NULL)
			    ae->SysFlags |= SYS_SOLID_FLOOR;
        		else
        		    {
        		    ae->SysFlags &= ~SYS_SOLID_FLOOR;
        		    }
    		break;
    		}

	    if (!strnicmp(LineBuffer,"RESOLUTION:",11))
    		{
    		Resolution = (short)atoi(&LineBuffer[11]);
    		break;
    		}

   	    if (!strnicmp(LineBuffer,"LOADTYPE:",9))
    		{
    		ae->bmLoadType = (short)atoi(&LineBuffer[9]);	// Sets for GIF or BBM
    		break;
    		}

	    break;
    	}
    }

if (!result)
    {
    result = AckCreateObject(ae,0);	// Create a dummy object for later
    if (!result)
	ae->ObjList[0]->Active = 0;	// Turn off checking the object
    }

return(result);
}
#endif


//**** End of Source ****

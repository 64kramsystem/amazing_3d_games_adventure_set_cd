// Simple BBM file displayer
// (c) 1994 Lary Myers

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include "ack3d.h"

extern	UCHAR	   colordat[];

    char    *smFont;
    UCHAR   FontTransparent;
    UCHAR   FontColor;
    UCHAR   TextBGcolor;
    short   bmWidth;
    short   bmHeight;
    short   HaveMouse;
    char    szCommandLine[180];
    char    FileName[128];
    char    Path[128];

typedef struct _node {
    struct  _node   *Back;
    struct  _node   *Fwd;
    char    Name[128];
    } NODE;

    NODE    *NodeAnchor;
    NODE    *NodeCurrent;
    NODE    *NodeTop;

short MouseInstalled(void);
short MouseReadCursor(short *row,short *col);
void MouseReleased(void);
void HideMouse(void);
void ShowMouse(void);

//=============================================================================
//
//=============================================================================
short LoadSmallFont(void)
{
    short   ht,wt;
    int	    len;

ht = 2;
smFont = AckReadiff("spfont.bbm");
if (smFont == NULL)
    return(-1);

ht = (*(short *)smFont);
wt = (*(short *)&smFont[2]);
len = ht * wt;
memmove(smFont,&smFont[4],len);

return(0);
}

//=============================================================================
//
//=============================================================================
void smWriteChar(short x,short y,unsigned char ch)
{
		int	 FontOffset,VidOffset;
		int	 row,col;
		UCHAR	 *Video;

VidOffset = (y * 320) + x;
Video = (UCHAR *)0xA0000;
Video += VidOffset;
col = ch - 32;
FontOffset = col * 5;

for (row = 0; row < 5; row++)
    {
    if (!FontTransparent)
	{
	Video[0] = TextBGcolor;
	Video[1] = TextBGcolor;
	Video[2] = TextBGcolor;
	Video[3] = TextBGcolor;
	}

    if (smFont[FontOffset])
	Video[0] = FontColor;
    if (smFont[FontOffset+1])
	Video[1] = FontColor;
    if (smFont[FontOffset+2])
	Video[2] = FontColor;
    if (smFont[FontOffset+3])
	Video[3] = FontColor;

    Video += 320;
    FontOffset += 294;
    }


}

//=============================================================================
//
//=============================================================================
short smWriteString(short x,short y,char *s)
{
    short   OrgX;
    char    ch;

OrgX = x;

while (*s)
    {
    ch = *s++;

    if (ch == 10)
	{
	x = OrgX;
	y += 8;
	continue;
	}

    if (ch < ' ')
	continue;

    ch = toupper(ch);
    smWriteChar(x,y,ch);
    x += 5;
    }

return(y);
}



//=============================================================================
//
//=============================================================================
void GetPath(char *s)
{
    int	    len;

len = strlen(s) - 1;
while (len > 0)
    {
    if (s[len] == '\\' || s[len] == ':')
	{
	len++;
	strncpy(Path,s,len);
	Path[len] = '\0';
	break;
	}
    len--;
    }


}

//=============================================================================
//
//=============================================================================
void ClearLastBitmap(void)
{
    int	    row;
    UCHAR   *Video;

if (bmWidth < 0)
    return;

Video = (UCHAR *)0xA0000;

for (row = 0; row < bmHeight;row++)
    {
    memset(Video,0,bmWidth);
    Video += 320;
    }

}

//=============================================================================
//
//=============================================================================
void ShowBitmap(UCHAR *bmBuf)
{
    int	    row;
    UCHAR   *Video;

Video = (UCHAR *)0xA0000;

for (row = 0; row < bmHeight;row++)
    {
    memcpy(Video,bmBuf,bmWidth);
    Video += 320;
    bmBuf += bmWidth;
    }

AckSetPalette(colordat);

}

//=============================================================================
//
//=============================================================================
void ShowName(char *name)
{
    int	    row;
    UCHAR   *Video;
    char    szText[128];


if (HaveMouse)
    HideMouse();
Video = (UCHAR *)0xA0000;
Video += 59280;
for (row = 0; row < 12; row++)
    {
    memset(Video,0,100);
    Video += 320;
    }

sprintf(szText,"%s   %d x %d",name,bmWidth,bmHeight);
smWriteString(80,185,szText);
if (HaveMouse)
    ShowMouse();
}

//=============================================================================
//
//=============================================================================
void DrawFileBox(void)
{
    short   row;
    UCHAR   *Video,*vPtr;

if (HaveMouse)
    HideMouse();
Video = (UCHAR *)0xA0000;
memset(Video,15,72);
Video += 320;
for (row = 1; row < 193; row++)
    {
    *Video = 15;
    Video[71] = 15;
    Video += 320;
    }
memset(Video,15,72);
if (HaveMouse)
    ShowMouse();
}

//=============================================================================
//
//=============================================================================
void HighLiteNode(void)
{
    short   i,y,x;
    UCHAR   *Video,*vPtr;
    NODE    *node;

if (HaveMouse)
    HideMouse();

node = NodeTop;
y = 1;
for (i = 0; i < 24; i++)
    {
    if (node == NULL)
	break;

    if (node == NodeCurrent)
	{
	Video = (UCHAR *)0xA0000 + (y * 320) + 1;
	for (y = 0; y < 7;y++)
	    {
	    vPtr = Video;
	    for (x = 0; x < 70; x++)
		{
		*vPtr ^= FontColor;
		vPtr++;
		}
	    Video += 320;
	    }
	break;
	}
    node = node->Fwd;
    y += 8;
    }

if (HaveMouse)
    ShowMouse();
}

//=============================================================================
//
//=============================================================================
void ClearString(short x,short y)
{
    UCHAR   *Video;

Video = (UCHAR *)0xA0000 + (y * 320) + x;
for (y = 0; y < 7; y++)
    {
    memset(Video,0,69);
    Video += 320;
    }
}

//=============================================================================
//
//=============================================================================
void DrawUpArrow(short x,short y,UCHAR color)
{
    short   offset;
    UCHAR   *Video = (UCHAR *)0xA0000;

offset = (y * 320) + x;
Video += offset;
memset(Video,color,9);
Video += 320;
*Video = color;
Video[8] = color;
Video[4] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[3] = color;
Video[4] = color;
Video[5] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[2] = color;
Video[4] = color;
Video[6] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[4] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[4] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[4] = color;
Video += 320;
memset(Video,color,9);
}

//=============================================================================
//
//=============================================================================
void DrawDownArrow(short x,short y,UCHAR color)
{
    short   offset;
    UCHAR   *Video = (UCHAR *)0xA0000;

offset = (y * 320) + x;
Video += offset;
memset(Video,color,9);
Video += 320;
*Video = color;
Video[8] = color;
Video[4] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[4] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[4] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[2] = color;
Video[4] = color;
Video[6] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[3] = color;
Video[4] = color;
Video[5] = color;
Video += 320;
*Video = color;
Video[8] = color;
Video[4] = color;
Video += 320;
memset(Video,color,9);
}


//=============================================================================
//
//=============================================================================
void ShowArrows(void)
{
    UCHAR   TopColor,BotColor;
    short   i;
    NODE    *node;

TopColor = 0;
BotColor = 15;
node = NodeTop;

if (node->Back != NULL)
    TopColor = 15;

for (i = 24; i > 0; i--)
    {
    if (node->Fwd == NULL)
	{
	BotColor = 0;
	break;
	}
    node = node->Fwd;
    }


DrawUpArrow(73,4,TopColor);
DrawDownArrow(73,16,BotColor);



}

//=============================================================================
//
//=============================================================================
void ShowFileList(void)
{
    short   i,y;
    NODE    *node;

if (HaveMouse)
    HideMouse();
DrawFileBox();
node = NodeTop;
y = 2;

for (i = 0; i < 24; i++)
    {
    if (node == NULL)
	break;
    ClearString(2,y);
    smWriteString(2,y,node->Name);
    y += 8;
    node = node->Fwd;
    }

for (;i < 24; i++)
    {
    ClearString(2,y);
    y += 8;
    }

smWriteString(90,2,szCommandLine);
ShowArrows();
if (HaveMouse)
    ShowMouse();
}

//=============================================================================
//
//=============================================================================
unsigned short Inkey(void)
{
    unsigned short key;
    union REGPACK regs;

key = 0;
memset(&regs,0,sizeof(union REGPACK));
regs.w.ax=0x0100;
intr(0x16,&regs);
key = regs.w.ax;
if (key)
    {
    regs.w.ax=0;
    intr(0x16,&regs);
    key = regs.w.ax;
    }

return(key);
}

//=============================================================================
//
//=============================================================================
short NodeCount(void)
{
    short   count = 0;
    NODE    *node;

node = NodeTop;

while (node != NULL)
    {
    if (node == NodeCurrent)
	break;
    node = node->Fwd;
    count++;
    }

return(count);
}

//=============================================================================
//
//=============================================================================
void ClearScreen(void)
{
    UCHAR   *Video;

Video = (UCHAR *)0xA0000;
memset(Video,0,64000);
}

//=============================================================================
//
//=============================================================================
void SortNodes(void)
{
    short   bFlag;
    NODE    *node,*nodeNext,*BackNode,*FwdNode;

bFlag = 1;

while (bFlag)
    {
    bFlag = 0;
    node = NodeAnchor;

    while (node != NULL)
	{
	nodeNext = node->Fwd;
	if (nodeNext != NULL)
	    {
	    if (strcmp(node->Name,nodeNext->Name) > 0)
		{
		bFlag = 1;
		if (node == NodeAnchor)
		    NodeAnchor = nodeNext;
		BackNode = node->Back;
		FwdNode = nodeNext->Fwd;
		if (BackNode)
		    BackNode->Fwd = nodeNext;
		if (FwdNode)
		    FwdNode->Back = node;
		node->Back = nodeNext;
		nodeNext->Fwd = node;
		node->Fwd = FwdNode;
		nodeNext->Back = BackNode;
		break;
		}
	    }
	node = node->Fwd;
	}
    }


}

//=============================================================================
//
//=============================================================================
void main(int argc,char **argv)
{
    int	    done;
    short   i,mButton,mx,my;
    UCHAR   *bmBuffer;
    char    ch;
    char    ThisFile[128];
    struct  find_t ffblk;
    NODE    *node;
    unsigned short key;

printf("VBBM - BBM Viewer 1.0\n");
if (argc < 2)
    {
    printf("Syntax: VBBM filename[.BBM]\n");
    return;
    }

if (LoadSmallFont())
    {
    printf("Unable to find SPFONT.BBM file.\n");
    return;
    }

FontColor = 32;
sprintf(szCommandLine,"Files in: %s",argv[1]);
strcpy(FileName,argv[1]);
GetPath(FileName);


HaveMouse = 0;
if (MouseInstalled() == -1)
    HaveMouse = 1;

AckSetVGAmode();

if (HaveMouse)
    ShowMouse();

NodeAnchor = NULL;
done = _dos_findfirst(FileName,0,&ffblk);
while (!done)
    {
    node = malloc(sizeof(NODE));
    if (node == NULL)
	break;
    memset(node,0,sizeof(NODE));
    if (NodeAnchor == NULL)
	NodeAnchor = NodeCurrent = node;
    else
	{
	NodeCurrent->Fwd = node;
	node->Back = NodeCurrent;
	NodeCurrent = node;
	}
    strcpy(node->Name,ffblk.name);
    done = _dos_findnext(&ffblk);
    }

bmWidth = -1;
SortNodes();
NodeTop = NodeCurrent = NodeAnchor;
ShowFileList();
HighLiteNode();
done = 0;
while (!done)
    {
    key = Inkey();

    if (!key && HaveMouse > 0)
	{
	mButton = ReadMouseCursor(&my,&mx);
	if (mButton & 1)
	    {
	    if (mx < 72)
		{
		my -= 2;
		my /= 8;
		node = NodeTop;
		key = 0x1C0D;
		while (my-- > 0)
		    {
		    if (node->Fwd == NULL)
			{
			key = 0;
			break;
			}
		    node = node->Fwd;
		    }
		if (key)
		    {
		    HighLiteNode();
		    NodeCurrent = node;
		    HighLiteNode();
		    MouseReleased();
		    }
		}
	    if (mx > 73 && mx < 81)
		{
		if (my > 4 && my < 10)
		    key = 0x4800;
		if (my > 16 && my < 22)
		    key = 0x5000;
		}
	    }
	}

    switch (key)
	{
	case 0x11B:
	    done = 1;
	    break;

	case 0x4800:
	    if (NodeCurrent->Back != NULL)
		{
		HighLiteNode();
		if (NodeCurrent == NodeTop)
		    {
		    NodeTop = NodeTop->Back;
		    ShowFileList();
		    }
		NodeCurrent = NodeCurrent->Back;
		HighLiteNode();
		}
	    break;

	case 0x5000:
	    if (NodeCurrent->Fwd != NULL)
		{
		HighLiteNode();
		NodeCurrent = NodeCurrent->Fwd;
		if (NodeCount() > 23)
		    {
		    NodeTop = NodeTop->Fwd;
		    ShowFileList();
		    }
		HighLiteNode();
		}
	    break;

	case 0x4700:	// Home
	    HighLiteNode();
	    NodeTop = NodeCurrent = NodeAnchor;
	    ShowFileList();
	    HighLiteNode();
	    break;

	case 0x4F00:	// End
	    HighLiteNode();
	    while (NodeCurrent->Fwd != NULL)
		NodeCurrent = NodeCurrent->Fwd;
	    NodeTop = NodeCurrent;
	    for (i = 0; i < 23; i++)
		{
		if (NodeTop->Back == NULL)
		    break;

		NodeTop = NodeTop->Back;
		}
	    ShowFileList();
	    HighLiteNode();
	    break;

	case 0x4900:	// PgUp
	    HighLiteNode();
	    for (i = 0; i < 23; i++)
		{
		if (NodeTop->Back == NULL)
		    break;
		if (NodeCurrent->Back == NULL)
		    break;
		NodeCurrent = NodeCurrent->Back;
		NodeTop = NodeTop->Back;
		}
	    ShowFileList();
	    HighLiteNode();
	    break;

	case 0x5100:	// PgDown
	    HighLiteNode();
	    for (i = 0; i < 23; i++)
		{
		if (NodeTop->Fwd == NULL)
		    break;
		if (NodeCurrent->Fwd == NULL)
		    break;
		NodeCurrent = NodeCurrent->Fwd;
		NodeTop = NodeTop->Fwd;
		}
	    ShowFileList();
	    HighLiteNode();
	    break;

	case 0x1C0D:	// Enter
	    if (HaveMouse)
		HideMouse();
	    sprintf(ThisFile,"%s%s",Path,NodeCurrent->Name);
	    bmBuffer = AckReadiff(ThisFile);
	    if (bmBuffer != NULL)
		{
		ClearScreen();
		bmWidth = (*(short *)bmBuffer);
		bmHeight = (*(short *)&bmBuffer[2]);
		ShowBitmap(&bmBuffer[4]);
		ShowName(NodeCurrent->Name);
		AckFree(bmBuffer);
		}

	    while (!inkey())
		{
		if (HaveMouse)
		    {
		    mButton = ReadMouseCursor(&my,&mx);
		    if (mButton)
			{
			MouseReleased();
			break;
			}
		    }
		}
	    ClearScreen();
	    ShowFileList();
	    HighLiteNode();
	    if (HaveMouse)
		ShowMouse();
	    break;


	default:
	    break;
	}
    }

AckSetTextmode();

}


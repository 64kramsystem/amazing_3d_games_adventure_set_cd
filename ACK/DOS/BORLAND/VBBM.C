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

    char    FileName[128];
    char    Path[128];

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
void ShowBitmap(UCHAR *bmBuf)
{
    int	    row;
    UCHAR   *Video;

Video = (UCHAR *)0xA0000;

for (row = 0; row < 64; row++)
    {
    memmove(Video,bmBuf,64);
    Video += 320;
    bmBuf += 64;
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

Video = (UCHAR *)0xA0000;
Video += 10320;
for (row = 0; row < 12; row++)
    {
    memset(Video,0,100);
    Video += 320;
    }

smWriteString(80,32,name);

}

//=============================================================================
//
//=============================================================================
void main(int argc,char **argv)
{
    int	    done;
    UCHAR   *bmBuffer;
    char    ch;
    char    ThisFile[128];
    struct  find_t ffblk;

printf("VBBM - BBM Viewer 1.0\n");
if (argc < 2)
    {
    printf("Syntax: VBBM filename[.BBM]\n");
    return;
    }

if (LoadSmallFont())
    {
    printf("Unable to find SMFONT.BBM file.\n");
    return;
    }

FontColor = 32;
strcpy(FileName,argv[1]);
GetPath(FileName);

AckSetVGAmode();

done = _dos_findfirst(FileName,0,&ffblk);

while (!done)
    {
    sprintf(ThisFile,"%s%s",Path,ffblk.name);

    bmBuffer = AckReadiff(ThisFile);
    if (bmBuffer != NULL)
	{
	ShowBitmap(&bmBuffer[4]);
	ShowName(ffblk.name);
	}
    ch = getch();
    if (ch == 27) break;
    done = _dos_findnext(&ffblk);
    }

AckSetTextmode();


}


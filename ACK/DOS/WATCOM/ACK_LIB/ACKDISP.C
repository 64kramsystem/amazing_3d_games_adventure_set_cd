/******************* ( Animation Construction Kit 3D ) ***********************/
/*		       Display ViewWindow to Screen			     */
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

void AckDrawPage(void);

//=============================================================================
// This function copies the screen buffer to the video. Call AckBuildView()
// before calling this function.
//=============================================================================
short AckDisplayScreen(void)
{

// Let the assembly routine do the hard work
AckDrawPage();

return(0);
}

//=============================================================================
// Sets the 256 color palette. pBuf contains the RGB values to set
//=============================================================================
void AckSetPalette(UCHAR *pbuf)
{
    short     cnt,index;

cnt = 256;
index = 0;

while (cnt-- > 0)
    {
    outp(0x3c8,index++);
    outp(0x3c9,*pbuf++);
    outp(0x3c9,*pbuf++);
    outp(0x3c9,*pbuf++);
    }

}


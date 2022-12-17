/******************* ( Animation Construction Kit 3D ) ***********************/
/*			  Background Routines				     */
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

void AckBuildCeilingFloor(UCHAR far *,short,short,short,short,short,short);

/****************************************************************************
**									   **
****************************************************************************/
short AckBuildBackground(ACKENG *ae)
{

/* Let the assembly routine do all the hard work */

#if FLOOR_ACTIVE
#else
AckBuildCeilingFloor(ae->BkgdBuffer,
		     ae->LightFlag,
		     ae->TopColor,
		     ae->BottomColor,
		     ae->WinStartY,
		     ae->WinEndY,
		     ae->CenterRow);
#endif

return(0);
}


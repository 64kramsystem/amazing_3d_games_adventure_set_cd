/******************* ( Animation Construction Kit 3D ) ***********************/
/*			    Wrapup Routines				     */
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

//=============================================================================
// Call this function when exiting the program.
// It frees memory and insures the resource file is closed.
//=============================================================================
short AckWrapUp(ACKENG *ae)
{

AckFree(LongTanTable);
AckFree(LongInvTanTable);
AckFree(CosTable);
AckFree(SinTable);
AckFree(LongCosTable);
AckFree(xNextTable);
AckFree(yNextTable);
AckFree(ViewCosTable);
AckFree(AdjustTable);

if (ae->OverlayBuffer != NULL)
    AckFree(ae->OverlayBuffer);
ae->OverlayBuffer = NULL;

if (ae->BkgdBuffer != NULL)
    AckFree(ae->BkgdBuffer);
ae->BkgdBuffer = NULL;

if (ae->ScreenBuffer != NULL)
    AckFree(ae->ScreenBuffer);
ae->ScreenBuffer = NULL;

if (rsHandle)
    {
    close(rsHandle);
    rsHandle = 0;
    }

return(0);
}

//====== End of Code ==========================================================


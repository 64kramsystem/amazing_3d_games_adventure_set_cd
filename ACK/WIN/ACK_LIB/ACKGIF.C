// This source file contains the functions needed to read in GIF files.
// (c) 1995 ACK Software (Lary Myers)
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <bios.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"


//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
// Read a GIF format file and return a buffer containing the uncompressed
// image.
//ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ
#pragma argsused
unsigned char *AckReadgif (char *picname)
{
// This is a stub routine used only as a place holder for the actual
// GIF read routine. It was omitted based on current patent issues.
return(NULL);
}
// **** End of Source ****


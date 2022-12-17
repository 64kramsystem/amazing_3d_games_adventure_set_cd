//--------------------------------------------------------------------------
//
//  File:           DEBUG.CPP
//  Programmer:     Bob Provencher
//  Create Date:    12/4/94
//  Last Mod Date:  1/2/95
//  Description:    Debug stream class
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//--------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "debug.h"

#ifdef __BORLANDC__
const char* crlf = "\r\n";
#endif

dbugstream& dbugstream::write( LPBYTE b, WORD len )
{
	len = min( (WORD)16, len );
	for ( int i = 0; i < len; )
	{
		for ( int j = 0; j < 4 && i < len; i++, j++ )
			write_hex( b[ i ] );
		write( " " );
	}
	return *this;
}

dbugstream& dbugstream::write_hex( BYTE c )
{

	static char hex[] = "0123456789ABCDEF";

	char hi, lo;

	hi = hex[ c >> 4 ];
	lo = hex[ c & 0x0F ];

	write( hi );
	return write( lo );

}





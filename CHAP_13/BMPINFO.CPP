//---------------------------------------------------------------------------
//
//	File:			BMPINFO.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:    Bitmap info implementation
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "bmpinfo.h"

#ifndef __LOGPAL_H__
#include "logpal.h"
#endif

RGBQUAD BitmapInfo::rgb = { 0, 0, 0, 0 };

BOOL BitmapInfo::ResizeTo( DWORD dwNewColors )
{
	DWORD dwNewBytes = NumBytes( dwNewColors );
	DWORD dwBytes = NumBytes();
	BITMAPINFO FAR* lpNewBmpInfo = 0;
	if ( dwNewBytes )
	{
		lpNewBmpInfo = (BITMAPINFO FAR*)GlobalAllocPtr( GMEM_MOVEABLE, dwNewBytes );
		if ( !lpNewBmpInfo )
			return FALSE;
		_hmemset( lpNewBmpInfo, 0, dwNewBytes );
	}
	if ( lpBmpInfo )
	{
		if ( lpNewBmpInfo )
			_hmemcpy( lpNewBmpInfo, lpBmpInfo,
					  min( dwBytes, dwNewBytes ) );
	}
	Delete();
	lpBmpInfo = lpNewBmpInfo;
	return TRUE;
}

BOOL BitmapInfo::CopyColors( const LogPalette& pal )
{
	BOOL fReturn = FALSE;
	UINT colors = pal.Colors();
	if ( ResizeTo( colors ) )
	{
		for ( int i = 0; i < colors; i++ )
			Copy( Color( i ), pal[ i ] );
		fReturn = TRUE;
	}
	return fReturn;
}

void BitmapInfo::Dump( dbugstream& debug ) const
{
	debug << " Start BitmapInfo"             << crlf
		  << "  this: "      << (void*)this  << crlf
		  << "  lpBmpInfo: " << lpBmpInfo    << crlf;
	if ( IsCreated() )
		Dump( debug, *lpBmpInfo );
	debug << " End BitmapInfo"				<< crlf;
}

void BitmapInfo::Dump( dbugstream& debug, const BITMAPINFO& bi ) const
{
	debug << "   Start BITMAPINFO" << crlf;
	Dump( debug, bi.bmiHeader );
	DWORD c = NumColors( bi.bmiHeader );
	if ( c > 0 )
	{
		debug << "     Colors:" << crlf;
		for ( int i = 0; i < c; i++ )
		{
			debug << "      bmiColors[ " << (BYTE)i << " ]:";
			Dump( debug, Color( bi, i ) );
		}
	}
	debug << "   End BITMAPINFO" << crlf;
}

void BitmapInfo::Dump( dbugstream& debug, const BITMAPINFOHEADER& bih ) const
{
	debug << "     Start BITMAPINFOHEADER" 							   << crlf;
	debug << "      bmiHeader.biSize:	       " << bih.biSize          << crlf;
	debug << "      bmiHeader.biWidth:         " << bih.biWidth         << crlf;
	debug << "      bmiHeader.biHeight:        " << bih.biHeight        << crlf;
	debug << "      bmiHeader.biPlanes:        " << bih.biPlanes        << crlf;
	debug << "      bmiHeader.biBitCount:      " << bih.biBitCount      << crlf;
	debug << "      bmiHeader.biCompression:   " << bih.biCompression   << crlf;
	debug << "      bmiHeader.biSizeImage:     " << bih.biSizeImage     << crlf;
	debug << "      bmiHeader.biXPelsPerMeter: " << bih.biXPelsPerMeter << crlf;
	debug << "      bmiHeader.biYPelsPerMeter: " << bih.biYPelsPerMeter << crlf;
	debug << "      bmiHeader.biClrUsed:       " << bih.biClrUsed       << crlf;
	debug << "      bmiHeader.biClrImportant:  " << bih.biClrImportant  << crlf;
	debug << "     End BITMAPINFOHEADER" 							   << crlf;
}

void BitmapInfo::Dump( dbugstream& debug, const RGBQUAD& rgb ) const
{
	debug << " rgbBlue: "     << rgb.rgbBlue
		  << " rgbGreen: "    << rgb.rgbGreen
		  << " rgbRed: "      << rgb.rgbRed
		  << " rgbReserved: " << rgb.rgbReserved << crlf;
}


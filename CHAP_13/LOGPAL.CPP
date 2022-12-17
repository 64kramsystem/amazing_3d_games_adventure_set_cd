//---------------------------------------------------------------------------
//
//	File:			LOGPAL.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	LogicalPalette class implementation
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "logpal.h"

#ifndef __GEN_H__
#include "gen.h"
#endif

#ifndef __DIBITMAP_H__
#include "dibitmap.h"
#endif

PALETTEENTRY LogPalette::pal = { 0, 0, 0, 0 };

BOOL LogPalette::ResizeTo( WORD wNewSize )
{
	DWORD dwNewBytes = NumBytes( wNewSize );
	DWORD dwBytes = NumBytes();
	LOGPALETTE FAR* lpNewLogPalette = 0;
	if ( dwNewBytes )
	{
		lpNewLogPalette = (LOGPALETTE FAR*)new LPBYTE[ (size_t)dwNewBytes ];
		if ( !lpNewLogPalette )
			return FALSE;
		_fmemset( lpNewLogPalette, 0, (size_t)dwNewBytes );
	}
	if ( lpNewLogPalette )
	{
		if ( lpLogPalette )
		{
			_fmemcpy( lpNewLogPalette, lpLogPalette,
					  (size_t)min( dwBytes, dwNewBytes ) );
		}
		else
			lpNewLogPalette->palVersion = 0x0300;
		lpNewLogPalette->palNumEntries = wNewSize;
	}
	delete [] (LPBYTE)lpLogPalette;
	lpLogPalette = lpNewLogPalette;
	return TRUE;
}

BOOL LogPalette::GetSystemEntries( UINT uStart, UINT uEntries )
{
	if ( IsCreated() && uStart + uEntries <= Size() )
	{
		HDC hDC = GetDC( 0 );
		if ( hDC )
		{
			UINT u = GetSystemPaletteEntries( hDC, uStart, uEntries,
											  lpLogPalette->palPalEntry +
											  	uStart );
			ReleaseDC( 0, hDC );
			return u == uEntries;
		}
	}
	return FALSE;
}

BOOL LogPalette::CopyColors( const DIBitmap& dib )
{
	BOOL fReturn = FALSE;
	const BitmapInfo& bi( dib.Info() );
	UINT colors = bi.Colors();
	if ( ResizeTo( (WORD)colors ) )
	{
		for ( int i = 0; i < colors; i++ )
			Copy( (*this)[ i ], bi[ i ] );
		fReturn = TRUE;
	}
	return fReturn;
}

BOOL LogPalette::SetEntryFlags( UINT wStart, UINT wEntries, BYTE peFlags )
{
	if ( Palette() && wStart + wEntries <= Size() )
	{
		for ( int i = wStart; i < wStart + wEntries; i++ )
			(*this)[ i ].peFlags = peFlags;
		return TRUE;
	}
	else
		return FALSE;
}

void LogPalette::Dump( dbugstream& debug ) const
{
	debug << "Start LogPalette"				   << crlf
		  << " this: "         << (void*)this  << crlf
		  << " lpLogPalette: " << lpLogPalette << crlf;
	if ( IsCreated() )
		Dump( debug, *lpLogPalette );
	debug << "End LogPalette"				   << crlf;
}

void LogPalette::Dump( dbugstream& debug, const LOGPALETTE& pal ) const
{
	debug << " Start LOGPALETTE"                      << crlf
		  << "  palVersion:    " << pal.palVersion 	  << crlf
		  << "  palNumEntries: " << pal.palNumEntries << crlf;
	DWORD c = NumColors( pal );
	if ( c > 0 )
	{
		debug << "  Colors:" << crlf;
		for ( int i = 0; i < c; i++ )
		{
			debug << "   palPalEntry[ " << (BYTE)i << " ]: ";
			Dump( debug, Color( pal, i ) );
		}
	}
	debug << " End LOGPALETTE"                        << crlf;
}

void LogPalette::Dump( dbugstream& debug, const PALETTEENTRY& pal ) const
{
	debug << " peRed: "   << pal.peRed
		  << " peGreen: " << pal.peGreen
		  << " peBlue: "  << pal.peBlue
		  << " peFlags: " << pal.peFlags << crlf;
}



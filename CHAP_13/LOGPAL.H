#ifndef __LOGPAL_H__
#define __LOGPAL_H__

//---------------------------------------------------------------------------
//
//	File:			LOGPAL.H
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	LogPalette class definition
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#ifndef __PRECOMP_H__
#include "precomp.h"
#endif

#ifndef __CLASSDEF_H__
#include "classdef.h"
#endif

#ifndef __DEBUG_H__
#include "debug.h"
#endif

class LogPalette
{

private:

	LOGPALETTE FAR*	lpLogPalette;

	static	PALETTEENTRY pal;

	UINT NumBytes( WORD wPalSize ) const
	{
		return wPalSize > 0
			? ( 2 * sizeof( WORD ) ) + ( wPalSize * sizeof( PALETTEENTRY ) )
			: 0;
	}

	UINT NumColors( const LOGPALETTE& pal ) const
	{
		return pal.palNumEntries;
	}

	WORD Size( const LOGPALETTE& logpal ) const
	{
		return logpal.palNumEntries;
	}

	PALETTEENTRY& Color( LOGPALETTE& logpal, UINT index )
	{
		if ( index < Size( logpal ) )
			return logpal.palPalEntry[ index ];
		else
			return pal;
	}

	PALETTEENTRY Color( const LOGPALETTE& logpal, UINT index ) const
	{
		if ( index < Size( logpal ) )
			return logpal.palPalEntry[ index ];
		else
			return pal;
	}

public:

	LogPalette() :
		lpLogPalette( 0 )
	{
	}

	LogPalette( const LogPalette& pal )
	{
		Copy( pal );
	}

	~LogPalette()
	{
		if ( IsCreated() )
			Delete();
	}

	LogPalette& operator = ( const LogPalette& pal )
	{
		Copy( pal );
		return *this;
	}

	BOOL Copy( const LogPalette& pal )
	{
		BOOL fReturn = FALSE;
		if ( ResizeTo( pal.Size() ) )
		{
			if ( lpLogPalette )
				_fmemcpy( lpLogPalette, pal.lpLogPalette, (size_t)NumBytes() );
			fReturn = TRUE;
		}
		return fReturn;
	}

	void Copy( PALETTEENTRY& pal, const RGBQUAD& rgb )
	{
		pal.peRed = rgb.rgbRed;
		pal.peGreen = rgb.rgbGreen;
		pal.peBlue = rgb.rgbBlue;
	}

	BOOL CopyColors( const DIBitmap& bi );

	UINT Colors() const
	{
		return IsCreated() ? NumColors( *lpLogPalette ) : 0;
	}

	BOOL IsCreated() const
	{
		return lpLogPalette != 0;
	}

	PALETTEENTRY& Color( UINT index )
	{
		if ( IsCreated() )
			return Color( *lpLogPalette, index );
		else
			return pal;
	}

	PALETTEENTRY& operator[] ( UINT index )
	{
		return Color( index );
	}

	PALETTEENTRY Color( UINT index ) const
	{
		if ( IsCreated() )
			return Color( *lpLogPalette, index );
		else
			return pal;
	}

	PALETTEENTRY operator[] ( UINT index ) const
	{
		return Color( index );
	}

	BOOL Delete()
	{
		return IsCreated() ? ResizeTo( 0 ) : FALSE;
	}

	LOGPALETTE FAR* Palette() const
	{
		return (LOGPALETTE FAR*)lpLogPalette;
	}

	WORD Size() const
	{
		return lpLogPalette ? Size( *lpLogPalette ) : (WORD)0;
	}

	UINT NumBytes() const
	{
		return NumBytes( Size() );
	}

	BOOL ResizeTo( WORD wNewSize );

	BOOL GetSystemEntries( UINT uStart, UINT uEntries );

	BOOL SetEntryFlags( UINT wStart, UINT wEntries, BYTE peFlags );

	void Dump( dbugstream& debug ) const;
	void Dump( dbugstream& debug, const LOGPALETTE& pal ) const;
	void Dump( dbugstream& debug, const PALETTEENTRY& entry ) const;

};

inline dbugstream& operator << ( dbugstream& debug, const LogPalette& logPal )
{
	logPal.Dump( debug );
	return debug;
}

#endif



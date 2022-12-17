#ifndef __WINGBMP_H__
#define __WINGBMP_H__

//---------------------------------------------------------------------------
//
//	File:			WINGBMP.H
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	WinGBitmap class definition
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

#ifndef __WINGDC_H__
#include "wingdc.h"
#endif

#ifndef __BMPINFO_H__
#include "bmpinfo.h"
#endif

#ifndef __MAP_H__
#include "map.h"
#endif 

#define LINE_SHIFT  5
#define TILE_SHIFT  10

class WinGBitmap
{

friend class WinGDC;

private:

	HBITMAP		hBmp;
	LPBYTE		lpBits;
	WinGDC*		pDC;
	BitmapInfo	bmp;

public:

	WinGBitmap() :
		hBmp( 0 ),
		lpBits( 0 ),
		pDC( 0 )
	{
	}

	WinGBitmap( const WinGDC& dcWinG, const BitmapInfo& bmp  ) :
		hBmp( 0 ),
		lpBits( 0 ),
		pDC( 0 )
	{
		Create( dcWinG, bmp );
	}

	WinGBitmap( const WinGDC& dcWinG, const BITMAPINFO& Header  ) :
		hBmp( 0 ),
		lpBits( 0 ),
		pDC( 0 )
	{
		Create( dcWinG, Header );
	}

	~WinGBitmap()
	{
		if ( IsCreated() )
			Delete();
	}

	BOOL IsCreated() const
	{
		return hBmp != 0;
	}

	BOOL IsSelected() const
	{
		return pDC != 0;
	}

	BOOL Create( const WinGDC& dcWinG, const BitmapInfo& bmp )
	{
		if ( bmp.IsCreated() )
			return Create( dcWinG, *bmp.Info() );
		else
			return FALSE;
	}

	BOOL Create( const WinGDC& dcWinG, const BITMAPINFO& Header );

	BOOL Delete()
	{
		BOOL fReturn = FALSE;
		if ( IsCreated() )
		{

			if ( IsSelected() )
				Deselect();

			fReturn = DeleteBitmap( hBmp );

			if ( fReturn )
			{
				hBmp = 0;
				lpBits = 0;
				fReturn = bmp.Delete();
			}

		}
		return fReturn;
	}

	DWORD Width() const
	{
		return bmp.Width();
	}

	DWORD Height() const
	{
		return bmp.Height();
	}

	DWORD Size() const
	{
		return Height() * Width();
	}

	int Orient() const
	{
    	return bmp.SignedHeight() > 0 ? 1 : -1; 
	}

	BOOL SelectInto( WinGDC& dcWinG );
	BOOL Deselect();

#if defined( TILE1 )
	void BitBlt( const DIBitmap& src, int x, int y );
	void BitBlt( const DIBitmap& src, int destx, int desty,
				 int srcx, int srcy, int srcw, int srch );
	void NoClipBitBlt( const DIBitmap& src, int destx, int desty,
						int srcx, int srcy, int srcw, int srch );
#endif

#if defined( TILE2 ) || defined( TILE3 ) || defined( TILE4 )
	void Tile( const Map& map, const DIBitmap& tiles );
#endif

	void Tile( const Map& map, const DIBitmap& tiles,
			   WORD x, WORD y, WORD w, WORD h );
	void Dump( dbugstream& debug ) const;
	void Dump( dbugstream& debug, LPBYTE lpBits ) const;

	BOOL Set( BYTE color )
	{
		BOOL fReturn = FALSE;
		if ( IsCreated() )
		{
			_hmemset( lpBits, color, Size() );
			fReturn = TRUE;
		}
		return fReturn;			
	}

};

#ifdef TILE4
	extern "C" void FAR PASCAL Tile( LPVOID dest, WORD wdest, WORD hdest, 
						  int orient, LPVOID tiles, LPVOID map, WORD tile_w,
                          WORD tile_h );
#endif

inline dbugstream& operator << ( dbugstream& debug, const WinGBitmap& bmp )
{
	bmp.Dump( debug );
	return debug;
}

#endif



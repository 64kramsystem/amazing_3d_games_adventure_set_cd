#ifndef __BMPINFO_H__
#define __BMPINFO_H__

//---------------------------------------------------------------------------
//
//	File:			BMPINFO.H
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	BitmapInfo class definition
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

#ifndef __GEN_H__
#include "gen.h"
#endif

#ifndef __DEBUG_H__
#include "debug.h"
#endif

#ifndef __WINGTD_H__
#include "wingtd.h"
#endif

class BitmapInfo
{

friend class DIBitmap;
friend class WinGBitmap;

private:

	LPBITMAPINFO lpBmpInfo;

	static	RGBQUAD rgb;

	HINSTANCE Inst() const
	{
		return hInst;
	}

	DWORD NumBytes( DWORD dwColors ) const
	{
		return dwColors > 0
			? sizeof( BITMAPINFOHEADER ) + ( dwColors * sizeof( RGBQUAD ) )
			: 0;
	}

	WORD BitCount( const BITMAPINFOHEADER& bih ) const
	{
		return bih.biBitCount;
	}

	DWORD NumColors( const BITMAPINFO& bi ) const
	{
		return NumColors( bi.bmiHeader );
	}

	DWORD NumColors( const BITMAPINFOHEADER& bih ) const
	{
		return bih.biClrUsed ?
			bih.biClrUsed : 1U << BitCount( bih );
	}

	LONG Width( const BITMAPINFOHEADER& bih ) const
	{
		return ( ( bih.biWidth - 1 ) | 0x03 ) + 1;
	}

	DWORD Size( const BITMAPINFOHEADER& bih ) const
	{
		return bih.biSize;
	}

	DWORD SizeImage( const BITMAPINFOHEADER& bih ) const
	{
		return bih.biSizeImage ? bih.biSizeImage : Width( bih ) * Height( bih );
	}

	LONG Height( const BITMAPINFOHEADER& bih ) const
	{
		LONG lHeight = SignedHeight( bih );
		if ( lHeight < 0 )
			lHeight = -lHeight;
		return ( ( lHeight - 1 ) | 0x03 ) + 1;
	}

	LONG SignedHeight( const BITMAPINFOHEADER& bih ) const
	{
		return bih.biHeight;
	}

	RGBQUAD FAR* ColorTable( const BITMAPINFO& bi ) const
	{
		return (RGBQUAD FAR*)bi.bmiColors;
	}

	RGBQUAD& Color( BITMAPINFO& bi, UINT color )
	{
		if ( NumColors( bi.bmiHeader ) )
			return ColorTable( bi )[ color ];
		else
			return rgb;
	}

	RGBQUAD Color( const BITMAPINFO& bi, UINT color ) const
	{
		if ( NumColors( bi.bmiHeader ) )
			return ColorTable( bi )[ color ];
		else
			return rgb;
	}

public:

	BitmapInfo() :
		lpBmpInfo( 0 )
	{
	}

	BitmapInfo( const BitmapInfo& bi ) :
		lpBmpInfo( 0 )
	{
		Copy( bi );
	}

	BitmapInfo( const BITMAPINFO& bi ) :
		lpBmpInfo( 0 )
	{
		Copy( bi );
	}

	~BitmapInfo()
	{
		if ( IsCreated() )
			Delete();
	}

	BOOL IsCreated() const
	{
		return lpBmpInfo != 0;
	}

	BOOL Delete()
	{
		BOOL fReturn = FALSE;
		if ( IsCreated() )
		{
			GlobalFreePtr( lpBmpInfo );
			lpBmpInfo = 0;
			fReturn = TRUE;
		}
		return fReturn;
	}

	BitmapInfo& operator = ( const BitmapInfo& bi )
	{
		Copy( bi );
		return *this;
	}

	BOOL Copy( const BitmapInfo& bi )
	{
		return bi.IsCreated() ? Copy( *bi.lpBmpInfo ) : FALSE;
	}

	BOOL Copy( const BITMAPINFO& bi )
	{
		if ( ResizeTo( NumColors( bi.bmiHeader ) ) )
		{
			if ( lpBmpInfo )
				_hmemcpy( lpBmpInfo, (LPVOID)&bi,
						  NumBytes( NumColors( bi.bmiHeader )) );
			return TRUE;
		}
		return FALSE;
	}

	BOOL WinGRecommendFormat()
	{
		return IsCreated() ? WinGRecommendDIBFormat( lpBmpInfo ) : FALSE;
	}

	RGBQUAD& Color( UINT index )
	{
		if ( IsCreated() )
			return Color( *lpBmpInfo, index );
		else
			return rgb;
	}

	RGBQUAD& operator[] ( UINT index )
	{
		return Color( index );
	}

	RGBQUAD Color( UINT index ) const
	{
		if ( IsCreated() )
			return Color( *lpBmpInfo, index );
		else
			return rgb;
	}

	RGBQUAD operator[] ( UINT index ) const
	{
		return Color( index );
	}

	RGBQUAD FAR* ColorTable() const
	{
		return lpBmpInfo ? ColorTable( *lpBmpInfo ) : 0;
	}

	operator LPBITMAPINFO() const
	{
		return lpBmpInfo;
	}

	BITMAPINFOHEADER FAR* InfoHeader() const
	{
		return &lpBmpInfo->bmiHeader;
	}

	LPBITMAPINFO Info() const
	{
		return lpBmpInfo;
	}

	BOOL InfoHeader( BITMAPINFOHEADER& bih )
	{
		if ( InfoHeader() )
		{
			bih = *InfoHeader();
			return TRUE;
		}
		return FALSE;
	}

	DWORD Size() const
	{
		return InfoHeader() ? Size( *InfoHeader() ) : 0;
	}

	LONG Width() const
	{
		return InfoHeader() ? Width( *InfoHeader() ) : 0;
	}

	BOOL SetWidth( LONG l )
	{
		BOOL fReturn = FALSE;
		if ( InfoHeader() )
		{
			InfoHeader()->biWidth = l;
			fReturn = TRUE;
		}
		return fReturn;
	}

	LONG Height() const
	{
		LONG lHeight = SignedHeight();
		if ( lHeight < 0 )
			lHeight = -lHeight;
		return lHeight;
	}

	LONG SignedHeight() const
	{
		return InfoHeader() ? SignedHeight( *InfoHeader() ) : 0;
	}

	int Orientation() const
	{
		return SignedHeight() < 0 ? -1 : 1;
	}

	DWORD SizeImage() const
	{
		return InfoHeader() ? SizeImage( *InfoHeader() ) : 0;
	}

	BOOL SetHeight( LONG h )
	{
		BOOL fReturn = FALSE;
		if ( InfoHeader() )
		{
			InfoHeader()->biHeight = h;
			fReturn = TRUE;
		}
		return fReturn;			
	}

	UINT Planes() const
	{
		return InfoHeader() ? InfoHeader()->biPlanes : 0;
	}

	UINT BitCount() const
	{
		return lpBmpInfo ? BitCount( lpBmpInfo->bmiHeader ) : 0;
	}

	DWORD Compression() const
	{
		return InfoHeader() ? InfoHeader()->biCompression : 0;
	}

	UINT Colors() const
	{
		return InfoHeader() ? (WORD)NumColors( *InfoHeader() ) : 0;
	}

	DWORD NumBytes() const
	{
		return NumBytes( Colors() );
	}

	BOOL ResizeTo( DWORD dwNewColors );

	void Copy( RGBQUAD& rgb, const PALETTEENTRY& pal )
	{
		rgb.rgbRed		= pal.peRed;
		rgb.rgbGreen	= pal.peGreen;
		rgb.rgbBlue		= pal.peBlue;
		rgb.rgbReserved	= 0;
	}

	BOOL CopyColors( const LogPalette& pal );

	void Dump( dbugstream& debug ) const;
	void Dump( dbugstream& debug, const BITMAPINFO& bi ) const;
	void Dump( dbugstream& debug, const BITMAPINFOHEADER& bih ) const;
	void Dump( dbugstream& debug, const RGBQUAD& rgb ) const;

};

inline dbugstream& operator << ( dbugstream& debug, const BitmapInfo& bmpInfo )
{
	bmpInfo.Dump( debug );
	return debug;
}

#endif


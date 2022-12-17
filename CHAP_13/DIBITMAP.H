#ifndef __DIBITMAP_H__
#define __DIBITMAP_H__

//---------------------------------------------------------------------------
//
//	File:			DIBITMAP.H
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	DIBitmap class definition
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

#ifndef __BMPINFO_H__
#include "bmpinfo.h"
#endif

#ifndef __GEN_H__
#include "gen.h"
#endif

class DIBitmap
{

private:

	BitmapInfo	bmpInfo;
	LPBYTE		lpBits;

	//
	//	locate the bits for a DIB in "packed" format
	//

	LPBYTE Bits( LPBITMAPINFO lpBmpInfo ) const
	{
		LPBYTE lpBmp = 0;
		if ( lpBmpInfo )
			lpBmp =	(LPBYTE)lpBmpInfo +
						(WORD)bmpInfo.NumBytes( bmpInfo.NumColors( *lpBmpInfo ) );
		return lpBmp;
	}

public:

	DIBitmap() :
		lpBits( 0 )
	{
	}

	DIBitmap( const DIBitmap& bi ) :
		lpBits( 0 )
	{
		Copy( bi );
	}

	DIBitmap( LPCSTR lpszBmp ) :
		lpBits( 0 )
	{
		Load( lpszBmp );
	}

	DIBitmap( UINT uIdBmp ) :
		lpBits( 0 )
	{
		Load( uIdBmp );
	}

	~DIBitmap()
	{
		if ( IsCreated() )
			Delete();
	}

	BOOL IsCreated() const
	{
		return lpBits != 0;
	}

	BOOL Load( LPCSTR lpszBmp );

	BOOL Load( UINT uIdBmp )
	{
		return Load( MAKEINTRESOURCE( uIdBmp ) );
	}

	BOOL Delete()
	{
		if ( IsCreated() )
		{
			GlobalFreePtr( lpBits );
			lpBits = 0;
			return bmpInfo.Delete();
		}
		return FALSE;
	}

	DIBitmap& operator = ( const DIBitmap& bi )
	{
		Copy( bi );
		return *this;
	}

	BOOL Copy( const DIBitmap& bi )
	{
		if ( !IsCreated() && bi.IsCreated() )
		{
			bmpInfo.Copy( bi.bmpInfo );
			lpBits = (LPBYTE)GlobalAllocPtr( GMEM_MOVEABLE, bmpInfo.SizeImage() );
			if ( lpBits && bi.lpBits )
				_hmemcpy( lpBits, bi.lpBits, bmpInfo.SizeImage() );
			return IsCreated();
		}
		return FALSE;
	}

	const BitmapInfo& Info() const
	{
		return bmpInfo;
	}

	DWORD Width() const
	{
		return bmpInfo.Width();
	}

	DWORD Height() const
	{
		return bmpInfo.Height();
	}

    DWORD SizeImage() const
    {
        return bmpInfo.SizeImage();
    }

	LPBYTE Bits() const
	{
		return lpBits;
	}

	BOOL FlipVert();

    BOOL ResizeTo( UINT width, UINT height );

	void Dump( dbugstream& debug ) const
	{
		debug << "Start DIBitmap"            << crlf
			  << " this: " << (void*)this    << crlf
			  << bmpInfo
			  << " Bits: " << (LPVOID)lpBits << crlf
			  << "End DIBitmap"			     << crlf;
	}

};

inline dbugstream& operator << ( dbugstream& debug, const DIBitmap& dib )
{
	dib.Dump( debug );
	return debug;
}

#endif


//---------------------------------------------------------------------------
//
//	File:			DIBITMAP.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:    DIB class implementation
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#ifndef __GEN_H__
#include "gen.h"
#endif

#ifndef __RES_H__
#include "res.h"
#endif

#include "dibitmap.h"

BOOL DIBitmap::Load( LPCSTR lpszDib )
{

	LPBITMAPINFO lpBmpInfo;
	LPBYTE lpBmp;
	BOOL fReturn = FALSE;

	if ( !IsCreated() )
	{

		Resource resDib( lpszDib, RT_BITMAP );

		if ( resDib.IsLocked() )
		{

			lpBmpInfo = (LPBITMAPINFO)resDib.Res();

			if ( ( lpBmp = Bits( lpBmpInfo ) ) != 0 )
			{
				if ( bmpInfo.Copy( *lpBmpInfo ) )
				{
					DWORD size = bmpInfo.SizeImage();
					lpBits = (LPBYTE)GlobalAllocPtr( GMEM_MOVEABLE, size );
					_hmemcpy( lpBits, lpBmp, size );
                    FlipVert();
				}
			}

		}

		fReturn = bmpInfo.IsCreated() && IsCreated();

	}
	return fReturn;
}

BOOL DIBitmap::FlipVert()
{
	if ( IsCreated() )
	{
		BYTE huge* lpLine = (BYTE huge*)GlobalAllocPtr( GHND, Width() );
        BYTE huge* lp = lpBits;
		if ( lpLine )
		{
			for ( int i = 0; i < Height() / 2; i++ )
			{
				_hmemcpy( lpLine, lp+ ( i * Width() ), Width() );
				_hmemcpy( lp + ( i * Width() ),
						  lp + ( ( Height() - i - 1 ) * Width() ),
						  Width() );
				_hmemcpy( lp + ( ( Height() - i - 1 ) * Width() ),
						  lpLine, Width() );
				bmpInfo.SetHeight( -Height() );
			}
			GlobalFreePtr( lpLine );
		}
	}
	return FALSE;
}

BOOL DIBitmap::ResizeTo( UINT width, UINT height )
{

	if ( !width && !height )
		return Delete();

	width = ( ( width - 1 ) | 3 ) + 1;
	LPBYTE lp =
		(LPBYTE)GlobalAllocPtr( GHND, (DWORD)width * (DWORD)height );
	if ( lp )
	{

		if ( lpBits && GlobalFreePtr( lpBits ) )
			return FALSE;

		long h = height;
		if ( !bmpInfo.IsCreated() )
			bmpInfo.ResizeTo( 256 );
		if ( bmpInfo.SignedHeight() < 0 )
			h = -h;
		bmpInfo.SetHeight( h );
		bmpInfo.SetWidth( width );
		lpBits = lp;
		return TRUE;

	}

	return FALSE;

}



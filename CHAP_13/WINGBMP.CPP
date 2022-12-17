//---------------------------------------------------------------------------
//
//	File:			WINGBMP.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	WinGBitmap class
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#ifndef __WINGBMP_H__
#include "wingbmp.h"
#endif

//
//	Use TASM for inline assembler
//
#pragma inline

BOOL WinGBitmap::SelectInto( WinGDC& dcWinG )
{
	return dcWinG.Select( *this );
}

BOOL WinGBitmap::Deselect()
{
	return IsSelected() ? pDC->DeselectBmp() : FALSE;
}

BOOL WinGBitmap::Create( const WinGDC& dcWinG, const BITMAPINFO& Header )
{
	BOOL fReturn = FALSE;
	if ( !IsCreated() )
	{
		hBmp = WinGCreateBitmap( dcWinG.hDC, &Header, &(LPVOID)lpBits );
		if ( IsCreated() )
			bmp.Copy( Header );
		fReturn = IsCreated() && bmp.IsCreated();
	}
	return fReturn;
}

#if defined( TILE1 )
void WinGBitmap::BitBlt( const DIBitmap& src, int destx, int desty )
{
	BitBlt( src, destx, desty, 0, 0, (int)src.Width(), (int)src.Height() );
}

void WinGBitmap::BitBlt( const DIBitmap& src, int destx, int desty,
						 int srcx, int srcy, int srcw, int srch )
{
	//
	//	clip source boundaries
	//
	int width  = max( 0, min( srcw, (int)src.Width()  - srcx ) );
	int height = max( 0, min( srch, (int)src.Height() - srcy ) );
	//
	//	clip dest boundaries
	//
	width  = max( 0, min( width,  (int)Width()  - destx ) );
	height = max( 0, min( height, (int)Height() - desty ) );

	NoClipBitBlt( src, destx, desty, srcx, srcy, width, height );

}

void WinGBitmap::NoClipBitBlt( const DIBitmap& src, int destx, int desty,
								int srcx, int srcy, int w, int h )
{
	for ( int i = 0; i < h; i++ )
	{
		_hmemcpy( (BYTE HUGE*)lpBits   	 + ( (DWORD)Width()	    * (DWORD)( i + desty ) ) + destx,
				  (BYTE HUGE*)src.Bits() + ( (DWORD)src.Width() * (DWORD)( i + srcy  ) ) + srcx,
				  w );
	}
}
#endif

void WinGBitmap::Dump( dbugstream& debug ) const
{
	debug << "Start WinGBitmap"  		 << crlf
		  << " this: "   << (void*)this  << crlf
		  << " hBmp: "   << hBmp 		 << crlf
		  << " pDC: "    << pDC  		 << crlf
		  << bmp;
	Dump( debug, lpBits );
	debug << "End WinGBitmap"    << crlf;
}

void WinGBitmap::Dump( dbugstream& debug, LPBYTE lpBits ) const
{
	debug << " lpBits: " << (LPVOID)lpBits << crlf
		  << " Bits:   ";
	debug << (LPVOID)lpBits      << crlf;
}

#if defined( TILE2 )
void WinGBitmap::Tile( const Map& map, const DIBitmap& tiles )
{

	int orient, i, j, map_width, w, h, tile_start, edge_left,
		inc_line, tile_line;

	LPBYTE lpTiles = tiles.Bits();

	LPBYTE lpMapRow = map.MapBytes();
	LPBYTE lpMapPos;

	BYTE huge* lpBmpLine = lpBits;
	BYTE huge* lpBmpPos;

	map_width     = map.Width();

	w = (int)Width();
	h = (int)Height();

	orient = bmp.SignedHeight() > 0 ? 1 : -1;

	inc_line = w  * orient;

	if ( orient > 0 )
		lpBmpLine = (BYTE huge*)lpBits + ( (DWORD)w * ( h - 1 ) );

	for ( i = 0; i < h; i++  )
	{

		lpMapPos = lpMapRow;
		lpBmpPos = lpBmpLine;

		tile_line = ( i & 0x1F ) << 5;	// ( i % 32 ) * 32

		for ( j = 0; j < w - 32; j += 32 )
		{
			tile_start  = ( *lpMapPos << 10 ) + tile_line;
			_hmemcpy( lpBmpPos, lpTiles + tile_start, 32 );
			lpBmpPos += 32;
			lpMapPos++;
		}

		edge_left = (int)min( 32UL, Width() - j );
		tile_start  = ( *lpMapPos << 10 ) + tile_line;
		_hmemcpy( lpBmpPos, lpTiles + tile_start, edge_left );

		lpBmpLine -= inc_line;

		  if ( ( i & 0xE0 ) && !( i & 0x1F ) )
			lpMapRow  += map_width;

	}
}

#endif

#if defined( TILE3 )
void WinGBitmap::Tile( const Map& map, const DIBitmap& tiles )
{

	int orient, map_width, map_view_w, i, j, k,
		dib_y, dib_w, dib_h, dib_line_dec, edge_right, edge_bottom, tile;

	LPBYTE lpTiles = tiles.Bits();
	LPBYTE lpTileLine;

	LPBYTE lpMapRow = map.MapBytes();
	LPBYTE lpMapPos;

	BYTE huge* lpBmpLine = lpBits;
	BYTE huge* lpBmpTileLine;

	dib_w = (int)Width();
	dib_h = (int)Height();

	map_width     	= map.Width();
	map_view_w 	  	= dib_w >> 5;

	orient = bmp.SignedHeight() > 0 ? 1 : -1;

    dib_line_dec = dib_w * orient; 

	//
	//	scan tile rows in map
	//

	for ( i = 0; i < dib_h; i += 32  )
	{

		dib_y = orient > 0 ? dib_h - i - 1 : i;

														// point to correct dibitmap line
		lpBmpLine = (BYTE huge*)lpBits + ( (DWORD)dib_y * (DWORD)dib_w );

      												// point to current map row
		lpMapPos  = lpMapRow;

		//
		//	for each tile
		//

		for ( j = 0; j < map_view_w; j++ )
		{

			//
			//	draw the tile
			//

			tile = *lpMapPos;					// get tile type;

			lpTileLine = lpTiles + ( tile << 10 );
													// point to correct tile

			lpBmpTileLine = lpBmpLine;		// current dibitmap tile line

			edge_right  = min( 32, dib_w - ( j << 5 ) );
			edge_bottom = min( 32, dib_h - i );

			for ( k = 0; k < edge_bottom; k++ )
			{
				asm
				{
					push	ds
					push	es
					les	di, lpBmpTileLine
					lds	si, lpTileLine
					mov	cx, edge_right
					shr	cx, 1
					rep	movsw
					rcl	cx, 1
					rep	movsb
					pop	es
					pop	ds
				}
				//_hmemcpy( lpBmpTileLine, lpTileLine, edge_right );
				lpTileLine    += 32;					// point to next tile line
				lpBmpTileLine -= dib_line_dec;	// next dibitmap line
			}

			lpMapPos++;							// next tile
			lpBmpLine += 32;					// next dibitmap tile

		}

      lpMapRow += map_width;				// next tile row

	}

}
#endif

void WinGBitmap::Tile( const Map& map, const DIBitmap& tiles,
					   WORD start_x, WORD start_y,
					   WORD w_tiles, WORD h_tiles )
{
	LPBYTE start_map = map[ start_y ] + start_x;
	::Tile( lpBits, (WORD)Width(), (WORD)Height(), Orient(),
			tiles.Bits(), start_map, w_tiles, h_tiles );
}


#ifndef __WINGDC_H__
#define __WINGDC_H__

//---------------------------------------------------------------------------
//
//	File:			WINGDC.H
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	WinGDC class definition.
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

#ifndef __WINGBMP_H__
#include "wingbmp.h"
#endif

#ifndef __DIBITMAP_H__
#include "dibitmap.h"
#endif

class WinGDC
{

friend class WinGBitmap;

private:

	HDC			hDC;
	int			nSaveDC;
	WinGBitmap*	pBmp;
	HBITMAP		hBmpDef;

	BOOL IsSaved() const
	{
		return nSaveDC != 0;
	}

	BOOL Save()
	{
		nSaveDC = SaveDC( hDC );
		return IsSaved();
	}

	BOOL Restore()
	{
		if ( IsCreated() && IsSaved() && RestoreDC( hDC, nSaveDC ) )
		{
			nSaveDC = 0;
			return TRUE;
		}
		else
			return FALSE;
	}

public:

	WinGDC() :
		hDC( 0 ),
		nSaveDC( 0 ),
		pBmp( 0 ),
		hBmpDef( 0 )
	{
		Create();
	}

	~WinGDC()
	{
		if ( IsCreated() )
			Delete();
	}

	BOOL IsCreated() const
	{
		return hDC != 0;
	}

	BOOL HasSelectedBmp() const
	{
		return pBmp != 0;
	}

	BOOL Create()
	{
		BOOL fReturn = FALSE;
		if ( !IsCreated() )
		{
			hDC = WinGCreateDC();
			if ( IsCreated() )
				fReturn = Save();
		}
		return fReturn;
	}

	BOOL Delete()
	{
		BOOL fReturn = FALSE;
		if ( IsCreated() )
		{
			if ( HasSelectedBmp() )
				DeselectBmp();
			if ( IsSaved() )
				Restore();
			if ( DeleteDC( hDC ) )
			{
				hDC = 0;
				fReturn = TRUE;
			}
		}
		return fReturn;
	}

	HDC Handle()
	{
		return hDC;		
	}

	operator HDC()
	{
		return hDC;
	}

	BOOL Select( WinGBitmap& bmpWinG );
	BOOL DeselectBmp();

	BOOL BitBlt( HDC hDCDest, int destx, int desty, int w, int h,
				 int srcx, int srcy )
	{
		return WinGBitBlt( hDCDest, destx, desty, w, h, hDC, srcx, srcy );
	}

	BOOL SetDIBColorTable( const BitmapInfo& bmp )
	{
		if ( IsCreated() && bmp.IsCreated() )
			return WinGSetDIBColorTable( hDC, 0, (UINT)bmp.Colors(),
										 bmp.ColorTable() );
		else
			return FALSE;										 
	}

	void Dump( dbugstream& debug ) const;

};

inline dbugstream& operator << ( dbugstream& debug, const WinGDC& dc )
{
	dc.Dump( debug );
	return debug;
}

#endif


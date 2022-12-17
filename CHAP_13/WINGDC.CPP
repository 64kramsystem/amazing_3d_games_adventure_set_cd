//---------------------------------------------------------------------------
//
//	File:			WINGDC.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	WinGDC class
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#ifndef __WINGDC_H__
#include "wingdc.h"
#endif

#ifndef __WINGBMP_H__
#include "wingbmp.h"
#endif

BOOL WinGDC::Select( WinGBitmap& bmpWinG )
{
	if ( !bmpWinG.IsSelected() && bmpWinG.IsCreated() )
	{
		HBITMAP hBmpOld = SelectBitmap( hDC, bmpWinG.hBmp );
		if ( hBmpOld )
		{
			if ( HasSelectedBmp() )
			{
				if ( hBmpDef == pBmp->hBmp )
				{
					pBmp->pDC = 0;
					pBmp = &bmpWinG;
					return TRUE;
				}
			}
			else
			{
				hBmpDef = hBmpOld;
				bmpWinG.pDC = this;
				pBmp = &bmpWinG;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL WinGDC::DeselectBmp()
{
	if ( HasSelectedBmp() )
	{
		HBITMAP hBmpSel = SelectBitmap( hDC, hBmpDef );
		if ( hBmpSel && hBmpSel == pBmp->hBmp )
		{
			hBmpDef = 0;
			pBmp->pDC = 0;
			pBmp = 0;
			return TRUE;
		}
	}
	return FALSE;
}

void WinGDC::Dump( dbugstream& debug ) const
{
	debug << "Start WinGDC"			 	 << crlf
		  << " this:    " << (void*)this << crlf
		  << " hDC:     " << hDC     	 << crlf
		  << " nSaveDC: " << nSaveDC 	 << crlf
		  << " pBmp:    " << pBmp    	 << crlf
		  << " hBmpDef: " << hBmpDef 	 << crlf
		  << "End WinGDC"			 	 << crlf;
}


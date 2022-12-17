#ifndef __PALETTE_H__
#define __PALETTE_H__

//---------------------------------------------------------------------------
//
//	File:			PALETTE.H
//	Programmer: 	Bob Provencher
//	Create Date:	11/9/94
//	Last Mod Date:	1/2/95
//	Description:	Palette class definition
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

#ifndef __LOGPAL_H__
#include "logpal.h"
#endif

class Palette
{

private:

	LogPalette	LogPal;
	HPALETTE	hPal;

public:

	Palette() :
		hPal( 0 )
	{
	}

	Palette( const Palette& pal ) :
		hPal( 0 )
	{
		Copy( pal );
	}

	Palette( const LogPalette& pal ) :
		hPal( 0 )
	{
		Create( pal );
	}

	~Palette()
	{
		if ( IsCreated() )
			Delete();
	}

	Palette& operator = ( const Palette& pal )
	{
		Copy( pal );
		return *this;
	}

	BOOL Copy( const Palette& pal )
	{
		return Create( pal.LogPal );
	}

	BOOL IsCreated() const
	{
		return hPal != 0;
	}

	BOOL Create( const LogPalette& pal )
	{
		BOOL fReturn = FALSE;
		if ( !IsCreated() && !LogPal.IsCreated() )
		{
			LogPal.Copy( pal );
			fReturn = Create();
		}
		return fReturn;
	}

	BOOL Create()
	{
		BOOL fReturn = FALSE;
		if ( !IsCreated() && LogPal.IsCreated() )
		{
			hPal = CreatePalette( LogPal.Palette() );
			fReturn = IsCreated();
		}
		return fReturn;
	}

	BOOL Delete()
	{
		BOOL fReturn = FALSE;
		if ( IsCreated() && DeletePalette( hPal ) )
		{
			hPal = 0;
			fReturn = TRUE;
		}
        return fReturn;
	}

	HPALETTE Handle() const
	{
		return hPal;
	}

	operator HPALETTE() const
	{
		return Handle();
	}

};

#endif



#ifndef __RES_H__
#define __RES_H__

//--------------------------------------------------------------------------
//
//	File:			RES.H
//	Programmer:		Bob Provencher
//	Create Date: 	12/1/94
//	Last Mod Date:	1/2/95
//	Description:	Resource load class
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//--------------------------------------------------------------------------

#ifndef __PRECOMP_H__
#include "precomp.h"
#endif

#ifndef __GEN_H__
#include "gen.h"
#endif

#ifndef __WINGTD_H__
#include "wingtd.h"
#endif

class Resource
{

private:

	HRSRC		hRsrc;
	HGLOBAL		hGlobal;
	LPVOID		lpRes;

	static HINSTANCE   hInst;

	static HINSTANCE Inst()
	{
		return hInst;
	}

public:

	BOOL IsFound() const
	{
		return hRsrc != 0;
	}

	BOOL IsLoaded() const
	{
		return hGlobal != 0;
	}

	BOOL IsLocked() const
	{
		return lpRes != 0;
	}

	LPVOID Res() const
	{
		return lpRes;
	}

	Resource() :
		hRsrc( 0 ),
		hGlobal( 0 ),
		lpRes( 0 )
	{
	}

	Resource( UINT uName, UINT uType ) :
		hRsrc( 0 ),
		hGlobal( 0 ),
		lpRes( 0 )
	{
		if ( Load( MAKEINTRESOURCE( uName ), MAKEINTRESOURCE( uType ) ) )
			Lock();
	}

	Resource( LPCSTR lpszName, LPCSTR lpszType ) :
		hRsrc( 0 ),
		hGlobal( 0 ),
		lpRes( 0 )
	{
		if ( Load( lpszName, lpszType ) )
			Lock();
	}

	~Resource()
	{
		if ( IsLocked() )
			Unlock();
		if ( IsLoaded() )
			Free();
		if ( IsFound() )
			Lose();
	}

	DWORD Size() const
	{
		if ( IsFound() )
			return ::SizeofResource( Inst(), hRsrc );
		else
        	return 0;
    }

	BOOL Find( LPCSTR lpszName, LPCSTR lpszType )
	{
		if ( !IsFound() )
		{
			hRsrc = ::FindResource( Inst(), lpszName, lpszType );
			return IsFound();
		}
		return FALSE;
	}

	BOOL Lose()
	{
		if ( !IsLoaded() && !IsLocked() )
		{
			hRsrc = 0;
			return TRUE;
		}
		return FALSE;
	}

	BOOL Load()
	{
		if ( IsFound() )
		{
			hGlobal = ::LoadResource( Inst(), hRsrc );
			return IsLoaded();
		}
		return FALSE;
	}

	BOOL Load( LPCSTR lpszName, LPCSTR lpszType )
	{
		if ( !IsLoaded() && !IsFound() )
		{
			if ( Find( lpszName, lpszType ) )
				Load();
			return IsLoaded();
		}
		return FALSE;
	}

	BOOL Lock()
	{
		if ( !IsLocked() && IsLoaded() )
		{
			lpRes = ::LockResource( hGlobal );
			return IsLocked();
		}
		return FALSE;
	}

	BOOL Free()
	{
		if ( IsLoaded() && !IsLocked() )
		{
			if ( ::FreeResource( hGlobal ) )
				hGlobal = 0;
			return !IsLoaded();			
		}
		return FALSE;
	}

	BOOL Unlock()
	{
		if ( IsLocked() )
		{
#ifndef __WIN32__
			if ( ::UnlockResource( hGlobal ) )
#endif			
				lpRes = 0;
			return !IsLocked();
		}
		return FALSE;
	}


};

#endif




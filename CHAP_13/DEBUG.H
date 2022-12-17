#ifndef __DEBUG_H__
#define __DEBUG_H__

//---------------------------------------------------------------------------
//
//  File:           DEBUG.H
//  Programmer:     Bob Provencher
//  Create Date:    12/3/94
//  Last Mod Date:  1/2/95
//  Description:    dbugstream class definition
//
//  Copyright (c) 1994, 1995. Aesir Software, Inc.
//  All Rights Reserved.
//
//---------------------------------------------------------------------------

#ifndef __PRECOMP_H__
#include "precomp.h"
#endif

#ifndef __GEN_H__
#include "gen.h"
#endif

#ifdef __BORLANDC__
extern const char* crlf;
#endif

class dbugstream
{

public:

//
//	common debug stream functions
//

	dbugstream& write( LPCSTR c )
	{
		::OutputDebugString( c );
		return *this;
	}

	dbugstream& write( char c )
	{
		char c2[ 2 ];
		c2[ 0 ] = c;
		c2[ 1 ] = 0;
		return write( c2 );
	}

	dbugstream& write( BYTE b )
	{
		char c[ 4 ];
		sprintf( c, "%03d", (int)b );
		return write( c );
	}

	dbugstream& write( LPBYTE b, WORD len = 16 );

	dbugstream& write( int i )
	{
		char c[ 7 ];
		sprintf( c, "%d", i );
				return write( c );
	}

	dbugstream& write( UINT u )
	{
		char c[ 7 ];
		sprintf( c, "%u", u );
		return write( c );
	}

	dbugstream& write( short s )
	{
		return write( (int)s );
	}

	dbugstream& write( WORD w )
	{
		return write( (UINT)w );
	}

	dbugstream& write( LONG l )
	{
		char c[ 11 ];
		sprintf( c, "%ld", l );
		return write( c );
	}

	dbugstream& write( DWORD dw )
	{
		char c[ 11 ];
		sprintf( c, "%lu", dw );
		return write( c );
	}

	dbugstream& write( HDC h )
	{
		return write( (HANDLE)h );
	}

	dbugstream& write( HBITMAP h )
	{
		return write( (HANDLE)h );
	}

	dbugstream& write_hex( DWORD dw )
	{
		write_hex( HIWORD( dw ) );
		return write_hex( LOWORD( dw ) );
	}

	dbugstream& write_hex( BYTE c );

	dbugstream& write_hex( WORD w )
	{
		write_hex( HIBYTE( w ) );
		return write_hex( LOBYTE( w ) );
	}

//
//	common debug stream operators
//

	dbugstream& operator << ( LPCSTR c )
	{
		return write( c );
	}

	dbugstream& operator << ( char c )
	{
		return write( c );
	}

	dbugstream& operator << ( BYTE c )
	{
		return write( c );
	}

	dbugstream& operator << ( int i )
	{
		return write( i );
	}

	dbugstream& operator << ( UINT u )
	{
		return write( u );
	}

	dbugstream& operator << ( HDC h )
	{
		return write( h );
	}

	dbugstream& operator << ( HBITMAP h )
	{
		return write( h );
	}

	dbugstream& operator << ( LONG l )
	{
		return write( l );
	}

	dbugstream& operator << ( DWORD u )
	{
		return write( u );
	}

	dbugstream& operator << ( LPBYTE b )
	{
		return write( b );
	}


//
//	16-bit Win16 debug stream functions
//

#if !defined( __WIN32__ ) && !defined( M_I386 )

	dbugstream& write( LPVOID vfp )
	{
		write_hex( SELECTOROF( vfp ) );
		write( ':' );
		return write_hex( OFFSETOF( vfp ) );
	}

	dbugstream& write( HANDLE h )
	{
		write( "DS:" );
		return write_hex( (WORD)h );
	}

	dbugstream& write( const char near* c )
	{
		return write( (LPCSTR)c );
	}

	dbugstream& write_hex( UINT u )
	{
		return write_hex( (WORD)u );
	}

	dbugstream& operator << ( LPVOID lpv )
	{
		return write( lpv );
	}

	dbugstream& operator << ( HANDLE h  )
	{
		return write( h );
	}

	dbugstream& operator << ( const char near* c )
	{
		return write( (LPCSTR)c );
	}

#else

//
//      if Win32 or 32-bit Win16
//

	dbugstream& write_hex( UINT u )
	{
		return write_hex( (DWORD)u );
	}

	dbugstream& write( void* vp )
	{
		return write_hex( (DWORD)vp );
	}

	dbugstream& operator << ( void* p  )
	{
		return write( p );
	}

#endif

};

#endif




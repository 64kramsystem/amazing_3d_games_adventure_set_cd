#ifndef __GEN_H__
#define __GEN_H__

//---------------------------------------------------------------------------
//
//  File:           GEN.H
//  Programmer:     Bob Provencher
//  Create Date:    11/27/94
//  Last Mod Date:  1/8/95
//  Description:    general purpose function prototypes
//
//  Copyright (c) 1994, 1995. Aesir Software, Inc.
//  All Rights Reserved.
//
//---------------------------------------------------------------------------

#ifndef __PRECOMP_H__
#include "precomp.h"
#endif

#ifndef __WINGTD_RH__
#include "wingtd.rh"
#endif

#if defined __WIN32__
#define HUGE
#else
#define HUGE    huge
#endif

#if !defined( __WIN32__ ) && !defined( M_I386 )
inline WORD MAKEWORD( BYTE a, BYTE b )
{
	return ( (WORD)a << 8 ) | (WORD)b;
}
#endif

inline DWORD MAKEDWORD( WORD a, WORD b )
{
	return ( (DWORD)a << 16 ) | (DWORD)b ;
}

inline DWORD MAKEDWORD( BYTE a, BYTE b, BYTE c, BYTE d )
{
		return ( (DWORD)MAKEWORD( a, b ) << 16 ) |
					 (DWORD)MAKEWORD( c, d );
}

#ifdef __cplusplus
extern "C" {
#endif

#if defined( __WIN32__ )
#define _hmemcpy        memcpy
#define _hmemset        memset
#else

void FAR PASCAL _hmemcpy( LPVOID dest, LPVOID src, DWORD size );
void FAR PASCAL _hmemset( LPVOID dest, WORD byte, DWORD size );

#endif

#ifdef __cplusplus
}
#endif

#endif


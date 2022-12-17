//---------------------------------------------------------------------------
//
//	File:			MAP.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	12/16/94
//	Last Mod Date:	1/2/95
//	Description:    Map class implementation
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#ifndef __MAP_H__
#include "map.h"
#endif

#ifndef __RES_H__
#include "res.h"
#endif

#ifndef __GEN_H__
#include "gen.h"
#endif

BOOL Map::Load( LPCSTR s )
{

    Resource res( s, MAKEINTRESOURCE( RT_MAP ) );

    if ( res.IsLocked() )
    {

        WORD* p = (WORD*)res.Res();

        DWORD size = (DWORD)p[ 0 ] * (DWORD)p[ 1 ];

        LPBYTE pNewMap = (LPBYTE)GlobalAllocPtr( GMEM_MOVEABLE, size );

        if ( pNewMap )
        {

            Delete();

            _hmemcpy( pNewMap, (LPBYTE)res.Res() + ( 4 * sizeof( WORD ) ), size );

            width    = p[ 0 ];
            height   = p[ 1 ];
            player_x = p[ 2 ];
            player_y = p[ 3 ];

            map = pNewMap;

            return TRUE;

        }

    }

    return FALSE;

}



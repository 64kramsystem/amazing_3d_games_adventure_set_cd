#ifndef __MAP_H__
#define __MAP_H__

//---------------------------------------------------------------------------
//
//	File:	        MAP.H
//	Programmer:     Bob Provencher
//	Create Date:    12/16/94
//	Last Mod Date:  1/2/95
//	Description:    Map class definition
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

#include "tiles.h"

class Map
{

private:

    WORD    width;
    WORD    height;
	WORD    player_x;
	WORD    player_y;
	LPBYTE  map;

public:

    Map() :
        width( 0 ),
        height( 0 ),
        map( 0 )
    {
    }

    Map( UINT u ) :
        width( 0 ),
        height( 0 ),
        map( 0 )
    {
        Load( u );
	}

	Map( LPCSTR s ) :
        width( 0 ),
        height( 0 ),
        map( 0 )
    {
        Load( s );
    }

    ~Map()
    {
        if ( IsLoaded() )
            Delete();
    }

    void Delete()
    {
        if ( map )
            GlobalFreePtr( map );
        map = 0;
        height = width = 0;
	}

	BOOL Load( UINT u )
    {
        return Load( MAKEINTRESOURCE( u ) );
    }

    BOOL Load( LPCSTR res );

    BOOL IsLoaded() const
    {
        return map != 0;
    }

    WORD Width() const
    {
        return width;
    }

    WORD Height() const
    {
        return height;
	}

	LPBYTE MapBytes() const
    {
        return map;
    }

    BOOL Tile( WORD x, WORD y, BYTE& b )
    {
        if ( IsLoaded() && x < Width() && y < Height() )
        {
            b = map[ ( width * y ) + x ];
            return TRUE;
        }
        else
            return FALSE;
    }

	BYTE Tile( WORD x, WORD y )
	{
		BYTE b;
		Tile( x, y, b );
        return b;
	}

	LPBYTE operator [] ( WORD y ) const
    {
        LPBYTE ret = 0;
        if ( IsLoaded() && y < Height() )
			ret = map + ( width * y );
        return ret;
	}

	WORD PlayerX() const
	{
		return player_x;
	}

	WORD PlayerY() const
	{
		return player_y;
	}

	POINT PlayerPos() const
	{
		DWORD d = MAKELONG( player_x, player_y );
		return MAKEPOINT( d );
	}

	BOOL Move( int x, int y )
	{
		int new_x, new_y;
		new_x = (int)player_x + x;
		new_y = (int)player_y + y;
		if ( new_x >= 0 && new_x < width && new_y < height && new_y >= 0 &&
			 Tile( new_x, new_y ) != TILE_WALL )
		{
			*( map + ( player_y * width ) + player_x ) = TILE_FLOOR;
			player_x = new_x;
			player_y = new_y;
			*( map + ( player_y * width ) + player_x ) = TILE_PLAYER;
			return TRUE;
		}
		return FALSE;
	}

	BOOL MoveX( int x )
	{
		return Move( x, 0 );
	}

	BOOL MoveY( int y )
	{
		return Move( 0, y );
	}

};

#endif


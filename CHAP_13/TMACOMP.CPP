//---------------------------------------------------------------------------
//
//	File:			TMACOMP.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	12/5/94
//	Last Mod Date:	1/2/95
//	Description:    Text map compiler
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <iostream.h>
#include <fstream.h>
#include <cstring.h>
#include <stdio.h>

#include "tiles.h"

#define STATE_WIDTH		0
#define STATE_HEIGHT	1
#define STATE_LINES		2
#define STATE_DONE		3
#define STATE_ERROR		4

#define TMAP_FLOOR		' '
#define TMAP_PLAYER		'P'
#define TMAP_WALL		'X'
#define TMAP_GOLD		'G'
#define TMAP_MONSTER	'M'

void Usage()
{
	cout << "Usage: TMACOMP inputfile.tma outputfile.cma\n";
}

int main( int argc, char* argv[] )
{

	string infilename;
	string outfilename;

	if ( argc != 3 )
	{
		Usage();
		return 0;
	}

	infilename  = argv[ 1 ];
	outfilename = argv[ 2 ];

	cout << "Opening " << infilename << "...\n";

	ifstream in( infilename.c_str() );

	if ( !in )
	{
		cout << "Error opening " << infilename << ".\n";
		return 0;
	}

	cout << "Opening " << outfilename << "...\n";

	ofstream out( outfilename.c_str(), ios::out | ios::binary );

	if ( !out )
	{
		cout << "Error opening " << outfilename << ".\n";
		return 0;
	}

	cout << "Allocating input file buffer...\n";

	char* buffer = new char[ 258 ];

	if ( !buffer )
	{
		cout << "Out of memory.\n";
		return 0;
	}

	cout << "Parsing input file...\n";

	int state = 0;

	int line = 1;
	int mapline = 0;

	short width = 0;
	short height = 0;

	short player_x = -1;
	short player_y = -1;

	const char* sWidth  = "width=";
	const char* sHeight = "height=";

	BYTE* map = 0;

	while ( in && state != STATE_ERROR )
	{

		cout << "\r" << line;

		if ( in.getline( buffer, 256 ) )
			line++;
		else if ( in.eof() )
		{
			state = STATE_DONE;
			break;
		}
		else
		{
			cout << "\nI/O error reading " << infilename << ".\n";
			state = STATE_ERROR;
			break;
		}

		if ( buffer[ 0 ] == ';' )
			continue;

		switch ( state )
		{

			case STATE_WIDTH:

				if ( strnicmp( buffer, sWidth, strlen( sWidth ) ) )
				{
					cout << "\nError: missing keyword Width=\n";
					state = STATE_ERROR;
				}
				else
				{
					int r = sscanf( buffer + 6, "%d", &width );
					if ( r == 1 )
						state++;
					else
					{
						cout << "\nError reading Width.\n";
						state = STATE_ERROR;
					}
				}

				break;

			case STATE_HEIGHT:

				if ( strnicmp( buffer, sHeight, strlen( sHeight ) ) )
				{
					cout << "\nError: missing keyword Height=\n";
					state = STATE_ERROR;
				}
				else
				{
					int r = sscanf( buffer + 7, "%d", &height );
					if ( r == 1 )
					{
						state++;
						map = new BYTE[ width * height ];
						if ( map )
							memset( map, 0, width * height );
						else							
						{
							cout << "\nOut of memory.\n";
							state = STATE_ERROR;
						}
					}
					else
					{
						cout << "\nError reading Width.\n";
						state = STATE_ERROR;
					}
				}

				break;

			case STATE_LINES:
			{

				int start = mapline * width;

				for ( int i = 0; state == STATE_LINES && i < width; i++ )
				{

					int x;

					switch ( buffer[ i ] )
					{

						case TMAP_FLOOR:
							x = TILE_FLOOR;
							break;

						case TMAP_PLAYER:
							player_x = i;
							player_y = mapline;
							x = TILE_PLAYER;
							break;

						case TMAP_WALL:
							x = TILE_WALL;
							break;

						case TMAP_GOLD:
							x = TILE_GOLD;
							break;

						case TMAP_MONSTER:
							x = TILE_MONSTER;
							break;

						default:
							cout << "\nError: unknown token '"
								 << buffer[ i ] << "'\n";
							state = STATE_ERROR;
							break;

					}

					map[ start + i ] = x;

				}

				mapline++;

			}


		}

	}

	if ( player_x == -1 || player_y == -1 )
	{
		cout << "Error: Player start not defined.\n";
		state = STATE_ERROR;
	}		

	if ( state == STATE_DONE )
	{
		out.write( (char*)&width, sizeof( short ) );
		out.write( (char*)&height, sizeof( short ) );
		out.write( (char*)&player_x, sizeof( short ) );
		out.write( (char*)&player_y, sizeof( short ) );
		out.write( map, width * height );
		cout << "\nCompilation successful.\n";
	}
	else
		cout << "\nCompilation not successful.\n";

	delete [] buffer;
	delete [] map;

	return 0;

}


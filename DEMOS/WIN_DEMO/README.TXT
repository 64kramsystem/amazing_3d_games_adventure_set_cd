		First 3-D Development (FD)


	This is a MFC Windows based adaptation of the Lary Myers
ACK 3-D engine, highly modified, updated and optimized, using
WinG to achieve up to 18 frames per second.  Plays .MID or .RMI
music, sound FX using .WAV files, low resolution (160 by 100)
and high resolution (320 by 200) modes, screen sizes from 320 by
200 up to 640 by 400, weapons (fists, pistol, shotgun, machine gun
and rocket laucher), doors, keys, medikits, ammo, saved games,
floor squares, floor maps, God mode (double right click on Help
dialog), vertical motion (walking jog and jumping) and more.
Everything you need for a Wolfenstien style game except your
creativity and graphics.  Includes an updated MEDIT map editor.


		Version 1.0

*	Initial release to public domain.


		Version 1.1

What's Changed:

*	Miscellaneous fixes (Major fix for low resolution mode; if
	you got killed, you received the dreaded "General Protection
	Fault." This was due to the screen size parameter passed to
	the TintScreen routine being incorrect).  Other fixes were
	very minor and I term them cosmetic.

*	Fixed God mode so all walls and all objects will be seen in
	map.  Use mouse to move around map.

*	Added notification (via status window) when entering Special
	squares: Start, Up, Down and Goal.

*	Added floor squares.  For full bitmapped floors you'll have to
	wait on Lary Myers new book; I wouldn't spoil it for the world.
	This will give you something to work with and a good feeling
	of down.  It will also let you see how it's done (look at the
	code in ack\ackflrs.cpp).  Floors go especially well with what
	follows...

*	Added vertical movement (Yes, that's right, now you can jog up
	and down while walking.  And you can jump!)  In fact, I added a
	floor level force field object; if you walk through it, it hurts.
	But you can jump over it; with some practice.  Use F2, F3 and F4
	to preview half higher, half lower and normal height, respectively.

*	Added game timing for each level for scoring purposes.  Also,
	saved present time for level with saved game information so you
	can continue a game where you left off.


		Version 1.2

What's Changed:

*	Separated bitmap image information from the Object list information
	in the ACKENG structure.  Only the changeable information is in the
	structure.  Evrything else is in a new structure array.  This
	reduced the size of saved games.

*	Fixed a bug in the draw_object assembly language routine that
	affected light shading over distance.

*	Made the WinG routines always select Top-Down bitmap drawing.  Some
	people saw the screen upside down.  The assembly routines should be
	modified to do either Top-Down or Bottom-Up drawing to maximize
	performance.



John Lundy
71673,1203

PS-	Wanted:  Talented computer artist that can help generate some
	"good" graphics for walls and objects.

      Initial Notes for Windows version of ACK-3D Map Environment

Requirements:

    386 (or 486) CPU w/ 8 Meg
    SVGA card
    Windows 3.1 or later
    WinG.DLL (WinG32.DLL,WinGDE.DLL)
    Win32s installed

Installing the Editor:

    * Copy the files in this directory to your hard drive and change the
      attributes to take off the read-only settings on the files.
	(attrib *.* -r)

    * Copy the Borland *.DLL files into your Windows\System directory. Be sure your
      PATH statement points to this directory. These files are located in the BORLAND
      directory on the companion CD-ROM.

    * Make sure Win32s and WinG are installed on your system. (If you are running
      Windows 95 or Windows NT, you will not need to install Win32s)
      Win32s and WinG are located in the TOOLS directory on the companion CD-ROM.


Running the Editor:

    From the Program Manager select File, Run, and click on ACKEDIT.EXE

Using the Editor:

    There are 3 screens associated with the Editor. They are below;

+------------------+------+
|		   |	  |
|		   |	  |
|		   |  B	  |
|	 A	   |	  |
|		   |	  |
|		   +----------+
|		   |	      |
|		   |	C     |
+------------------+----------+

    A = Overhead view of the Map, similiar to the DOS version. The following
	actions can be taken;
	* Left Mouse -> Places walls or objects in either a side-wall or the
	  entire square.
	* Shift Left Mouse -> Relocates the POV to the square clicked on. The
	  angle doesn't change.
	* Ctrl Left Mouse -> Grabs the bitmap and object and makes it the
	  currently selected.
    B = Bitmap or Object display. The current bitmap is highlighted.
    C = The actual 3D view of the current POV, the following actions,
	* Arrow keys move around like normal
	* Left Mouse -> Clicking around the borders will move the POV.


P.S. - If you have trouble running this program -- try using the executable
file located in the ALT_EDIT directory. It's a little cludgy, but so is 
Windows 95!!!



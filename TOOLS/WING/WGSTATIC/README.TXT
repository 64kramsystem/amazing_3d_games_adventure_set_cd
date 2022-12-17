STATIC - A WinG Sample Application
--------------------------------------------------------------------------
Written by Robert B. Hess


This is a very quick sample application that illustrates what could be
considered a 'minimum' WinG application.

What it does, is create a bitmap, and gets a pointer to the data buffer
associated with the bitmap. It also creates a palette, and properly
associated this palette with the bitmap, and then it randomly selects
a color index for the first pixel in the bitmap, and then proceeds to
walk the entire bitmap, indexing the color of each pixel based on the
first pixel.

To use this as a starting point for your WinG application, you would want
to modify the code that sets the colors of the selected palette (I'm just
using random values), and then you would want to modify the code that is
being used to assign the values of the pixels to be appropriate for your
application.

The files used in this sample are:

The Source Files:
Static.C
Static.H

The Resource File:
Static.RC

Some additional files:
Static.DEF
Static.ICO

'Human Readable' nmake scripts:
MAKEFILE     - 32 bits
MAKEFILE.16  - 16 bits

VC++ Generated nmake scripts:
Static32.MAK - 32 bits
Static16.MAK - 16 bits

The executable images:
Static32.EXE - 32 bits (Intel)
Static16.EXE - 16 bits


Over time, this program will most likely be updated to include new or
more accurate information and methods. Please be sure to check back
with the location that you downloaded this from in order to see if
a newer version exists.


-Robert

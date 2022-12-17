//---------------------------------------------------------------------------
//
//	File:			ABOUT.CPP
//	Programmer: 	Bob Provencher
//	Create Date:	11/6/94
//	Last Mod Date:	1/2/95
//	Description:	About dialog box dialog procedure
//
//	Copyright (c) 1994, 1995. Aesir Software, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "about.h"

BOOL CALLBACK _export About( HWND hDlg, UINT message, WPARAM wParam, 
							 LPARAM )
{

    switch ( message )
    {

        case WM_INITDIALOG:            

            return TRUE;

        case WM_COMMAND:        
       
            if ( wParam == IDOK || wParam == IDCANCEL )
            {
                EndDialog( hDlg, TRUE );
                return TRUE;
            }
            break;

    }

	return FALSE;

}


// stdafx.cpp : source file that includes just the standard includes
// TestHarness.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

void DoEvents()
{
    MSG msg;
    BOOL result;

    while ( ::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        result = ::GetMessage(&msg, NULL, 0, 0);
        if (result == 0) // WM_QUIT
        {                
            ::PostQuitMessage(msg.wParam);
            break;
        }
        else if (result == -1)
        {
             // Handle errors/exit application, etc.
        }
        else 
        {
            ::TranslateMessage(&msg);
            :: DispatchMessage(&msg);
        }
    }
}
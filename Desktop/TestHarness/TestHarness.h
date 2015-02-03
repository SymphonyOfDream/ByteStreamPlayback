
// TestHarness.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTestHarnessApp:
// See TestHarness.cpp for the implementation of this class
//

class CTestHarnessApp : public CWinAppEx
{
public:
	CTestHarnessApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTestHarnessApp theApp;
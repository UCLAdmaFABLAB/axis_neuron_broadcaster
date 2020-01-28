
// demo_MFC.h : main header file for the PROJECT_NAME application
//
#include "afxwin.h"
#pragma once
#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CdemoMFCApp:
// See demo_MFC.cpp for the implementation of this class
//

class CdemoMFCApp : public CWinApp
{
public:
	CdemoMFCApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CdemoMFCApp theApp;

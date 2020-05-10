// Word.h : main header file for the Word application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CWordApp:
// See Word.cpp for the implementation of this class
//

class CWordApp : public CWinApp
{
public:
	CWordApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWordApp theApp;
// ImageScan.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols	 


// CImageScanApp:
// See ImageScan.cpp for the implementation of this class
//

class CImageScanApp : public CWinApp
{
public:
	CImageScanApp(); 	

// Overrides
	public:
	virtual BOOL InitInstance();

	GdiplusStartupInput m_gdiplusStartupInput; 
	ULONG_PTR m_gdiplusToken; 

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CImageScanApp theApp;
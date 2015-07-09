// PicConver.h: interface for the CPicConver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICCONVER_H__6D1BC470_4B1C_4243_8770_BCF1D16B3000__INCLUDED_)
#define AFX_PICCONVER_H__6D1BC470_4B1C_4243_8770_BCF1D16B3000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPicConver  
{
public:
	CPicConver();
	virtual ~CPicConver();

	BOOL MBmpToMImage(CMemFile& cbfBmp, CMemFile& cbfImage, CString strType);	
	BOOL GetImageCLSID(const WCHAR* format, CLSID* pCLSID);	
};

#endif // !defined(AFX_PICCONVER_H__6D1BC470_4B1C_4243_8770_BCF1D16B3000__INCLUDED_)

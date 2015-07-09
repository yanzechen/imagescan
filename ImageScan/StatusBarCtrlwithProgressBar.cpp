// StatusBarCtrlwithProgressBar.cpp : implementation file
//

#include "stdafx.h"
#include "ImageScan.h"
#include "StatusBarCtrlwithProgressBar.h"


// CStatusBarCtrlwithProgressBar

IMPLEMENT_DYNAMIC(CStatusBarCtrlwithProgressBar, CStatusBarCtrl)

CStatusBarCtrlwithProgressBar::CStatusBarCtrlwithProgressBar()
{
	
}

CStatusBarCtrlwithProgressBar::~CStatusBarCtrlwithProgressBar()
{
	
}

BOOL CStatusBarCtrlwithProgressBar::CreateProgressBar()
	{ 
	BOOL bRet = FALSE;	
	CRect rcClt;
	GetClientRect(&rcClt);
	POINT pos;
	SIZE siPros;

	siPros.cx = (int)(rcClt.Width() / 8.0f);
	siPros.cy = (int)((rcClt.Height() * 1) / 2.0f);

	pos.x = rcClt.Width() - siPros.cx - 5;
	pos.y = (int)((rcClt.Height() - siPros.cy) / 1.4f);

	bRet = m_ProgressCtrl.Create(WS_CHILD|PBS_SMOOTH, CRect(pos, siPros), 
		this, 1);

	return bRet;
	}

BOOL CStatusBarCtrlwithProgressBar::DestroyProgressBar()
	{
	BOOL bRet = TRUE;

	if (NULL != m_ProgressCtrl.m_hWnd)
		{
		bRet = m_ProgressCtrl.DestroyWindow();	 
		}

	return bRet;
	}
BEGIN_MESSAGE_MAP(CStatusBarCtrlwithProgressBar, CStatusBarCtrl)
END_MESSAGE_MAP()



// CStatusBarCtrlwithProgressBar message handlers



BOOL CStatusBarCtrlwithProgressBar::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bRet = CStatusBarCtrl::Create(dwStyle, rect, pParentWnd, nID);
	BOOL bRet2 = CreateProgressBar();

	return (bRet&bRet2);
	}

BOOL CStatusBarCtrlwithProgressBar::DestroyWindow()
	{
	// TODO: Add your specialized code here and/or call the base class
	DestroyProgressBar();

	return CStatusBarCtrl::DestroyWindow();
	}

void CStatusBarCtrlwithProgressBar::SetRange( short nLower, short nUpper )
	{
		m_ProgressCtrl.SetRange(nLower, nUpper);
	}

void CStatusBarCtrlwithProgressBar::GetRange( int& nLower, int& nUpper )
	{
		m_ProgressCtrl.GetRange(nLower, nUpper);
	}

int CStatusBarCtrlwithProgressBar::SetPos( int nPos )
	{
	  return m_ProgressCtrl.SetPos(nPos);
	}

int CStatusBarCtrlwithProgressBar::GetPos()
	{
		return m_ProgressCtrl.GetPos();
	}

BOOL CStatusBarCtrlwithProgressBar::SetText( LPCTSTR lpszText, int nPane, int nType, int nProgressPos )
	{
	BOOL bRet = CStatusBarCtrl::SetText(lpszText, nPane, nType);
	m_ProgressCtrl.SetPos(nProgressPos);

		return bRet;
	}

BOOL CStatusBarCtrlwithProgressBar::ShowProgressBar( int nCmdShow )
	{
		return m_ProgressCtrl.ShowWindow(nCmdShow);
	}
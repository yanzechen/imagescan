// ImageViewerFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ImageScan.h"
#include "ImageViewerFrame.h"

#include "ImageViewerView.h"
#include "ImageViewerBar.h"
#include "PicConver.h"


// CImageViewerFrame dialog

IMPLEMENT_DYNAMIC(CImageViewerFrame, CDialog)

CImageViewerFrame::CImageViewerFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CImageViewerFrame::IDD, pParent)	
	{
	m_iFrameWid = 0;
	m_iFrameHei = 0;
	m_iImageOrgWid = 30;
	m_iImageOrgHei = 10;
	m_iImageCurWid = 0;
	m_iImageCurHei = 0;

	m_iBarHei = 10;

	m_pBitmap = NULL;

	m_pView = NULL;
	m_pBar = NULL;

}

CImageViewerFrame::~CImageViewerFrame()
	{
	SAFEDELETE(m_pBitmap);	
	}

void CImageViewerFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CImageViewerFrame::CusLoadImage(CString strImagePath)
	{
	if (strImagePath.IsEmpty())
	{
	return FALSE;
	}

	Bitmap *pImage = NULL;
	pImage = Bitmap::FromFile((LPCWSTR)strImagePath.AllocSysString(), FALSE);
	ASSERT(NULL != pImage);
	if (NULL == pImage)
	{
	return FALSE;
	}

	m_strImagePath = strImagePath;

	return CusLoadImage(pImage); 	   
	}

BOOL CImageViewerFrame::CusLoadImage( Bitmap *pBitmap )
	{
		if (NULL == pBitmap)
		{
		return FALSE;
		}

		SAFEDELETE(m_pBitmap);
		m_pBitmap = pBitmap;	
		m_pView->SetBitmap(m_pBitmap);

		m_iImageOrgWid = pBitmap->GetWidth();
		m_iImageOrgHei = pBitmap->GetHeight();

		SetWindowTitle();		

		static BOOL bIsFrist = TRUE;  
		if (bIsFrist)
		{
		ShowWindow(SW_SHOWMAXIMIZED); 		
		bIsFrist = FALSE;
		}		

		CRect rcBarWnd;
		m_pBar->GetWindowRect(&rcBarWnd);
		if ((int)m_iBarHei < rcBarWnd.Height())
		{
		m_iBarHei = rcBarWnd.Height();
		}

		m_pView->AjustWndSize();
		m_pBar->AjustWndSize();		
		
		m_pView->Invalidate(TRUE);		

		return TRUE;
	}

BEGIN_MESSAGE_MAP(CImageViewerFrame, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CImageViewerFrame message handlers

void CImageViewerFrame::OnClose()
	{
	// TODO: Add your message handler code here and/or call default	
	if (NULL != m_pBitmap && !m_strImagePath.IsEmpty())
		{
		CLSID clImageClsid;	 
		CPicConver PicConver;
		PicConver.GetImageCLSID(L"image/bmp", &clImageClsid);
		m_pBitmap->Save((LPCWSTR)m_strImagePath.AllocSysString(), &clImageClsid);
		SAFEDELETE(m_pBitmap);
		}

	CDialog::OnClose();
	}

BOOL CImageViewerFrame::OnInitDialog()
	{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pBar = new CImageViewerBar;
	ASSERT(NULL != m_pBar);
	m_pView = new CImageViewerView;
	ASSERT(NULL != m_pView);

	m_pBar->Create(IDD_IMAGEVIEWER_BAR, this);
	m_pView->Create(IDD_IMAGEVIEWER_VIEW, this);

	m_pBar->ShowWindow(SW_SHOW);
	m_pView->ShowWindow(SW_SHOW);
	

	// "FrameDlg"初始宽度与高度为物理屏幕(除过任务栏)尺寸
	m_iFrameWid = GetSystemMetrics(SM_CXFULLSCREEN);
	m_iFrameHei = GetSystemMetrics(SM_CYFULLSCREEN);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	}

void CImageViewerFrame::OnSize(UINT nType, int cx, int cy)
	{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (NULL != m_pBar && NULL != m_pView)
		{		
		m_pView->AjustWndSize();
		m_pBar->AjustWndSize();
		m_pView->Invalidate();
		}

	}

Bitmap* CImageViewerFrame::GetBitmap()
	{
	return m_pBitmap;
	}
void CImageViewerFrame::OnMove(int x, int y)
	{
	CDialog::OnMove(x, y);

	// TODO: Add your message handler code here	
	}


void CImageViewerFrame::SetWindowTitle()
	{
	// +++++
	CString strTitle;
	CString strTmp;
	strTitle = _T("Image Viewer---Resolution: ");
	strTmp.Format(_T("%d x %d"), m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
	strTitle += strTmp;
	SetWindowText(strTitle);		
	// -----
	}
void CImageViewerFrame::OnDestroy()
	{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	if (IsWindow(m_pView->m_hWnd))
		{
		m_pView->DestroyWindow();
		delete m_pView;
		m_pView = NULL;
		}

	if (IsWindow(m_pBar->m_hWnd))
		{
		m_pBar->DestroyWindow();
		delete m_pBar;
		m_pBar = NULL;
		}	
	}

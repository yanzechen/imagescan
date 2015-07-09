// ImageViewerView.cpp : implementation file
//

#include "stdafx.h"
#include "ImageScan.h"
#include "ImageViewerView.h"

#include "ImageViewerFrame.h"


// CImageViewerView dialog

IMPLEMENT_DYNAMIC(CImageViewerView, CDialog)

CImageViewerView::CImageViewerView(CWnd* pParent /*=NULL*/)
: CDialog(CImageViewerView::IDD, pParent)
	{
	m_pParentDlg = NULL;
	m_pBitmap = NULL;

	bIsHasHScroll = FALSE;
	bIsHasVScroll = FALSE;

	bIsTrueSizeMode = FALSE;
	}

CImageViewerView::~CImageViewerView()
	{
	}

void CImageViewerView::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	}


BEGIN_MESSAGE_MAP(CImageViewerView, CDialog)
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

void CImageViewerView::AjustWndSize()
	{	
	CRect rcParentClt;
	m_pParentDlg->GetClientRect(&rcParentClt);	

	MoveWindow(rcParentClt.left,
		rcParentClt.top,
		rcParentClt.Width(),
		(rcParentClt.Height()-m_pParentDlg->m_iBarHei),
		TRUE);	

	if (bIsTrueSizeMode)
	{
	ScrollDisplay();
	}

	

	}

// CImageViewerView message handlers

void CImageViewerView::OnPaint()
	{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	if (NULL != m_pBitmap)
		{
		CRect rcClt;
		GetClientRect(&rcClt);

		if (bIsHasHScroll || bIsHasVScroll)
			{
			HBITMAP hNewBitmap, hOldBitmap;
			HDC hdcMemory;

			hdcMemory = CreateCompatibleDC(dc.m_hDC);	
			const Color cl(0, 0, 0);
			m_pBitmap->GetHBITMAP(cl, &hNewBitmap);
			hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hNewBitmap);
			BitBlt(dc.m_hDC, rcClt.left, rcClt.top, rcClt.Width(), rcClt.Height(),
				hdcMemory, GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT), SRCCOPY);
			SelectObject(hdcMemory, hOldBitmap);
			DeleteObject(hNewBitmap);
			DeleteDC(hdcMemory);			
			}
		else
			{
			CRect rcDest;
			double dHeiPerWidRation = 1.0;
			// 缩放显示
			if (rcClt.Width() < (int)m_pBitmap->GetWidth() || rcClt.Height() < (int)m_pBitmap->GetHeight())
				{
				if (NULL != m_pBitmap)
					{
					dHeiPerWidRation = m_pBitmap->GetHeight() / (m_pBitmap->GetWidth() * 1.0);
					}		
				RectCalc(rcClt, dHeiPerWidRation, rcDest);
				}
			// 不缩放显示
			else
				{					
				// 获取"rcContainer"的中心点坐标
				UINT ixMid = rcClt.left + (rcClt.Width() / 2);
				UINT iyMid = rcClt.top + (rcClt.Height() /2);

				rcDest.left = ixMid - (m_pBitmap->GetWidth() / 2);
				rcDest.top = iyMid - (m_pBitmap->GetHeight() / 2);
				rcDest.right = ixMid + (m_pBitmap->GetWidth() / 2);
				rcDest.bottom = iyMid + (m_pBitmap->GetHeight() / 2);				
				}	

			Graphics gc(dc.m_hDC);
			gc.DrawImage(m_pBitmap, (REAL)rcDest.left, (REAL)rcDest.top, (REAL)rcDest.Width(), (REAL)rcDest.Height());
			}
		}
	}

BOOL CImageViewerView::OnInitDialog()
	{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pParentDlg = (CImageViewerFrame *)GetParent();
	ASSERT(NULL != m_pParentDlg);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	}

void CImageViewerView::SetBitmap( Bitmap *pBitmap )
	{
	m_pBitmap = pBitmap;
	ASSERT(NULL != m_pBitmap);
	}
void CImageViewerView::OnMove(int x, int y)
	{
	CDialog::OnMove(x, y);

	// TODO: Add your message handler code here

	}

void CImageViewerView::ImageZoomOut()
	{
	ASSERT(0);
	}

void CImageViewerView::ImageZoomIn()
	{
	ASSERT(0);
	}

void CImageViewerView::ImageLeftRotate()
	{
	if (NULL != m_pBitmap)
		{
		m_pBitmap->RotateFlip(Rotate270FlipNone);
		Invalidate();
		}
	}

void CImageViewerView::ImageRightRotate()
	{
	if (NULL != m_pBitmap)
		{
		m_pBitmap->RotateFlip(Rotate90FlipNone);
		Invalidate();
		}
	}

void CImageViewerView::ImageHMirror()
	{
	if (NULL != m_pBitmap)
		{
		m_pBitmap->RotateFlip(RotateNoneFlipX);
		Invalidate();
		}
	}

void CImageViewerView::ImageVMirror()
	{
	if (NULL != m_pBitmap)
		{
		m_pBitmap->RotateFlip(RotateNoneFlipY);
		Invalidate();
		}
	}

void CImageViewerView::RectCalc(const CRect& rcContainer, const double dHeiPerWidRation, CRect& rcDest)
	{	

	UINT iWid = 0;
	UINT iHei = 0;

	// 图片宽度大于或等于高度
	if (dHeiPerWidRation <= 1)
		{
		if ((rcContainer.Width()*dHeiPerWidRation) < rcContainer.Height())
			{
			iWid = rcContainer.Width();
			}
		else
			{
			iWid = (UINT)(rcContainer.Height() / dHeiPerWidRation);
			}	
		iHei = (UINT)(iWid * dHeiPerWidRation);
		}
	// 图片高度大于宽度
	else
		{
		if ((rcContainer.Height()*(1/dHeiPerWidRation)) < rcContainer.Width())
			{
			iHei = rcContainer.Height();
			}
		else
			{
			iHei = (UINT)(rcContainer.Width() / (1/dHeiPerWidRation));
			}

		iWid = (UINT)(iHei * (1/dHeiPerWidRation));
		}


	UINT iLeft = 0;
	UINT iTop = 0;
	UINT iRight = 0;
	UINT iBottom = 0;

	// 获取"rcContainer"的中心点坐标
	UINT ixMid = rcContainer.left + (rcContainer.Width() / 2);
	UINT iyMid = rcContainer.top + (rcContainer.Height() / 2);

	iLeft = ixMid - (iWid / 2);
	iTop = iyMid - (iHei / 2);
	iRight = ixMid + (iWid / 2);
	iBottom = iyMid + (iHei / 2);

	rcDest.left = iLeft;
	rcDest.top = iTop;
	rcDest.right = iRight;
	rcDest.bottom = iBottom;
	}

void CImageViewerView::ImageTrueSize()
	{
	if (NULL != m_pBitmap)
		{

		bIsTrueSizeMode = TRUE;

		ScrollDisplay();
		}

		Invalidate();
		
	}

void CImageViewerView::ImageProperSize()
	{
	if (NULL != m_pBitmap)
		{
		// 取消滚动条
		// +++++
		if (bIsHasHScroll)
			{
			SCROLLINFO si;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			si.nMin = 0;
			si.nMax = 0;
			si.nPage = 0;			
			si.nPos = 0;
			SetScrollInfo(SB_HORZ, &si, TRUE);	

			bIsHasHScroll = FALSE;
			}

		if (bIsHasVScroll)
			{
			SCROLLINFO si;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			si.nMin = 0;
			si.nMax = 0;
			si.nPage = 0;			
			si.nPos = 0;
			SetScrollInfo(SB_VERT, &si, TRUE);	

			bIsHasVScroll = FALSE;
			}
		// -----

		bIsTrueSizeMode = FALSE;

		Invalidate();
		}
	}
void CImageViewerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
	// TODO: Add your message handler code here and/or call default
	int iOldPos = 0;
	int iNewPos = 0;
	int iOffset = 0;
	int iMinPos = 0;
	int iMaxPos = 0;
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;

	GetScrollInfo(SB_HORZ, &si);			

	iNewPos = iOldPos = si.nPos;
	iMinPos = si.nMin;
	iMaxPos = si.nMax - si.nPage;				

	switch (nSBCode)
		{
		case SB_ENDSCROLL:
			return;
		case SB_LEFT:
			iNewPos = iMinPos;
			break;
		case SB_RIGHT:
			iNewPos = iMaxPos;
			break;
		case SB_LINELEFT:
			iNewPos -= 1;
			break;
		case SB_LINERIGHT:
			iNewPos += 1;
			break;
		case SB_PAGELEFT:
			iNewPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			iNewPos += si.nPage;
			break;
		case SB_THUMBPOSITION:
			iNewPos = nPos;
			break;
		case SB_THUMBTRACK:
			iNewPos = nPos;
			break;				
		default:
			break;
		}

	if (iNewPos < iMinPos)
		{
		iNewPos = iMinPos;
		}
	else if (iNewPos > iMaxPos)
		{
		iNewPos = iMaxPos;
		}
	iOffset = iOldPos - iNewPos;

	SetScrollPos(SB_HORZ, iNewPos, TRUE);
	ScrollWindow(iOffset, 0, NULL, NULL);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

void CImageViewerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
	// TODO: Add your message handler code here and/or call default
	int iOldPos = 0;
	int iNewPos = 0;
	int iOffset = 0;
	int iMinPos = 0;
	int iMaxPos = 0;
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;

	GetScrollInfo(SB_VERT, &si);


	iNewPos = iOldPos = si.nPos;
	iMinPos = si.nMin;
	iMaxPos = si.nMax - si.nPage;

	switch (nSBCode)
		{
		case SB_BOTTOM:
			break;
		case SB_LINEDOWN:
			iNewPos += 1;
			break;
		case SB_LINEUP:
			iNewPos -= 1;
			break;
		case SB_PAGEDOWN:
			iNewPos += si.nPage;
			break;
		case SB_PAGEUP:
			iNewPos -= si.nPage;
			break;
		case SB_THUMBPOSITION:
			iNewPos = nPos;
			break;
		case SB_THUMBTRACK:
			iNewPos = nPos;
			break;
		case SB_TOP:
			break;
		case SB_ENDSCROLL:
			return;
		default:
			break;
		}

	if (iNewPos < iMinPos)
		{
		iNewPos = iMinPos;
		}
	else if (iNewPos > iMaxPos)
		{
		iNewPos = iMaxPos;
		}
	iOffset = iOldPos - iNewPos;

	SetScrollPos(SB_VERT, iNewPos, TRUE);
	ScrollWindow(0, iOffset, NULL, NULL);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
	}

void CImageViewerView::ScrollDisplay()
	{
		{
		// 添加滚动条
		// +++++
		// 真实大小显示
		CRect rcClt;
		GetClientRect(&rcClt);
		bIsHasHScroll = ((int)m_pBitmap->GetWidth() > rcClt.Width());
		bIsHasVScroll = ((int)m_pBitmap->GetHeight() > rcClt.Height());

		if (bIsHasHScroll)
			{
			SCROLLINFO si;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			si.nMin = 0;
			si.nMax = m_pBitmap->GetWidth() - rcClt.Width();
			si.nPage = si.nMax / 16;
			si.nMax += si.nPage;
			si.nPos = 0;
			SetScrollInfo(SB_HORZ, &si, TRUE);		
			}
		else
			{
			SCROLLINFO si;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			si.nMin = 0;
			si.nMax = 0;
			si.nPage = 0;			
			si.nPos = 0;
			SetScrollInfo(SB_HORZ, &si, TRUE);	
			}

		if (bIsHasVScroll)
			{
			SCROLLINFO si;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			si.nMin = 0;
			si.nMax = m_pBitmap->GetHeight() - rcClt.Height();
			si.nPage = si.nMax / 16;
			si.nMax += si.nPage;
			si.nPos = 0;
			SetScrollInfo(SB_VERT, &si, TRUE);
			}
		else
			{
			SCROLLINFO si;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			si.nMin = 0;
			si.nMax = 0;
			si.nPage = 0;			
			si.nPos = 0;
			SetScrollInfo(SB_VERT, &si, TRUE);	
			}
		}
	}
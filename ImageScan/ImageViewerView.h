#pragma once

#include "resource.h"

class CImageViewerFrame;

// CImageViewerView dialog

class CImageViewerView : public CDialog
{
	DECLARE_DYNAMIC(CImageViewerView)

public:
	CImageViewerView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageViewerView();

private:
	CImageViewerFrame *m_pParentDlg;
	Bitmap *m_pBitmap;

	BOOL bIsHasHScroll;
	BOOL bIsHasVScroll;

	BOOL bIsTrueSizeMode;

public:
	void AjustWndSize();
	void SetBitmap(Bitmap *pBitmap);

	void ImageZoomOut();
	void ImageZoomIn();
	void ImageLeftRotate();
	void ImageRightRotate();
	void ImageHMirror();
	void ImageVMirror();

	void ImageTrueSize();		// 真实图片Size显示
	void ImageProperSize();		// 最合适图片Size显示  	

// Dialog Data
	enum { IDD = IDD_IMAGEVIEWER_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);	
	void RectCalc(const CRect& rcContainer, const double dHeiPerWidRation, CRect& rcDest);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	
	void ScrollDisplay();
	};

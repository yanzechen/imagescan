#pragma once

#include "ImageViewerView.h"
#include "ImageViewerBar.h"

// CImageViewerFrame dialog

class CImageViewerFrame : public CDialog
{
	DECLARE_DYNAMIC(CImageViewerFrame)

public:
	CImageViewerFrame(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageViewerFrame();

private:
	UINT m_iFrameWid;				// 记录"FrameDlg"的宽度
	UINT m_iFrameHei;				// 记录"FrameDlg"的高度

	UINT m_iImageCurWid;			// 图片当前宽度
	UINT m_iImageCurHei;			// 图片当前高度

	Bitmap *m_pBitmap;
	CString m_strImagePath;

public:
	UINT m_iImageOrgWid;			// 图片原始宽度
	UINT m_iImageOrgHei;			// 图片原始高度
	CImageViewerView *m_pView;		// 真正图片显示Dlg
	CImageViewerBar	*m_pBar;		// 图片设置工具栏(对图片进行缩放, 旋转等操作)
	UINT m_iBarHei;

	BOOL CusLoadImage(Bitmap *pBitmap);
	BOOL CusLoadImage(CString strImagePath);
	Bitmap* GetBitmap();

// Dialog Data
	enum { IDD = IDD_IMAGEVIEWER_FRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	void SetWindowTitle();
	afx_msg void OnDestroy();
	};

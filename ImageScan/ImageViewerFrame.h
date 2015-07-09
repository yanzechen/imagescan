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
	UINT m_iFrameWid;				// ��¼"FrameDlg"�Ŀ��
	UINT m_iFrameHei;				// ��¼"FrameDlg"�ĸ߶�

	UINT m_iImageCurWid;			// ͼƬ��ǰ���
	UINT m_iImageCurHei;			// ͼƬ��ǰ�߶�

	Bitmap *m_pBitmap;
	CString m_strImagePath;

public:
	UINT m_iImageOrgWid;			// ͼƬԭʼ���
	UINT m_iImageOrgHei;			// ͼƬԭʼ�߶�
	CImageViewerView *m_pView;		// ����ͼƬ��ʾDlg
	CImageViewerBar	*m_pBar;		// ͼƬ���ù�����(��ͼƬ��������, ��ת�Ȳ���)
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

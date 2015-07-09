#pragma once

//#include "ImageViewerFrame.h"
class CImageViewerFrame;

// CImageViewerBar dialog

class CImageViewerBar : public CDialog
{
	DECLARE_DYNAMIC(CImageViewerBar)

public:
	CImageViewerBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageViewerBar();


private:
	CImageViewerFrame *m_pParentDlg;	
	CToolTipCtrl m_ToolTipCtrl;

public:
	void AjustWndSize();

// Dialog Data
	enum { IDD = IDD_IMAGEVIEWER_BAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBnClickedBut1();
	afx_msg void OnBnClickedBut2();
	afx_msg void OnBnClickedBut3();
	afx_msg void OnBnClickedBut4();
	afx_msg void OnBnClickedBut5();
	afx_msg void OnBnClickedBut6();
	void InitToolTipCtrl(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	void InitButtonBitmap();
	afx_msg void OnDestroy();
	};

// ImageViewerBar.cpp : implementation file
//

#include "stdafx.h"
#include "ImageScan.h"
#include "ImageViewerBar.h"

#include "ImageViewerFrame.h"



// CImageViewerBar dialog

IMPLEMENT_DYNAMIC(CImageViewerBar, CDialog)

CImageViewerBar::CImageViewerBar(CWnd* pParent /*=NULL*/)
	: CDialog(CImageViewerBar::IDD, pParent)
{
	m_pParentDlg = NULL;
}

CImageViewerBar::~CImageViewerBar()
{
}

void CImageViewerBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CImageViewerBar::AjustWndSize()
	{	
		CRect rcParentWnd;
//		m_pParentDlg->GetWindowRect(&rcParentWnd);		// Wrong
		m_pParentDlg->GetClientRect(&rcParentWnd);

		CRect rcWnd;
		GetWindowRect(&rcWnd);		

		int ixDest = (rcParentWnd.Width() - rcWnd.Width()) / 2;
		int iyDest = ((m_pParentDlg->m_iBarHei - rcWnd.Height()) / 2) + (rcParentWnd.Height() - m_pParentDlg->m_iBarHei);	

		MoveWindow(ixDest,
			iyDest,
			rcWnd.Width(),
			rcWnd.Height(),
			TRUE);
	}

BEGIN_MESSAGE_MAP(CImageViewerBar, CDialog)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BUT1, &CImageViewerBar::OnBnClickedBut1)
	ON_BN_CLICKED(IDC_BUT2, &CImageViewerBar::OnBnClickedBut2)
	ON_BN_CLICKED(IDC_BUT3, &CImageViewerBar::OnBnClickedBut3)
	ON_BN_CLICKED(IDC_BUT4, &CImageViewerBar::OnBnClickedBut4)
	ON_BN_CLICKED(IDC_BUT5, &CImageViewerBar::OnBnClickedBut5)
	ON_BN_CLICKED(IDC_BUT6, &CImageViewerBar::OnBnClickedBut6)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CImageViewerBar message handlers

BOOL CImageViewerBar::OnInitDialog()
	{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pParentDlg = (CImageViewerFrame *)GetParent();
	ASSERT(NULL != m_pParentDlg);

	// Ĭ��ͼƬ��ʾΪ"����ʴ�С"
	GetDlgItem(IDC_BUT1)->EnableWindow(FALSE);

	// ���ToolTips
	InitToolTipCtrl();

	// ΪButton���ͼƬ
	InitButtonBitmap();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	}

void CImageViewerBar::OnMove(int x, int y)
	{
	CDialog::OnMove(x, y);

	// TODO: Add your message handler code here
	}

void CImageViewerBar::OnBnClickedBut1()
	{
	// TODO: Add your control notification handler code here
	// ����ʴ�С
	m_pParentDlg->m_pView->ImageProperSize();

	GetDlgItem(IDC_BUT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT2)->EnableWindow(TRUE);
	}

void CImageViewerBar::OnBnClickedBut2()
	{
	// TODO: Add your control notification handler code here
	// ��ʵ��С
	m_pParentDlg->m_pView->ImageTrueSize();

	GetDlgItem(IDC_BUT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUT2)->EnableWindow(FALSE);
	}

void CImageViewerBar::OnBnClickedBut3()
	{
	// TODO: Add your control notification handler code here
	// ������תͼ��
	m_pParentDlg->m_pView->ImageLeftRotate();

	//ѡ����ͼ��ֱ��ʱ仯
	m_pParentDlg->SetWindowTitle();			
	}

void CImageViewerBar::OnBnClickedBut4()
	{
	// TODO: Add your control notification handler code here
	// ������תͼ��
	m_pParentDlg->m_pView->ImageRightRotate();

	// ѡ����ͼ��ֱ��ʱ仯
	m_pParentDlg->SetWindowTitle();
	}

void CImageViewerBar::OnBnClickedBut5()
	{
	// TODO: Add your control notification handler code here
	// ˮƽ����ͼ��
	m_pParentDlg->m_pView->ImageHMirror();
	}

void CImageViewerBar::OnBnClickedBut6()
	{
	// TODO: Add your control notification handler code here
	// ��ֱ����ͼ��
	m_pParentDlg->m_pView->ImageVMirror();
	}

void CImageViewerBar::InitToolTipCtrl(void)
	{
	if (!m_ToolTipCtrl.Create(this, TTS_ALWAYSTIP))
		{
		MessageBox(_T("Create tooltipctrl failed!"), NULL, MB_OK | MB_ICONINFORMATION);
		return;
		}

	// ����TipWnd�������
	m_ToolTipCtrl.SetMaxTipWidth(250);

	// ����TipWnd��ʾʱ��Ϊ10s
	m_ToolTipCtrl.SetDelayTime(TTDT_AUTOPOP, 10000);

	// �������ͣ0.5s��, ����TipWnd��ʾ
	m_ToolTipCtrl.SetDelayTime(TTDT_INITIAL, 500);

	// ���Tips
	// +++++
	TCHAR szTip[MAX_PATH] = {0};

	UINT i = 0;
	for (i = 0; i < 6; i++)
	{
	LoadString(NULL, IDS_BAR_PROPERSIZE+i, szTip, MAX_PATH);
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_BUT1+i), szTip);
	}
	// -----

	m_ToolTipCtrl.Activate(TRUE);
	}

void CImageViewerBar::InitButtonBitmap()
	{
		UINT i = 0;
		CButton *pBut = NULL;
		for (i = 0; i < 6; i++)
		{
			pBut = (CButton *)GetDlgItem(IDC_BUT1+i);
			ASSERT(NULL != pBut);
			pBut->SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1+i)));
		}
	}

BOOL CImageViewerBar::PreTranslateMessage(MSG* pMsg)
	{
	// TODO: Add your specialized code here and/or call the base class
	if (NULL != m_ToolTipCtrl.m_hWnd)
	{
	m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
	}

void CImageViewerBar::OnClose()
	{
	// TODO: Add your message handler code here and/or call default	

	CDialog::OnClose();
	}

void CImageViewerBar::OnDestroy()
	{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	if (NULL != m_ToolTipCtrl.m_hWnd)
		{
		m_ToolTipCtrl.Activate(FALSE);
		}
	}

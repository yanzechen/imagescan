// ColorBrightnessAjustment.cpp : implementation file
//

#include "stdafx.h"
#include "ImageScan.h"
#include "ColorBrightnessAjustment.h"	
#include "ImageScanDlg.h"
#include <atlbase.h>


// CColorBrightnessAjustment dialog

IMPLEMENT_DYNAMIC(CColorBrightnessAjustment, CDialog)

CColorBrightnessAjustment::CColorBrightnessAjustment(CWnd* pParent /*=NULL*/)
: CDialog(CColorBrightnessAjustment::IDD, pParent)	 	
	{
	m_iBrightness = 0;
	m_iRGain = 0;
	m_iGGain = 0;
	m_iBGain = 0; 

	m_dwRegRGain = 0;
	m_dwRegGGain = 0;
	m_dwRegBGain = 0;

	int i = 0, j = 0;
	for (i = 0; i < 3; i++)
		{
		for (j = 0; j < 3; j++)
			{
			m_InCMt.CMt[i][j] = 0;
			m_OutCMt.CMt[i][j] = 0;
			}
		}
	}

CColorBrightnessAjustment::~CColorBrightnessAjustment()
	{
	DetachRawParsPointer();
	}

void CColorBrightnessAjustment::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_CBA_BRIHTNESS, m_sldBrightness);
	DDX_Control(pDX, IDC_SLIDER_CBA_RGAIN, m_sldRGain);
	DDX_Control(pDX, IDC_SLIDER_CBA_GGAIN, m_sldGGain);
	DDX_Control(pDX, IDC_SLIDER_CBA_BGAIN, m_sldBGain);
	DDX_Text(pDX, IDC_EDIT_CBA_BRIGHTNESS, m_iBrightness);
	DDV_MinMaxInt(pDX, m_iBrightness, -64, 64);
	DDX_Text(pDX, IDC_EDIT_CBA_RGAIN, m_iRGain);
	DDV_MinMaxInt(pDX, m_iRGain, -6, 6);
	DDX_Text(pDX, IDC_EDIT_CBA_GGAIN, m_iGGain);
	DDV_MinMaxInt(pDX, m_iGGain, -6, 6);
	DDX_Text(pDX, IDC_EDIT_CBA_BGAIN, m_iBGain);
	DDV_MinMaxInt(pDX, m_iBGain, -6, 6);
	}

BEGIN_MESSAGE_MAP(CColorBrightnessAjustment, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUT_EXIT, &CColorBrightnessAjustment::OnBnClickedButExit)
	ON_BN_CLICKED(IDC_BUT_CBA_DEF, &CColorBrightnessAjustment::OnBnClickedButCbaDef)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CColorBrightnessAjustment message handlers

void CColorBrightnessAjustment::InitSliderCtrls()
	{
	m_sldBrightness.SetRange(-64, 64, TRUE);
	m_sldRGain.SetRange(-6, 6, TRUE);
	m_sldGGain.SetRange(-6, 6, TRUE);
	m_sldBGain.SetRange(-6, 6, TRUE);

	m_sldBrightness.SetTicFreq(8);
	m_sldRGain.SetTicFreq(1);
	m_sldGGain.SetTicFreq(1);
	m_sldBGain.SetTicFreq(1);	
	}

void CColorBrightnessAjustment::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
	// TODO: Add your message handler code here and/or call default
	int iID = pScrollBar->GetDlgCtrlID();

	switch (iID)
		{
		case IDC_SLIDER_CBA_BRIHTNESS:
			m_iBrightness = m_sldBrightness.GetPos();
			break;
		case IDC_SLIDER_CBA_RGAIN:
			m_iRGain = m_sldRGain.GetPos();
			break;
		case IDC_SLIDER_CBA_GGAIN:
			m_iGGain = m_sldGGain.GetPos();
			break;
		case IDC_SLIDER_CBA_BGAIN:
			m_iBGain = m_sldBGain.GetPos();
			break;
		} 

	if (IDC_SLIDER_CBA_BRIHTNESS == iID)
		{
		SetBrightness();
		}
	else
		{
		SetRGBGain();
		}

	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

void CColorBrightnessAjustment::AttchRawParsPointer( CRawPars *pPars )
	{
	DetachRawParsPointer();
	ASSERT(NULL != pPars);
	m_kk = pPars;
	}

void CColorBrightnessAjustment::DetachRawParsPointer()
	{
	m_kk = NULL;
	}

void CColorBrightnessAjustment::InitBrightnessAndRGBGain()
	{
	ASSERT(NULL != m_kk);

	CImageScanDlg *pParentDlg = (CImageScanDlg*)GetParent();
	if (NULL == pParentDlg)
		{
		return;
		}
	IBaseFilter *pFilter = pParentDlg->GetSrcFilter();
	if (NULL == pFilter)
		{
		return;
		}

	BOOL bRet = FALSE;
	IAMVideoProcAmp *pProcAmp = NULL;
	HRESULT hr;

	hr = pFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
	if (FAILED(hr))
		{
		return;
		}	

	long Val, FlagBrightness;
	hr = pProcAmp->Get(VideoProcAmp_Brightness, &Val, &FlagBrightness);
	if (FAILED(hr))
		{
		SAFERELEASE(pProcAmp);
		return;
		} 	

	// Read RGBGain from Registry
	//	VERIFY(ReadRGBGainFromRegistry());

	// 非FS_300, FS_500, FS_900设备不支持RGBGain调节
	BOOL bTargetDev = (DEVTYPE_FS_Ohter != pParentDlg->GetDeviceType()); 	
	if (!bTargetDev)
		{
		m_dwRegRGain = 0;
		m_dwRegGGain = 0;
		m_dwRegBGain = 0;
		}

	m_dwRegRGain = 0;
	m_dwRegGGain = 0;
	m_dwRegBGain = 0;

	m_iBrightness = Val;
	m_iRGain = m_dwRegRGain;
	m_iGGain = m_dwRegGGain;
	m_iBGain = m_dwRegBGain;

	m_sldBrightness.SetPos(m_iBrightness);
	m_sldRGain.SetPos(m_iRGain);
	m_sldGGain.SetPos(m_iGGain);
	m_sldBGain.SetPos(m_iBGain); 	

	// 如果"Brightness"是自动控制的, 则说明用户无法通过手动
	// 调节亮度, 因此禁掉"Brightness"滑动条
	GetDlgItem(IDC_SLIDER_CBA_BRIHTNESS)->EnableWindow((VideoProcAmp_Flags_Auto == FlagBrightness) ? FALSE : TRUE);	

	GetDlgItem(IDC_SLIDER_CBA_RGAIN)->EnableWindow(bTargetDev);
	GetDlgItem(IDC_SLIDER_CBA_GGAIN)->EnableWindow(bTargetDev);
	GetDlgItem(IDC_SLIDER_CBA_BGAIN)->EnableWindow(bTargetDev);	

	UpdateData(FALSE);  
	}

BOOL CColorBrightnessAjustment::OnInitDialog()
	{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	ReadRGBGainFromRegistry();

	InitSliderCtrls();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	}

void CColorBrightnessAjustment::OnBnClickedButExit()
	{
	// TODO: Add your control notification handler code here
	WriteRGBGainToRegistry();
	OnOK();
	}

void CColorBrightnessAjustment::OnBnClickedButCbaDef()
	{
	// TODO: Add your control notification handler code here
	m_iBrightness = 0;
	m_iRGain = 0;
	m_iGGain = 0;
	m_iBGain = 0;

	m_sldBrightness.SetPos(m_iBrightness);
	m_sldRGain.SetPos(m_iRGain);
	m_sldGGain.SetPos(m_iGGain);
	m_sldBGain.SetPos(m_iBGain);

	SetBrightness();
	SetRGBGain();

	UpdateData(FALSE);
	} 

HBRUSH CColorBrightnessAjustment::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here	
	pDC->SetTextColor(RGB(0, 0, 0)); 	

	// TODO:  Return a different brush if the default is not desired
	return hbr;
	} 

void CColorBrightnessAjustment::NotifyUpdateInColorMatrix()
	{
	CImageScanDlg *pParentDlg = (CImageScanDlg*)GetParent();
	if (NULL == pParentDlg)
		{
		return;
		}

	BYTE byYR, byYG, byYB;
	BYTE byURH, byURL, byUGH, byUGL, byUBH, byUBL;
	BYTE byVRH, byVRL, byVGH, byVGL, byVBH, byVBL; 
	int iTmp; 	

	pParentDlg->ReadFromASIC(ASIC_YR_ADDR, &byYR);
	pParentDlg->ReadFromASIC(ASIC_YG_ADDR, &byYG);
	pParentDlg->ReadFromASIC(ASIC_YB_ADDR, &byYB);
	pParentDlg->ReadFromASIC(ASIC_URH_ADDR, &byURH);
	pParentDlg->ReadFromASIC(ASIC_URL_ADDR, &byURL);
	pParentDlg->ReadFromASIC(ASIC_UGH_ADDR, &byUGH);
	pParentDlg->ReadFromASIC(ASIC_UGL_ADDR, &byUGL);
	pParentDlg->ReadFromASIC(ASIC_UBH_ADDR, &byUBH);
	pParentDlg->ReadFromASIC(ASIC_UBL_ADDR, &byUBL);
	pParentDlg->ReadFromASIC(ASIC_VRH_ADDR, &byVRH);
	pParentDlg->ReadFromASIC(ASIC_VRL_ADDR, &byVRL);
	pParentDlg->ReadFromASIC(ASIC_VGH_ADDR, &byVGH);
	pParentDlg->ReadFromASIC(ASIC_VGL_ADDR, &byVGL);
	pParentDlg->ReadFromASIC(ASIC_VBH_ADDR, &byVBH);
	pParentDlg->ReadFromASIC(ASIC_VBL_ADDR, &byVBL);

	m_InCMt.CMt[0][0] = byYR;
	m_InCMt.CMt[0][1] = byYG;
	m_InCMt.CMt[0][2] = byYB;

	iTmp = byURL + (byURH<<8);
	m_InCMt.CMt[1][0] = (iTmp > 511) ? (iTmp-1024) : iTmp;

	iTmp = byUGL + (byUGH<<8);
	m_InCMt.CMt[1][1] = (iTmp > 511) ? (iTmp-1024) : iTmp;

	iTmp = byUBL + (byUBH<<8);
	m_InCMt.CMt[1][2] = (iTmp > 511) ? (iTmp-1024) : iTmp;

	iTmp = byVRL + (byVRH<<8);
	m_InCMt.CMt[2][0] = (iTmp > 511) ? (iTmp-1024) : iTmp;

	iTmp = byVGL + (byVGH<<8);
	m_InCMt.CMt[2][1] = (iTmp > 511) ? (iTmp-1024) : iTmp;

	iTmp = byVBL + (byVBH<<8);
	m_InCMt.CMt[2][2] = (iTmp > 511) ? (iTmp-1024) : iTmp; 
	}

BOOL CColorBrightnessAjustment::ReadRGBGainFromRegistry()
	{
	CRegKey regKey;
	LONG lRet = 0;
	CString strRGBGain;
	strRGBGain.LoadString(IDS_REGPATH);

	lRet = regKey.Open(HKEY_LOCAL_MACHINE, strRGBGain);
	if (ERROR_SUCCESS == lRet)
		{
		regKey.QueryDWORDValue(_T("RGain"), m_dwRegRGain);
		regKey.QueryDWORDValue(_T("GGain"), m_dwRegGGain);
		regKey.QueryDWORDValue(_T("BGain"), m_dwRegBGain);
		regKey.Close();
		}

	return TRUE;
	}

BOOL CColorBrightnessAjustment::WriteRGBGainToRegistry()
	{	
	CRegKey regKey;
	LONG lRet = 0;
	CString strRGBGain;
	strRGBGain.LoadString(IDS_REGPATH);

	lRet = regKey.Create(HKEY_LOCAL_MACHINE, strRGBGain);
	if (ERROR_SUCCESS == lRet)
		{
		regKey.SetDWORDValue(_T("RGain"), m_iRGain);
		regKey.SetDWORDValue(_T("GGain"), m_iGGain);
		regKey.SetDWORDValue(_T("BGain"), m_iBGain);
		regKey.Close();
		}

	return TRUE;
	}


void CColorBrightnessAjustment::OnClose()
	{
	// TODO: Add your message handler code here and/or call default	
	VERIFY(WriteRGBGainToRegistry());

	CDialog::OnClose();
	}

void CColorBrightnessAjustment::SetBrightness()
	{
	CImageScanDlg *pParentDlg = (CImageScanDlg*)GetParent();
	if (NULL == pParentDlg)
		{
		return;
		}

	// Set Brightness to Preview
	// +++++
	IBaseFilter *pFilter = pParentDlg->GetSrcFilter();
	if (NULL == pFilter)
		{
		return;
		}

	BOOL bRet = FALSE;
	IAMVideoProcAmp *pProcAmp = NULL;
	HRESULT hr;

	hr = pFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
	if (FAILED(hr))
		{
		return;
		}

	hr = pProcAmp->Set(VideoProcAmp_Brightness, m_iBrightness, VideoProcAmp_Flags_Manual);
	if (FAILED(hr))
		{
		SAFERELEASE(pProcAmp);
		return;
		}
	// ----- 

	// Set Brightness to Snapshot
	m_kk->Brightness = m_iBrightness; 	

#if MyNotifySetDlgUpdate
	pParentDlg->m_SettingDlg.NotifyUpdateRawPars();
#endif
	}

void CColorBrightnessAjustment::SetRGBGain()
	{
	CImageScanDlg *pParentDlg = (CImageScanDlg*)GetParent();
	if (NULL == pParentDlg)
		{
		return;
		} 
#if 1  	
	double RGBGain[3] = {
		1 + m_iRGain/32.0,
		1 + m_iGGain/32.0,
		1 + m_iBGain/32.0
		};

	// OutColorMatrix = InColorMatrix * RGBGainMatrix
	int h = 0, w = 0, i = 0;
	for (h = 0; h < 3; h++)
		{
		for (w = 0; w < 3; w++)
			{
			m_OutCMt.CMt[h][w] = 0;
			}
		}
	for (w = 0; w < 3; w++)
		{
		m_OutCMt.CMt[0][w] = (int) (m_InCMt.CMt[0][w] * RGBGain[w]);
		m_OutCMt.CMt[1][w] = (int) (m_InCMt.CMt[1][w] * RGBGain[w]);
		m_OutCMt.CMt[2][w] = (int) (m_InCMt.CMt[2][w] * RGBGain[w]); 		
		}

	BYTE byYR, byYG, byYB;
	BYTE byURH, byURL, byUGH, byUGL, byUBH, byUBL;
	BYTE byVRH, byVRL, byVGH, byVGL, byVBH, byVBL; 
	int iTmp;

	byYR = m_OutCMt.CMt[0][0];
	byYG = m_OutCMt.CMt[0][1];
	byYB = m_OutCMt.CMt[0][2];

	iTmp = (m_OutCMt.CMt[1][0] < 0) ? (1024+m_OutCMt.CMt[1][0]) : m_OutCMt.CMt[1][0];
	byURH = (iTmp&0x300)>>8;
	byURL = iTmp&0x0FF;

	iTmp = (m_OutCMt.CMt[1][1] < 0) ? (1024+m_OutCMt.CMt[1][1]) : m_OutCMt.CMt[1][1];
	byUGH = (iTmp&0x300)>>8;
	byUGL = iTmp&0x0FF;

	iTmp = (m_OutCMt.CMt[1][2] < 0) ? (1024+m_OutCMt.CMt[1][2]) : m_OutCMt.CMt[1][2];
	byUBH = (iTmp&0x300)>>8;
	byUBL = iTmp&0x0FF;

	iTmp = (m_OutCMt.CMt[2][0] < 0) ? (1024+m_OutCMt.CMt[2][0]) : m_OutCMt.CMt[2][0];
	byVRH = (iTmp&0x300)>>8;
	byVRL = iTmp&0x0FF;

	iTmp = (m_OutCMt.CMt[2][1] < 0) ? (1024+m_OutCMt.CMt[2][1]) : m_OutCMt.CMt[2][1];
	byVGH = (iTmp&0x300)>>8;
	byVGL = iTmp&0x0FF;

	iTmp = (m_OutCMt.CMt[2][2] < 0) ? (1024+m_OutCMt.CMt[2][2]) : m_OutCMt.CMt[2][2];
	byVBH = (iTmp&0x300)>>8;
	byVBL = iTmp&0x0FF;

	pParentDlg->WriteToASIC(ASIC_YR_ADDR, byYR);
	pParentDlg->WriteToASIC(ASIC_YG_ADDR, byYG);
	pParentDlg->WriteToASIC(ASIC_YB_ADDR, byYB);
	pParentDlg->WriteToASIC(ASIC_URH_ADDR, byURH);
	pParentDlg->WriteToASIC(ASIC_URL_ADDR, byURL);
	pParentDlg->WriteToASIC(ASIC_UGH_ADDR, byUGH);
	pParentDlg->WriteToASIC(ASIC_UGL_ADDR, byUGL);
	pParentDlg->WriteToASIC(ASIC_UBH_ADDR, byUBH);
	pParentDlg->WriteToASIC(ASIC_UBL_ADDR, byUBL);
	pParentDlg->WriteToASIC(ASIC_VRH_ADDR, byVRH);
	pParentDlg->WriteToASIC(ASIC_VRL_ADDR, byVRL);
	pParentDlg->WriteToASIC(ASIC_VGH_ADDR, byVGH);
	pParentDlg->WriteToASIC(ASIC_VGL_ADDR, byVGL);
	pParentDlg->WriteToASIC(ASIC_VBH_ADDR, byVBH);
	pParentDlg->WriteToASIC(ASIC_VBL_ADDR, byVBL);

	pParentDlg->WriteToASIC(ASIC_RGBGAINEN_ADDR, 0x01);			// 写生效 
	// -----
#endif

	// Set RGBGain to Snapshot
	for (h = 0; h < 3; h++)
		{
		for (w = 0; w < 3; w++)
			{
			m_kk->OutMatrix[h*3+w] = m_OutCMt.CMt[h][w];
			}
		}

#if MyNotifySetDlgUpdate
	pParentDlg->m_SettingDlg.NotifyUpdateRawPars();
#endif
}

BOOL CColorBrightnessAjustment::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::PreTranslateMessage(pMsg);
}

void CColorBrightnessAjustment::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnTimer(nIDEvent);
}

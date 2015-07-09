#pragma once
#include "afxcmn.h"	  
#include "RawPars.h"

// CColorBrightnessAjustment dialog

class CColorBrightnessAjustment : public CDialog
{
	DECLARE_DYNAMIC(CColorBrightnessAjustment)

public:
	CColorBrightnessAjustment(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorBrightnessAjustment();

// Dialog Data
	enum { IDD = IDD_COLORBRIGHTNESSAJUSTMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CSliderCtrl m_sldBrightness;
	CSliderCtrl m_sldRGain;
	CSliderCtrl m_sldGGain;
	CSliderCtrl m_sldBGain;

	int m_iBrightness;
	int m_iRGain;
	int m_iGGain;
	int m_iBGain;

	DWORD m_dwRegRGain;
	DWORD m_dwRegGGain;
	DWORD m_dwRegBGain;

	CRawPars *m_kk;
	MyColorMatrix m_InCMt;
	MyColorMatrix m_OutCMt;

	BOOL ReadRGBGainFromRegistry();
	BOOL WriteRGBGainToRegistry();

protected:
	void InitSliderCtrls();	 	 

public:
	void AttchRawParsPointer( CRawPars *pPars );		
	void DetachRawParsPointer();
	void SetBrightness();
	void SetRGBGain();
	void InitBrightnessAndRGBGain();  

	void NotifyUpdateInColorMatrix();

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);   
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButExit();
	afx_msg void OnBnClickedButCbaDef();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent); 
};

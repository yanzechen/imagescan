#pragma once

#include "RawPars.h"  

extern CRawPars gPars;

// CSetting dialog

class CSetting : public CDialog
{
	DECLARE_DYNAMIC(CSetting)

public:
	CSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetting();

// Dialog Data
	enum { IDD = IDD_SETTING };		

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButReset();
	afx_msg void OnBnClickedButSet();  
	afx_msg void OnBnClickedRadioYuv();
	afx_msg void OnBnClickedRadioY();
	afx_msg void OnBnClickedRadioU();
	afx_msg void OnBnClickedRadioV(); 
	afx_msg void OnBnClickedCheckReversegamma(); 	
	afx_msg void OnBnClickedCheckRgbgamma();
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar); 
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);   
	afx_msg void OnEnChangeEditGammaration();
	afx_msg void OnEnChangeEditBrightness();
	afx_msg void OnEnChangeEditContrast();
	afx_msg void OnEnChangeEditSaturation();
	afx_msg void OnEnChangeEditHue();
	afx_msg void OnEnGammaChange(UINT uiId);
	afx_msg void OnEnInputColorMatrixChange(UINT uiId);	
	afx_msg void OnEnChangeEditEdgeGain();
	afx_msg void OnEnChangeEditRgain();
	afx_msg void OnEnChangeEditGgain();
	afx_msg void OnEnChangeEditBgain();
	afx_msg void OnEnChangeEditRoffset();
	afx_msg void OnEnChangeEditGoffset();
	afx_msg void OnEnChangeEditBoffset();
	afx_msg void OnEnChangeEditUvgain1();
	afx_msg void OnEnChangeEditUvgain2();
	afx_msg void OnEnChangeEditUvgain3();
	afx_msg void OnEnChangeEditUvgain4();
	afx_msg void OnEnChangeEditEdgethd();
	afx_msg void OnEnChangeEditEdgeythd();
	afx_msg void OnEnChangeEditEdgeythd2();
	afx_msg void OnEnChangeEditYhb();
	afx_msg void OnEnChangeEditYlb();
	
	virtual BOOL OnInitDialog();

	void SetDevType(CUSTOMDEVICETYPE cusDevType);	 

	void AttchRawParsPointer( CRawPars *pPars );		
	void DetachRawParsPointer();
	void NotifyUpdateRawPars();
		

protected:
	void SetYUVChannel();
	void SavePars();
	void LoadRawPars(); 
	void LoadRGBGammaPars();
	void LoadYGammaPars();
	void UpdateRGBGammaUI();
	void UpdateYGammaUI();
	void SaveRGBGammaUI();
	void SaveYGammaUI();
	void UpdateMatrix(CRawPars *pPars);		
	void UpdateOutputColorMatrix();
	void InitSliderCtrls();
	void UpdateRawDataUI();
	void GammaGroupCtrlTextChange(BOOL bRGBGamma = FALSE);


	// Control var
	// +++++
	double m_dGammaVal;
	BOOL m_bGammaEn;
	BOOL m_bReverseGamma;
	BOOL m_bUVGainEn;

	int m_iBrightness;
	int m_iContrast; 
	int m_iEdgeGain;
	double m_dSat;
	double m_dHue; 	
	
	int m_iRGain;
	int m_iGGain;
	int m_iBGain;

	int m_iROffset;
	int m_iGOffset;
	int m_iBOffset;

	int m_iUVGain1;
	int m_iUVGain2;
	int m_iUVGain3;
	int m_iUVGain4;

	int m_iYEdgeHB;
	int m_iYEdgeLB;
	int m_iEdgeThd;

	CSliderCtrl m_sldBrightness;
	CSliderCtrl m_sldContrast;
	CSliderCtrl m_sldSat;
	CSliderCtrl m_sldHue; 
	CSliderCtrl m_sldEdgeGain;
	CSliderCtrl m_sldRGain;	
	CSliderCtrl m_sldGGain;
	CSliderCtrl m_sldBGain;
	CSliderCtrl m_sldROffset;
	CSliderCtrl m_sldGOffset;
	CSliderCtrl m_sldBOffset;
	CSliderCtrl m_sldUVGain1;
	CSliderCtrl m_sldUVGain2;
	CSliderCtrl m_sldUVGain3;
	CSliderCtrl m_sldUVGain4;
	CSliderCtrl m_sldYEdgeHB;
	CSliderCtrl m_sldYEdgeLB;
	CSliderCtrl m_sldEdgeThd;
	CSliderCtrl m_sldYHB;
	CSliderCtrl m_sldYLB;

	// 输入Color Matrix
	int iMatrixY1;
	int iMatrixY2;
	int iMatrixY3;
	int iMatrixU1;
	int iMatrixU2;
	int iMatrixU3;
	int iMatrixV1;
	int iMatrixV2;
	int iMatrixV3;

	// 输出Color Matrix
	// 输出Color Matrix最终计算值为: 
	// 例: (iMatrixOY1 * (1/64))
	int iMatrixOY1;
	int iMatrixOY2;
	int iMatrixOY3;
	int iMatrixOU1;
	int iMatrixOU2;
	int iMatrixOU3;
	int iMatrixOV1;
	int iMatrixOV2;
	int iMatrixOV3;
	// ----- 	
	
	int iGammaIdx;
	YUVChannelType m_YUVChannel;
	BYTE m_RGBGammaNode[RGBGNODENUM];
	BYTE m_YGammaNode[YGNODENUM];
	CUSTOMDEVICETYPE m_DevType;
	double m_dRGBGammaVal;
	double m_dYGammaVal;
	BOOL m_bRGBGammaEn;
	BOOL m_bYGammaEn;
	BOOL m_bRGBReverseGamma;
	BOOL m_bYEdgeEnhanceEn; 
	BOOL m_bYUVProcEn;
	BYTE m_YHB;
	BYTE m_YLB;
	NEGATIVEIMPType m_negativeImpType;	

	CRawPars *m_RawPars;
};


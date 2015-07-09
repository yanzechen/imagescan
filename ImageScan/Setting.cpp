// Setting.cpp : implementation file
//

#include "stdafx.h"
#include "ImageScan.h"
#include "Setting.h"
#include "RawPars.h"
#include "ImageScanDlg.h" 
#include "ReadWriteASIC.h"
#include <math.h>


// CSetting dialog

IMPLEMENT_DYNAMIC(CSetting, CDialog)

CSetting::CSetting(CWnd* pParent /*=NULL*/)
: CDialog(CSetting::IDD, pParent) 
	{ 
	m_dGammaVal = 0.0;
	m_bGammaEn = FALSE;
	m_bReverseGamma = FALSE;
	m_bUVGainEn = FALSE;

	m_iBrightness = 0;
	m_iContrast = 0;
	m_iEdgeGain = 0;
	m_dSat = 0.0;
	m_dHue = 0.0;

	m_iRGain = 0;
	m_iGGain = 0;
	m_iBGain = 0;

	m_iROffset = 0;
	m_iGOffset = 0;
	m_iBOffset = 0;

	m_iUVGain1 = 0;
	m_iUVGain2 = 0;
	m_iUVGain3 = 0;
	m_iUVGain4 = 0;

	m_iYEdgeHB = 0;
	m_iYEdgeLB = 0;
	m_iEdgeThd = 0;

	// Input Color Matrix
	iMatrixY1 = 0;
	iMatrixY2 = 0;
	iMatrixY3 = 0;
	iMatrixU1 = 0;
	iMatrixU2 = 0;
	iMatrixU3 = 0;
	iMatrixV1 = 0;
	iMatrixV2 = 0;
	iMatrixV3 = 0;

	// Output Color Matrix
	iMatrixOY1 = 0;
	iMatrixOY2 = 0;
	iMatrixOY3 = 0;
	iMatrixOU1 = 0;
	iMatrixOU2 = 0;
	iMatrixOU3 = 0;
	iMatrixOV1 = 0;
	iMatrixOV2 = 0;
	iMatrixOV3 = 0;

	iGammaIdx = -1;
	m_YUVChannel = YUV_YUV;

	int i = 0;
	for (i = 0; i < RGBGNODENUM; i++)
		{
		m_RGBGammaNode[i] = 0;
		}
	for (i = 0; i < YGNODENUM; i++)
		{
		m_YGammaNode[i] = 0;
		}

	m_DevType = DEVTYPE_FS_900;
	m_dRGBGammaVal = 1.0;
	m_dYGammaVal = 1.0;
	m_bRGBGammaEn = FALSE;
	m_bRGBReverseGamma = FALSE;
	m_bYGammaEn = FALSE;
	m_bYEdgeEnhanceEn = FALSE;	

	m_bYUVProcEn = TRUE;
	m_YHB = 0;
	m_YLB = 0;
	m_negativeImpType = NEGATIVEIMP_ReverseRGBGamma;

	m_RawPars = NULL;
	}

CSetting::~CSetting()
	{
	   DetachRawParsPointer();
	}

void CSetting::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_GAMMAENABLE, m_bGammaEn);
	DDX_Check(pDX, IDC_CHECK_REVERSEGAMMA, m_bReverseGamma);
	DDX_Check(pDX, IDC_CHECK_YEDGEENHANCEEN, m_bYEdgeEnhanceEn);
	DDX_Check(pDX, IDC_CHECK_UVGAINEN, m_bUVGainEn);
	DDX_Check(pDX, IDC_CHECK_YUVPROCENABLE, m_bYUVProcEn);

	DDX_Text(pDX, IDC_EDIT_GAMMARATION, m_dGammaVal); 
	DDV_MinMaxDouble(pDX, m_dGammaVal, 0.00, 2.00);	 	
	DDX_Text(pDX, IDC_EDIT_BRIGHTNESS, m_iBrightness);
	DDX_Text(pDX, IDC_EDIT_CONTRAST, m_iContrast);
	DDX_Text(pDX, IDC_EDIT_EDGEGAIN, m_iEdgeGain);
	DDX_Text(pDX, IDC_EDIT_SATURATION, m_dSat);
	DDV_MinMaxDouble(pDX, m_dSat, 0.00, 8.00);
	DDX_Text(pDX, IDC_EDIT_HUE, m_dHue);
	DDV_MinMaxDouble(pDX, m_dHue, -180.0, 180.0);
	DDX_Text(pDX, IDC_EDIT_MATRIXY_Y1, iMatrixY1);
	DDX_Text(pDX, IDC_EDIT_MATRIX_Y2, iMatrixY2);
	DDX_Text(pDX, IDC_EDIT_MATRIX_Y3, iMatrixY3);
	DDX_Text(pDX, IDC_EDIT_MATRIX_U1, iMatrixU1);
	DDX_Text(pDX, IDC_EDIT_MATRIX_U2, iMatrixU2);
	DDX_Text(pDX, IDC_EDIT_MATRIX_U3, iMatrixU3);
	DDX_Text(pDX, IDC_EDIT_MATRIX_V1, iMatrixV1);
	DDX_Text(pDX, IDC_EDIT_MATRIX_V2, iMatrixV2);
	DDX_Text(pDX, IDC_EDIT_MATRIX_V3, iMatrixV3);
	DDX_Text(pDX, IDC_EDIT_MATRIXY_OY1, iMatrixOY1);
	DDX_Text(pDX, IDC_EDIT_MATRIX_OY2, iMatrixOY2);
	DDX_Text(pDX, IDC_EDIT_MATRIX_OY3, iMatrixOY3);
	DDX_Text(pDX, IDC_EDIT_MATRIX_OU1, iMatrixOU1);
	DDX_Text(pDX, IDC_EDIT_MATRIX_OU2, iMatrixOU2);
	DDX_Text(pDX, IDC_EDIT_MATRIX_OU3, iMatrixOU3);
	DDX_Text(pDX, IDC_EDIT_MATRIX_OV1, iMatrixOV1);
	DDX_Text(pDX, IDC_EDIT_MATRIX_OV2, iMatrixOV2);
	DDX_Text(pDX, IDC_EDIT_MATRIX_OV3, iMatrixOV3);
	DDX_Text(pDX, IDC_EDIT_RGAIN, m_iRGain);
	DDX_Text(pDX, IDC_EDIT_GGAIN, m_iGGain);
	DDX_Text(pDX, IDC_EDIT_BGAIN, m_iBGain);
	DDX_Text(pDX, IDC_EDIT_ROFFSET, m_iROffset);
	DDX_Text(pDX, IDC_EDIT_GOFFSET, m_iGOffset);
	DDX_Text(pDX, IDC_EDIT_BOFFSET, m_iBOffset);
	DDX_Text(pDX, IDC_EDIT_UVGAIN1, m_iUVGain1);
	DDX_Text(pDX, IDC_EDIT_UVGAIN2, m_iUVGain2);
	DDX_Text(pDX, IDC_EDIT_UVGAIN3, m_iUVGain3);
	DDX_Text(pDX, IDC_EDIT_UVGAIN4, m_iUVGain4);
	DDX_Text(pDX, IDC_EDIT_EDGEYTHD, m_iYEdgeHB);
	DDX_Text(pDX, IDC_EDIT_EDGEYTHD2, m_iYEdgeLB);
	DDX_Text(pDX, IDC_EDIT_EDGETHD, m_iEdgeThd);
	DDX_Text(pDX, IDC_EDIT_YHB, m_YHB);
	DDX_Text(pDX, IDC_EDIT_YLB, m_YLB);

	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS, m_sldBrightness);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST, m_sldContrast);	
	DDX_Control(pDX, IDC_SLIDER_SATURATION, m_sldSat);
	DDX_Control(pDX, IDC_SLIDER_HUE, m_sldHue);	
	DDX_Control(pDX, IDC_SLIDER_EDGEGAIN, m_sldEdgeGain);
	DDX_Control(pDX, IDC_SLIDER_RGAIN, m_sldRGain);
	DDX_Control(pDX, IDC_SLIDER_GGAIN, m_sldGGain);
	DDX_Control(pDX, IDC_SLIDER_BGAIN, m_sldBGain);
	DDX_Control(pDX, IDC_SLIDER_ROFFSET, m_sldROffset);
	DDX_Control(pDX, IDC_SLIDER_GOFFSET, m_sldGOffset);
	DDX_Control(pDX, IDC_SLIDER_BOFFSET, m_sldBOffset);	
	DDX_Control(pDX, IDC_SLIDER_UVGAIN1, m_sldUVGain1);
	DDX_Control(pDX, IDC_SLIDER_UVGAIN2, m_sldUVGain2);
	DDX_Control(pDX, IDC_SLIDER_UVGAIN3, m_sldUVGain3);
	DDX_Control(pDX, IDC_SLIDER_UVGAIN4, m_sldUVGain4);
	DDX_Control(pDX, IDC_SLIDER_EDGEYTHD, m_sldYEdgeHB);
	DDX_Control(pDX, IDC_SLIDER_EDGEYTHD2, m_sldYEdgeLB);
	DDX_Control(pDX, IDC_SLIDER_EDGETHD, m_sldEdgeThd);
	DDX_Control(pDX, IDC_SLIDER_YHB, m_sldYHB);
	DDX_Control(pDX, IDC_SLIDER_YLB, m_sldYLB);
	}


BEGIN_MESSAGE_MAP(CSetting, CDialog)  
	ON_BN_CLICKED(IDC_BUT_RESET, &CSetting::OnBnClickedButReset)
	ON_BN_CLICKED(IDC_BUT_SET, &CSetting::OnBnClickedButSet)
	ON_BN_CLICKED(IDC_RADIO_YUV, &CSetting::OnBnClickedRadioYuv)
	ON_BN_CLICKED(IDC_RADIO_Y, &CSetting::OnBnClickedRadioY)
	ON_BN_CLICKED(IDC_RADIO_U, &CSetting::OnBnClickedRadioU)
	ON_BN_CLICKED(IDC_RADIO_V, &CSetting::OnBnClickedRadioV)
	ON_BN_CLICKED(IDC_CHECK_REVERSEGAMMA, &CSetting::OnBnClickedCheckReversegamma)
	ON_BN_CLICKED(IDC_CHECK_RGBGAMMA, &CSetting::OnBnClickedCheckRgbgamma)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_GAMMARATION, &CSetting::OnEnChangeEditGammaration) 	
	ON_EN_CHANGE(IDC_EDIT_BRIGHTNESS, &CSetting::OnEnChangeEditBrightness)
	ON_EN_CHANGE(IDC_EDIT_CONTRAST, &CSetting::OnEnChangeEditContrast)
	ON_EN_CHANGE(IDC_EDIT_SATURATION, &CSetting::OnEnChangeEditSaturation)
	ON_EN_CHANGE(IDC_EDIT_HUE, &CSetting::OnEnChangeEditHue)
	ON_EN_CHANGE(IDC_EDIT_EDGEGAIN, &CSetting::OnEnChangeEditEdgeGain)
	ON_EN_CHANGE(IDC_EDIT_RGAIN, &CSetting::OnEnChangeEditRgain)
	ON_EN_CHANGE(IDC_EDIT_GGAIN, &CSetting::OnEnChangeEditGgain)
	ON_EN_CHANGE(IDC_EDIT_BGAIN, &CSetting::OnEnChangeEditBgain)
	ON_EN_CHANGE(IDC_EDIT_ROFFSET, &CSetting::OnEnChangeEditRoffset)
	ON_EN_CHANGE(IDC_EDIT_GOFFSET, &CSetting::OnEnChangeEditGoffset)
	ON_EN_CHANGE(IDC_EDIT_BOFFSET, &CSetting::OnEnChangeEditBoffset)	
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_G0, IDC_EDIT_G23, OnEnGammaChange)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_MATRIXY_Y1, IDC_EDIT_MATRIX_V3, OnEnInputColorMatrixChange)	
	ON_EN_CHANGE(IDC_EDIT_UVGAIN1, &CSetting::OnEnChangeEditUvgain1)
	ON_EN_CHANGE(IDC_EDIT_UVGAIN2, &CSetting::OnEnChangeEditUvgain2)
	ON_EN_CHANGE(IDC_EDIT_UVGAIN3, &CSetting::OnEnChangeEditUvgain3)
	ON_EN_CHANGE(IDC_EDIT_UVGAIN4, &CSetting::OnEnChangeEditUvgain4)
	ON_EN_CHANGE(IDC_EDIT_EDGETHD, &CSetting::OnEnChangeEditEdgethd)
	ON_EN_CHANGE(IDC_EDIT_EDGEYTHD, &CSetting::OnEnChangeEditEdgeythd)
	ON_EN_CHANGE(IDC_EDIT_EDGEYTHD, &CSetting::OnEnChangeEditEdgeythd2)
	ON_EN_CHANGE(IDC_EDIT_YHB, &CSetting::OnEnChangeEditYhb)
	ON_EN_CHANGE(IDC_EDIT_YLB, &CSetting::OnEnChangeEditYlb) 	
END_MESSAGE_MAP()


// CSetting message handlers

void CSetting::OnBnClickedButReset()
	{
	// TODO: Add your control notification handler code here
	ASSERT(0);
	}

void CSetting::OnBnClickedButSet()
	{
	// TODO: Add your control notification handler code here
	SavePars();
	}

void CSetting::OnBnClickedRadioYuv()
	{
	// TODO: Add your control notification handler code here
	m_YUVChannel = YUV_YUV;		// YUV
	}

void CSetting::OnBnClickedRadioY()
	{
	// TODO: Add your control notification handler code here
	m_YUVChannel = YUV_Y;		// Y
	}

void CSetting::OnBnClickedRadioU()
	{
	// TODO: Add your control notification handler code here
	m_YUVChannel = YUV_U;		// U
	}

void CSetting::OnBnClickedRadioV()
	{
	// TODO: Add your control notification handler code here
	m_YUVChannel = YUV_V;		// V
	}

void CSetting::OnPaint()
	{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here	 
#if 1
	BYTE GammaVal[RGBGNODENUM] = {0};
	for (int i = 0; i < RGBGNODENUM; i++)
		{
		GammaVal[i] = GetDlgItemInt(IDC_EDIT_G0+i);
		}

	int i = 0,StartX = 23,StartY = 30,RectSize=3;	
	dc.PatBlt(StartX-4,StartY-4,256+8,256+8,WHITENESS);

	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RGBGAMMA))
		{
		for(i = 0; i < 24; i++)
			{
			if (i<3)
				{
				dc.MoveTo(StartX + (i<<2), StartY + 256);
				dc.LineTo(StartX + (i<<2), StartY + 260);
				}			
			else if(i<6)
				{
				dc.MoveTo(StartX + 16+((i-3)<<3), StartY + 256);
				dc.LineTo(StartX + 16+((i-3)<<3), StartY + 260);
				}
			else if (i<15)
				{
				dc.MoveTo(StartX + 48+((i-6)<<4), StartY + 256);
				dc.LineTo(StartX + 48+((i-6)<<4), StartY + 260);
				}
			else if (i<23)
				{
				dc.MoveTo(StartX + 184+((i-15)<<3), StartY + 256);
				dc.LineTo(StartX + 184+((i-15)<<3), StartY + 260);
				}
			else 
				{
				dc.MoveTo(StartX + 255, StartY + 256);
				dc.LineTo(StartX + 255, StartY + 260);
				}  
			}
		dc.Rectangle(StartX, StartY, StartX+256, StartY+256);
		dc.MoveTo(StartX, StartY+256-GammaVal[0]);

		dc.Rectangle(StartX +RectSize, StartY+256-GammaVal[0]+RectSize,
			StartX-RectSize, StartY+256-GammaVal[0]-RectSize);
		for (i=1; i<24; i++)
			{
			if (i<3)
				{
				dc.LineTo(StartX + (i<<2),  StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX + (i<<2)+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX + (i<<2)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}			
			else if(i<6)
				{
				dc.LineTo(StartX +16+((i-3)<<3),  StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX +16+((i-3)<<3)+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX +16+((i-3)<<3)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}			
			else if(i<15)
				{
				dc.LineTo(StartX +48+((i-6)<<4),  StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX +48+((i-6)<<4)+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX +48+((i-6)<<4)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}
			else if (i<23)
				{
				dc.LineTo(StartX + 184+((i-15)<<3), StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX + 184+((i-15)<<3)+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX + 184+((i-15)<<3)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}
			else
				{
				dc.LineTo(StartX + 255, StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX + 255+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX + 255-RectSize, StartY+256-GammaVal[i]-RectSize);
				}	
			}
		}
	else
		{
		for (i=0; i<18; i++)
			{
			if (i<5)
				{
				dc.MoveTo(StartX + (i<<3), StartY + 256);
				dc.LineTo(StartX + (i<<3), StartY + 260);
				}
			else if(i<9)
				{
				dc.MoveTo(StartX + 48+((i-5)<<4), StartY + 256);
				dc.LineTo(StartX + 48+((i-5)<<4), StartY + 260);
				}
			else if(i<12)
				{
				dc.MoveTo(StartX + 128+((i-9)<<5), StartY + 256);
				dc.LineTo(StartX + 128+((i-9)<<5), StartY + 260);
				}
			else if(i<14)
				{
				dc.MoveTo(StartX + 208+((i-12)<<4), StartY + 256);
				dc.LineTo(StartX + 208+((i-12)<<4), StartY + 260);
				}
			else 
				{
				dc.MoveTo(StartX + 232+((i-14)<<3), StartY + 256);
				dc.LineTo(StartX + 232+((i-14)<<3), StartY + 260);
				}
			}

		dc.Rectangle(StartX, StartY, StartX+256, StartY+256);
		dc.MoveTo(StartX, StartY+256-GammaVal[0]);

		dc.Rectangle(StartX+RectSize, StartY+256-GammaVal[0]+RectSize,
			StartX-RectSize, StartY+256-GammaVal[0]-RectSize);
		for (i=1; i<18; i++)
			{
			if (i<5)
				{
				dc.LineTo(StartX + (i<<3),  StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX + (i<<3)+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX + (i<<3)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}
			else if(i<9)
				{
				dc.LineTo(StartX + 48+((i-5)<<4),  StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX + 48+((i-5)<<4)+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX + 48+((i-5)<<4)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}
			else if(i<12)
				{
				dc.LineTo(StartX + 128+((i-9)<<5),  StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX + 128+((i-9)<<5)+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX + 128+((i-9)<<5)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}
			else if(i<14)
				{
				dc.LineTo(StartX + 208+((i-12)<<4), StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX + 208+((i-12)<<4)+RectSize, StartY+256-GammaVal[i]+RectSize,
					StartX + 208+((i-12)<<4)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}
			else if(i<18)
				{
				dc.LineTo(StartX + 232+((i-14)<<3), StartY + 256-(GammaVal[i]));
				dc.Rectangle(StartX + 232+((i-14)<<3), StartY+256-GammaVal[i]+RectSize,
					StartX + 232+((i-14)<<3)-RectSize, StartY+256-GammaVal[i]-RectSize);
				}
			}
		}
#endif
	// Do not call CDialog::OnPaint() for painting messages
	}

void CSetting::OnMouseMove(UINT nFlags, CPoint point)
	{
	// TODO: Add your message handler code here and/or call default
	UpdateData();
	BYTE GammaVal[RGBGNODENUM] = {0};
	for (int i = 0; i < RGBGNODENUM; i++)
		{
		GammaVal[i] = GetDlgItemInt(IDC_EDIT_G0+i);
		}

	int i = 0, Gamma = 0, StartX = 23, StartY = 30, RectSize=3;	
	if((nFlags & MK_LBUTTON)&(iGammaIdx == -1))
		{
		if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RGBGAMMA))
			{	
			for (i = 0; i < 24; i++)
				{
				if (i< 3)
					{
					if (point.x < StartX+(i<<2)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+(i<<2)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					}
				if (i < 6)
					{
					if (point.x < StartX+16+((i-3)<<3)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+16+((i-3)<<3)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					}
				else if (i < 15)
					{
					if (point.x < StartX+48+((i-6)<<4)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+48+((i-6)<<4)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					}
				else if (i < 23)
					{
					if (point.x < StartX+184+((i-15)<<3)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+184+((i-15)<<3)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					}
				else
					{
					if (point.x < StartX+255+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+255-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					} 
				}
			}
		else
			{
			for(i=0; i<18; i++)
				{
				if (i < 4)
					{
					if (point.x < StartX+(i<<3)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+(i<<3)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					}
				else if (i < 8)
					{
					if (point.x < StartX+32+((i-4)<<4)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+((i-4)<<4)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					}
				else if (i < 11)
					{
					if (point.x < StartX+96+((i-8)<<5)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+96+((i-8)<<5)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					}
				else if (i< 13)
					{
					if (point.x < StartX+192+((i-11)<<4)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+192+((i-11)<<4)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}

					}
				else
					{
					if (point.x < StartX+224+((i-13)<<3)+RectSize && point.y < StartY+256-GammaVal[i]+RectSize
						&& point.x > StartX+224+((i-13)<<3)-RectSize && point.y > StartY+256-GammaVal[i]-RectSize)
						{
						iGammaIdx = i;
						}
					}				
				}
			}		
		}

	if(iGammaIdx != -1){
		Gamma = (StartY + 256 - point.y);
		GammaVal[iGammaIdx] = (Gamma < 0)?0:((Gamma > 255)?255:Gamma);

		//prevent mouse move outside control window
		if(point.y < 3 || point.y > 300 || point.x < 3 || point.x > 422)
			iGammaIdx = -1;		

		for (i = 0; i < RGBGNODENUM; i++)
			{		
			SetDlgItemInt(IDC_EDIT_G0+i, GammaVal[i], TRUE);
			}		

		UpdateData(FALSE);
		Invalidate(FALSE);
		}

	CDialog::OnMouseMove(nFlags, point);
	}

void CSetting::OnLButtonUp(UINT nFlags, CPoint point)
	{
	// TODO: Add your message handler code here and/or call default
	if (iGammaIdx > RGBGNODENUM-1 || iGammaIdx < 0)
		{
		return;
		}

	iGammaIdx = -1;

	CDialog::OnLButtonUp(nFlags, point);
	} 	 	

void CSetting::SetYUVChannel()
	{ 
	CButton *pBut = NULL;

	switch (m_YUVChannel)
		{
		case YUV_YUV:
			pBut = (CButton*)GetDlgItem(IDC_RADIO_YUV);
			break;
		case YUV_Y:
			pBut = (CButton*)GetDlgItem(IDC_RADIO_Y);
			break;
		case YUV_U:
			pBut = (CButton*)GetDlgItem(IDC_RADIO_U);
			break;
		case YUV_V:
			pBut = (CButton*)GetDlgItem(IDC_RADIO_V);
			break;
		}

	ASSERT(NULL != pBut);
	pBut->SetCheck(BST_CHECKED);
	}

void CSetting::SavePars()
	{
	ASSERT(NULL != m_RawPars);

	UpdateData();

	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RGBGAMMA))
		{
		SaveRGBGammaUI();
		}
	else
		{
		SaveYGammaUI();
		}

	BOOL bChecked = IsDlgButtonChecked(IDC_RADIO_NEGATIVEIMPTYPE_REVERSERGBGAMMA);
	m_negativeImpType =  bChecked ? NEGATIVEIMP_ReverseRGBGamma : NEGATIVEIMP_255SubRGBData;

	m_RawPars->dRGBGammaVal = m_dRGBGammaVal;
	m_RawPars->dYGammaVal = m_dYGammaVal;

	m_RawPars->bRGBGammaEn = m_bRGBGammaEn;
	m_RawPars->bYGammaEn = m_bYGammaEn;

	//	m_RawPars->bNegative = m_bRGBReverseGamma;

	m_RawPars->bYEdgeEnhanceEn = m_bYEdgeEnhanceEn;
	m_RawPars->bUVGainEn = m_bUVGainEn;

	for (int i = 0; i < RGBGNODENUM; i++)
		{
		m_RawPars->RGBGammaNode[i] = m_RGBGammaNode[i];
		}
	for (int i = 0; i < YGNODENUM; i++)
		{
		m_RawPars->YGammaNode[i] = m_YGammaNode[i];
		}	

	// Brightness
	m_RawPars->Brightness = m_iBrightness;

	// Contrast
	m_RawPars->Contrast = m_iContrast;

	// Saturation
	m_RawPars->Sat = m_dSat;

	// Hue
	m_RawPars->Hue = m_dHue;

	// YEdgeGain
	m_RawPars->YEdgeGain = m_iEdgeGain;

	// EdgeThd
	m_RawPars->YEdgeHB = m_iYEdgeHB;
	m_RawPars->YEdgeLB = m_iYEdgeLB;
	m_RawPars->YEdgeThd = m_iEdgeThd;

	// RGB Gain
	m_RawPars->RGain = m_iRGain;
	m_RawPars->GGain = m_iGGain;
	m_RawPars->BGain = m_iBGain;

	// RGB Offset
	m_RawPars->ROffset = m_iROffset;
	m_RawPars->GOffset = m_iGOffset;
	m_RawPars->BOffset = m_iBOffset;

	// UVGain
	m_RawPars->UVGain[0] = m_iUVGain1;
	m_RawPars->UVGain[1] = m_iUVGain2;
	m_RawPars->UVGain[2] = m_iUVGain3;
	m_RawPars->UVGain[3] = m_iUVGain4;

	// YUV Display Channel
	m_RawPars->YUVChannel = m_YUVChannel;

	// Input Color Matrix
	m_RawPars->InMatrix[0] = iMatrixY1;
	m_RawPars->InMatrix[1] = iMatrixY2;
	m_RawPars->InMatrix[2] = iMatrixY3;
	m_RawPars->InMatrix[3] = iMatrixU1;
	m_RawPars->InMatrix[4] = iMatrixU2;
	m_RawPars->InMatrix[5] = iMatrixU3;
	m_RawPars->InMatrix[6] = iMatrixV1;
	m_RawPars->InMatrix[7] = iMatrixV2;
	m_RawPars->InMatrix[8] = iMatrixV3;	

	// Output Color Matrix
	m_RawPars->OutMatrix[0] = iMatrixOY1;
	m_RawPars->OutMatrix[1] = iMatrixOY2;
	m_RawPars->OutMatrix[2] = iMatrixOY3;
	m_RawPars->OutMatrix[3] = iMatrixOU1;
	m_RawPars->OutMatrix[4] = iMatrixOU2;
	m_RawPars->OutMatrix[5] = iMatrixOU3;
	m_RawPars->OutMatrix[6] = iMatrixOV1;
	m_RawPars->OutMatrix[7] = iMatrixOV2;
	m_RawPars->OutMatrix[8] = iMatrixOV3;

	m_RawPars->YHB = m_YHB;
	m_RawPars->YLB = m_YLB;
	m_RawPars->bYUVProcEn = m_bYUVProcEn;
	m_RawPars->negativeImpType = m_negativeImpType;
	}

void CSetting::OnEnChangeEditGammaration()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
#if 1
	UpdateData(TRUE);
	BYTE GammaVal[RGBGNODENUM] = {0};
	for (int i = 0; i < RGBGNODENUM; i++)
		{
		GammaVal[i] = GetDlgItemInt(IDC_EDIT_G0+i);
		}

	// RGBGammaNode  
	BYTE RGBGammaValTmp[RGBGNODENUM] = {0};	
	if (DEVTYPE_FS_900 == m_DevType)
		{
		BYTE by9MRGBGammaValTmp[RGBGNODENUM] = {
			0, 5, 16, 42, 60, 78, 104, 125, 142, 154,
			164, 174, 181, 189, 197, 203, 206, 210,
			217, 225, 231, 239, 246, 255			
			};

		for (int i = 0; i < RGBGNODENUM; i++)
			{
			RGBGammaValTmp[i] = by9MRGBGammaValTmp[i];
			}
		}
	else if (DEVTYPE_FS_500 == m_DevType)
		{
		BYTE by5MRGBGammaValTmp[RGBGNODENUM] = {
			0, 12, 20, 38, 56, 73, 102, 124, 145, 162,
			178, 189, 195, 202, 208, 213, 216, 220, 223, 228,
			234, 239, 245, 255
			}; 
		for (int i = 0; i < RGBGNODENUM; i++)
			{
			RGBGammaValTmp[i] = by5MRGBGammaValTmp[i];
			}
		}

	BYTE YGammaValTmp[YGNODENUM] = {
		0, 8, 16, 24, 32, 48, 64, 80, 96, 128,
		160, 192, 208, 224, 232, 240, 248, 255		
		};

	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RGBGAMMA))
		{
		for (int i = 0; i < RGBGNODENUM; i++)
			{
			GammaVal[i] = (BYTE)(255*pow(((double)RGBGammaValTmp[i]/255.0), m_dGammaVal)); 

			}
		}
	else
		{
		for (int i = 0; i < YGNODENUM; i++)
			{
			GammaVal[i] = (BYTE)(255*pow(((double)YGammaValTmp[i]/255.0), m_dGammaVal)); 

			}
		}

	for (int i = 0; i < RGBGNODENUM; i++)
		{
		SetDlgItemInt(IDC_EDIT_G0+i, GammaVal[i], FALSE);
		} 	

	Invalidate();
#endif 
	}

void CSetting::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
	// TODO: Add your message handler code here and/or call default
#if 1
	int iID = pScrollBar->GetDlgCtrlID();

	switch (iID)
		{
		case IDC_SLIDER_BRIGHTNESS:
			m_iBrightness = m_sldBrightness.GetPos();
			break;
		case IDC_SLIDER_CONTRAST:
			m_iContrast = m_sldContrast.GetPos();
			break;
		case IDC_SLIDER_SATURATION:
			m_dSat = 0.01 * m_sldSat.GetPos();
			UpdateData(FALSE);
			UpdateOutputColorMatrix();
			break;
		case IDC_SLIDER_HUE:
			m_dHue = 0.1 * m_sldHue.GetPos();
			UpdateData(FALSE);
			UpdateOutputColorMatrix();
			break;
		case IDC_SLIDER_EDGEGAIN:
			m_iEdgeGain = m_sldEdgeGain.GetPos();
			break;
		case IDC_SLIDER_RGAIN:
			m_iRGain = m_sldRGain.GetPos();
			break;
		case IDC_SLIDER_GGAIN:
			m_iGGain = m_sldGGain.GetPos();
			break;
		case IDC_SLIDER_BGAIN:
			m_iBGain = m_sldBGain.GetPos();
			break;
		case IDC_SLIDER_ROFFSET:
			m_iROffset = m_sldROffset.GetPos();
			break;
		case IDC_SLIDER_GOFFSET:
			m_iGOffset = m_sldGOffset.GetPos();
			break;
		case IDC_SLIDER_BOFFSET:
			m_iBOffset = m_sldBOffset.GetPos();
			break;
		case IDC_SLIDER_UVGAIN1:
			m_iUVGain1 = m_sldUVGain1.GetPos();
			break;
		case IDC_SLIDER_UVGAIN2:
			m_iUVGain2 = m_sldUVGain2.GetPos();
			break;
		case IDC_SLIDER_UVGAIN3:
			m_iUVGain3 = m_sldUVGain3.GetPos();
			break;
		case IDC_SLIDER_UVGAIN4:
			m_iUVGain4 = m_sldUVGain4.GetPos();
			break;
		case IDC_SLIDER_EDGEYTHD:
			m_iYEdgeHB = m_sldYEdgeHB.GetPos();
			break;
		case IDC_SLIDER_EDGEYTHD2:
			m_iYEdgeLB = m_sldYEdgeLB.GetPos();
			break;
		case IDC_SLIDER_EDGETHD:
			m_iEdgeThd = m_sldEdgeThd.GetPos();
			break;
		case IDC_SLIDER_YHB:
			m_YHB = m_sldYHB.GetPos();
			break;
		case IDC_SLIDER_YLB:
			m_YLB = m_sldYLB.GetPos();
			break;
		}

	UpdateData(FALSE);
#endif

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

void CSetting::OnEnChangeEditBrightness()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_sldBrightness.SetPos(m_iBrightness);
	}

void CSetting::OnEnChangeEditContrast()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_sldContrast.SetPos(m_iContrast);
	}

void CSetting::OnEnChangeEditSaturation()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_sldSat.SetPos((int)(100*m_dSat));

	UpdateOutputColorMatrix();
	}

void CSetting::OnEnChangeEditHue()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_sldSat.SetPos((int)(10*m_dHue));

	UpdateOutputColorMatrix();
	}

BOOL CSetting::OnInitDialog()
	{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	iGammaIdx = -1; 	
	InitSliderCtrls();
	GammaGroupCtrlTextChange();

	CheckDlgButton(IDC_RADIO_NEGATIVEIMPTYPE_REVERSERGBGAMMA, BST_CHECKED);
	m_negativeImpType = NEGATIVEIMP_ReverseRGBGamma;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	}

void CSetting::InitSliderCtrls()
	{
	m_sldBrightness.SetRange(-64, 64, TRUE);
	m_sldContrast.SetRange(0, 64, TRUE);
	m_sldSat.SetRange(0, 800, TRUE);			// [0.00, 8.00]
	m_sldHue.SetRange(-1800, 1800, TRUE);		// [-180.0, 180.0]
	m_sldEdgeGain.SetRange(0, 255);

	m_sldRGain.SetRange(0, 255);
	m_sldGGain.SetRange(0, 255);
	m_sldBGain.SetRange(0, 255);

	m_sldROffset.SetRange(-128, 127, TRUE);
	m_sldGOffset.SetRange(-128, 127, TRUE);
	m_sldBOffset.SetRange(-128, 127, TRUE);

	m_sldUVGain1.SetRange(-128, 127, TRUE);
	m_sldUVGain2.SetRange(-128, 127, TRUE);
	m_sldUVGain3.SetRange(-128, 127, TRUE);
	m_sldUVGain4.SetRange(-128, 127, TRUE);

	m_sldYEdgeHB.SetRange(0, 255);
	m_sldYEdgeLB.SetRange(0, 255);
	m_sldEdgeThd.SetRange(0, 255);

	m_sldYHB.SetRange(0, 255);
	m_sldYLB.SetRange(0, 255);

	}

void CSetting::OnBnClickedCheckReversegamma()
	{
	// TODO: Add your control notification handler code here		
	// Out Gamma
	UpdateData();

	BYTE byTmp = 0;
	for (int i = 0; i < RGBGNODENUM; i++)
		{
		byTmp = 255 - GetDlgItemInt(IDC_EDIT_G0+i);
		SetDlgItemInt(IDC_EDIT_G0+i, byTmp, FALSE);
		}

	Invalidate();
	}

void CSetting::SetDevType( CUSTOMDEVICETYPE cusDevType )
	{
	m_DevType = cusDevType;
	}

void CSetting::OnBnClickedCheckRgbgamma()
	{
	// TODO: Add your control notification handler code here

	BOOL bShow = FALSE;		

	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RGBGAMMA))
		{
		SaveYGammaUI();
		UpdateRGBGammaUI();
		bShow = TRUE;
		}
	else
		{
		SaveRGBGammaUI();
		UpdateYGammaUI();
		bShow = FALSE;
		} 

	for (int i = IDC_EDIT_G18; i <= IDC_EDIT_G23; i++)
		{
		GetDlgItem(i)->ShowWindow(bShow);
		}

	BOOL bRGBGamma = bShow;
	GammaGroupCtrlTextChange(bRGBGamma);
	}

void CSetting::SaveRGBGammaUI()
	{
	m_dRGBGammaVal = m_dGammaVal;
	m_bRGBGammaEn = m_bGammaEn;
	m_bRGBReverseGamma = m_bReverseGamma; 	

	for (int i = 0; i < RGBGNODENUM; i++)
		{
		m_RGBGammaNode[i] = GetDlgItemInt(IDC_EDIT_G0+i);
		}
	}

void CSetting::SaveYGammaUI()
	{
	m_dYGammaVal = m_dGammaVal;
	m_bYGammaEn = m_bGammaEn;

	for (int i = 0; i < YGNODENUM; i++)
		{
		m_YGammaNode[i] = GetDlgItemInt(IDC_EDIT_G0+i);
		} 
	}

void CSetting::UpdateRawDataUI()
	{
	// Slider
	m_sldRGain.SetPos(m_iRGain);
	m_sldGGain.SetPos(m_iGGain);
	m_sldBGain.SetPos(m_iBGain);
	m_sldROffset.SetPos(m_iROffset);
	m_sldGOffset.SetPos(m_iGOffset);
	m_sldBOffset.SetPos(m_iBOffset);
	}

void CSetting::UpdateRGBGammaUI()
	{
	// Output RGBGamma 	
	for (int i = 0; i < RGBGNODENUM; i++)
		{
		SetDlgItemInt(IDC_EDIT_G0+i, m_RGBGammaNode[i], FALSE);
		}	

	for (int i = IDC_EDIT_G18; i <= IDC_EDIT_G23; i++)
		{
		GetDlgItem(i)->ShowWindow(SW_SHOW);
		}

	m_dGammaVal = m_dRGBGammaVal;
	m_bGammaEn = m_bRGBGammaEn;
	m_bReverseGamma = m_bRGBReverseGamma;

	UpdateData(FALSE);
	Invalidate();  
	}

void CSetting::UpdateYGammaUI()
	{
	for (int i = 0; i < YGNODENUM; i++)
		{
		SetDlgItemInt(IDC_EDIT_G0+i, m_YGammaNode[i], FALSE);
		}	

	for (int i = IDC_EDIT_G18; i <= IDC_EDIT_G23; i++)
		{
		GetDlgItem(i)->ShowWindow(SW_HIDE);
		}

	m_dGammaVal = m_dYGammaVal;
	m_bGammaEn = m_bYGammaEn;

	SetYUVChannel();

	// Slider
	m_sldBrightness.SetPos(m_iBrightness);
	m_sldContrast.SetPos(m_iContrast);
	m_sldSat.SetPos((int)(100*m_dSat));
	m_sldHue.SetPos((int)(10*m_dHue)); 
	m_sldEdgeGain.SetPos(m_iEdgeGain);
	m_sldEdgeThd.SetPos(m_iEdgeThd);
	m_sldYEdgeHB.SetPos(m_iYEdgeHB);
	m_sldYEdgeLB.SetPos(m_iYEdgeLB);
	m_sldUVGain1.SetPos(m_iUVGain1);
	m_sldUVGain2.SetPos(m_iUVGain2);
	m_sldUVGain3.SetPos(m_iUVGain3);
	m_sldUVGain4.SetPos(m_iUVGain4);

	UpdateData(FALSE);
	Invalidate(TRUE);
	}

void CSetting::LoadRawPars()
	{
	ASSERT(NULL != m_RawPars);

	m_iRGain = m_RawPars->RGain;
	m_iGGain = m_RawPars->GGain;
	m_iBGain = m_RawPars->BGain;

	m_iROffset = m_RawPars->ROffset;
	m_iGOffset = m_RawPars->GOffset;
	m_iBOffset = m_RawPars->BOffset;
	}

void CSetting::LoadRGBGammaPars()
	{
	ASSERT(NULL != m_RawPars);	

	m_bRGBGammaEn = m_RawPars->bRGBGammaEn; 
	m_dRGBGammaVal = m_RawPars->dRGBGammaVal;
	m_bRGBReverseGamma = m_RawPars->bNegative;
	for (int i = 0; i < RGBGNODENUM; i++)
		{
		m_RGBGammaNode[i] = m_RawPars->RGBGammaNode[i];
		}

	m_YHB = m_RawPars->YHB;
	m_YLB = m_RawPars->YLB;
	m_negativeImpType = m_RawPars->negativeImpType;

	UpdateData(FALSE);
	}

void CSetting::LoadYGammaPars()
	{
	ASSERT(NULL != m_RawPars);

	m_bYGammaEn = m_RawPars->bYGammaEn;
	m_dYGammaVal = m_RawPars->dYGammaVal;
	m_bYEdgeEnhanceEn = m_RawPars->bYEdgeEnhanceEn;
	m_bUVGainEn = m_RawPars->bUVGainEn;
	for (int i = 0; i < YGNODENUM; i++)
		{
		m_YGammaNode[i] = m_RawPars->YGammaNode[i];
		}

	// Brihgtness
	m_iBrightness = m_RawPars->Brightness;

	// Contrast
	m_iContrast = m_RawPars->Contrast;

	// Saturation
	m_dSat = m_RawPars->Sat;

	// Hue
	m_dHue = m_RawPars->Hue;

	// YEdgeGain
	m_iEdgeGain = m_RawPars->YEdgeGain;

	// EdgeThd
	m_iYEdgeHB = m_RawPars->YEdgeHB;
	m_iYEdgeLB = m_RawPars->YEdgeLB;
	m_iEdgeThd = m_RawPars->YEdgeThd;


	// UVGain
	m_iUVGain1 = m_RawPars->UVGain[0];
	m_iUVGain2 = m_RawPars->UVGain[1];
	m_iUVGain3 = m_RawPars->UVGain[2];
	m_iUVGain4 = m_RawPars->UVGain[3];

	// YUV Display Channel
	m_YUVChannel = m_RawPars->YUVChannel;

	// Input Color Matrix
	iMatrixY1 = m_RawPars->InMatrix[0];
	iMatrixY2 = m_RawPars->InMatrix[1];
	iMatrixY3 = m_RawPars->InMatrix[2];
	iMatrixU1 = m_RawPars->InMatrix[3];
	iMatrixU2 = m_RawPars->InMatrix[4];
	iMatrixU3 = m_RawPars->InMatrix[5];
	iMatrixV1 = m_RawPars->InMatrix[6];
	iMatrixV2 = m_RawPars->InMatrix[7];
	iMatrixV3 = m_RawPars->InMatrix[8];		

	// Output Color Matrix
	iMatrixOY1 = m_RawPars->OutMatrix[0];
	iMatrixOY2 = m_RawPars->OutMatrix[1];
	iMatrixOY3 = m_RawPars->OutMatrix[2];
	iMatrixOU1 = m_RawPars->OutMatrix[3];
	iMatrixOU2 = m_RawPars->OutMatrix[4];
	iMatrixOU3 = m_RawPars->OutMatrix[5];
	iMatrixOV1 = m_RawPars->OutMatrix[6];
	iMatrixOV2 = m_RawPars->OutMatrix[7];
	iMatrixOV3 = m_RawPars->OutMatrix[8];

	m_bYUVProcEn = m_RawPars->bYUVProcEn;

	UpdateData(FALSE);
	} 

void CSetting::OnEnGammaChange(UINT uiId)
	{
	/*UpdateData();
	Invalidate();*/
	}

void CSetting::OnEnInputColorMatrixChange(UINT uiId)
	{
	UpdateOutputColorMatrix();
	}

void CSetting::UpdateOutputColorMatrix()
	{
	UpdateData();

	CRawPars Pars;
	CRawPars *pPars = &Pars; 
	pPars->bUseDefColorMatrix = FALSE;

	// Saturation
	pPars->Sat = m_dSat;

	// Hue
	pPars->Hue = m_dHue;

	// Input Color Matrix 
	pPars->InMatrix[0] = iMatrixY1;
	pPars->InMatrix[1] = iMatrixY2;
	pPars->InMatrix[2] = iMatrixY3;
	pPars->InMatrix[3] = iMatrixU1;
	pPars->InMatrix[4] = iMatrixU2;
	pPars->InMatrix[5] = iMatrixU3;
	pPars->InMatrix[6] = iMatrixV1;
	pPars->InMatrix[7] = iMatrixV2;
	pPars->InMatrix[8] = iMatrixV3;

	UpdateMatrix(pPars);

	// Output Color Matrix
	iMatrixOY1 = pPars->OutMatrix[0];
	iMatrixOY2 = pPars->OutMatrix[1];
	iMatrixOY3 = pPars->OutMatrix[2];
	iMatrixOU1 = pPars->OutMatrix[3];
	iMatrixOU2 = pPars->OutMatrix[4];
	iMatrixOU3 = pPars->OutMatrix[5];
	iMatrixOV1 = pPars->OutMatrix[6];
	iMatrixOV2 = pPars->OutMatrix[7];
	iMatrixOV3 = pPars->OutMatrix[8];

	UpdateData(FALSE);
	}



void CSetting::UpdateMatrix(CRawPars *pPars)
	{
	ASSERT(NULL != pPars);

	//Y =  0.299000*R + 0.587000*G + 0.114000*B
	//U = -0.168736*R - 0.331264*G + 0.500000*B
	//V =  0.500000*R - 0.418688*G - 0.081312*B
	const double dRGB2YUV[3][3] =
		{
			{    0.299000,   0.587000,   0.114000},
			{   -0.168736,  -0.331264,   0.500000},
			{    0.500000,  -0.418688,  -0.081312}
		};	

	const double UNIT = 1.0 / 64;  
	double dMatrix[3][3];		
	double dHue  = pPars->Hue;
	double dSatu = pPars->Sat;
	double dMatrixHueSat[3][3];
	dMatrixHueSat[0][0] = 1;
	dMatrixHueSat[0][1] = 0;
	dMatrixHueSat[0][2] = 0;
	dMatrixHueSat[1][0] = 0;
	dMatrixHueSat[1][1] = dSatu *cos(-dHue *3.1415926 /180);
	dMatrixHueSat[1][2] = dSatu *sin(-dHue *3.1415926 /180);
	dMatrixHueSat[2][0] = 0;
	dMatrixHueSat[2][1] = -dSatu *sin(-dHue *3.1415926 /180);
	dMatrixHueSat[2][2] =  dSatu *cos(-dHue *3.1415926 /180);

	if (pPars->bUseDefColorMatrix)
		{
		for (int j = 0; j < 3; j++)
			{
			for (int i = 0; i < 3; i++)
				{
				dMatrix[j][i] = 0;
				for (int k = 0; k < 3; k++)
					dMatrix[j][i] += (dMatrixHueSat[j][k] * dRGB2YUV[k][i]);
				}			
			}
		}
	else
		{
		for (int j = 0; j < 3; j++)
			{
			for (int i = 0; i < 3; i++)
				{
				dMatrix[j][i] = 0;
				for (int k = 0; k < 3; k++)
					dMatrix[j][i] += (dMatrixHueSat[j][k] * (pPars->InMatrix[3*j+i]*UNIT));
				}			
			}
		}

	if (pPars->bBWMode)
		{
		dMatrix[1][2] = dMatrix[1][1] = dMatrix[1][0] = 0.0;
		dMatrix[2][2] = dMatrix[2][1] = dMatrix[2][0] = 0.0;			
		}

	pPars->OutMatrix[0] = (int)floor((dMatrix[0][0]*64)+0.5);
	pPars->OutMatrix[1] = (int)floor((dMatrix[0][1]*64)+0.5);
	pPars->OutMatrix[2] = (int)floor((dMatrix[0][2]*64)+0.5);
	pPars->OutMatrix[3] = (int)floor((dMatrix[1][0]*64)+0.5);
	pPars->OutMatrix[4] = (int)floor((dMatrix[1][1]*64)+0.5);
	pPars->OutMatrix[5] = (int)floor((dMatrix[1][2]*64)+0.5);
	pPars->OutMatrix[6] = (int)floor((dMatrix[2][0]*64)+0.5);
	pPars->OutMatrix[7] = (int)floor((dMatrix[2][1]*64)+0.5);
	pPars->OutMatrix[8] = (int)floor((dMatrix[2][2]*64)+0.5);  		
	}


void CSetting::OnEnChangeEditEdgeGain()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldEdgeGain.SetPos(m_iEdgeGain);
	}

void CSetting::OnEnChangeEditRgain()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldRGain.SetPos(m_iRGain);
	}

void CSetting::OnEnChangeEditGgain()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldGGain.SetPos(m_iGGain);
	}

void CSetting::OnEnChangeEditBgain()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldBGain.SetPos(m_iBGain);
	}

void CSetting::OnEnChangeEditRoffset()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldROffset.SetPos(m_iROffset);
	}

void CSetting::OnEnChangeEditGoffset()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldGOffset.SetPos(m_iGOffset);
	}

void CSetting::OnEnChangeEditBoffset()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldBOffset.SetPos(m_iBOffset);
	}

void CSetting::OnEnChangeEditUvgain1()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldUVGain1.SetPos(m_iUVGain1);
	}

void CSetting::OnEnChangeEditUvgain2()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldUVGain2.SetPos(m_iUVGain2);
	}

void CSetting::OnEnChangeEditUvgain3()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldUVGain3.SetPos(m_iUVGain3);
	}

void CSetting::OnEnChangeEditUvgain4()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldUVGain4.SetPos(m_iUVGain4);
	}

void CSetting::OnEnChangeEditEdgethd()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldEdgeThd.SetPos(m_iEdgeThd);
	}

void CSetting::OnEnChangeEditEdgeythd()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldYEdgeHB.SetPos(m_iYEdgeHB);
	} 

void CSetting::OnEnChangeEditEdgeythd2()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldYEdgeLB.SetPos(m_iYEdgeLB);
	}

void CSetting::OnEnChangeEditYhb()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldYHB.SetPos(m_YHB);
	}

void CSetting::OnEnChangeEditYlb()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	m_sldYLB.SetPos(m_YLB);
	}

void CSetting::GammaGroupCtrlTextChange(BOOL bRGBGamma)
	{
	CString strGammaSettting;
	CString strGammaValue;
	CString strOutGamma;

	if (bRGBGamma)
		{
		strGammaSettting = _T("RGB Gamma Setting");
		strGammaValue = _T("RGB Gamma Value");
		strOutGamma = _T("Out RGB Gamma");
		}
	else
		{
		strGammaSettting = _T("Y Gamma Setting");
		strGammaValue = _T("Y Gamma Value");
		strOutGamma = _T("Out Y Gamma");
		}

	GetDlgItem(IDC_STATIC_GAMMASETTING)->SetWindowText(strGammaSettting);
	GetDlgItem(IDC_STATIC_GAMMAVALUE)->SetWindowText(strGammaValue);
	GetDlgItem(IDC_STATIC_OUTGAMMA)->SetWindowText(strOutGamma);
	}

void CSetting::AttchRawParsPointer( CRawPars *pPars )
	{
		DetachRawParsPointer();
		ASSERT(NULL != pPars);
		m_RawPars = pPars;
	}

void CSetting::DetachRawParsPointer()
	{
		m_RawPars = NULL;
	}

void CSetting::NotifyUpdateRawPars()
	{
	LoadRawPars();
	LoadRGBGammaPars(); 
	LoadYGammaPars();

	UpdateRawDataUI();

	CheckDlgButton(IDC_CHECK_RGBGAMMA, BST_UNCHECKED);
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RGBGAMMA))
		{
		UpdateRGBGammaUI();
		}
	else
		{
		UpdateYGammaUI();
		}

	if (NEGATIVEIMP_ReverseRGBGamma == m_negativeImpType)
		{
		CheckDlgButton(IDC_RADIO_NEGATIVEIMPTYPE_REVERSERGBGAMMA, BST_CHECKED);
		}
	else
		{
		CheckDlgButton(IDC_RADIO_NEGATIVEIMPTYPE_255SUBRGBDATA, BST_CHECKED);
		}

	UpdateData(FALSE);
	Invalidate(TRUE);
	}
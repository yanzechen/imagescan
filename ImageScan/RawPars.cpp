#include "StdAfx.h"
#include "RawPars.h"  

CRawPars::CRawPars(void)
	{ 
	uiWid = 0;
	uiHei = 0;
	pbSrc = NULL;
	pbSrcBackup = NULL;
	pbDst = NULL;

	pbY = NULL;
	pbU = NULL;
	pbV = NULL;
	pbYUY2 = NULL;

	// RGBGamma and UVGain
	// +++++
	bRGBGammaEn = FALSE;
	bNegative = FALSE;
	dRGBGammaVal = 1.0;

	// Gain
	GlobalGain = 0;
	RGain = 0;
	GGain = 0;
	BGain = 0;

	// Offset
	ROffset = 0;
	GOffset = 0;
	BOffset = 0;

	Brightness = 0;
	Contrast = 0;
	ContrastCenter = 0;
	ContrastLmt = 0;

	Sat = 1.0;
	Hue = 0.0;
	YEdgeGain = 0;	

	bYGammaEn = FALSE;
	bUVGainEn = FALSE;
	bBWMode = FALSE;

	dYGammaVal = 1.0;		

	YUVChannel = YUV_YUV;		// YUV

	memset(RGBGammaNode, 0, RGBGNODENUM);
	memset(RGBGammaTable, 0, 256);
	memset(YGammaNode, 0, RGBGNODENUM);
	memset(YGammaTable, 0, 256);

	int i = 0, j = 0;
	for (i = 0; i < UVGAINNUM; i++)
		{
		UVGain[i] = 0;
		} 	
	// -----

	// Lens Compensation
	// +++++ 	
	for (i = 0; i < GAINTABLEHEI; i++)
		{
		for (j = 0; j < GAINTABLEWID; j++)
			{
			LCWeiMtxB[i][j] = 0;
			LCWeiMtxG[i][j] = 0;
			LCWeiMtxR[i][j] = 0;
			}
		}	

	for (i = 0; i < RGBCOMPNUM; i++)
		{
		dAR[i] = 0.0;
		dAG[i] = 0.0;
		dAB[i] = 0.0;
		}

	for (i = 0; i < 4; i++)
		{
		UVGain[i] = 0;
		}

	bLens_Comp_En = FALSE;

	iLc_Shift9M = 0;
	iLc_X_Offset9M = 0;
	iLc_Y_Offset9M = 0;

	iLc_Shift5M = 0;
	iLc_X_Offset5M = 0;
	iLc_Y_Offset5M = 0;

	iLc_Shift3M = 0;
	iLc_X_Offset3M = 0;
	iLc_Y_Offset3M = 0;

	iLc_Shift0 = 0;
	iLc_X_Offset0 = 0;
	iLc_Y_Offset0 = 0;

	iLc_Shift1 = 0;
	iLc_X_Offset1 = 0;
	iLc_Y_Offset1 = 0;

	iLc_Shift2 = 0;
	iLc_X_Offset2 = 0;
	iLc_Y_Offset2 = 0;

	iLc_Shift3 = 0;
	iLc_X_Offset3 = 0;
	iLc_Y_Offset3 = 0;

	iLc_Shift4 = 0;
	iLc_X_Offset4 = 0;
	iLc_Y_Offset4 = 0;

	iLc_Shift5 = 0;
	iLc_X_Offset5 = 0;
	iLc_Y_Offset5 = 0;

	dLenCompRation = 1.0;

	xCenter = 0;
	yCenter = 0;
	// ----- 

	bUseDefColorMatrix = TRUE;
	bUpdateReg = TRUE;
	bUpdateUI = TRUE;

	YHB = 0;
	YLB = 0;

	negativeImpType = NEGATIVEIMP_ReverseRGBGamma;

	bGbGrBalanceEn = TRUE;
	GbGrBalanceThd = 4;	
	}

CRawPars::~CRawPars(void)
	{
	} 





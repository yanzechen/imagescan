#pragma once

#include "RawPars.h"


// GbGrBalance_Template:
// []		 []
//	    []
// []		 []
typedef struct  
	{
	BYTE cnt;			// Center
	BYTE lt;			// Left-top of center
	BYTE rt;			// Right-top of center
	BYTE lb;			// Left-bottom of center
	BYTE rb;			// Right-bottom of center
	} GbGrBalance_TemplateValues;

typedef struct  
{
	BYTE Val[9];
} _3x3_TemplateValues;

class CRawUtil
	{
	public:
		CRawUtil(void);
		~CRawUtil(void);

	UINT m_uiWid;
	UINT m_uiHei;
	int m_iCntX;
	int m_iCntY;
	int m_iRadius;
	int m_iLev;		

	BOOL m_b276LSCIsShow;
	float fLSC_B_Gain_Ratio;
	float fLSC_G_Gain_Ratio;
	float fLSC_R_Gain_Ratio;
	float fLCGainRatio;		
	int m_nBlkSize;
	int m_nWMHeight;
	int m_nWMWidth;
	UINT8 BGainTable[GAINTABLEHEI*GAINTABLEWID], 
		GGainTable[GAINTABLEHEI*GAINTABLEWID], 
		RGainTable[GAINTABLEHEI*GAINTABLEWID];

	private:
	void GbGrBalance_GetValuesByPos(BYTE *pbSrc, UINT uiWid, UINT uiHei, 
		int h, int w, GbGrBalance_TemplateValues& cusPos);

	public:
		void RawProcess(CRawPars *pPars);
		void UVProc(CRawPars *pPars); 				
		void CreateYGammaTable(CRawPars *pPars);
		void CreateRGBGammaTable(CRawPars *pPars);
		void CreateGammaTable(CRawPars *pPars);
		void CreateParamUI(CRawPars *pPars);
		void SetBrightness(CRawPars *pPars);
		void SetContrast(CRawPars *pPars);
		void SetGammaValue(CRawPars *pPars);		
		void CreateParamReg(CRawPars *pPars);
		void SetContrastCenter(CRawPars *pPars);
		void SetYGammaValue(const double &dYGammaValue, CRawPars *pPars);					
		void YUY2ToRGB24( BYTE *pbyteSrc, BYTE *pbyteDest, long lwidth, long lheight );		
		void YUY2ToRGB24(CRawPars *pPars);
		void UpdateMatrix(CRawPars *pPars);
		void RGB24toYUY2_byMatrix(CRawPars *pPars, BYTE *yuv);
		void RGB24toYUY2_byUVGain(CRawPars *pPars, BYTE *yuv);
		void RGBGammaProc(CRawPars *pPars);
		void ColorIn(CRawPars *pPars); 		
		void EdgeEnhance(CRawPars *pPars);
		void YGammaProc(CRawPars *pPars);
		void UVGainProc(CRawPars *pPars);
		void RGB24ToPlaneYUV422(CRawPars *pPars);		
		void PlanYUV422ToYUY2(CRawPars *pPars);
		void ColorCorpping(CRawPars *pPars);
		void GbGrBalance(CRawPars *pPars);

		// Filtering
	void MidFiltering_GetValueByPos(BYTE *pbSrc, UINT uiWid, UINT uiHei,
		int h, int w, _3x3_TemplateValues& vals);
		BYTE MidFiltering_GetMidValue(_3x3_TemplateValues vals);
		void MidFiltering(UINT uiWid, UINT uiHei, BYTE *pbSrc);
		void MidFiltering_Y(CRawPars *pPars);
		void MidFiltering_U(CRawPars *pPars);
		void MidFiltering_V(CRawPars *pPars);

	// Lens shading compensation
	void GenerateWeightMatrix(CRawPars *pPars);
	void GenerateWeightMatrix(BYTE *in, int width, int height);
	void LensShadingCompensation( CRawPars *pPars );

	void GaussianFilter(UINT uiWid, UINT uiHei, BYTE *pbSrc);
	void GaussianFilter_Y(CRawPars *pPars);
	void GaussianFilter_U(CRawPars *pPars);
	void GaussianFilter_V(CRawPars *pPars);
	};

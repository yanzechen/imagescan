#pragma once

class CRawPars
	{
	public:
		CRawPars(void);
		~CRawPars(void); 		

	public:			
		// +++++
		CString strFilePath;				// In: Raw file path
		UINT uiWid;							// In: Raw file width
		UINT uiHei;						    // In: Raw file height
		BYTE *pbSrc;						// In: Raw file data
		BYTE *pbSrcBackup;					// In: Raw file data backup		(Not Use)
		BYTE *pbDst;					    // Out: Processed data
		// -----

		// YUV
		// +++++

		// pbU, pbV分别保存的是在U, V平面经过亚采样(SubSample)后的U,V数据, 
		// 所以size = RawWid x RawHie / 2
		// +++
		BYTE *pbY;							// In: Y data (memory allocated by caller, size = RawWid x RawHei)					
		BYTE *pbU;							// In: U data (memory allocated by caller, size = RawWid x RawHei / 2)					
		BYTE *pbV;							// In: V data  (memory allocated by caller, size = RawWid x RawHei / 2)						
		BYTE *pbYUY2;						// In: YUY2 data (memory allocated by caller, size = RawWid x RawHei x 2)
		// ---

		// RGBGamma and UVGain
		// 1. bNegative是将RGBGamma曲线通过"255-'RGBGamma'"获得负片输出效果
		// 2. dRGBGammaVal用来控制RGBGamma曲线斜率
		// +++++
		BOOL bRGBGammaEn;					// UI_In: RGBGamma enable
		BOOL bNegative;						// UI_In: RGBReverseGamma enable
		double dRGBGammaVal; 				// UI_In: RGBGamma value

		// Gain
		int GlobalGain;						// UI_In:		(Not use)
		int RGain;							// UI_In:		
		int GGain;							// UI_In:		
		int BGain;							// UI_In:		

		// Offset							// UI_In:		
		int ROffset;						// UI_In:
		int GOffset;						// UI_In:
		int BOffset;
		// UI_In:
		int Brightness;						// UI_In:
		int Contrast;						// UI_In:
		int ContrastCenter;					// UI_In:
		int ContrastLmt;					// UI_In:
		double Sat;							// UI_In: Saturation	ValueRange:[0.00, 4.00]
		double Hue;							// UI_In: Hue			ValueRange:degree[-180.0, 180.0]

		// Definition(YEdgeEnhance)
		int YEdgeGain;						// UI_In: 
		BOOL bYEdgeEnhanceEn;				// UI_In: YEdgeEnhance enable
		BYTE YEdgeHB;						// UI_In: Y LB
		BYTE YEdgeLB;						// UI_In: Y HB
		BYTE YEdgeThd;	 					// UI_In: 

		BOOL bYGammaEn;						// UI_In: YGamma enable


		BOOL bUVGainEn;						// UI_In: UVGain enable
		BOOL bBWMode;						// UI_In: Output Black&White image 
		double dYGammaVal;					// UI_In: YGamma value 		

		// YUV output channel selection
		YUVChannelType YUVChannel;			// UI_In: YUV_YUV:YUV, YUV_Y:Y, YUV_U:U, YUV_V:V	 		

		BYTE RGBGammaNode[RGBGNODENUM];
		BYTE RGBGammaTable[256];
		BYTE YGammaNode[YGNODENUM];
		BYTE YGammaTable[256];		
		BYTE UVGain[UVGAINNUM];

		int InMatrix[9];				   // UI_In:
		int OutMatrix[9];				   // UI_In:

		BOOL bUseDefColorMatrix;
		BOOL bUpdateReg;
		BOOL bUpdateUI;


		// Color Cropping(Killer)
		// +++++
		BYTE YHB;							// UI_In:
		BYTE YLB;							// UI_In:
		// -----

		BOOL bYUVProcEn;					// UI_In:

		NEGATIVEIMPType negativeImpType;
		// ----- 

		//Lens Compensation		(Not use)
		// +++++
		UINT8 LCWeiMtxB[GAINTABLEHEI][GAINTABLEWID], 
			LCWeiMtxG[GAINTABLEHEI][GAINTABLEWID], 
			LCWeiMtxR[GAINTABLEHEI][GAINTABLEWID];
		double dAR[RGBCOMPNUM];
		double dAG[RGBCOMPNUM];
		double dAB[RGBCOMPNUM];

		BOOL bLens_Comp_En;

	BYTE iLc_Shift9M;
	UINT16 iLc_X_Offset9M;
	UINT16 iLc_Y_Offset9M;

	BYTE iLc_Shift5M;
	UINT16 iLc_X_Offset5M;
	UINT16 iLc_Y_Offset5M;

	BYTE iLc_Shift3M;
	UINT16 iLc_X_Offset3M;
	UINT16 iLc_Y_Offset3M;

		BYTE iLc_Shift0;
		UINT16 iLc_X_Offset0;
		UINT16 iLc_Y_Offset0;

		BYTE iLc_Shift1;
		UINT16 iLc_X_Offset1;
		UINT16 iLc_Y_Offset1;

		BYTE iLc_Shift2;
		UINT16 iLc_X_Offset2;
		UINT16 iLc_Y_Offset2;

		BYTE iLc_Shift3;
		UINT16 iLc_X_Offset3;
		UINT16 iLc_Y_Offset3;

		BYTE iLc_Shift4;
		UINT16 iLc_X_Offset4;
		UINT16 iLc_Y_Offset4;

		BYTE iLc_Shift5;
		UINT16 iLc_X_Offset5;
		UINT16 iLc_Y_Offset5;	 

		double dLenCompRation;			

		int xCenter;
		int yCenter; 		
		// -----	

		BOOL bGbGrBalanceEn;
		BYTE GbGrBalanceThd;
	};



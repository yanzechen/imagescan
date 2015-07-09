#include "StdAfx.h"
#include "RawUtil.h"
#include "RAW8ToRGB24.h"
#include <list>


CRawUtil::CRawUtil(void)
{

}

CRawUtil::~CRawUtil(void)
{

}

void CRawUtil::RawProcess( CRawPars *pPars )
{ 		
	ASSERT(NULL != pPars);	
	
	if (pPars->bLens_Comp_En)
	{ 
		if ( (pPars->uiWid == _5M_RAW_SIZE_WID && pPars->uiHei == _5M_RAW_SIZE_HEI)
			|| (pPars->uiWid == _9M_RAW_SIZE_WID && pPars->uiHei == _9M_RAW_SIZE_HEI) )
		{ 
			GenerateWeightMatrix(pPars);
			LensShadingCompensation(pPars);	
		}		
	}

	CreateParamReg(pPars);	

	// Raw插值转换, RGBGamma处理
	ColorIn(pPars);  

	// YGamma, ColorMatrix, 饱和度, 色度处理
	if (pPars->bYUVProcEn)
	{
		UVProc(pPars);
	}
}

void CRawUtil::RGBGammaProc(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei;
	BYTE *RGBGammaTable = pPars->RGBGammaTable;
	BOOL bNegative = pPars->bNegative;
	NEGATIVEIMPType negativeImpType = pPars->negativeImpType;

	int BOffset = CLIP(pPars->BOffset-64, -128, 127);
	int GOffset = CLIP(pPars->GOffset-64, -128, 127);
	int ROffset = CLIP(pPars->ROffset-64, -128, 127);

	int BGain = (CLIP(pPars->BGain, 0, 255))+128;
	int GGain = (CLIP(pPars->GGain, 0, 255))+128;
	int RGain = (CLIP(pPars->RGain, 0, 255))+128; 	

	UINT h = 0, w = 0;	
	int iTmp = 0; 
	int iB, iGb, iGr, iR; 	

	if (bNegative && ( NEGATIVEIMP_255SubRGBData == negativeImpType ))
	{
		if (!pPars->bRGBGammaEn)
		{ 
			for (h = 0; h < HEI; h+=2)
			{
				for (w = 0; w < WID; w+=2)
				{
					iB = pPars->pbSrc[h*WID+w];
					iGb = pPars->pbSrc[h*WID+(w+1)];
					iGr = pPars->pbSrc[(h+1)*WID+w];
					iR = pPars->pbSrc[(h+1)*WID+(w+1)];					

					iB += BOffset;
					iGb += GOffset;
					iGr += GOffset;
					iR += ROffset;	

					iB = iB > 255 ? 255 : (iB < 0 ? 0 : iB);
					iGb = iGb > 255 ? 255 : (iGb < 0 ? 0 : iGb);
					iGr = iGr > 255 ? 255 : (iGr < 0 ? 0 : iGr);
					iR = iR > 255 ? 255 : (iR < 0 ? 0 : iR);

					iB = (iB*BGain)>>8;
					iGb = (iGb*GGain)>>8;
					iGr = (iGr*GGain)>>8;
					iR = (iR*RGain)>>8;				

					iB = iB > 255 ? 255 : (iB < 0 ? 0 : iB);
					iGb = iGb > 255 ? 255 : (iGb < 0 ? 0 : iGb);
					iGr = iGr > 255 ? 255 : (iGr < 0 ? 0 : iGr);
					iR = iR > 255 ? 255 : (iR < 0 ? 0 : iR);

					pPars->pbSrc[h*WID+w] = 255 - iB;
					pPars->pbSrc[h*WID+(w+1)] = 255 - iGb;
					pPars->pbSrc[(h+1)*WID+w] = 255 - iGr;
					pPars->pbSrc[(h+1)*WID+(w+1)] = 255 - iR;
				}
			}
		}
		else
		{
			for (h = 0; h < HEI; h+=2)
			{
				for (w = 0; w < WID; w+=2)
				{
					iB = pPars->pbSrc[h*WID+w];
					iGb = pPars->pbSrc[h*WID+(w+1)];
					iGr = pPars->pbSrc[(h+1)*WID+w];
					iR = pPars->pbSrc[(h+1)*WID+(w+1)];						

					iB += BOffset;
					iGb += GOffset;
					iGr += GOffset;
					iR += ROffset;

					iB = iB > 255 ? 255 : (iB < 0 ? 0 : iB);
					iGb = iGb > 255 ? 255 : (iGb < 0 ? 0 : iGb);
					iGr = iGr > 255 ? 255 : (iGr < 0 ? 0 : iGr);
					iR = iR > 255 ? 255 : (iR < 0 ? 0 : iR);

					iB = (iB*BGain)>>8;
					iGb = (iGb*GGain)>>8;
					iGr = (iGr*GGain)>>8;
					iR = (iR*RGain)>>8;

					iB = iB > 255 ? 255 : (iB < 0 ? 0 : iB);
					iGb = iGb > 255 ? 255 : (iGb < 0 ? 0 : iGb);
					iGr = iGr > 255 ? 255 : (iGr < 0 ? 0 : iGr);
					iR = iR > 255 ? 255 : (iR < 0 ? 0 : iR); 					

					pPars->pbSrc[h*WID+w] = 255 - RGBGammaTable[iB];
					pPars->pbSrc[h*WID+(w+1)] = 255 - RGBGammaTable[iGb];
					pPars->pbSrc[(h+1)*WID+w] = 255 - RGBGammaTable[iGr];
					pPars->pbSrc[(h+1)*WID+(w+1)] = 255 - RGBGammaTable[iR];
				}
			} 		
		}
	}
	else
	{
		if (!pPars->bRGBGammaEn)
		{ 
			for (h = 0; h < HEI; h+=2)
			{
				for (w = 0; w < WID; w+=2)
				{
					iB = pPars->pbSrc[h*WID+w];
					iGb = pPars->pbSrc[h*WID+(w+1)];
					iGr = pPars->pbSrc[(h+1)*WID+w];
					iR = pPars->pbSrc[(h+1)*WID+(w+1)];

					iB += BOffset;
					iGb += GOffset;
					iGr += GOffset;
					iR += ROffset;

					iB = iB > 255 ? 255 : (iB < 0 ? 0 : iB);
					iGb = iGb > 255 ? 255 : (iGb < 0 ? 0 : iGb);
					iGr = iGr > 255 ? 255 : (iGr < 0 ? 0 : iGr);
					iR = iR > 255 ? 255 : (iR < 0 ? 0 : iR);

					iB = (iB*BGain)>>8;
					iGb = (iGb*GGain)>>8;
					iGr = (iGr*GGain)>>8;
					iR = (iR*RGain)>>8;				

					iB = iB > 255 ? 255 : (iB < 0 ? 0 : iB);
					iGb = iGb > 255 ? 255 : (iGb < 0 ? 0 : iGb);
					iGr = iGr > 255 ? 255 : (iGr < 0 ? 0 : iGr);
					iR = iR > 255 ? 255 : (iR < 0 ? 0 : iR); 					

					pPars->pbSrc[h*WID+w] = iB;
					pPars->pbSrc[h*WID+(w+1)] = iGb;
					pPars->pbSrc[(h+1)*WID+w] = iGr;
					pPars->pbSrc[(h+1)*WID+(w+1)] = iR;
				}
			}
		}
		else
		{
			for (h = 0; h < HEI; h+=2)
			{
				for (w = 0; w < WID; w+=2)
				{
					iB = pPars->pbSrc[h*WID+w];
					iGb = pPars->pbSrc[h*WID+(w+1)];
					iGr = pPars->pbSrc[(h+1)*WID+w];
					iR = pPars->pbSrc[(h+1)*WID+(w+1)];

					iB += BOffset;
					iGb += GOffset;
					iGr += GOffset;
					iR += ROffset;

					iB = iB > 255 ? 255 : (iB < 0 ? 0 : iB);
					iGb = iGb > 255 ? 255 : (iGb < 0 ? 0 : iGb);
					iGr = iGr > 255 ? 255 : (iGr < 0 ? 0 : iGr);
					iR = iR > 255 ? 255 : (iR < 0 ? 0 : iR);

					iB = (iB*BGain)>>8;
					iGb = (iGb*GGain)>>8;
					iGr = (iGr*GGain)>>8;
					iR = (iR*RGain)>>8;

					iB = iB > 255 ? 255 : (iB < 0 ? 0 : iB);
					iGb = iGb > 255 ? 255 : (iGb < 0 ? 0 : iGb);
					iGr = iGr > 255 ? 255 : (iGr < 0 ? 0 : iGr);
					iR = iR > 255 ? 255 : (iR < 0 ? 0 : iR); 					

					pPars->pbSrc[h*WID+w] = RGBGammaTable[iB];
					pPars->pbSrc[h*WID+(w+1)] = RGBGammaTable[iGb];
					pPars->pbSrc[(h+1)*WID+w] = RGBGammaTable[iGr];
					pPars->pbSrc[(h+1)*WID+(w+1)] = RGBGammaTable[iR];				
				}
			} 		
		}
	} 

}

#define TESTMODE 0
void CRawUtil::ColorIn(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	RGBGammaProc(pPars); 

	// 在插值时, 使用5x5的模板来计算, 为了处理Raw图像边缘, 需要对
	// 图像做Padding, 即: 在图像四边分别增加2行形成一个"回"字型
	// Padding
	// +++++
	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei;
	const UINT PWID = WID + 2*2;
	const UINT PHEI = HEI + 2*2;
	const UINT PSIZE = PWID * PHEI;
	UINT h = 0, w = 0;
	BYTE *pPad = NULL;
	pPad = new BYTE[PSIZE];
	ASSERT(NULL != pPad);  

	// 对Padding的四个角赋值
	for (h = 0; h < 2; h++)
	{
		for (w = 0; w < 2; w++)
		{
			pPad[PWID*h+w] = pPars->pbSrc[WID*(1-h)+(1-w)];												// Left-Top
			pPad[PWID*h+((PWID-2)+w)] = pPars->pbSrc[WID*(1-h)+(WID-2)+(1-w)];							// Right-Top
			pPad[PWID*((PHEI-2)+h)+w] = pPars->pbSrc[WID*((HEI-2)+(1-h))+(1-w)];						// Left-Bottom
			pPad[PWID*((PHEI-2)+h)+(PWID-2)+w] = pPars->pbSrc[WID*((HEI-2)+(1-h))+(WID-2)+(1-w)];		// Right-Bottom
		}
	} 	

	// 对Padding后的四边(不包括四个角)进行赋值
	// +++

	// Top, Bottom
	for (h = 0; h < 2; h++)
	{
		for (w = 0; w < WID; w++)
		{
			pPad[PWID*h+(w+2)] = pPars->pbSrc[WID*(1-h)+w];							// Top
			pPad[PWID*((PHEI-2)+h)+(w+2)] = pPars->pbSrc[WID*((HEI-1)-h)+w];		// Bottom
		}
	}

	// Left, Right
	for (h = 0; h < HEI; h++)
	{
		for (w = 0; w < 2; w++)
		{
			pPad[PWID*(h+2)+w] = pPars->pbSrc[WID*h+(1-w)];							// Left
			pPad[PWID*(h+2)+(PWID-2+w)] = pPars->pbSrc[WID*h+((WID-1)-w)];			// Right
		}
	}

	for (h = 0; h < HEI; h++)
	{
		for (w = 0; w < WID; w++)
		{
			pPad[(h+2)*PWID+(w+2)] = pPars->pbSrc[WID*h+w];
		}
	}
	// --- 
	// ----- 	 

	// Raw插值转换
	// +++++
	UINT i = 0, j = 0;
	ULONG lIdx = 0, lTmp = 0; 	
	DWORD dwBout = 0, dwGout = 0, dwRout = 0;	
	PRGBPixel pRGBDst = (PRGBPixel)(pPars->pbDst);

	CRAW8ToRGB24 *pRawConv = NULL;
	pRawConv = new CRAW8ToRGB24(pPad, PWID, PHEI);
	ASSERT(NULL != pRawConv); 

#if TESTMODE
	RGBPixel rgbPixel;
	rgbPixel.byB = 0;
	rgbPixel.byG = 0;
	rgbPixel.byR = 255;
#endif

	for (h = 0; h < HEI; h+=2)
	{
		for (w = 0; w < WID; w+=2)
		{
#if !TESTMODE
			pRGBDst[WID*h+w] = pRawConv->BToRGB(h, w);
			pRGBDst[WID*h+(w+1)] = pRawConv->GbToRGB(h, w+1);
			pRGBDst[WID*(h+1)+w] = pRawConv->GrToRGB(h+1, w);
			pRGBDst[WID*(h+1)+(w+1)] = pRawConv->RToRGB(h+1, w+1);
#else
			pRGBDst[WID*h+w] = rgbPixel;
			pRGBDst[WID*h+(w+1)] = rgbPixel;
			pRGBDst[WID*(h+1)+w] = rgbPixel;
			pRGBDst[WID*(h+1)+(w+1)] = rgbPixel;
#endif
		}
	}
	// -----

	SAFEDELETE(pRawConv);				
	SAFEDELETEARRAY(pPad);	
}

void CRawUtil::UVProc(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	RGB24ToPlaneYUV422(pPars); 


	if (pPars->bYEdgeEnhanceEn)
	{
		EdgeEnhance(pPars);
	}

	if (pPars->bYGammaEn)
	{
		YGammaProc(pPars);
	}

	PlanYUV422ToYUY2(pPars);

	YUY2ToRGB24(pPars);	
} 

void CRawUtil::RGB24ToPlaneYUV422(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei;

	BYTE *pbDst = pPars->pbDst;					// In: RGB24 data
	BYTE *pbY = pPars->pbY;						// Out: Y data
	BYTE *pbU = pPars->pbU;					    // Out: U data
	BYTE *pbV = pPars->pbV;					    // Out: V data	  
	int *pOM = pPars->OutMatrix;  				// In: Color Matrix	 
	BYTE *pUVGain = pPars->UVGain;
	BOOL bUVGainEn = pPars->bUVGainEn;

	int Y1, Cb1, Cr1, Y2, Cb2, Cr2, Cb, Cr, h, w;	
	BYTE r1, g1, b1, r2, g2, b2;  
	int newCb1, newCr1, newCb2, newCr2;

	if (!bUVGainEn)
	{
		for (h = 0; h < HEI; h++ )
		{
			for (w = 0; w < WID; w+=2) 		
			{
				b1 = *pbDst++;
				g1 = *pbDst++;
				r1 = *pbDst++;	

				b2 = *pbDst++;
				g2 = *pbDst++;
				r2 = *pbDst++; 			

				Y1 = ( (pOM[0]*r1) + (pOM[1]*g1) + (pOM[2]*b1) )>>6;
				Cb1 = ( (pOM[3]*r1) + (pOM[4]*g1) + (pOM[5]*b1) )>>6;
				Cr1 = ( (pOM[6]*r1) + (pOM[7]*g1) + (pOM[8]*b1) )>>6; 

				Y2 = ( (pOM[0]*r2) + (pOM[1]*g2) + (pOM[2]*b2) )>>6;
				Cb2 = ( (pOM[3]*r2) + (pOM[4]*g2) + (pOM[5]*b2) )>>6;
				Cr2 = ( (pOM[6]*r2) + (pOM[7]*g2) + (pOM[8]*b2) )>>6;  

				Y1 = Y1 > 255 ? 255 : Y1;
				Y2 = Y2 > 255 ? 255 : Y2; 					

				Cb = 128 + (Cb1 + Cb2) / 2;
				Cr = 128 + (Cr1 + Cr2) / 2;

				Cb = Cb > 255 ? 255 : (Cb < 0 ? 0 : Cb);
				Cr = Cr > 255 ? 255 : (Cr < 0 ? 0 : Cr);  

				*pbY = (BYTE)Y1;  pbY++;
				*pbU = (BYTE)Cb;  pbU++;
				*pbY = (BYTE)Y2;  pbY++;
				*pbV = (BYTE)Cr;  pbV++;			
			}
		}
	}
	else
	{
		for (h = 0; h < HEI; h++ )
		{
			for (w = 0; w < WID; w+=2) 		
			{
				b1 = *pbDst++;
				g1 = *pbDst++;
				r1 = *pbDst++;	

				b2 = *pbDst++;
				g2 = *pbDst++;
				r2 = *pbDst++; 			

				Y1 = ( (pOM[0]*r1) + (pOM[1]*g1) + (pOM[2]*b1) )>>6;
				Cb1 = ( (pOM[3]*r1) + (pOM[4]*g1) + (pOM[5]*b1) )>>6;
				Cr1 = ( (pOM[6]*r1) + (pOM[7]*g1) + (pOM[8]*b1) )>>6; 

				Y2 = ( (pOM[0]*r2) + (pOM[1]*g2) + (pOM[2]*b2) )>>6;
				Cb2 = ( (pOM[3]*r2) + (pOM[4]*g2) + (pOM[5]*b2) )>>6;
				Cr2 = ( (pOM[6]*r2) + (pOM[7]*g2) + (pOM[8]*b2) )>>6;  

				Y1 = Y1 > 255 ? 255 : Y1;
				Y2 = Y2 > 255 ? 255 : Y2;

				newCb1 = (Cb1*pUVGain[0] + Cr1*pUVGain[1])>>6;
				newCr1 = (Cb1*pUVGain[2] + Cr1*pUVGain[3])>>6;
				newCb2 = (Cb2*pUVGain[0] + Cr2*pUVGain[1])>>6;
				newCr2 = (Cr2*pUVGain[2] + Cr2*pUVGain[3])>>6; 	

				Cb1 = newCb1;
				Cr1 = newCr1;
				Cb2 = newCb2;
				Cr2 = newCr2;			

				Cb = 128 + (Cb1 + Cb2) / 2;
				Cr = 128 + (Cr1 + Cr2) / 2;

				Cb = Cb > 255 ? 255 : (Cb < 0 ? 0 : Cb);
				Cr = Cr > 255 ? 255 : (Cr < 0 ? 0 : Cr);  

				*pbY = (BYTE)Y1;  pbY++;
				*pbU = (BYTE)Cb;  pbU++;
				*pbY = (BYTE)Y2;  pbY++;
				*pbV = (BYTE)Cr;  pbV++;			
			}
		}
	}	
}	

void CRawUtil::EdgeEnhance(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei;
	const UINT PWID = WID + 2;				// Padding width
	const UINT PHEI = HEI + 2;				// Padding height

	BYTE *pbY = pPars->pbY;
	BYTE *pbNewY = new BYTE[PWID*PHEI];
	ASSERT(NULL != pbNewY);
	if (NULL == pbNewY)
	{
		return;
	}

	int h =0, w = 0;
	// 使用3x3模板进行边缘增强, 为了处理原始图像边缘, 需要对原始图像四周
	// 做扩展(Padding), 即四周分别增加一行 
	pbNewY[0*PWID+0] = pbY[1*WID+1];													// Left-Top
	pbNewY[0*PWID+(PWID-1)] = pbY[1*WID+((WID-1)-1)];									// Right-Top
	pbNewY[(PHEI-1)*PWID+0] = pbY[((HEI-1)-1)*WID+1];									// Left-Bottom
	pbNewY[(PHEI-1)*PWID+(PWID-1)] = pbY[((HEI-1)-1)*WID+((WID-1)-1)];					// Right-Bottom

	// Top&Bottom Paddding Line	 
	for (w = 0; w < WID; w++)
	{
		pbNewY[0*PWID+(w+1)] = pbY[1*WID+w];
		pbNewY[(PHEI-1)*PWID+(w+1)] = pbY[((HEI-1)-1)*WID+w];
	} 

	// Left&Right Padding Line
	for (h = 0; h < HEI; h++)
	{  
		pbNewY[(h+1)*PWID+0] = pbY[h*WID+1];
		pbNewY[(h+1)*PWID+(PWID-1)] = pbY[h*WID+((WID-1)-1)];
	}

	for (h = 0; h < HEI; h++)
	{
		for (w = 0; w < WID; w++)
		{
			pbNewY[(h+1)*PWID+(w+1)] = pbY[h*WID+w];
		}
	}

	int YEdgeHB = pPars->YEdgeHB;
	int YEdgeLB = pPars->YEdgeLB;
	int YEdgeThd = pPars->YEdgeThd;
	BYTE Edge_Gain = pPars->YEdgeGain>>5;


	int YIn = 0;						// 存贮3x3模板中心值 
	int YOut = 0;  
	int dt[8] = {0};					// 存贮3x3模板(除中心值外)的8个值  
	int YDiff = 0;

	int iTmp = 0;
	for (h = 0; h < HEI; h++)
	{
		for (w = 0; w < WID; w++)
		{
			YIn = pbNewY[(h+1)*PWID+(w+1)];

			{
				dt[0] = pbNewY[h*PWID+w];
				dt[1] = pbNewY[h*PWID+(w+1)];
				dt[2] = pbNewY[h*PWID+(w+2)];
				dt[3] = pbNewY[(h+1)*PWID+w];
				dt[4] = pbNewY[(h+1)*PWID+(w+2)];
				dt[5] = pbNewY[(h+2)*PWID+w];
				dt[6] = pbNewY[(h+2)*PWID+(w+1)];
				dt[7] = pbNewY[(h+2)*PWID+(w+2)];  

				YDiff = (YIn*8) - (dt[0] + dt[1] + dt[2] + dt[3] + dt[4] + dt[5] + dt[6] + dt[7]); 

				if (YDiff >= YEdgeThd)
				{
					iTmp = YIn + (YDiff - YEdgeThd)*Edge_Gain;
					YOut = (iTmp > 255) ? 255 : ( (iTmp < 0) ?  0 : iTmp );
				}
				else if (YDiff <= (-YEdgeThd))
				{
					iTmp = YIn + (YDiff + YEdgeThd)*Edge_Gain;
					YOut = (iTmp > 255) ? 255 : ( (iTmp < 0) ?  0 : iTmp );
				}
				else
				{
					YOut = YIn;
				}

				pbY[h*WID+w] = YOut; 	
			}
		}
	}	

	SAFEDELETEARRAY(pbNewY);
}

void CRawUtil::YGammaProc(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei;
	BYTE *pbY = pPars->pbY;
	BYTE *pYGammaTable = pPars->YGammaTable;

	int h, w;
	for (h = 0; h < HEI; h++)
	{
		for (w = 0; w < WID; w++)
		{ 
			*pbY++ = pYGammaTable[*pbY];
		}
	} 
}

void CRawUtil::UVGainProc(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	const UINT WID = pPars->uiWid / 2;
	const UINT HEI = pPars->uiHei;
	BYTE *pbU = pPars->pbU;
	BYTE *pbV = pPars->pbV;	
	BYTE *pUVGain = pPars->UVGain;
	int oldU, oldV;
	int newU, newV;

	int h, w;
	for (h = 0; h < HEI; h++)
	{
		for (w = 0; w < WID; w++)
		{
			oldU = *pbU;
			oldV = *pbV; 			

			newU = (oldU*pUVGain[0] + oldV*pUVGain[1])>>6;
			newV = (oldU*pUVGain[2] + oldV*pUVGain[3])>>6;				

			newU = newU > 255 ? 255 : newU;
			newV = newV > 255 ? 255 : newV;

			*pbU = (BYTE)newU;
			*pbV = (BYTE)newV;

			pbU++;
			pbV++; 
		}
	} 
} 

// 将平面型422转换为紧缩型422
void CRawUtil::PlanYUV422ToYUY2(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei; 
	BYTE *pYUY2 = pPars->pbYUY2;  
	BYTE *pY = pPars->pbY;
	BYTE *pU = pPars->pbU;
	BYTE *pV = pPars->pbV; 
	YUVChannelType yuvCh = pPars->YUVChannel;
	BOOL bBWMode = pPars->bBWMode;

	if (bBWMode)
	{
		yuvCh = YUV_Y;
	}

	int h, w;
	for (h = 0; h < HEI; h++)
	{
		for (w = 0; w < WID; w+=2)
		{ 
			if (YUV_YUV == yuvCh)
			{
				*pYUY2++ = *pY++;
				*pYUY2++ = *pU++; 
				*pYUY2++ = *pY++;
				*pYUY2++ = *pV++; 				
			}
			else if (YUV_Y == yuvCh)
			{
				*pYUY2++ = *pY++;
				*pYUY2++ = 128; 
				*pYUY2++ = *pY++;
				*pYUY2++ = 128;				
			}
			else if (YUV_U == yuvCh)
			{
				*pYUY2++ = *pY++;
				*pYUY2++ = *pU++; 
				*pYUY2++ = *pY++;
				*pYUY2++ = 128; 
			}
			else if (YUV_V == yuvCh)
			{
				*pYUY2++ = *pY++;
				*pYUY2++ = 128; 
				*pYUY2++ = *pY++;
				*pYUY2++ = *pV++; 
			}   
		}
	}
}

void CRawUtil::YUY2ToRGB24(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei; 
	BYTE *pbYUY2 = pPars->pbYUY2;  

	YUY2ToRGB24(pbYUY2, pPars->pbDst, WID, HEI); 
}

void CRawUtil::YUY2ToRGB24( BYTE *pbyteSrc, BYTE *pbyteDest, long lwidth, long lheight )
{ 
	LONG width, height;
	LONG bOffset, yOffset, uOffset, vOffset;
	double bValue, gValue, rValue;	
	width = lwidth;
	height = lheight;
	const BYTE colorMatrix[9] = {

	};

	int h, w;
	for (h = 0; h < height; ++h) 
	{
		for (w = 0; w < width; ++w) 
		{
			yOffset = ( h * width + w) * 2;
			uOffset = ( h * width + w)/2 * 4 + 1;
			vOffset = ( h * width + w)/2 * 4 + 3;
			bOffset = ((height - h - 1) * width + w) * 3;

			bValue = 1.164 * (pbyteSrc[yOffset] - 16) + 2.017 * (pbyteSrc[uOffset] - 128);
			gValue = 1.164 * (pbyteSrc[yOffset] - 16) - 0.392 * (pbyteSrc[uOffset] - 128) - 0.813 * (pbyteSrc[vOffset] - 128);
			rValue = 1.164 * (pbyteSrc[yOffset] - 16) + 1.596 * (pbyteSrc[vOffset] - 128);

			pbyteDest[bOffset + 0] = (bValue > 255) ? (BYTE)255 : (bValue < 0 ) ? (BYTE)0 : (BYTE)bValue;
			pbyteDest[bOffset + 1] = (gValue > 255) ? (BYTE)255 : (gValue < 0 ) ? (BYTE)0 : (BYTE)gValue;
			pbyteDest[bOffset + 2] = (rValue > 255) ? (BYTE)255 : (rValue < 0 ) ? (BYTE)0 : (BYTE)rValue;
		}
	}	
}

#define PI 3.1415926
void CRawUtil::RGB24toYUY2_byUVGain(CRawPars *pPars, BYTE *yuv)
{
	ASSERT(NULL != pPars);
	ASSERT(NULL != yuv); 

	ASSERT(0);
}	

void CRawUtil::RGB24toYUY2_byMatrix(CRawPars *pPars, BYTE *yuv)
{
	ASSERT(NULL != pPars);
	ASSERT(NULL != yuv);  

	ASSERT(0);
}


void CRawUtil::UpdateMatrix(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	ASSERT(0);
}

void CRawUtil::SetBrightness(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	if ((pPars->Brightness > -128) && (pPars->Brightness < 128))
	{
		CreateRGBGammaTable(pPars);
	}
}

void CRawUtil::SetContrast(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	if ((pPars->Contrast >= 0) && (pPars->Contrast <= 255))
	{
		CreateRGBGammaTable(pPars);
	}
}


void CRawUtil::SetGammaValue(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	if (pPars->dRGBGammaVal >= 0.0)
	{
		CreateGammaTable(pPars);
	}
}

void CRawUtil::SetContrastCenter(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	if ((pPars->ContrastCenter < 0) || (pPars->ContrastLmt < 0)
		|| (pPars->ContrastCenter > 255) || (pPars->ContrastLmt > 255))
	{
		return;
	}

	if (pPars->ContrastLmt >= pPars->ContrastCenter)
	{
		return;
	}

	CreateRGBGammaTable(pPars);
}

void CRawUtil::SetYGammaValue(const double &dYGammaValue, CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	if ((dYGammaValue >= 0.0) && (dYGammaValue <= 1.00))
	{
		pPars->dYGammaVal = dYGammaValue;
		CreateYGammaTable(pPars);
	}
} 

void CRawUtil::CreateParamReg(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	CreateRGBGammaTable(pPars);
	CreateYGammaTable(pPars);
}

void CRawUtil::CreateParamUI(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	SetBrightness(pPars);
	SetContrast(pPars);	
	SetGammaValue(pPars);
}	 

void CRawUtil::CreateRGBGammaTable(CRawPars *pPars)
{ 
	ASSERT(NULL != pPars);

	int i = 0, Idx = 0, Offset = 0;
	BYTE *RGBGammaTable = pPars->RGBGammaTable;
	//	BYTE *RGBGamma = pPars->RGBGammaNode;
	BYTE RGBGamma[RGBGNODENUM];
	BOOL bNegative = pPars->bNegative; 
	NEGATIVEIMPType negativeImpType = pPars->negativeImpType;
	int iTmp = 0;

	for (i = 0; i < RGBGNODENUM; i++)
	{
		RGBGamma[i] = pPars->RGBGammaNode[i];
	}

	/*if (bNegative && ( NEGATIVEIMP_ReverseRGBGamma == negativeImpType))
	{
	for (i = 0; i < RGBGNODENUM; i++)
	{
	RGBGamma[i] = 255 - pPars->RGBGammaNode[i];
	}
	}*/

	// 256 = (2 x 4) + (3 x 8) + (9 x 16) + (8 x 8) + (1 x 16)
	for (i = 0; i < 256; i ++)
	{
		if (i < 8)		// 8	[0, 1]
		{
			Idx = i>>2;
			Offset = i&0x03;
			if (RGBGamma[Idx+1]>RGBGamma[Idx])
			{
				iTmp =  RGBGamma[Idx]+(Offset*(RGBGamma[Idx+1]+1-RGBGamma[Idx])>>2);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{ 
				iTmp =  RGBGamma[Idx]-(Offset*(RGBGamma[Idx]+1-RGBGamma[Idx+1])>>2);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255); 
			}
		}
		else if (i < 32)	// 24	[2, 4]
		{
			Idx = 2 + ((i-8)>>3);
			Offset = (i-8)&0x07;
			if (RGBGamma[Idx+1]>RGBGamma[Idx])
			{
				iTmp =  RGBGamma[Idx]+(Offset*(RGBGamma[Idx+1]+1-RGBGamma[Idx])>>3);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{
				iTmp =  RGBGamma[Idx]-(Offset*(RGBGamma[Idx]+1-RGBGamma[Idx+1])>>3);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);
			}
		}
		else if (i < 176)	// 144	[5, 13]
		{
			Idx = 5 + ((i-32)>>4);
			Offset = (i-32)&0x0f;
			if (RGBGamma[Idx+1]>RGBGamma[Idx])
			{
				iTmp =  RGBGamma[Idx]+(Offset*(RGBGamma[Idx+1]+1-RGBGamma[Idx])>>4);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{ 
				iTmp =  RGBGamma[Idx]-(Offset*(RGBGamma[Idx]+1-RGBGamma[Idx+1])>>4);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);
			}
		}
		else if (i< 240)	// 64	[14, 21]
		{
			Idx = 14 + ((i-176)>>3);
			Offset = (i-176)&0x07;
			if (RGBGamma[Idx+1]>RGBGamma[Idx])
			{
				iTmp =  RGBGamma[Idx]+(Offset*(RGBGamma[Idx+1]+1-RGBGamma[Idx])>>3);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{				
				iTmp =  RGBGamma[Idx]-(Offset*(RGBGamma[Idx]+1-RGBGamma[Idx+1])>>3);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);

			}
		}
		else	// 16	[22]
		{
			Idx = 22 + ((i-240)>>4);	 
			Offset = (i-240)&0x0f;
			if (RGBGamma[Idx+1]>RGBGamma[Idx])
			{
				iTmp =  RGBGamma[Idx]+(Offset*(RGBGamma[Idx+1]+1-RGBGamma[Idx])>>4);
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{
				iTmp =  RGBGamma[Idx]-(Offset*(RGBGamma[Idx]+1-RGBGamma[Idx+1])>>4); 
				RGBGammaTable[i] = CLIP(iTmp, 0, 255);
			}
		}		
	} 
}

void CRawUtil::CreateGammaTable(CRawPars *pPars)
{
	ASSERT(NULL != pPars);	

	CreateRGBGammaTable(pPars);
}

void CRawUtil::CreateYGammaTable(CRawPars *pPars)
{ 
	ASSERT(NULL != pPars);

	int i = 0, Idx = 0, Offset = 0;
	BYTE *YGamma = pPars->YGammaNode;
	BYTE *YGammaTable = pPars->YGammaTable;
	int iTmp = 0;

	// 256 = (4 x 8) + (4 x 16) + (3 x 32) + (2 x 16) + (4 x 8)
	for (i = 0;i < 256; i ++)
	{
		if (i < 32)		// 0~31: 32		[0, 3]
		{
			Idx = i>>3;
			Offset = i&0x07;
			if (YGamma[Idx+1]>YGamma[Idx])
			{
				iTmp = YGamma[Idx]+(Offset*(YGamma[Idx+1]+1-YGamma[Idx])>>3);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{
				iTmp = YGamma[Idx]-(Offset*(YGamma[Idx]-YGamma[Idx+1])>>3);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
		}
		else if(i < 96)		// 64	[4, 7]
		{
			Idx = 4+((i-32)>>4);
			Offset = (i-32)&0x0F;
			if (YGamma[Idx+1]>YGamma[Idx])
			{
				iTmp = YGamma[Idx]+(Offset*(YGamma[Idx+1]+1-YGamma[Idx])>>4);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{
				iTmp = YGamma[Idx]-(Offset*(YGamma[Idx]+1-YGamma[Idx+1])>>4);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
		}
		else if(i < 192)	// 96	[8, 10]
		{
			Idx = 8+((i-96)>>5);
			Offset = (i-96)&0x1F;
			if (YGamma[Idx+1]>YGamma[Idx])
			{
				iTmp = YGamma[Idx]+(Offset*(YGamma[Idx+1]+1-YGamma[Idx])>>5);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{
				iTmp = YGamma[Idx]-(Offset*(YGamma[Idx]+1-YGamma[Idx+1])>>5);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
		}
		else if(i < 224)	// 32	[11, 12]
		{
			Idx = 11+((i-192)>>4);
			Offset = (i-192)&0x0F;
			if (YGamma[Idx+1]>YGamma[Idx])
			{
				iTmp = YGamma[Idx]+(Offset*(YGamma[Idx+1]+1-YGamma[Idx])>>4);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{
				iTmp = YGamma[Idx]-(Offset*(YGamma[Idx]+1-YGamma[Idx+1])>>4);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
		}
		else	// 32	[13, 16]
		{
			Idx = 13+((i-224)>>3);
			Offset = (i-224)&0x07;
			if (YGamma[Idx+1]>YGamma[Idx])	
			{
				iTmp = YGamma[Idx]+(Offset*(YGamma[Idx+1]+1-YGamma[Idx])>>3);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
			else
			{
				iTmp = YGamma[Idx]-(Offset*(YGamma[Idx]+1-YGamma[Idx+1])>>3);
				YGammaTable[i] = CLIP(iTmp, 0, 255);
			}
		}
	}

	// Brightness  & Contrast
	const double dUNIT = 1 / 32.0;
	for (i = 0; i < 256; i++)
	{ 		
		iTmp = (YGammaTable[i] - pPars->ContrastCenter)*(pPars->Contrast*dUNIT) + pPars->Brightness + pPars->ContrastCenter;
		YGammaTable[i] = CLIP(iTmp, 0, 255);
	} 	
}

// 实现所谓的"Color Killer"功能
void CRawUtil::ColorCorpping( CRawPars *pPars )
{
	ASSERT(NULL != pPars); 

	if (0 == pPars->YHB && 0 == pPars->YLB)
	{
		return;
	}

	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei;

	BYTE *pbDst = pPars->pbDst;					// In: RGB24 data  	   
	BYTE YHB = pPars->YHB;
	BYTE YLB = pPars->YLB;
	int *pOM = pPars->OutMatrix; 

	int Y1, h, w;	
	BYTE r1, g1, b1;  
	int iR, iG, iB;
	for (h = 0; h < HEI; h++ )
	{
		for (w = 0; w < WID; w++) 		
		{

			b1 = *pbDst;
			g1 = *(pbDst+1);
			r1 = *(pbDst+2);	

			Y1 = ( (pOM[0]*r1) + (pOM[1]*g1) + (pOM[2]*b1) )>>6; 
			Y1 = Y1 > 255 ? 255 : Y1;

			if (Y1 < 32 || Y1 > 220)
			{
				iB = b1 + YHB;
				iB = iB > 255 ? 255 : iB;
				iB = iB - (YLB + YHB);
				iB = iB < 0 ? 0 : iB; 
				*pbDst = (BYTE)iB;

				iG = b1 + YHB;
				iG = iG > 255 ? 255 : iG;
				iG = iG - (YLB + YHB);
				iG = iG < 0 ? 0 : iG; 
				*(pbDst+1) = (BYTE)iG;

				iR = b1 + YHB;
				iR = iR > 255 ? 255 : iR;
				iR = iR - (YLB + YHB);
				iR = iR < 0 ? 0 : iR; 
				*(pbDst+2) = (BYTE)iR;

				pbDst += 3;
			}

		}
	}	
}

// GbGrBalance原理:
// 提供一簡單的作法: 在 Bayer Raw data 上求出中間G 與周圍四點G的平均差值 Gdiff
// 當差值小於某 Thd (parameter) 令 Gcenter = Gcenter - Gdiff/2
void CRawUtil::GbGrBalance( CRawPars *pPars )
{
	ASSERT(NULL != pPars);		

	const UINT WID = pPars->uiWid;
	const UINT HEI = pPars->uiHei;
	BYTE *pbSrc = pPars->pbSrc;	 
	BYTE GbGrBalanceThd = pPars->GbGrBalanceThd;
	GbGrBalance_TemplateValues cusPos;
	int Gdiff = 0;

	int h = 0, w = 0;  	

	for (h = 0; h < HEI; h+=2)
	{
		for (w = 1; w < WID; w+=2)
		{
			GbGrBalance_GetValuesByPos(pbSrc, WID, HEI, h, w, cusPos);

			Gdiff = cusPos.cnt*4 - (cusPos.lt + cusPos.rt + cusPos.lb + cusPos.rb);
			Gdiff = (int)(Gdiff/4.0);

			if (abs(Gdiff) < GbGrBalanceThd)
			{
				cusPos.cnt =  cusPos.cnt - Gdiff/2;
				pbSrc[WID*h+w] = cusPos.cnt;
			}
		}
	}

	for (h = 1; h < HEI; h+=2)
	{
		for (w = 0; w < WID; w+=2)
		{
			GbGrBalance_GetValuesByPos(pbSrc, WID, HEI, h, w, cusPos);

			Gdiff = cusPos.cnt*4 - (cusPos.lt + cusPos.rt + cusPos.lb + cusPos.rb);
			Gdiff = (int)(Gdiff/4.0);

			if (abs(Gdiff) < GbGrBalanceThd)
			{
				cusPos.cnt =  cusPos.cnt - Gdiff/2;
				pbSrc[WID*h+w] = cusPos.cnt;
			}
		}
	}
}

void CRawUtil::GbGrBalance_GetValuesByPos(BYTE *pbSrc, UINT uiWid, UINT uiHei, int h, int w, GbGrBalance_TemplateValues& cusPos)
{
	ASSERT(NULL != pbSrc);

	cusPos.cnt = pbSrc[uiWid*h+w];
	if (0 == h)
	{		
		cusPos.lt = pbSrc[uiWid*(h+1)+(w-1)];
		cusPos.rt = pbSrc[uiWid*(h+1)+(w+1)];
		cusPos.lb = pbSrc[uiWid*(h+1)+(w-1)];
		cusPos.rb = pbSrc[uiWid*(h+1)+(w+1)];
	}
	else  if ((uiHei-1) == h)
	{
		cusPos.lt = pbSrc[uiWid*(h-1)+(w-1)];
		cusPos.rt = pbSrc[uiWid*(h-1)+(w+1)];
		cusPos.lb = pbSrc[uiWid*(h-1)+(w-1)];
		cusPos.rb = pbSrc[uiWid*(h-1)+(w+1)];
	}
	else if (0 == w)
	{
		cusPos.lt = pbSrc[uiWid*(h-1)+(w+1)];
		cusPos.rt = pbSrc[uiWid*(h+1)+(w+1)];
		cusPos.lb = pbSrc[uiWid*(h-1)+(w+1)];
		cusPos.rb = pbSrc[uiWid*(h+1)+(w+1)];
	}
	else if ((uiWid-1) == w)
	{
		cusPos.lt = pbSrc[uiWid*(h-1)+(w-1)];
		cusPos.rt = pbSrc[uiWid*(h+1)+(w-1)];
		cusPos.lb = pbSrc[uiWid*(h-1)+(w-1)];
		cusPos.rb = pbSrc[uiWid*(h+1)+(w-1)];
	}
	else
	{
		cusPos.lt = pbSrc[uiWid*(h-1)+(w-1)];
		cusPos.rt = pbSrc[uiWid*(h-1)+(w+1)];
		cusPos.lb = pbSrc[uiWid*(h+1)+(w-1)];
		cusPos.rb = pbSrc[uiWid*(h+1)+(w+1)];
	}
} 

BYTE CRawUtil::MidFiltering_GetMidValue(_3x3_TemplateValues vals)
{
	std::list<BYTE> list;
	std::list<BYTE>::iterator listItor;

	const UINT elemNUM = 9;
	for (int i = 0; i < elemNUM; i++)
	{
		list.push_back(vals.Val[i]);
	}
	list.sort();

	listItor = list.begin();
	// 注: std::list<Type>::iterator 不接受"+" and "+=" 操作符
	listItor++;
	listItor++;
	listItor++;
	listItor++;

	return *listItor;
}

void CRawUtil::MidFiltering_GetValueByPos(BYTE *pbSrc, UINT uiWid, UINT uiHei, int h, int w, _3x3_TemplateValues& vals)
{
	ASSERT(NULL != pbSrc);

	vals.Val[0] = pbSrc[(h-1)*uiWid+(w-1)];
	vals.Val[1] = pbSrc[(h-1)*uiWid+w];
	vals.Val[2] = pbSrc[(h-1)*uiWid+(w+1)];
	vals.Val[3] = pbSrc[h*uiWid+(w-1)];
	vals.Val[4] = pbSrc[h*uiWid+w];
	vals.Val[5] = pbSrc[h*uiWid+w+1];
	vals.Val[6] = pbSrc[(h+1)*uiWid+(w-1)];
	vals.Val[7] = pbSrc[(h+1)*uiWid+w];
	vals.Val[8] = pbSrc[(h+1)*uiWid+(w+1)];
}

// 中值滤波
// 以3x3模板对图像进行处理, 对图像边缘不做处理
void CRawUtil::MidFiltering( UINT uiWid, UINT uiHei, BYTE *pbSrc )
{
	ASSERT(NULL != pbSrc);

	BYTE *pbSrcTmp = NULL;
	const UINT uiSIZE = uiWid * uiHei;
	pbSrcTmp = new BYTE[uiSIZE];
	ASSERT(NULL != pbSrcTmp);
	if (NULL == pbSrcTmp)
	{
		return;
	}
	memcpy(pbSrcTmp, pbSrc, uiSIZE);   

	int h = 0, w = 0;
	int m = 0, n = 0;
	BYTE temp[9];
	int max = 0;

	for (h = 1; h < uiHei-1; h++)
	{
		for (w = 1; w < uiWid-1; w++)
		{
			temp[0] = pbSrc[(h-1)*uiWid+(w-1)];
			temp[1] = pbSrc[(h-1)*uiWid+w];
			temp[2] = pbSrc[(h-1)*uiWid+(w+1)];
			temp[3] = pbSrc[h*uiWid+(w-1)];
			temp[4] = pbSrc[h*uiWid+w];
			temp[5] = pbSrc[h*uiWid+w+1];
			temp[6] = pbSrc[(h+1)*uiWid+(w-1)];
			temp[7] = pbSrc[(h+1)*uiWid+w];
			temp[8] = pbSrc[(h+1)*uiWid+(w+1)];

			//利用气泡法计算中值
			max = temp[0];
			for (m = 0; m < 3*3-1; m++)
			{
				for (n = 0; n < 3*3-m-1; n++)
				{
					if(temp[n] > temp[n+1])
					{
						max = temp[n];
						temp[n] = temp[n+1];
						temp[n+1] = max;
					}										
				}				
			}

			pbSrc[h*uiWid+w] = temp[(3*3-1)/2];	 
		}
	} 	

	SAFEDELETEARRAY(pbSrcTmp);
}

void CRawUtil::MidFiltering_Y( CRawPars *pPars )
{
	ASSERT(NULL != pPars);

	MidFiltering(pPars->uiWid, pPars->uiHei, pPars->pbY);	
}

void CRawUtil::MidFiltering_U( CRawPars *pPars )
{
	ASSERT(NULL != pPars);

	MidFiltering(pPars->uiWid/2, pPars->uiHei, pPars->pbU);  
}

void CRawUtil::MidFiltering_V( CRawPars *pPars )
{
	ASSERT(NULL != pPars);

	MidFiltering(pPars->uiWid/2, pPars->uiHei, pPars->pbV);
}


#define SCALEFACTOR 7
void CRawUtil::LensShadingCompensation( CRawPars *pPars )
{
	ASSERT(NULL != pPars); 	

	int x, y;
	int i = 0;
	int nShift,iLc_X_Offset,iLc_Y_Offset;

	if ((pPars->uiWid == _9M_RAW_SIZE_WID) && (pPars->uiHei == _9M_RAW_SIZE_HEI))
	{
		pPars->iLc_Shift9M = 7;
		pPars->iLc_X_Offset9M = 0;
		pPars->iLc_Y_Offset9M = 32;

		nShift = pPars->iLc_Shift9M;
		iLc_X_Offset = pPars->iLc_X_Offset9M;
		iLc_Y_Offset = pPars->iLc_Y_Offset9M;
	}
	else if ((pPars->uiWid == _5M_RAW_SIZE_WID) && (pPars->uiHei == _5M_RAW_SIZE_HEI))
	{
		pPars->iLc_Shift5M = 7;
		pPars->iLc_X_Offset5M = 416;
		pPars->iLc_Y_Offset5M = 300;  		

		nShift = pPars->iLc_Shift5M;
		iLc_X_Offset = pPars->iLc_X_Offset5M;
		iLc_Y_Offset = pPars->iLc_Y_Offset5M;
	}
	else if ((pPars->uiWid == _3M_RAW_SIZE_WID) && (pPars->uiHei == _3M_RAW_SIZE_HEI))
	{
		nShift = pPars->iLc_Shift3M;
		iLc_X_Offset = pPars->iLc_X_Offset3M;
		iLc_Y_Offset = pPars->iLc_Y_Offset3M;
	} 
	else if((pPars->uiWid == 1600) && (pPars->uiHei == 1200))
	{
		nShift = pPars->iLc_Shift0;
		iLc_X_Offset = pPars->iLc_X_Offset0;
		iLc_Y_Offset = pPars->iLc_Y_Offset0;
	}
	else if((pPars->uiWid == 1280) && (pPars->uiHei == 1024))
	{
		nShift = pPars->iLc_Shift1;
		iLc_X_Offset = pPars->iLc_X_Offset1;
		iLc_Y_Offset = pPars->iLc_Y_Offset1;
	}
	else if((pPars->uiWid == 1280) && (pPars->uiHei == 960))
	{
		nShift = pPars->iLc_Shift2;
		iLc_X_Offset = pPars->iLc_X_Offset2;
		iLc_Y_Offset = pPars->iLc_Y_Offset2;
	}
	else if((pPars->uiWid == 1280) && (pPars->uiHei == 720))
	{
		nShift = pPars->iLc_Shift3;
		iLc_X_Offset = pPars->iLc_X_Offset3;
		iLc_Y_Offset = pPars->iLc_Y_Offset3;
	}
	else if((pPars->uiWid == 800) && (pPars->uiHei == 600))
	{
		nShift = pPars->iLc_Shift4;
		iLc_X_Offset = pPars->iLc_X_Offset4;
		iLc_Y_Offset = pPars->iLc_Y_Offset4;
	}
	else if((pPars->uiWid == 640) && (pPars->uiHei == 480))
	{
		nShift = pPars->iLc_Shift5;
		iLc_X_Offset = pPars->iLc_X_Offset5;
		iLc_Y_Offset = pPars->iLc_Y_Offset5;
	}
	else
	{
		return;	 
	}

	{				
		int esiADD = (int)pPars->pbSrc;		
		int AddEsi = esiADD;		

		int xPos[2], yPos[2];
		UINT16 xDis[2], yDis[2];				
		UINT16 xDis1[2];
		int Temp = 1 << nShift;	
		UINT16 UnitGain = 1<<SCALEFACTOR;

		for(y=0; y<pPars->uiHei; y+=2)//The Odd Line: B G
		{
			yPos[0] = CLIP(( y + iLc_Y_Offset) >> nShift, 0, GAINTABLEHEI-1);
			yPos[1] = CLIP(((y + iLc_Y_Offset) >> nShift) + 1, 0, GAINTABLEHEI-1);
			yDis[0]	= max((y + iLc_Y_Offset) - (yPos[0] << nShift),0);
			yDis[1]	= Temp - yDis[0];

			for(x=0; x<pPars->uiWid; x+=8)		
			{
				xPos[0] = CLIP((x + iLc_X_Offset) >> nShift, 0, GAINTABLEWID-1);
				xPos[1] = CLIP(((x + iLc_X_Offset) >> nShift) + 1, 0, GAINTABLEWID-1);
				xDis[0]	= max((x + iLc_X_Offset) - (xPos[0]  << nShift),0);
				xDis[1]	= Temp - xDis[0];						
				xDis1[0]= xDis[0]+1;
				xDis1[1]= xDis[1]-1;

				UINT16 i16_yDis[4] = {yDis[1],yDis[0],yDis[1],yDis[0]}; 
				UINT16 i16_x[4] = {-2,2,-2,2};		//The change of i16_xDis
				UINT16 i16_xDis[4] = {xDis[1],xDis[0],xDis1[1],xDis1[0]};

				UINT16 i16_WeiB[4] = {
					pPars->LCWeiMtxB[yPos[0]][xPos[0]]+UnitGain,
					pPars->LCWeiMtxB[yPos[1]][xPos[0]]+UnitGain,
					pPars->LCWeiMtxB[yPos[0]][xPos[1]]+UnitGain,
					pPars->LCWeiMtxB[yPos[1]][xPos[1]]+UnitGain
				};
				UINT16 i16_WeiG[4] = {
					pPars->LCWeiMtxG[yPos[0]][xPos[0]]+UnitGain,
					pPars->LCWeiMtxG[yPos[1]][xPos[0]]+UnitGain,
					pPars->LCWeiMtxG[yPos[0]][xPos[1]]+UnitGain,
					pPars->LCWeiMtxG[yPos[1]][xPos[1]]+UnitGain
				}; 				


				//8*1 pixels per loop					
				esiADD = AddEsi+y * pPars->uiWid+x;

				__asm
				{
					mov			esi,			esiADD;		//source data & des data: BYTE
					pxor		mm4,			mm4;		// 清零m4
					movd		mm4,			nShift;
					//--------------------------------------------------------------------------------							
					movq		mm1,			[i16_yDis];	//mm1: yDis0 yDis1 yDis0 yDis1							
					movq		mm7,			mm1;
					pmaddwd		mm7,			[i16_WeiB];	//mm7: H：yDis0 * iWei11+yDis1 * iWei01 ，L：yDis0 * iWei10+yDis1 * iWei00 
					psrld		mm7,			mm4;		//mm7: 00  (yDis0 * iWei11+yDis1 * iWei01)   00   	(yDis0 * iWei10+yDis1 * iWei00)					
					movq		mm0,			mm1;
					pmaddwd		mm0,			[i16_WeiG];
					psrld		mm0,			mm4;
					packssdw	mm7,			mm0;		//mm7: (yDis0 * iWei11G+yDis1 * iWei01G)  (yDis0 * iWei10G+yDis1 * iWei00G)	 (yDis0 * iWei11B+yDis1 * iWei01B) (yDis0 * iWei10B+yDis1 * iWei00B)					
					//--------------------------------------------------------------------------------							
					movq		mm2,			[i16_xDis];	//mm2: j+1 xDis1-1 j xDis1 
					movq		mm3,			mm2;				
					pmaddwd		mm3,			mm7;		//mm3: H: weight1G L: weight1B
					psrld		mm3,			mm4;

					paddsw		mm2,			[i16_x];	//mm2: j+3 xDis1-3 j+2 xDis1-2 
					movq		mm5,			mm2;							
					pmaddwd		mm5,			mm7;		//mm5：H: weight2G L: weight2B
					psrld		mm5,			mm4;
					packssdw	mm3,			mm5;		//mm3：weight2G weight2B, weight1G weight1B

					paddsw		mm2,			[i16_x];	//mm2: j+5 xDis1-5 j+4 xDis1-4 
					movq		mm0,			mm2;				
					pmaddwd		mm0,			mm7;		//mm0: H: weight3G L: weight3B
					psrld		mm0,			mm4;

					paddsw		mm2,			[i16_x];	//mm2: j+7 xDis1-7 j+6 xDis1-6 
					movq		mm5,			mm2;							
					pmaddwd		mm5,			mm7;		//mm5：H: weight4G L: weight4B
					psrld		mm5,			mm4;
					packssdw	mm0,			mm5;		//mm0：weight4G weight4B, weight3G weight3B
					//--------------------------------------------------------------------------------						
					movq        mm1,			mm3;
					movd        mm5,			[esi];		//mm5: 2G 2B 1G 1B 
					pxor		mm4,			mm4;
					punpcklbw	mm5,			mm4;		//mm5: 00 2G 00 2B 00 1G 00 1B
					pmullw		mm1,			mm5;		//mm1: L：weight2G*2G weight2B*2B weight1G*1G weight1B*1B
					pmulhw		mm3,			mm5;
					movq		mm5,			mm1;
					punpckhwd	mm5,			mm3;		//mm5:	weight2G*2G weight2B*2B
					punpcklwd	mm1,			mm3;		//mm1:	weight1G*1G weight1B*1B
					psrld		mm5,			SCALEFACTOR;
					psrld		mm1,			SCALEFACTOR;
					packssdw	mm1,			mm5;		//mm1： 2G 2B 1G 1B 

					movq        mm2,			mm0;
					movd        mm5,			[esi+4];	//mm5: 4G 4B 3G 3B 
					pxor		mm4,			mm4;
					punpcklbw	mm5,			mm4;		//mm5: 00 4G 00 4B 00 3G 00 3B
					pmullw		mm2,			mm5;		//mm2: L：weight4G*4G weight4B*4B weight3G*3G weight3B*3B
					pmulhw		mm0,			mm5;
					movq		mm5,			mm2;
					punpckhwd	mm5,			mm0;		//mm5:	weight4G*4G weight4B*4B
					punpcklwd	mm2,			mm0;		//mm2:	weight3G*3G weight3B*3B
					psrld		mm5,			SCALEFACTOR;
					psrld		mm2,			SCALEFACTOR;
					packssdw	mm2,			mm5;		//mm2： 4G 4B 3G 3B 

					packuswb	mm1,			mm2;		//mm1:  4G 4B 3G 3B 2G 2B 1G 1B
					movq		[esi],			mm1;
				}
			}						
		}
		for(y=1; y<pPars->uiHei; y+=2)//The Even Line: G R
		{
			yPos[0] = CLIP(( y + iLc_Y_Offset) >> nShift, 0, GAINTABLEHEI-1);
			yPos[1] = CLIP(((y + iLc_Y_Offset) >> nShift) + 1, 0, GAINTABLEHEI-1);
			yDis[0]	= max((y + iLc_Y_Offset) - (yPos[0] << nShift),0);
			yDis[1]	= Temp - yDis[0];

			for(x=0; x<pPars->uiWid; x+=8)		
			{
				xPos[0] = CLIP((x + iLc_X_Offset) >> nShift, 0, GAINTABLEWID-1);
				xPos[1] = CLIP(((x + iLc_X_Offset) >> nShift) + 1, 0, GAINTABLEWID-1);
				xDis[0]	= max((x + iLc_X_Offset) - (xPos[0]  << nShift),0);
				xDis[1]	= Temp - xDis[0];						
				xDis1[0]= xDis[0]+1;
				xDis1[1]= xDis[1]-1;

				UINT16  i16_yDis[4] = {yDis[1],yDis[0],yDis[1],yDis[0]}; 
				UINT16  i16_x[4] = {-2,2,-2,2};		//The change of i16_xDis
				UINT16  i16_xDis[4] = {xDis[1],xDis[0],xDis1[1],xDis1[0]};

				UINT16  i16_WeiG[4] = {
					pPars->LCWeiMtxG[yPos[0]][xPos[0]]+UnitGain,
					pPars->LCWeiMtxG[yPos[1]][xPos[0]]+UnitGain,
					pPars->LCWeiMtxG[yPos[0]][xPos[1]]+UnitGain,
					pPars->LCWeiMtxG[yPos[1]][xPos[1]]+UnitGain
				};
				UINT16  i16_WeiR[4] = {
					pPars->LCWeiMtxR[yPos[0]][xPos[0]]+UnitGain,
					pPars->LCWeiMtxR[yPos[1]][xPos[0]]+UnitGain,
					pPars->LCWeiMtxR[yPos[0]][xPos[1]]+UnitGain,
					pPars->LCWeiMtxR[yPos[1]][xPos[1]]+UnitGain
				};			


				//8*1 pixels per loop
				esiADD = AddEsi+y * pPars->uiWid+x;

				__asm
				{
					mov			esi,			esiADD;	
					pxor		mm4,			mm4;
					movd		mm4,			nShift;
					//--------------------------------------------------------------------------------							
					movq		mm1,			[i16_yDis];	//mm1: yDis0 yDis1 yDis0 yDis1							
					movq		mm7,			mm1;
					pmaddwd		mm7,			[i16_WeiG];	//mm0: H：yDis0 * iWei11+yDis1 * iWei01 ，L：yDis0 * iWei10+yDis1 * iWei00 
					psrld		mm7,			mm4;//5;//eax;		//mm0: 00  (yDis0 * iWei11+yDis1 * iWei01)   00   	(yDis0 * iWei10+yDis1 * iWei00)					
					movq		mm0,			mm1;
					pmaddwd		mm0,			[i16_WeiR];
					psrld		mm0,			mm4;//5;//eax;
					packssdw	mm7,			mm0;		//mm7: (yDis0 * iWei11G+yDis1 * iWei01G)  (yDis0 * iWei10G+yDis1 * iWei00G)	 (yDis0 * iWei11B+yDis1 * iWei01B) (yDis0 * iWei10B+yDis1 * iWei00B)					
					//--------------------------------------------------------------------------------							
					movq		mm2,			[i16_xDis];	//mm2: j+1 xDis1-1 j xDis1 
					movq		mm3,			mm2;				
					pmaddwd		mm3,			mm7;		//mm3: H: weight1R L: weight1G
					psrld		mm3,			mm4;//5;//eax;

					paddsw		mm2,			[i16_x];	//mm2: j+3 xDis1-3 j+2 xDis1-2 
					movq		mm5,			mm2;							
					pmaddwd		mm5,			mm7;		//mm5：H: weight2R L: weight2G
					psrld		mm5,			mm4;//5;//eax;
					packssdw	mm3,			mm5;		//mm3：weight2R weight2G, weight1R weight1G

					paddsw		mm2,			[i16_x];	//mm2: j+5 xDis1-5 j+4 xDis1-4 
					movq		mm0,			mm2;				
					pmaddwd		mm0,			mm7;		//mm0: H: weight3R L: weight3G
					psrld		mm0,			mm4;//5;//eax;

					paddsw		mm2,			[i16_x];	//mm2: j+7 xDis1-7 j+6 xDis1-6 
					movq		mm5,			mm2;							
					pmaddwd		mm5,			mm7;		//mm5：H: weight4R L: weight4G
					psrld		mm5,			mm4;//5;//eax;
					packssdw	mm0,			mm5;		//mm0：weight4R weight4G, weight3R weight3G
					//--------------------------------------------------------------------------------						
					movq        mm1,			mm3;						
					movd        mm5,			[esi];		//mm5: 2R 2G 1R 1G 
					pxor		mm4,			mm4;
					punpcklbw	mm5,			mm4;		//mm5: 00 2R 00 2G 00 1R 00 1G
					pmullw		mm1,			mm5;		//mm1: L：weight2R*2R weight2G*2G weight1R*1R weight1G*1G
					pmulhw		mm3,			mm5;
					movq		mm5,			mm1;
					punpckhwd	mm5,			mm3;		//mm5:	weight2R*2R weight2G*2G
					punpcklwd	mm1,			mm3;		//mm1:	weight1R*1R weight1G*1G
					psrld		mm5,			SCALEFACTOR;
					psrld		mm1,			SCALEFACTOR;
					packssdw	mm1,			mm5;		//mm1： 2R 2G 1R 1G 

					movq        mm2,			mm0;						
					movd        mm5,			[esi+4];	//mm5: 4R 4G 3R 3G 
					pxor		mm4,			mm4;
					punpcklbw	mm5,			mm4;		//mm5: 00 4R 00 4G 00 3R 00 3G
					pmullw		mm2,			mm5;		//mm2: L：weight4R*4R weight4G*4G weight3R*3R weight3G*3G
					pmulhw		mm0,			mm5;
					movq		mm5,			mm2;
					punpckhwd	mm5,			mm0;		//mm5:	weight4R*4R weight4G*4G
					punpcklwd	mm2,			mm0;		//mm2:	weight3R*3R weight3G*3G
					psrld		mm5,			SCALEFACTOR;
					psrld		mm2,			SCALEFACTOR;
					packssdw	mm2,			mm5;		//mm2： 4R 4G 3R 3G 

					packuswb	mm1,			mm2;		//mm1:  4R 4G 3R 3G 2R 2G 1R 1G
					movq		[esi],			mm1;
				}
			}						
		}
	}	
	__asm emms;
}

// void GenerateWeightMatrix(UInt8 *in, int width, int height)
void CRawUtil::GenerateWeightMatrix(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	UINT8 *in = pPars->pbSrc;
	UINT width = pPars->uiWid;
	UINT height = pPars->uiHei;	 		

	int SmoothFactor = 64;	
	UINT *raw = (UINT *)calloc(height*width*3, sizeof(UINT));			// ps raw

	UINT *pB = NULL, *pG = NULL, *pR = NULL;
	UINT maxB = 0, maxG = 0, maxR = 0;
	UINT B = 0, G = 0, R = 0;
	int i = 0, j = 0;
	int pos = 0;
	int wHeight = 0, wWidth = 0;
	int h = 0, w = 0;
	UINT *tempWM = NULL;
	UINT *tempBuf = NULL;
	UINT *wB = NULL, *wG = NULL, *wR = NULL;
	int tempBlkSizeH = 0, tempBlkSizeV = 0, tempBlkSize = 0, BlkShift = 0;

	int m_nWMHeight = 0, m_nWMWidth = 0;  

	// Get max block size
	tempBlkSizeH = width/(GAINTABLEWID-1);
	tempBlkSizeV = height/(GAINTABLEHEI-1);	  
	tempBlkSize = max(tempBlkSizeH, tempBlkSizeV); 

	if (tempBlkSize<=1)
	{
		BlkShift = 0;
	}
	else if (tempBlkSize<=2)
	{
		BlkShift = 1;
	}
	else if (tempBlkSize<=4)
	{
		BlkShift = 2;
	}
	else if (tempBlkSize<=8)
	{
		BlkShift = 3;
	}
	else if (tempBlkSize<=16)
	{	
		BlkShift = 4;
	}
	else if (tempBlkSize<=32)
	{
		BlkShift = 5;
	}
	else if (tempBlkSize<=64)
	{
		BlkShift = 6;
	}
	else if (tempBlkSize<=128)
	{
		BlkShift = 7;
	}
	else
		return;

	// 为方便运算, 希望最终Block Size是2的幂次方
	int m_nBlkSize = 1<<BlkShift;

	m_nWMHeight	= wHeight	= min(((height+m_nBlkSize-1)>>BlkShift) + 1, GAINTABLEHEI);
	m_nWMWidth	= wWidth	= min(((width+m_nBlkSize-1) >>BlkShift) + 1, GAINTABLEWID);

	h = (wHeight-1)*2;
	w = (wWidth-1)*2;

	tempWM	= (UINT *)calloc(h*w*3, sizeof(UINT));
	tempBuf	= (UINT *)calloc(width*height, sizeof(UINT));

	wB	= (UINT *)calloc(wHeight*wWidth, sizeof(UINT));
	wG	= (UINT *)calloc(wHeight*wWidth, sizeof(UINT));
	wR	= (UINT *)calloc(wHeight*wWidth, sizeof(UINT));

	//bayer pattern -> ps raw
	pB	= raw;
	pG	= pB + width*height;
	pR	= pG + width*height;
	for(i=0; i<height; i+=2)
	{
		for(j=0; j<width; j+=2)
		{
			pos = i*width + j;

			B = in[pos];
			G = (in[pos+1] + in[pos+width]) >> 1;
			R = in[pos+width+1];

			// bayer pattern -> ps raw 过程中查找每个plane最大值
			if(B > maxB)
				maxB = B;
			if(G > maxG)
				maxG = G;
			if( R > maxR)
				maxR = R;

			pB[pos+width+1] = pB[pos+width] 
			= pB[pos+1] = pB[pos] = B;

			pG[pos+width+1] = pG[pos+width] 
			= pG[pos+1] = pG[pos] = G;

			pR[pos+width+1] = pR[pos+width] 
			= pR[pos+1] = pR[pos] = R;
		}
	}

	//********Smooth Raw, begin************
	// 这里平滑图像使用图像积分的方法, 如果使用常规模板方法, 当模板值很大(如: 129x129)
	// 平滑处理速度很慢
	//step 1 : integral map, begin
	//H
	for(i=0; i<height; i++)
	{
		for(j=1; j<width; j++)
		{
			pB[i*width + j] += pB[i*width + j-1];
			pG[i*width + j] += pG[i*width + j-1];
			pR[i*width + j] += pR[i*width + j-1];
		}
	}
	//V
	for(i=1; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			pB[i*width + j] += pB[(i-1)*width + j];
			pG[i*width + j] += pG[(i-1)*width + j];
			pR[i*width + j] += pR[(i-1)*width + j];
		}
	}
	//step 1 : integral map, end
	//step 2 : smooth, begin
	UINT *temp;
	int k;
	int x0, x1, y0 ,y1;
	for(k=0; k<3; k++)
	{
		switch(k)
		{
		case 0:
			temp = pB;
			break;
		case 1:
			temp = pG;
			break;
		case 2:
			temp = pR;
			break;
		default:
			return;
		}

		for(i=0; i<height; i++)
		{
			for(j=0; j<width; j++)
			{ 				
				x0 = max(0, j - SmoothFactor/2);
				x1 = min(width-1, j + SmoothFactor/2);
				y0 = max(0, i - SmoothFactor/2);
				y1 = min(height-1, i + SmoothFactor/2);

				tempBuf[i*width + j] = (temp[y1*width + x1] + temp[y0*width + x0]
				- temp[y0*width + x1] - temp[y1*width + x0])
					/ (x1 - x0) / (y1 - y0);
			}
		}

		memcpy(temp, tempBuf, height*width*sizeof(UINT));
	}
	free(tempBuf);
	//step 2 : smooth, end
	//********Smooth Raw, end**************

	//weight = max / pixelValue
	// Generate gain table
#if 0
	for(i=0; i<height*width; i++)
	{
		if(*pB == 0)
		{
			*pB++ = maxB << SCALEFACTOR;
		}
		else
		{
			*pB++ = (maxB << SCALEFACTOR) / (*pB);
		}

		if(*pG == 0)
		{
			*pG++ = maxG << SCALEFACTOR;
		}
		else
		{
			*pG++ = (maxG << SCALEFACTOR) / (*pG);
		}

		if(*pR == 0)
		{
			*pR++ = maxR << SCALEFACTOR;
		}
		else
		{
			*pR++ = (maxR<<SCALEFACTOR) / (*pR);
		}
	}
#else
	for(i=0; i<height*width; i++)
	{
		if(*pB == 0)
		{
			*pB++ = maxB << 8;
		}
		else
		{
			*pB++ = (maxB << 8) / (*pB);
		}

		if(*pG == 0)
		{
			*pG++ = maxG << 8;
		}
		else
		{
			*pG++ = (maxG << 8) / (*pG);
		}

		if(*pR == 0)
		{
			*pR++ = maxR << 8;
		}
		else
		{
			*pR++ = (maxR<<8) / (*pR);
		}
	}
#endif


	//*****weight Matrix integral map, begin*******
	pB	= raw;
	pG	= pB + width*height;
	pR	= pG + width*height;
	//H
	for(i=0; i<height; i++)
	{
		for(j=1; j<width; j++)
		{
			pB[i*width + j] += pB[i*width + j-1];
			pG[i*width + j] += pG[i*width + j-1];
			pR[i*width + j] += pR[i*width + j-1];
		}
	}
	//V
	for(i=1; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			pB[i*width + j] += pB[(i-1)*width + j];
			pG[i*width + j] += pG[(i-1)*width + j];
			pR[i*width + j] += pR[(i-1)*width + j];
		}
	}
	//*****weight Matrix integral map, end*******

	//************product temp weight matrix, begin***********
	//temp weight matrix size : (wWidth-1)*(wHeight-1)*2*2
	UINT *tempB = tempWM;
	UINT *tempG = tempB + h*w;
	UINT *tempR = tempG + h*w;
	UINT *pW;
	UINT tl, tr, bl, br;
	int HalfBlkSize=1<<(BlkShift-1);

	for(k =0; k<3; k++)
	{
		switch(k)
		{
		case 0://B
			temp	= tempB;
			pW		= pB;
			break;
		case 1://G
			temp	= tempG;
			pW		= pG;
			break;
		case 2:
			temp	= tempR;
			pW		= pR;
			break;
		}
		for(i=0; i<h; i++)
		{
			for(j=0; j<w; j++)
			{
				x0 = j*HalfBlkSize - 1;	x1 = (j+1)*HalfBlkSize - 1;
				y0 = i*HalfBlkSize - 1;	y1 = (i+1)*HalfBlkSize - 1;

				//if (x0>=width-1)
				if (x0 >= (int)(width-1))
				{
					temp[i*w + j] = temp[i*w + j-1];
				}
				//else if (y0>=height-1)
				else if (y0 >= (int)(height-1))
				{
					temp[i*w + j] = temp[(i-1)*w + j];
				}
				else
				{
					x0=MIN(x0,width-1);
					x1=MIN(x1,width-1);
					y0=MIN(y0,height-1);
					y1=MIN(y1,height-1);

					if(x0<0 || y0<0)
						tl = 0;
					else
						tl = pW[y0*width + x0];

					if(x1<0 || y0<0)
						tr = 0;
					else
						tr = pW[y0*width + x1];

					if(x0<0 || y1<0)
						bl = 0;
					else
						bl = pW[y1*width + x0];

					if(x1<0 || y1<0)
						br = 0;
					else
						br = pW[y1*width + x1];

					temp[i*w + j] = (br + tl - tr - bl) / (x1-x0) / (y1-y0);
				}
			}
		}
	}
	//************product temp weight matrix, end***********

	//************generate final weight matrix, begin*********
	//center: (wWidth-1)*(wHeight-1)
	for(i=1; i<wHeight-1; i++)
	{
		for(j=1; j<wWidth-1; j++)
		{
			x0 = j*2 - 1;	x1 = x0 + 1;
			y0 = i*2 - 1;	y1 = y0 + 1;

			wB[i*wWidth+j] = (tempB[y0*w + x0] + tempB[y0*w + x1] + tempB[y1*w + x0] + tempB[y1*w + x1]) >> 2;
			wG[i*wWidth+j] = (tempG[y0*w + x0] + tempG[y0*w + x1] + tempG[y1*w + x0] + tempG[y1*w + x1]) >> 2;
			wR[i*wWidth+j] = (tempR[y0*w + x0] + tempR[y0*w + x1] + tempR[y1*w + x0] + tempR[y1*w + x1]) >> 2;
		}
	}

#if 1
	//top left
	pos = 0;
	temp = tempB;
	wB[0] = ( 2*temp[pos]-temp[pos + w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	temp = tempG;
	wG[0] = ( 2*temp[pos]-temp[pos + w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	temp = tempR;
	wR[0] = ( 2*temp[pos]-temp[pos + w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	//top right
	pos = w - 1;
	temp = tempB;
	wB[wWidth-1] = (2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos + w - 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	temp = tempG;
	wG[wWidth-1] = (2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos + w - 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	temp = tempR;
	wR[wWidth-1] = (2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos + w - 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	//bottom left
	pos = (h - 1) * w;
	temp = tempB;
	wB[(wHeight-1)*wWidth] = (2*temp[pos]-temp[pos - w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	temp = tempG;
	wG[(wHeight-1)*wWidth] = (2*temp[pos]-temp[pos - w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	temp = tempR;
	wR[(wHeight-1)*wWidth] = (2*temp[pos]-temp[pos - w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	//bottom right
	pos = h * w - 1;
	temp = tempB;
	wB[wHeight*wWidth-1] = (2*temp[pos]-temp[pos - w - 1] + 2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	temp = tempG;
	wG[wHeight*wWidth-1] = (2*temp[pos]-temp[pos - w - 1] + 2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	temp = tempR;
	wR[wHeight*wWidth-1] = (2*temp[pos]-temp[pos - w - 1] + 2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;

	//top
	for(i=1; i<wWidth-1; i++)
	{
		pos = i*2 - 1;

		wB[i] = ( 2*tempB[pos]-tempB[pos + w] + 2*tempB[pos + 1]-tempB[pos + w + 1] + tempB[pos] + tempB[pos + 1] ) >> 2;
		wG[i] = ( 2*tempG[pos]-tempG[pos + w] + 2*tempG[pos + 1]-tempG[pos + w + 1] + tempG[pos] + tempG[pos + 1] ) >> 2;
		wR[i] = ( 2*tempR[pos]-tempR[pos + w] + 2*tempR[pos + 1]-tempR[pos + w + 1] + tempR[pos] + tempR[pos + 1] ) >> 2;
	}
	//bottom
	for(i=1; i<wWidth-1; i++)
	{
		pos = (h - 1)*w + i*2 - 1;

		wB[(wHeight-1)*wWidth+i] = ( 2*tempB[pos]-tempB[pos - w] + 2*tempB[pos + 1]-tempB[pos - w + 1] + tempB[pos] + tempB[pos + 1] ) >> 2;
		wG[(wHeight-1)*wWidth+i] = ( 2*tempG[pos]-tempG[pos - w] + 2*tempG[pos + 1]-tempG[pos - w + 1] + tempG[pos] + tempG[pos + 1] ) >> 2;
		wR[(wHeight-1)*wWidth+i] = ( 2*tempR[pos]-tempR[pos - w] + 2*tempR[pos + 1]-tempR[pos - w + 1] + tempR[pos] + tempR[pos + 1] ) >> 2;
	}
	//left
	for(i=1; i<wHeight-1; i++)
	{
		pos = (i*2 - 1)*w;

		wB[i*wWidth] = ( 2*tempB[pos]-tempB[pos + 1] + 2*tempB[pos + w]-tempB[pos + w + 1] + tempB[pos] + tempB[pos + w] ) >> 2;
		wG[i*wWidth] = ( 2*tempG[pos]-tempG[pos + 1] + 2*tempG[pos + w]-tempG[pos + w + 1] + tempG[pos] + tempG[pos + w] ) >> 2;
		wR[i*wWidth] = ( 2*tempR[pos]-tempR[pos + 1] + 2*tempR[pos + w]-tempR[pos + w + 1] + tempR[pos] + tempR[pos + w] ) >> 2;
	}
	//right
	for(i=1; i<wHeight-1; i++)
	{
		pos = (i*2 - 1)*w + w - 1;

		wB[i*wWidth+wWidth - 1] = ( 2*tempB[pos]-tempB[pos - 1] + 2*tempB[pos + w]-tempB[pos + w - 1] + tempB[pos] + tempB[pos + w] ) >> 2;
		wG[i*wWidth+wWidth - 1] = ( 2*tempG[pos]-tempG[pos - 1] + 2*tempG[pos + w]-tempG[pos + w - 1] + tempG[pos] + tempG[pos + w] ) >> 2;
		wR[i*wWidth+wWidth - 1] = ( 2*tempR[pos]-tempR[pos - 1] + 2*tempR[pos + w]-tempR[pos + w - 1] + tempR[pos] + tempR[pos + w] ) >> 2;
	}
#endif
	//************generate final weight matrix, end***********

	free(tempWM);

	//normalize 1
	int minB = 0xefff, minG = 0xefff, minR = 0xefff;
	int count = 0;
	int UnitGain = 1<<SCALEFACTOR;
	for(i=0; i<wHeight; i++)
	{
		for(j=0; j<wWidth; j++)
		{
			if(minB > wB[i*wWidth+j])
				minB = wB[i*wWidth+j];
			if(minG > wG[i*wWidth+j])
				minG = wG[i*wWidth+j];
			if(minR > wR[i*wWidth+j])
				minR = wR[i*wWidth+j];
		}
	}
#if 0
	for(i=0; i<wHeight; i++)
	{
		for(j=0; j<wWidth; j++)
		{
			wB[i*wWidth+j] = CLIP( (wB[i*wWidth+j] << SCALEFACTOR) / minB - 2*UnitGain, 0, 255);
			wG[i*wWidth+j] = CLIP( (wG[i*wWidth+j] << SCALEFACTOR) / minG - 2*UnitGain, 0, 255);
			wR[i*wWidth+j] = CLIP( (wR[i*wWidth+j] << SCALEFACTOR) / minR - 2*UnitGain, 0, 255);
		}
	}
#else
	for(i=0; i<wHeight; i++)
	{
		for(j=0; j<wWidth; j++)
		{
			wB[i*wWidth+j] = CLIP( (wB[i*wWidth+j] << 8) / minB - 2*UnitGain, 0, 255);
			wG[i*wWidth+j] = CLIP( (wG[i*wWidth+j] << 8) / minG - 2*UnitGain, 0, 255);
			wR[i*wWidth+j] = CLIP( (wR[i*wWidth+j] << 8) / minR - 2*UnitGain, 0, 255);
		}
	}
#endif

	int GetX, GetY;
	BYTE BGainTable[GAINTABLEHEI*GAINTABLEWID],GGainTable[GAINTABLEHEI*GAINTABLEWID],RGainTable[GAINTABLEHEI*GAINTABLEWID];

#if 0
	/*
	for (j=0; j<wHeight; j++)
	{
	for (i=0; i<wWidth; i++)
	{
	GetX = CLIP(i, 0, wWidth -1);
	GetY = CLIP(j, 0, wHeight-1);

	BGainTable[j*GAINTABLEWID+i] = wB[GetY*wWidth+GetX]; //For Weighting Matrix B
	GGainTable[j*GAINTABLEWID+i] = wG[GetY*wWidth+GetX]; //For Weighting Matrix G
	RGainTable[j*GAINTABLEWID+i] = wR[GetY*wWidth+GetX]; //For Weighting Matrix R
	}
	}*/
#else
	for (j=0; j<GAINTABLEHEI; j++)
	{
		for (i=0; i<GAINTABLEWID; i++)
		{
			GetX = CLIP(i, 0, wWidth -1);
			GetY = CLIP(j, 0, wHeight-1);

			BGainTable[j*GAINTABLEWID+i] = wB[GetY*wWidth+GetX]; //For Weighting Matrix B
			GGainTable[j*GAINTABLEWID+i] = wG[GetY*wWidth+GetX]; //For Weighting Matrix G
			RGainTable[j*GAINTABLEWID+i] = wR[GetY*wWidth+GetX]; //For Weighting Matrix R
		}
	}
#endif

	free(wB);
	free(wG);
	free(wR);  
	free(raw);

	pPars->dLenCompRation = 0.5;

	// Get "BGainTable", "GGainTable" and "RGainTable"
	for (i = 0; i < GAINTABLEHEI; i++)
	{
		for (j = 0; j < GAINTABLEWID; j++)
		{
			pPars->LCWeiMtxB[i][j] = BGainTable[GAINTABLEWID*i+j] * pPars->dLenCompRation;
			pPars->LCWeiMtxG[i][j] = GGainTable[GAINTABLEWID*i+j] * pPars->dLenCompRation;
			pPars->LCWeiMtxR[i][j] = RGainTable[GAINTABLEWID*i+j] * pPars->dLenCompRation;
		}
	}
}

void CRawUtil::GenerateWeightMatrix(BYTE *in, int width, int height)
{
	m_b276LSCIsShow = TRUE;
	fLSC_B_Gain_Ratio = 1.0f;
	fLSC_G_Gain_Ratio = 1.0f;
	fLSC_R_Gain_Ratio = 1.0f;	 	

	int SmoothFactor = 64;
	int nScaleFactor = 8; // 7;
	BYTE *raw = (BYTE *)calloc(height*width*3, sizeof(BYTE));

	BYTE *pB, *pG, *pR;
	BYTE maxB = 0, maxG = 0, maxR = 0;
	BYTE B, G, R;
	int i, j;
	int pos;
	int wHeight, wWidth;
	int h, w;
	BYTE *tempWM;
	BYTE *tempBuf;
	BYTE *wB, *wG, *wR;
	int tempBlkSizeH, tempBlkSizeV, tempBlkSize, BlkShift;

	int TableHSize, TableVSize;
	float fLSC_B_Ratio, fLSC_G_Ratio, fLSC_R_Ratio;

	if (m_b276LSCIsShow)
	{
		TableHSize = 20;
		TableVSize = 15;

		fLSC_B_Ratio = fLSC_B_Gain_Ratio;
		fLSC_G_Ratio = fLSC_G_Gain_Ratio;
		fLSC_R_Ratio = fLSC_R_Gain_Ratio;
	}
	else
	{
		TableHSize = 20;
		TableVSize = 16;

		fLSC_B_Ratio = fLCGainRatio;
		fLSC_G_Ratio = fLCGainRatio;
		fLSC_R_Ratio = fLCGainRatio;
	}

	tempBlkSizeH = width/TableHSize;
	tempBlkSizeV = height/TableVSize;

	tempBlkSize = max(tempBlkSizeH, tempBlkSizeV);

	if (tempBlkSize<=16)
	{	
		BlkShift = 4;
	}
	else if (tempBlkSize<=32)
	{
		BlkShift = 5;
	}
	else if (tempBlkSize<=64)
	{
		BlkShift = 6;
	}
	else if (tempBlkSize<=128)
	{
		BlkShift = 7;
	}
	else
		return;

	m_nBlkSize = 1<<BlkShift;

	m_nWMHeight= wHeight = min(((height+m_nBlkSize-1)>>BlkShift) + 1, (TableVSize+1));
	m_nWMWidth = wWidth = min(((width+m_nBlkSize-1) >>BlkShift) + 1, (TableHSize+1));

	h = (wHeight-1)*2;			// ?
	w = (wWidth-1)*2;			// ?

	tempWM	= (BYTE *)calloc(h*w*3, sizeof(BYTE));
	tempBuf	= (BYTE *)calloc(width*height, sizeof(BYTE));

	wB	= (BYTE *)calloc(wHeight*wWidth, sizeof(BYTE));
	wG	= (BYTE *)calloc(wHeight*wWidth, sizeof(BYTE));
	wR	= (BYTE *)calloc(wHeight*wWidth, sizeof(BYTE));

	//bayer pattern -> ps raw
	pB	= raw;
	pG	= pB + width*height;
	pR	= pG + width*height;
	for(i=0; i<height; i+=2)
	{
		for(j=0; j<width; j+=2)
		{
			pos = i*width + j;

			B = in[pos];
			G = (in[pos+1] + in[pos+width]) >> 1;
			R = in[pos+width+1];

			if(B > maxB)
				maxB = B;
			if(G > maxG)
				maxG = G;
			if( R > maxR)
				maxR = R;

			pB[pos+width+1] = pB[pos+width] 
			= pB[pos+1] = pB[pos] = B;
			pG[pos+width+1] = pG[pos+width] 
			= pG[pos+1] = pG[pos] = G;
			pR[pos+width+1] = pR[pos+width] 
			= pR[pos+1] = pR[pos] = R;
		}
	}

	//********Smooth Raw, begin************
	//step 1 : integral map, begin
	//H
	for(i=0; i<height; i++)
	{
		for(j=1; j<width; j++)
		{
			pB[i*width + j] += pB[i*width + j-1];
			pG[i*width + j] += pG[i*width + j-1];
			pR[i*width + j] += pR[i*width + j-1];
		}
	}
	//V
	for(i=1; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			pB[i*width + j] += pB[(i-1)*width + j];
			pG[i*width + j] += pG[(i-1)*width + j];
			pR[i*width + j] += pR[(i-1)*width + j];
		}
	}
	//step 1 : integral map, end
	//step 2 : smooth, begin
	BYTE *temp;
	int k;
	int x0, x1, y0 ,y1;
	for(k=0; k<3; k++)
	{
		switch(k)
		{
		case 0:
			temp = pB;
			break;
		case 1:
			temp = pG;
			break;
		case 2:
			temp = pR;
			break;
		default:
			return;
		}

		for(i=0; i<height; i++)
		{
			for(j=0; j<width; j++)
			{
				x0 = max(0, j - SmoothFactor/2);
				x1 = min(width-1, j + SmoothFactor/2);
				y0 = max(0, i - SmoothFactor/2);
				y1 = min(height-1, i + SmoothFactor/2);

				/*
				tempBuf[i*width + j] = (temp[y1*width + x1] + temp[y0*width + x0]
				- temp[y0*width + x1] - temp[y1*width + x0])
				/ (x1 - x0) / (y1 - y0);*/

				tempBuf[i*width + j] = ((temp[y1*width + x1] + temp[y0*width + x0]
				- temp[y0*width + x1] - temp[y1*width + x0])
					/ ((x1 - x0)*1.0)) * (y1 - y0);

			}
		}

		memcpy(temp, tempBuf, height*width*sizeof(BYTE));
	}
	free(tempBuf);
	//step 2 : smooth, end
	//********Smooth Raw, end**************

	//weight = max / pixelValue
	for(i=0; i<height*width; i++)
	{
		if(*pB == 0)
		{
			*pB++ = maxB << nScaleFactor;
		}
		else
		{
			*pB++ = (maxB << nScaleFactor) / (*pB);
		}

		if(*pG == 0)
		{
			*pG++ = maxG << nScaleFactor;
		}
		else
		{
			*pG++ = (maxG << nScaleFactor) / (*pG);
		}

		if(*pR == 0)
		{
			*pR++ = maxR << nScaleFactor;
		}
		else
		{
			*pR++ = (maxR<<nScaleFactor) / (*pR);
		}
	}

	//*****weight Matrix integral map, begin*******
	pB	= raw;
	pG	= pB + width*height;
	pR	= pG + width*height;
	//H
	for(i=0; i<height; i++)
	{
		for(j=1; j<width; j++)
		{
			pB[i*width + j] += pB[i*width + j-1];
			pG[i*width + j] += pG[i*width + j-1];
			pR[i*width + j] += pR[i*width + j-1];
		}
	}
	//V
	for(i=1; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			pB[i*width + j] += pB[(i-1)*width + j];
			pG[i*width + j] += pG[(i-1)*width + j];
			pR[i*width + j] += pR[(i-1)*width + j];
		}
	}
	//*****weight Matrix integral map, end*******

	//************product temp weight matrix, begin***********
	//temp weight matrix size : (wWidth-1)*(wHeight-1)*2*2
	BYTE *tempB = tempWM;
	BYTE *tempG = tempB + h*w;
	BYTE *tempR = tempG + h*w;
	BYTE *pW;
	BYTE tl, tr, bl, br;
	int HalfBlkSize=1<<(BlkShift-1);

	for(k =0; k<3; k++)
	{
		switch(k)
		{
		case 0://B
			temp	= tempB;
			pW		= pB;
			break;
		case 1://G
			temp	= tempG;
			pW		= pG;
			break;
		case 2:
			temp	= tempR;
			pW		= pR;
			break;
		}
		for(i=0; i<h; i++)
		{
			for(j=0; j<w; j++)
			{
				x0 = j*HalfBlkSize - 1;	x1 = (j+1)*HalfBlkSize - 1;
				y0 = i*HalfBlkSize - 1;	y1 = (i+1)*HalfBlkSize - 1;

				if (x0>=width-1)
				{
					temp[i*w + j] = temp[i*w + j-1];
				}
				else if (y0>=height-1)
				{
					temp[i*w + j] = temp[(i-1)*w + j];
				}
				else
				{
					x0=min(x0,width-1);
					x1=min(x1,width-1);
					y0=min(y0,height-1);
					y1=min(y1,height-1);

					if(x0<0 || y0<0)
						tl = 0;
					else
						tl = pW[y0*width + x0];

					if(x1<0 || y0<0)
						tr = 0;
					else
						tr = pW[y0*width + x1];

					if(x0<0 || y1<0)
						bl = 0;
					else
						bl = pW[y1*width + x0];

					if(x1<0 || y1<0)
						br = 0;
					else
						br = pW[y1*width + x1];

					/*temp[i*w + j] = (br + tl - tr - bl) / (x1-x0) / (y1-y0);*/
					temp[i*w + j] = ((br + tl - tr - bl) / ((x1-x0)*1.0)) * (y1-y0);
				}
			}
		}
	}
	//************product temp weight matrix, end***********

	//************generate final weight matrix, begin*********
	//center: (wWidth-1)*(wHeight-1)
	for(i=1; i<wHeight-1; i++)
	{
		for(j=1; j<wWidth-1; j++)
		{
			x0 = j*2 - 1;	x1 = x0 + 1;
			y0 = i*2 - 1;	y1 = y0 + 1;

			wB[i*wWidth+j] = (tempB[y0*w + x0] + tempB[y0*w + x1] + tempB[y1*w + x0] + tempB[y1*w + x1]) >> 2;
			wG[i*wWidth+j] = (tempG[y0*w + x0] + tempG[y0*w + x1] + tempG[y1*w + x0] + tempG[y1*w + x1]) >> 2;
			wR[i*wWidth+j] = (tempR[y0*w + x0] + tempR[y0*w + x1] + tempR[y1*w + x0] + tempR[y1*w + x1]) >> 2;
		}
	}

	//top left
	pos = 0;
	temp = tempB;
	wB[0] = ( 2*temp[pos]-temp[pos + w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	temp = tempG;
	wG[0] = ( 2*temp[pos]-temp[pos + w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	temp = tempR;
	wR[0] = ( 2*temp[pos]-temp[pos + w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	//top right
	pos = w - 1;
	temp = tempB;
	wB[wWidth-1] = (2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos + w - 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	temp = tempG;
	wG[wWidth-1] = (2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos + w - 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	temp = tempR;
	wR[wWidth-1] = (2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos + w - 1] + 2*temp[pos]-temp[pos + w] + temp[pos]) >> 2;
	//bottom left
	pos = (h - 1) * w;
	temp = tempB;
	wB[(wHeight-1)*wWidth] = (2*temp[pos]-temp[pos - w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	temp = tempG;
	wG[(wHeight-1)*wWidth] = (2*temp[pos]-temp[pos - w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	temp = tempR;
	wR[(wHeight-1)*wWidth] = (2*temp[pos]-temp[pos - w + 1] + 2*temp[pos]-temp[pos + 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	//bottom right
	pos = h * w - 1;
	temp = tempB;
	wB[wHeight*wWidth-1] = (2*temp[pos]-temp[pos - w - 1] + 2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	temp = tempG;
	wG[wHeight*wWidth-1] = (2*temp[pos]-temp[pos - w - 1] + 2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;
	temp = tempR;
	wR[wHeight*wWidth-1] = (2*temp[pos]-temp[pos - w - 1] + 2*temp[pos]-temp[pos - 1] + 2*temp[pos]-temp[pos - w] + temp[pos]) >> 2;

	//top
	for(i=1; i<wWidth-1; i++)
	{
		pos = i*2 - 1;

		wB[i] = ( 2*tempB[pos]-tempB[pos + w] + 2*tempB[pos + 1]-tempB[pos + w + 1] + tempB[pos] + tempB[pos + 1] ) >> 2;
		wG[i] = ( 2*tempG[pos]-tempG[pos + w] + 2*tempG[pos + 1]-tempG[pos + w + 1] + tempG[pos] + tempG[pos + 1] ) >> 2;
		wR[i] = ( 2*tempR[pos]-tempR[pos + w] + 2*tempR[pos + 1]-tempR[pos + w + 1] + tempR[pos] + tempR[pos + 1] ) >> 2;
	}
	//bottom
	for(i=1; i<wWidth-1; i++)
	{
		pos = (h - 1)*w + i*2 - 1;

		wB[(wHeight-1)*wWidth+i] = ( 2*tempB[pos]-tempB[pos - w] + 2*tempB[pos + 1]-tempB[pos - w + 1] + tempB[pos] + tempB[pos + 1] ) >> 2;
		wG[(wHeight-1)*wWidth+i] = ( 2*tempG[pos]-tempG[pos - w] + 2*tempG[pos + 1]-tempG[pos - w + 1] + tempG[pos] + tempG[pos + 1] ) >> 2;
		wR[(wHeight-1)*wWidth+i] = ( 2*tempR[pos]-tempR[pos - w] + 2*tempR[pos + 1]-tempR[pos - w + 1] + tempR[pos] + tempR[pos + 1] ) >> 2;
	}
	//left
	for(i=1; i<wHeight-1; i++)
	{
		pos = (i*2 - 1)*w;

		wB[i*wWidth] = ( 2*tempB[pos]-tempB[pos + 1] + 2*tempB[pos + w]-tempB[pos + w + 1] + tempB[pos] + tempB[pos + w] ) >> 2;
		wG[i*wWidth] = ( 2*tempG[pos]-tempG[pos + 1] + 2*tempG[pos + w]-tempG[pos + w + 1] + tempG[pos] + tempG[pos + w] ) >> 2;
		wR[i*wWidth] = ( 2*tempR[pos]-tempR[pos + 1] + 2*tempR[pos + w]-tempR[pos + w + 1] + tempR[pos] + tempR[pos + w] ) >> 2;
	}
	//right
	for(i=1; i<wHeight-1; i++)
	{
		pos = (i*2 - 1)*w + w - 1;

		wB[i*wWidth+wWidth - 1] = ( 2*tempB[pos]-tempB[pos - 1] + 2*tempB[pos + w]-tempB[pos + w - 1] + tempB[pos] + tempB[pos + w] ) >> 2;
		wG[i*wWidth+wWidth - 1] = ( 2*tempG[pos]-tempG[pos - 1] + 2*tempG[pos + w]-tempG[pos + w - 1] + tempG[pos] + tempG[pos + w] ) >> 2;
		wR[i*wWidth+wWidth - 1] = ( 2*tempR[pos]-tempR[pos - 1] + 2*tempR[pos + w]-tempR[pos + w - 1] + tempR[pos] + tempR[pos + w] ) >> 2;
	}
	//************generate final weight matrix, end***********

	free(tempWM);

	//normalize 1
	int minB = 0xefff, minG = 0xefff, minR = 0xefff;//Shelley 20100514
	int count = 0;
	int UnitGain = 1<<nScaleFactor;
	for(i=0; i<wHeight; i++)
	{
		for(j=0; j<wWidth; j++)
		{
			if(minB > wB[i*wWidth+j])
				minB = wB[i*wWidth+j];
			if(minG > wG[i*wWidth+j])
				minG = wG[i*wWidth+j];
			if(minR > wR[i*wWidth+j])
				minR = wR[i*wWidth+j];
		}
	}

	for(i=0; i<wHeight; i++)
	{
		for(j=0; j<wWidth; j++)
		{
			wB[i*wWidth+j] = (BYTE)(CLIP( (wB[i*wWidth+j] << nScaleFactor) / minB - UnitGain, 0, 255) * fLSC_B_Ratio + 0.5);//Shelley 20100514
			wG[i*wWidth+j] = (BYTE)(CLIP( (wG[i*wWidth+j] << nScaleFactor) / minG - UnitGain, 0, 255) * fLSC_G_Ratio + 0.5);//Shelley 20100514
			wR[i*wWidth+j] = (BYTE)(CLIP( (wR[i*wWidth+j] << nScaleFactor) / minR - UnitGain, 0, 255) * fLSC_R_Ratio + 0.5);//Shelley 20100514 			
		}
	}		


	int GetX, GetY;
	for (j=0; j<(TableVSize+1); j++)
	{
		for (i=0; i<(TableHSize+1); i++)
		{
			GetX = CLIP(i, 0, wWidth -1);
			GetY = CLIP(j, 0, wHeight-1);

			BGainTable[j*21+i] = wB[GetY*wWidth+GetX]; //For Weighting Matrix B
			GGainTable[j*21+i] = wG[GetY*wWidth+GetX]; //For Weighting Matrix G
			RGainTable[j*21+i] = wR[GetY*wWidth+GetX]; //For Weighting Matrix R
		}
	}
	free(wB);
	free(wG);
	free(wR);	
	free(raw);		// add [3/25/2011 haifeng_liu]
}

void CRawUtil::GaussianFilter(UINT uiWid, UINT uiHei, BYTE *pbSrc)
{
#if 1
	ASSERT(NULL != pbSrc);

	BYTE *pbSrcTmp = NULL;
	const UINT uiSIZE = uiWid * uiHei;
	pbSrcTmp = new BYTE[uiSIZE];
	ASSERT(NULL != pbSrcTmp);
	if (NULL == pbSrcTmp)
	{
		return;
	}
	memcpy(pbSrcTmp, pbSrc, uiSIZE);   

	int h = 0, w = 0;
	int m = 0, n = 0;
	BYTE temp[9] = {0};
	int iTmp = 0;

	for (h = 1; h < uiHei-1; h++)
	{
		for (w = 1; w < uiWid-1; w++)
		{
			temp[0] = pbSrc[(h-1)*uiWid+(w-1)];
			temp[1] = pbSrc[(h-1)*uiWid+w];
			temp[2] = pbSrc[(h-1)*uiWid+(w+1)];
			temp[3] = pbSrc[h*uiWid+(w-1)];
			temp[4] = pbSrc[h*uiWid+w];
			temp[5] = pbSrc[h*uiWid+w+1];
			temp[6] = pbSrc[(h+1)*uiWid+(w-1)];
			temp[7] = pbSrc[(h+1)*uiWid+w];
			temp[8] = pbSrc[(h+1)*uiWid+(w+1)];

			iTmp = temp[0] + (temp[1]*2) + temp[2]
			+ (temp[3]*2) + (temp[4]*4) + (temp[5]*2)
				+ temp[6] + (temp[7]*2) + temp[8];

			iTmp = iTmp >> 4;

			iTmp = iTmp > 255 ? 255 : iTmp;			

			pbSrc[h*uiWid+w] = 	iTmp;
		}
	} 	

	SAFEDELETEARRAY(pbSrcTmp);
#else
	ASSERT(NULL != pbSrc);

	BYTE *pbSrcTmp = NULL;
	const UINT uiSIZE = uiWid * uiHei;
	pbSrcTmp = new BYTE[uiSIZE];
	ASSERT(NULL != pbSrcTmp);
	if (NULL == pbSrcTmp)
	{
		return;
	}
	memcpy(pbSrcTmp, pbSrc, uiSIZE);   

	int h = 0, w = 0;
	int m = 0, n = 0;
	BYTE temp[25] = {0};
	int iTmp = 0; 
	int index = 0;

	int th = 0, tw = 0;
	int tmplate[25] = {
		1,	4,	7,	4,	1,
		4,	16,	26,	16,	4,
		7,	26,	41,	26,	7,
		4,	16,	26,	16,	4,
		1,	4,	7,	4,	1 
	}; 

	for (h = 2; h < uiHei-2; h++)
	{
		for (w = 2; w < uiWid-2; w++)
		{
			iTmp = 0;
			index = 0;

			for (th = -2; th < 3; th++)
			{
				for (tw = -2; tw < 3; tw++)
				{
					iTmp += pbSrc[(h+th)*uiWid+(w+tw)] * tmplate[index++];
				}
			}

			iTmp = iTmp/273;

			iTmp = iTmp > 255 ? 255 : iTmp;		

			pbSrc[h*uiWid+w] = 	iTmp;
		}
	} 	

	SAFEDELETEARRAY(pbSrcTmp);
#endif
}

void CRawUtil::GaussianFilter_Y(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	GaussianFilter(pPars->uiWid, pPars->uiHei, pPars->pbY); 
}

void CRawUtil::GaussianFilter_U(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	GaussianFilter(pPars->uiWid/2, pPars->uiHei, pPars->pbU);
}

void CRawUtil::GaussianFilter_V(CRawPars *pPars)
{
	ASSERT(NULL != pPars);

	GaussianFilter(pPars->uiWid/2, pPars->uiHei, pPars->pbV);
}



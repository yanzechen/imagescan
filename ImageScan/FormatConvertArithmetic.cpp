#include "stdafx.h"
#include "FormatConvertArithmetic.h"


CFormatConvertArithmetic::CFormatConvertArithmetic(void)
{
}

CFormatConvertArithmetic::~CFormatConvertArithmetic(void)
{
}

void CFormatConvertArithmetic::YUY2ToRGB24( BYTE *pbyteSrc, BYTE *pbyteDest, long lwidth, long lheight )
{
	LONG width, height;
	LONG bOffset, yOffset, uOffset, vOffset;
	double bValue, gValue, rValue;	
	width = lwidth;
	height = lheight;


	for (int h = 0; h < height; ++h) 
	{
		for (int w = 0; w < width; ++w) 
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

void CFormatConvertArithmetic::I420ToRGB24(BYTE *pbyteSrc, BYTE *pbyteDest, long lwidth, long lheight)
{
	LONG width, height;
	LONG bOffset, yOffset, uOffset, vOffset;
	double bValue, gValue, rValue; 
	width = lwidth;
	height = lheight;

	LONG yStart = 0;
	LONG uStart = width * height;
	LONG vStart = width * height * 5 / 4;

	for (int h = 0; h < height; ++h) 
	{
		for (int w = 0; w < width; ++w) 
		{
			yOffset = yStart + h * width + w;
			uOffset = uStart + (h / 2) * (width / 2) + (w / 2);
			vOffset = vStart + (h / 2) * (width / 2) + (w / 2);
			bOffset = ((height - h - 1) * width + w) * 3;

			bValue = pbyteSrc[yOffset] + 1.772 * (pbyteSrc[uOffset] - 128);
			gValue = pbyteSrc[yOffset] - 0.34414 * (pbyteSrc[uOffset] - 128) - 0.71414 * (pbyteSrc[vOffset] - 128);
			rValue = pbyteSrc[yOffset] + 1.402 * (pbyteSrc[vOffset] - 128);

			pbyteDest[bOffset + 0] = (bValue > 255) ? 255 : (bValue < 0 ) ? 0 : (BYTE)bValue;
			pbyteDest[bOffset + 1] = (gValue > 255) ? 255 : (gValue < 0 ) ? 0 : (BYTE)gValue;
			pbyteDest[bOffset + 2] = (rValue > 255) ? 255 : (rValue < 0 ) ? 0 : (BYTE)rValue;
		}
	}
}



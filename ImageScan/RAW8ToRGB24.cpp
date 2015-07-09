// RAW8ToRGB24.cpp: implementation of the RAW8ToRGB24 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RAW8ToRGB24.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRAW8ToRGB24::CRAW8ToRGB24( UINT w, UINT h )
{
	pImage = NULL;
	W = w;
	H = h;
}

CRAW8ToRGB24::CRAW8ToRGB24(BYTE *pImage, UINT w, UINT h )
	{
	ASSERT(NULL != pImage);
	this->pImage = pImage;
	W = w;
	H = h;
	}

CRAW8ToRGB24::~CRAW8ToRGB24(void)
{
}

void CRAW8ToRGB24::BToRGB( UINT iX, UINT iY, const BYTE *pImage, DWORD& dwBout, DWORD& dwGout, DWORD& dwRout )
{
	static BYTE b[9];
	static BYTE g[12];
	static BYTE r[4];

	b[0] = pImage[iX*W+iY];
	b[1] = pImage[iX*W+iY+2];
	b[2] = pImage[iX*W+iY+4];

	b[3] = pImage[(iX+2)*W+iY];
	b[4] = pImage[(iX+2)*W+iY+2];
	b[5] = pImage[(iX+2)*W+iY+4];

	b[6] = pImage[(iX+4)*W+iY];
	b[7] = pImage[(iX+4)*W+iY+2];
	b[8] = pImage[(iX+4)*W+iY+4];

	dwBout = b[0] + (b[1])*6 + b[2]
	+ (b[3])*6 + (b[4])*36 + (b[5])*6
		+ b[6] + (b[7])*6 + b[8];  	

	g[0] = pImage[iX*W+iY+1];
	g[1] = pImage[iX*W+iY+3];

	g[2] = pImage[(iX+1)*W+iY];
	g[3] = pImage[(iX+1)*W+iY+2];
	g[4] = pImage[(iX+1)*W+iY+4];

	g[5] = pImage[(iX+2)*W+iY+1];
	g[6] = pImage[(iX+2)*W+iY+3];

	g[7] = pImage[(iX+3)*W+iY];
	g[8] = pImage[(iX+3)*W+iY+2];
	g[9] = pImage[(iX+3)*W+iY+4];

	g[10] = pImage[(iX+4)*W+iY+1];
	g[11] = pImage[(iX+4)*W+iY+3];

	dwGout = g[0] + g[1]
	+ g[2] + (g[3])*14 + g[4]
	+ (g[5])*14 + (g[6])*14
		+ g[7] + (g[8])*14 + g[9]
	+ g[10] + g[11];

	r[0] = pImage[(iX+1)*W+iY+1];
	r[1] = pImage[(iX+1)*W+iY+3];
	r[2] = pImage[(iX+3)*W+iY+1];
	r[3] = pImage[(iX+3)*W+iY+3];

	dwRout = (r[0] + r[1] + r[2] + r[3])*16; 

	dwBout >>= 6;
	dwGout >>= 6;
	dwRout >>= 6;

	dwBout = CLIP(dwBout, 0, 255);
	dwGout = CLIP(dwGout, 0, 255);
	dwRout = CLIP(dwRout, 0, 255);	 
}
void CRAW8ToRGB24::GbToRGB( UINT iX, UINT iY, const BYTE *pImage, DWORD& dwBout, DWORD& dwGout, DWORD& dwRout )
{
	static BYTE b[6];
	static BYTE g[9];
	static BYTE r[6];

	b[0] = pImage[iX*W+iY+1];
	b[1] = pImage[iX*W+iY+3];
	b[2] = pImage[(iX+2)*W+iY+1];
	b[3] = pImage[(iX+2)*W+iY+3];
	b[4] = pImage[(iX+4)*W+iY+1];
	b[5] = pImage[(iX+4)*W+iY+3];

	dwBout = (b[0] + b[1])*4 
		+ (b[2] + b[3])*24
		+ (b[4] + b[5])*4;

	g[0] = pImage[iX*W+iY+2];

	g[1] = pImage[(iX+1)*W+iY+1];
	g[2] = pImage[(iX+1)*W+iY+3];

	g[3] = pImage[(iX+2)*W+iY];
	g[4] = pImage[(iX+2)*W+iY+2];
	g[5] = pImage[(iX+2)*W+iY+4];

	g[6] = pImage[(iX+3)*W+iY+1];
	g[7] = pImage[(iX+3)*W+iY+3];

	g[8] = pImage[(iX+4)*W+iY+2];	
	
	dwGout = (g[0])*2
		+ (g[1] + g[2])*8
		+ (g[3])*2 + (g[4])*24 + (g[5])*2
		+ (g[6] + g[7])*8
		+ (g[8])*2;

	r[0] = pImage[(iX+1)*W+iY];
	r[1] = pImage[(iX+1)*W+iY+2];
	r[2] = pImage[(iX+1)*W+iY+4];

	r[3] = pImage[(iX+3)*W+iY];
	r[4] = pImage[(iX+3)*W+iY+2];
	r[5] = pImage[(iX+3)*W+iY+4];

	dwRout = (r[0])*4 + (r[1])*24 + (r[2])*4
		+ (r[3])*4 + (r[4])*24 + (r[5])*4;

	dwBout >>= 6;
	dwGout >>= 6;
	dwRout >>= 6;

	dwBout = CLIP(dwBout, 0, 255);
	dwGout = CLIP(dwGout, 0, 255);
	dwRout = CLIP(dwRout, 0, 255);	
} 

void CRAW8ToRGB24::GrToRGB( UINT iX, UINT iY, const BYTE *pImage, DWORD& dwBout, DWORD& dwGout, DWORD& dwRout )
{
	static BYTE b[6];
	static BYTE g[9];
	static BYTE r[6];

	b[0] = pImage[(iX+1)*W+iY];
	b[1] = pImage[(iX+1)*W+iY+2];
	b[2] = pImage[(iX+1)*W+iY+4];

	b[3] = pImage[(iX+3)*W+iY];
	b[4] = pImage[(iX+3)*W+iY+2];
	b[5] = pImage[(iX+3)*W+iY+4];		

	dwBout = (b[0])*4 + (b[1])*24 + (b[2])*4
		+ (b[3])*4 + (b[4])*24 + (b[5])*4;

	g[0] = pImage[iX*W+iY+2];

	g[1] = pImage[(iX+1)*W+iY+1];
	g[2] = pImage[(iX+1)*W+iY+3];

	g[3] = pImage[(iX+2)*W+iY];
	g[4] = pImage[(iX+2)*W+iY+2];
	g[5] = pImage[(iX+2)*W+iY+4];

	g[6] = pImage[(iX+3)*W+iY+1];
	g[7] = pImage[(iX+3)*W+iY+3];

	g[8] = pImage[(iX+4)*W+iY+2];

	dwGout = (g[0])*2
		+ (g[1] + g[2])*8
		+ (g[3])*2 + (g[4])*24 + (g[5])*2
		+ (g[6] + g[7])*8
		+ (g[8])*2;

	r[0] = pImage[iX*W+iY+1];
	r[1] = pImage[iX*W+iY+3];

	r[2] = pImage[(iX+2)*W+iY+1];
	r[3] = pImage[(iX+2)*W+iY+3];

	r[4] = pImage[(iX+4)*W+iY+1];
	r[5] = pImage[(iX+4)*W+iY+3];

	dwRout = (r[0] + r[1])*4
		+ (r[2] + r[3])*24
		+ (r[4] + r[5])*4;	

	dwBout >>= 6;
	dwGout >>= 6;
	dwRout >>= 6;

	dwBout = CLIP(dwBout, 0, 255);
	dwGout = CLIP(dwGout, 0, 255);
	dwRout = CLIP(dwRout, 0, 255);	 
}  

void CRAW8ToRGB24::RToRGB( UINT iX, UINT iY, const BYTE *pImage, DWORD& dwBout, DWORD& dwGout, DWORD& dwRout )
{
	BYTE r[9];
	BYTE g[12];
	BYTE b[4];

	r[0] = pImage[iX*W+iY];
	r[1] = pImage[iX*W+iY+2];
	r[2] = pImage[iX*W+iY+4];

	r[3] = pImage[(iX+2)*W+iY];
	r[4] = pImage[(iX+2)*W+iY+2];
	r[5] = pImage[(iX+2)*W+iY+4];

	r[6] = pImage[(iX+4)*W+iY];
	r[7] = pImage[(iX+4)*W+iY+2];
	r[8] = pImage[(iX+4)*W+iY+4];

	dwRout = r[0] + (r[1])*6 + r[2]
	+ (r[3])*6 + (r[4])*36 + (r[5])*6
		+ r[6] + (r[7])*6 + r[8];

	g[0] = pImage[iX*W+iY+1];
	g[1] = pImage[iX+W+iY+3];

	g[2] = pImage[(iX+1)*W+iY];
	g[3] = pImage[(iX+1)*W+iY+2];
	g[4] = pImage[(iX+1)*W+iY+4];

	g[5] = pImage[(iX+2)*W+iY+1];
	g[6] = pImage[(iX+2)*W+iY+3];

	g[7] = pImage[(iX+3)*W+iY];
	g[8] = pImage[(iX+3)*W+iY+2];
	g[9] = pImage[(iX+3)*W+iY+4];

	g[10] = pImage[(iX+4)*W+iY+1];
	g[11] = pImage[(iX+4)*W+iY+3];

	dwGout = g[0] + g[1]
	+ g[2] + (g[3])*14 + g[4]
	+ (g[5] + g[6])*14
		+ g[7] + (g[8])*14 + g[9]
	+ g[10] + g[11];

	b[0] = pImage[(iX+1)*W+iY+1];
	b[1] = pImage[(iX+1)*W+iY+3];

	b[2] = pImage[(iX+3)*W+iY+1];
	b[3] = pImage[(iX+3)*W+iY+3];

	dwBout = (b[0] + b[1] + b[2] + b[3])*16;

	dwBout >>= 6;
	dwGout >>= 6;
	dwRout >>= 6;

	dwBout = CLIP(dwBout, 0, 255);
	dwGout = CLIP(dwGout, 0, 255);
	dwRout = CLIP(dwRout, 0, 255);	 
}

RGBPixel& CRAW8ToRGB24::BToRGB( UINT iX, UINT iY )
	{
	ASSERT(NULL != pImage);

	static BYTE b[9];
	static BYTE g[12];
	static BYTE r[4];

	b[0] = pImage[iX*W+iY];
	b[1] = pImage[iX*W+iY+2];
	b[2] = pImage[iX*W+iY+4];

	b[3] = pImage[(iX+2)*W+iY];
	b[4] = pImage[(iX+2)*W+iY+2];
	b[5] = pImage[(iX+2)*W+iY+4];

	b[6] = pImage[(iX+4)*W+iY];
	b[7] = pImage[(iX+4)*W+iY+2];
	b[8] = pImage[(iX+4)*W+iY+4];

	dwBout = b[0] + (b[1])*6 + b[2]
	+ (b[3])*6 + (b[4])*36 + (b[5])*6
		+ b[6] + (b[7])*6 + b[8];	

	g[0] = pImage[iX*W+iY+1];
	g[1] = pImage[iX*W+iY+3];

	g[2] = pImage[(iX+1)*W+iY];
	g[3] = pImage[(iX+1)*W+iY+2];
	g[4] = pImage[(iX+1)*W+iY+4];

	g[5] = pImage[(iX+2)*W+iY+1];
	g[6] = pImage[(iX+2)*W+iY+3];

	g[7] = pImage[(iX+3)*W+iY];
	g[8] = pImage[(iX+3)*W+iY+2];
	g[9] = pImage[(iX+3)*W+iY+4];

	g[10] = pImage[(iX+4)*W+iY+1];
	g[11] = pImage[(iX+4)*W+iY+3];

	dwGout = g[0] + g[1]
	+ g[2] + (g[3])*14 + g[4]
	+ (g[5] + g[6])*14
		+ g[7] + (g[8])*14 + g[9]
	+ g[10] + g[11];

	r[0] = pImage[(iX+1)*W+iY+1];
	r[1] = pImage[(iX+1)*W+iY+3];
	r[2] = pImage[(iX+3)*W+iY+1];
	r[3] = pImage[(iX+3)*W+iY+3];

	dwRout = (r[0] + r[1] + r[2] + r[3])*16;	  

	dwBout >>= 6;
	dwGout >>= 6;
	dwRout >>= 6;

	rgbPixel.byB = (BYTE)CLIP(dwBout, 0, 255);
	rgbPixel.byG = (BYTE)CLIP(dwGout, 0, 255);
	rgbPixel.byR = (BYTE)CLIP(dwRout, 0, 255);

	return rgbPixel; 
	}
RGBPixel& CRAW8ToRGB24::GbToRGB( UINT iX, UINT iY )
	{
	ASSERT(NULL != pImage);

	static BYTE b[6];
	static BYTE g[9];
	static BYTE r[6];

	b[0] = pImage[iX*W+iY+1];
	b[1] = pImage[iX*W+iY+3];
	b[2] = pImage[(iX+2)*W+iY+1];
	b[3] = pImage[(iX+2)*W+iY+3];
	b[4] = pImage[(iX+4)*W+iY+1];
	b[5] = pImage[(iX+4)*W+iY+3];

	dwBout = (b[0] + b[1])*4 
		+ (b[2] + b[3])*24
		+ (b[4] + b[5])*4;

	g[0] = pImage[iX*W+iY+2];

	g[1] = pImage[(iX+1)*W+iY+1];
	g[2] = pImage[(iX+1)*W+iY+3];

	g[3] = pImage[(iX+2)*W+iY];
	g[4] = pImage[(iX+2)*W+iY+2];
	g[5] = pImage[(iX+2)*W+iY+4];

	g[6] = pImage[(iX+3)*W+iY+1];
	g[7] = pImage[(iX+3)*W+iY+3];

	g[8] = pImage[(iX+4)*W+iY+2]; 
	
	dwGout = (g[0])*2
		+ (g[1] + g[2])*8
		+ (g[3])*2 + (g[4])*24 + (g[5])*2
		+ (g[6] + g[7])*8
		+ (g[8])*2;

	r[0] = pImage[(iX+1)*W+iY];
	r[1] = pImage[(iX+1)*W+iY+2];
	r[2] = pImage[(iX+1)*W+iY+4];

	r[3] = pImage[(iX+3)*W+iY];
	r[4] = pImage[(iX+3)*W+iY+2];
	r[5] = pImage[(iX+3)*W+iY+4];

	dwRout = (r[0])*4 + (r[1])*24 + (r[2])*4
		+ (r[3])*4 + (r[4])*24 + (r[5])*4;	

	dwBout >>= 6;
	dwGout >>= 6;
	dwRout >>= 6; 

	rgbPixel.byB = (BYTE)CLIP(dwBout, 0, 255);
	rgbPixel.byG = (BYTE)CLIP(dwGout, 0, 255);
	rgbPixel.byR = (BYTE)CLIP(dwRout, 0, 255);

	return rgbPixel;
	}
RGBPixel& CRAW8ToRGB24::GrToRGB( UINT iX, UINT iY )
	{
	ASSERT(NULL != pImage);

	static BYTE b[6];
	static BYTE g[9];
	static BYTE r[6];

	b[0] = pImage[(iX+1)*W+iY];
	b[1] = pImage[(iX+1)*W+iY+2];
	b[2] = pImage[(iX+1)*W+iY+4];

	b[3] = pImage[(iX+3)*W+iY];
	b[4] = pImage[(iX+3)*W+iY+2];
	b[5] = pImage[(iX+3)*W+iY+4];  
	
	dwBout = (b[0])*4 + (b[1])*24 + (b[2])*4
		+ (b[3])*4 + (b[4])*24 + (b[5])*4;

	g[0] = pImage[iX*W+iY+2];

	g[1] = pImage[(iX+1)*W+iY+1];
	g[2] = pImage[(iX+1)*W+iY+3];

	g[3] = pImage[(iX+2)*W+iY];
	g[4] = pImage[(iX+2)*W+iY+2];
	g[5] = pImage[(iX+2)*W+iY+4];

	g[6] = pImage[(iX+3)*W+iY+1];
	g[7] = pImage[(iX+3)*W+iY+3];

	g[8] = pImage[(iX+4)*W+iY+2];

	dwGout = (g[0])*2
		+ (g[1] + g[2])*8
		+ (g[3])*2 + (g[4])*24 + (g[5])*2
		+ (g[6] + g[7])*8
		+ (g[8])*2;	

	r[0] = pImage[iX*W+iY+1];
	r[1] = pImage[iX*W+iY+3];

	r[2] = pImage[(iX+2)*W+iY+1];
	r[3] = pImage[(iX+2)*W+iY+3];

	r[4] = pImage[(iX+4)*W+iY+1];
	r[5] = pImage[(iX+4)*W+iY+3];

	dwRout = (r[0] + r[1])*4
		+ (r[2] + r[3])*24
		+ (r[4] + r[5])*4; 	

	dwBout >>= 6;
	dwGout >>= 6;
	dwRout >>= 6;	 

	rgbPixel.byB = (BYTE)CLIP(dwBout, 0, 255);
	rgbPixel.byG = (BYTE)CLIP(dwGout, 0, 255);
	rgbPixel.byR = (BYTE)CLIP(dwRout, 0, 255);

	return rgbPixel;  
	}

RGBPixel& CRAW8ToRGB24::RToRGB( UINT iX, UINT iY )
	{
	ASSERT(NULL != pImage);

	BYTE r[9];
	BYTE g[12];
	BYTE b[4];

	r[0] = pImage[iX*W+iY];
	r[1] = pImage[iX*W+iY+2];
	r[2] = pImage[iX*W+iY+4];

	r[3] = pImage[(iX+2)*W+iY];
	r[4] = pImage[(iX+2)*W+iY+2];
	r[5] = pImage[(iX+2)*W+iY+4];

	r[6] = pImage[(iX+4)*W+iY];
	r[7] = pImage[(iX+4)*W+iY+2];
	r[8] = pImage[(iX+4)*W+iY+4];

	dwRout = r[0] + (r[1])*6 + r[2]
	+ (r[3])*6 + (r[4])*36 + (r[5])*6
		+ r[6] + (r[7])*6 + r[8];	

	g[0] = pImage[iX*W+iY+1];
	g[1] = pImage[iX+W+iY+3];

	g[2] = pImage[(iX+1)*W+iY];
	g[3] = pImage[(iX+1)*W+iY+2];
	g[4] = pImage[(iX+1)*W+iY+4];

	g[5] = pImage[(iX+2)*W+iY+1];
	g[6] = pImage[(iX+2)*W+iY+3];

	g[7] = pImage[(iX+3)*W+iY];
	g[8] = pImage[(iX+3)*W+iY+2];
	g[9] = pImage[(iX+3)*W+iY+4];

	g[10] = pImage[(iX+4)*W+iY+1];
	g[11] = pImage[(iX+4)*W+iY+3];

	dwGout = g[0] + g[1]
	+ g[2] + (g[3])*14 + g[4]
	+ (g[5] + g[6])*14
		+ g[7] + (g[8])*14 + g[9]
	+ g[10] + g[11];

	b[0] = pImage[(iX+1)*W+iY+1];
	b[1] = pImage[(iX+1)*W+iY+3];

	b[2] = pImage[(iX+3)*W+iY+1];
	b[3] = pImage[(iX+3)*W+iY+3];

	dwBout = (b[0] + b[1] + b[2] + b[3])*16; 	

	dwBout >>= 6;
	dwGout >>= 6;
	dwRout >>= 6;

	rgbPixel.byB = (BYTE)CLIP(dwBout, 0, 255);
	rgbPixel.byG = (BYTE)CLIP(dwGout, 0, 255);
	rgbPixel.byR = (BYTE)CLIP(dwRout, 0, 255);

	return rgbPixel;  
	}


void CRAW8ToRGB24::RGB24_GeomMirV( RGBPixel *pImage)
{
	RGB24_GeomMir(pImage, FALSE);
}

void CRAW8ToRGB24::RGB24_GeomMirH( RGBPixel *pImage)
{
	RGB24_GeomMir(pImage, TRUE);
}

void CRAW8ToRGB24::RGB24_GeomMir( RGBPixel *pImage, BOOL bDiretion )
{
	UINT i = 0, j = 0;	
	RGBPixel *pLine = NULL;
	RGBPixel *pSrc = NULL;
	RGBPixel *pDst = NULL;	

	pLine = new RGBPixel[W];
	if (NULL == pLine)
	{		
		return;
	}

	// Ë®Æ½¾µÏñ
	if (bDiretion)
	{
		for (i = 0; i < H; i++)
		{
			for (j = 0; j < W/2; j++)
			{
				pSrc = pImage + W*i + j;
				pDst = pImage + W*(i+1) - j;

				*pLine = *pDst;
				*pDst = *pSrc;
				*pSrc = *pLine;
			}
		}

	}
	// ´¹Ö±¾µÏñ
	else
	{
		for (i = 0; i < H/2; i++)
		{
			pSrc = pImage + W*i;
			pDst = pImage + W*(H-i-1);

			memcpy(pLine, pDst, sizeof(RGBPixel)*W);

			memcpy(pDst, pSrc, sizeof(RGBPixel)*W);

			memcpy(pSrc, pLine, sizeof(RGBPixel)*W);
		}

	}

	SAFEDELETEARRAY(pLine);
} 

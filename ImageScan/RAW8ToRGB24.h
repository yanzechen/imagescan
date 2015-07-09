// RAW8ToRGB24.h: interface for the RAW8ToRGB24 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAW8TORGB24_H__22C39C16_826A_4326_8771_83DD2DA98015__INCLUDED_)
#define AFX_RAW8TORGB24_H__22C39C16_826A_4326_8771_83DD2DA98015__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000  

typedef enum{ B, Gb, Gr, R} ColorType;
typedef struct  
{
	BYTE byB;
	BYTE byG;
	BYTE byR;
} RGBPixel, *PRGBPixel;

class CRAW8ToRGB24
{
public:
	CRAW8ToRGB24(UINT w, UINT h);
	CRAW8ToRGB24(BYTE *pImage, UINT w, UINT h);	
	~CRAW8ToRGB24(void);
	
private:
	UINT H;
	UINT W;
	BYTE *pImage;
	RGBPixel rgbPixel;
	DWORD dwBout;
	DWORD dwGout;
	DWORD dwRout;
	
public:
	void BToRGB(UINT iX, UINT iY, const BYTE *pImage, DWORD& dwBout, DWORD& dwGout, DWORD& dwRout);
	void GbToRGB(UINT iX, UINT iY, const BYTE *pImage, DWORD& dwBout, DWORD& dwGout, DWORD& dwRout);
	void GrToRGB(UINT iX, UINT iY, const BYTE *pImage, DWORD& dwBout, DWORD& dwGout, DWORD& dwRout);
	void RToRGB(UINT iX, UINT iY, const BYTE *pImage, DWORD& dwBout, DWORD& dwGout, DWORD& dwRout);

	RGBPixel& BToRGB(UINT iX, UINT iY);
	RGBPixel& GbToRGB(UINT iX, UINT iY);
	RGBPixel& GrToRGB(UINT iX, UINT iY);
	RGBPixel& RToRGB(UINT iX, UINT iY);		
	
	void RGB24_GeomMirV(RGBPixel *pImage);
	void RGB24_GeomMirH(RGBPixel *pImage);
	void RGB24_GeomMir(RGBPixel *pImage, BOOL bDirection);			
};


#endif // !defined(AFX_RAW8TORGB24_H__22C39C16_826A_4326_8771_83DD2DA98015__INCLUDED_)

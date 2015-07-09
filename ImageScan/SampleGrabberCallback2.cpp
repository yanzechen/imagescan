// SampleGrabberCallback.cpp: implementation of the CSampleGrabberCallback2 class.
//
//////////////////////////////////////////////////////////////////////
#include <afxdlgs.h>
#include "resource.h"
#include <streams.h>
#include <qedit.h>
#include <stdio.h>							// for "sprintf"
#include "stdafx.h"
#include "PicConver.h"
#include "SampleGrabberCallback2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSampleGrabberCallback2::CSampleGrabberCallback2()
	{
	m_bIsNeedSecondBuffer = FALSE;
	m_CusBuffer.pBuffer0 = NULL;
	m_CusBuffer.pBuffer1 = NULL;
	m_CusBuffer.sizeBuf0.cx = 0;
	m_CusBuffer.sizeBuf0.cy = 0;
	m_CusBuffer.sizeBuf1.cx = 0;
	m_CusBuffer.sizeBuf1.cy = 0;
	m_CusBuffer.ulBufLen0 = 0;
	m_CusBuffer.ulBufLen1 = 0;
	m_CusBuffer.bIsTwoBuffer = FALSE;
	}

CSampleGrabberCallback2::~CSampleGrabberCallback2()
	{
	FreeMediaType(m_mtStillMediaType);

	ReleaseCusBuffer();
	}


AM_MEDIA_TYPE * CSampleGrabberCallback2::GetStillMediaType()
	{
	return &m_mtStillMediaType;
	}

void CSampleGrabberCallback2::ReleaseCusBuffer()
	{
	SAFEDELETEARRAY(m_CusBuffer.pBuffer0);
	SAFEDELETEARRAY(m_CusBuffer.pBuffer1);

	m_CusBuffer.sizeBuf0.cx = 0;
	m_CusBuffer.sizeBuf0.cy = 0;
	m_CusBuffer.sizeBuf1.cx = 0;
	m_CusBuffer.sizeBuf1.cy = 0;
	m_CusBuffer.ulBufLen0 = 0;
	m_CusBuffer.ulBufLen1 = 0;

	m_CusBuffer.bIsTwoBuffer = FALSE;
	}

BOOL CSampleGrabberCallback2::ImageConvert()
	{
	BYTE *pBuffer = m_CusBuffer.pBuffer0;
	long lwidth = m_CusBuffer.sizeBuf0.cx;
	long lheight = m_CusBuffer.sizeBuf0.cy;
	ULONG BufferLen = m_CusBuffer.ulBufLen0;
	BYTE *pImage = NULL;
	long lBitCount = 0;
	long lsize = 0;

	if(MEDIASUBTYPE_YUY2 == m_mtStillMediaType.subtype	
		|| MEDIASUBTYPE_I420 == m_mtStillMediaType.subtype
		|| MEDIASUBTYPE_RGB24 == m_mtStillMediaType.subtype 
		|| MEDIASUBTYPE_RGB32 == m_mtStillMediaType.subtype)
		{				
		if (MEDIASUBTYPE_YUY2 == m_mtStillMediaType.subtype)
			{
			lsize = lwidth * lheight * 3;
			pImage = new BYTE[lsize];
			if(NULL == pImage)
				{
				return FALSE;
				}
			CFormatConvertArithmetic::YUY2ToRGB24(pBuffer, pImage, lwidth, lheight);
			lBitCount = 24;					
			}
		else if (MEDIASUBTYPE_I420 == m_mtStillMediaType.subtype)
			{
			lsize = lwidth * lheight * 3;
			pImage = new BYTE[lsize];
			if(NULL == pImage)
				return FALSE;
			CFormatConvertArithmetic::I420ToRGB24(pBuffer, pImage, lwidth, lheight);
			lBitCount = 24;	
			}
		// MEDIASUBTYPE_RGB24 or MEDIASUBTYPE_RGB32
		else				
			{
			lsize	= BufferLen;
			pImage = new BYTE[lsize];
			memcpy(pImage, pBuffer, lsize);
			if(MEDIASUBTYPE_RGB24 == m_mtStillMediaType.subtype)
				lBitCount = 24;
			else
				lBitCount = 32;					
			}

		BITMAPFILEHEADER	hdr;
		hdr.bfType		= MAKEWORD('B', 'M');	// always is "BM"
		hdr.bfSize		= lsize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		hdr.bfReserved1 = 0;
		hdr.bfReserved2 = 0;
		hdr.bfOffBits	 = (DWORD) (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

		BITMAPINFOHEADER bih; 
		ZeroMemory(&bih, sizeof(bih));
		bih.biSize = sizeof(BITMAPINFOHEADER); 
		bih.biWidth = lwidth; 
		bih.biHeight = lheight; 		
		bih.biBitCount = (unsigned short)lBitCount;
		bih.biPlanes = 1;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = 0;		

		// 初始化Bitmap		
		g_lpBits = pImage;		
		g_fhdr = hdr;
		g_ihdr = bih;			

		}
	else if (MEDIASUBTYPE_MJPG == m_mtStillMediaType.subtype)
		{
		CPicConver PicConver;
		CMemFile bmpFile;
		CMemFile jpgFile;				
		long lBmpBufferLen = 0;

		jpgFile.Attach(pBuffer, BufferLen);
		PicConver.MBmpToMImage(jpgFile, bmpFile, _T("bmp"));
		jpgFile.Detach();				

		// JPG转换后的BMP图像文件的长度(此长度包含了BITMAPINFOHEADER以及图像数据)
		lBmpBufferLen = (long)bmpFile.GetLength();

		BYTE *pbyteBmp = NULL;				
		pbyteBmp = new BYTE[lBmpBufferLen];
		if (NULL == pbyteBmp)
			{
			return FALSE;
			}

		BYTE *pbyteBuffer = bmpFile.Detach();
		memcpy(pbyteBmp, pbyteBuffer, lBmpBufferLen);

		// MSDN中CMemFile自动分配的Buffer最后在析构函数中会被自动释放, 
		// 在实际测试中发现, CMemFile自动分配的Buffer, 但是如果调用了
		// Detach方法, 那么CMemFile不会自动释放原先分配的Buffer, 必需
		// 手动释放, 否则会出现内存泄露		
		SAFEDELETEARRAY(pbyteBuffer);


		// 获得BITMAPFILEHEADER			
		g_fhdr = *((BITMAPFILEHEADER *)pbyteBmp);


		// 获得BITMAPINFOHEADER			
		g_ihdr = *((BITMAPINFOHEADER *)(pbyteBmp+sizeof(BITMAPFILEHEADER)));


		// 获得除BITMAPFILEHEADER, BITMAPINFOHEADER之外的图像数据长度
		long lDestLen = lBmpBufferLen - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);

		pImage = NULL;
		pImage = new BYTE[lDestLen];
		if (NULL == pImage)
			{			
			SAFEDELETEARRAY(pbyteBmp);
			return FALSE;
			}

		// 获得图像数据
		memcpy(pImage, pbyteBmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER), lDestLen);

		g_lpBits = pImage;

		// 释放临时转换Buffer
		SAFEDELETEARRAY(pbyteBmp);
		}	
	// RAW
	else
		{
		lsize = lwidth * lheight * 3;
		pImage = new BYTE[lsize];
		if (NULL == pImage)
			{
			return FALSE;
			}
		m_FmtConvAtc.RAW8ToRGB24(pBuffer, pImage, lwidth, lheight);
		lBitCount = 24;

		BITMAPFILEHEADER	hdr;
		hdr.bfType		= MAKEWORD('B', 'M');	// always is "BM"
		hdr.bfSize		= lsize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		hdr.bfReserved1 = 0;
		hdr.bfReserved2 = 0;
		hdr.bfOffBits	 = (DWORD) (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

		BITMAPINFOHEADER bih; 
		ZeroMemory(&bih, sizeof(bih));
		bih.biSize = sizeof(BITMAPINFOHEADER); 
		bih.biWidth = lwidth; 
		bih.biHeight = lheight; 		
		bih.biBitCount = (unsigned short)lBitCount;
		bih.biPlanes = 1;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = 0;		

		// 初始化Bitmap		
		g_lpBits = pImage;		
		g_fhdr = hdr;
		g_ihdr = bih;	
		}	
	}

BOOL CSampleGrabberCallback2::ImageConvert_900M()
	{
	ASSERT(NULL != m_CusBuffer.pBuffer0);
	ASSERT(NULL != m_CusBuffer.pBuffer1);
	if (NULL == m_CusBuffer.pBuffer0 || NULL == m_CusBuffer.pBuffer1)
		{
		return FALSE;
		}

	// +++++
	BYTE *pAllBuffer = NULL;	
	long lAllBufferLen = m_CusBuffer.sizeBuf0.cx * m_CusBuffer.sizeBuf0.cy * 2;
	pAllBuffer = new BYTE[lAllBufferLen];
	if (NULL == pAllBuffer)
		{
		return FALSE;
		}
	memset(pAllBuffer, 0, lAllBufferLen);	
	RGB24_GeomMirV(m_CusBuffer.pBuffer1, m_CusBuffer.sizeBuf1.cy/2, m_CusBuffer.sizeBuf1.cx);
	memcpy(pAllBuffer, m_CusBuffer.pBuffer0, m_CusBuffer.ulBufLen0);	
	memcpy(pAllBuffer+m_CusBuffer.ulBufLen0, m_CusBuffer.pBuffer1, m_CusBuffer.ulBufLen1);
	// -----

	BYTE *pImage = NULL;
	long lsize = m_CusBuffer.sizeBuf0.cx * m_CusBuffer.sizeBuf0.cy * 3;
	long lwidth = m_CusBuffer.sizeBuf0.cx;
	long lheight = m_CusBuffer.sizeBuf0.cy;
	pImage = new BYTE[lsize];
	if (NULL == pImage)
		{
		return FALSE;
		}

	m_FmtConvAtc.RAW8ToRGB24(pAllBuffer, pImage, lwidth, lheight);
	SAFEDELETEARRAY(pAllBuffer);	

	BITMAPFILEHEADER	hdr;
	hdr.bfType		= MAKEWORD('B', 'M');	// always is "BM"
	hdr.bfSize		= lsize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits	 = (DWORD) (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

	BITMAPINFOHEADER bih; 
	ZeroMemory(&bih, sizeof(bih));
	bih.biSize = sizeof(BITMAPINFOHEADER); 
	bih.biWidth = lwidth; 
	bih.biHeight = lheight; 		
	bih.biBitCount = 24;
	bih.biPlanes = 1;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = 0;		

	// 初始化Bitmap		
	g_lpBits = pImage;
	g_fhdr = hdr;
	g_ihdr = bih;		
	}

void CSampleGrabberCallback2::RGB24_GeomMirV( BYTE *pImage, const UINT iHeigh, const UINT iWidth )
	{
	RGB24_GeomMir(pImage, iHeigh, iWidth, FALSE);
	}


void CSampleGrabberCallback2::RGB24_GeomMirH( BYTE *pImage, const UINT iHeigh, const UINT iWidth )
	{
	RGB24_GeomMir((RGBPixel *)pImage, iHeigh, iWidth, TRUE);
	}

void CSampleGrabberCallback2::RGB24_GeomMir( RGBPixel *pImage, const UINT iHeigh, const UINT iWidth, BOOL bDiretion )
	{
	UINT i = 0, j = 0;	
	RGBPixel *pLine = NULL;
	RGBPixel *pSrc = NULL;
	RGBPixel *pDst = NULL;	

	pLine = new RGBPixel[iWidth];
	if (NULL == pLine)
		{		
		return;
		}

	// 水平镜像
	if (bDiretion)
		{
		for (i = 0; i < iHeigh; i++)
			{
			for (j = 0; j < iWidth/2; j++)
				{
				pSrc = pImage + iWidth*i + j;
				pDst = pImage + iWidth*(i+1) - j;

				*pLine = *pDst;
				*pDst = *pSrc;
				*pSrc = *pLine;
				}
			}

		}
	// 垂直镜像
	else
		{
		for (i = 0; i < iHeigh/2; i++)
			{
			pSrc = pImage + iWidth*i;
			pDst = pImage + iWidth*(iHeigh-i-1);

			memcpy(pLine, pDst, sizeof(RGBPixel)*iWidth);

			memcpy(pDst, pSrc, sizeof(RGBPixel)*iWidth);

			memcpy(pSrc, pLine, sizeof(RGBPixel)*iWidth);
			}

		}

	delete [] pLine;
	}

void CSampleGrabberCallback2::RGB24_GeomMir( BYTE *pImage, const UINT iHeigh, const UINT iWidth, BOOL bDiretion )
	{
	UINT i = 0, j = 0;	
	BYTE *pLine = NULL;
	BYTE *pSrc = NULL;
	BYTE *pDst = NULL;	

	pLine = new BYTE[iWidth];
	if (NULL == pLine)
		{		
		return;
		}

	// 水平镜像
	if (bDiretion)
		{
		/*for (i = 0; i < iHeigh; i++)
			{
			for (j = 0; j < iWidth/2; j++)
				{
				pSrc = pImage + iWidth*i + j;
				pDst = pImage + iWidth*(i+1) - j;

				*pLine = *pDst;
				*pDst = *pSrc;
				*pSrc = *pLine;
				}
			}*/

		}
	// 垂直镜像
	else
		{
		for (i = 0; i < iHeigh/2; i++)
			{
			pSrc = pImage + iWidth*i;
			pDst = pImage + iWidth*(iHeigh-i-1);

			memcpy(pLine, pDst, sizeof(BYTE)*iWidth);

			memcpy(pDst, pSrc, sizeof(BYTE)*iWidth);

			memcpy(pSrc, pLine, sizeof(BYTE)*iWidth);
			}

		}

	delete [] pLine;
	}
#include "StdAfx.h"
#include "StillSampleGrabberCallback2.h"
#include "RawUtil.h"
#include "RawPars.h"
#include "ImageScanDlg.h"

CStillSampleGrabberCallback2::CStillSampleGrabberCallback2(void)
	{
	m_bIsGetAMMEDIATYPE = FALSE;
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

CStillSampleGrabberCallback2::~CStillSampleGrabberCallback2(void)
	{
	if (m_bIsGetAMMEDIATYPE)
		{
		FreeMediaType(m_mtStillMediaType);	
		}

	ReleaseCusBuffer();
	}

AM_MEDIA_TYPE * CStillSampleGrabberCallback2::GetStillMediaType()
	{
	m_bIsGetAMMEDIATYPE = TRUE;

	return &m_mtStillMediaType;
	}

void CStillSampleGrabberCallback2::ReleaseCusBuffer()
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

BOOL CStillSampleGrabberCallback2::ImageConvert()
	{
	ASSERT(NULL != m_CusBuffer.pBuffer0);
	if (NULL == m_CusBuffer.pBuffer0)
		{
		return FALSE;
		}

	BYTE *pBuffer = m_CusBuffer.pBuffer0;   m_CusBuffer.pBuffer0 = NULL;
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
				SAFEDELETEARRAY(pBuffer);
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
				{
				SAFEDELETEARRAY(pBuffer);
				return FALSE;
				}
				
			CFormatConvertArithmetic::I420ToRGB24(pBuffer, pImage, lwidth, lheight);
			lBitCount = 24;	
			}
		// MEDIASUBTYPE_RGB24 or MEDIASUBTYPE_RGB32
		else				
			{
			lsize	= BufferLen;
			pImage = new BYTE[lsize];
			if (NULL == pImage)
			{
			SAFEDELETEARRAY(pBuffer);
			return FALSE;
			}
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
		g_lpStillBits = pImage;		
		//		g_fhdr = hdr;
		g_Stillihdr = bih;			

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
			SAFEDELETEARRAY(pBuffer);
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
		//		g_fhdr = *((BITMAPFILEHEADER *)pbyteBmp);


		// 获得BITMAPINFOHEADER			
		g_Stillihdr = *((BITMAPINFOHEADER *)(pbyteBmp+sizeof(BITMAPFILEHEADER)));


		// 获得除BITMAPFILEHEADER, BITMAPINFOHEADER之外的图像数据长度
		long lDestLen = lBmpBufferLen - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);

		pImage = NULL;
		pImage = new BYTE[lDestLen];
		if (NULL == pImage)
			{			
			SAFEDELETEARRAY(pbyteBmp);
			SAFEDELETEARRAY(pBuffer);
			return FALSE;
			}

		// 获得图像数据
		memcpy(pImage, pbyteBmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER), lDestLen);

		g_lpStillBits = pImage;

		// 释放临时转换Buffer
		SAFEDELETEARRAY(pbyteBmp);
		}	

	SAFEDELETEARRAY(pBuffer);

	return TRUE;
	}

BOOL CStillSampleGrabberCallback2::ImageConvert_5M()
	{	
	ASSERT(NULL != m_CusBuffer.pBuffer0);
	if (NULL == m_CusBuffer.pBuffer0)
		{
		return FALSE;
		}

	BYTE *pAllBuffer = m_CusBuffer.pBuffer0;	m_CusBuffer.pBuffer0 = NULL;
	long lwidth = m_CusBuffer.sizeBuf0.cx * 2;
	long lheight = m_CusBuffer.sizeBuf0.cy;	
	long lAllBufferLen = lwidth * lheight;

#if MySaveRawDataToFile
	static int siCounter = 0;
	CString strTmp;	  	

	siCounter++;
	strTmp.Format(_T("%s\\5M-%d.raw"), gstrRawFileSavePath, siCounter);	

	CFile cfWrite;
	cfWrite.Open(strTmp, CFile::modeCreate | CFile::modeWrite);
	cfWrite.Write(pAllBuffer, lAllBufferLen);
	cfWrite.Close();
#endif

	// Raw Process
	// +++++
	BYTE *pImage = NULL; 
	long lsize = lwidth * lheight * 3;		

	pImage = new BYTE[lsize];
	if (NULL == pImage)
		{
		SAFEDELETEARRAY(pAllBuffer);
		return FALSE;
		} 	

	SAFEDELETEARRAY(gPars.pbSrc);
	SAFEDELETEARRAY(gPars.pbDst);
	SAFEDELETEARRAY(gPars.pbY);
	SAFEDELETEARRAY(gPars.pbU);
	SAFEDELETEARRAY(gPars.pbV);
	SAFEDELETEARRAY(gPars.pbYUY2);

	gPars.uiWid = lwidth;
	gPars.uiHei = lheight;
	long lSizeY = gPars.uiWid * gPars.uiHei;
	long lSizeU = lSizeY / 2;
	long lSizeYUY2 = lSizeY * 2;

	gPars.pbSrc = pAllBuffer;	 pAllBuffer = NULL;
	gPars.pbDst = pImage;		 pImage = NULL;
	gPars.pbY = new BYTE[lSizeY];
	gPars.pbU = new BYTE[lSizeU];
	gPars.pbV = new BYTE[lSizeU];
	gPars.pbYUY2 = new BYTE[lSizeYUY2];
	if (NULL == gPars.pbY
		|| NULL == gPars.pbU
		|| NULL == gPars.pbV
		|| NULL == gPars.pbYUY2)
		{
		SAFEDELETEARRAY(gPars.pbSrc);
		SAFEDELETEARRAY(gPars.pbDst);
		SAFEDELETEARRAY(gPars.pbY);
		SAFEDELETEARRAY(gPars.pbU);
		SAFEDELETEARRAY(gPars.pbV);
		SAFEDELETEARRAY(gPars.pbYUY2);
		return FALSE;
		} 	
		  	
	gRawUtil.RawProcess(&gPars);

	SAFEDELETEARRAY(gPars.pbSrc);
	SAFEDELETEARRAY(gPars.pbY);
	SAFEDELETEARRAY(gPars.pbU);
	SAFEDELETEARRAY(gPars.pbV);
	SAFEDELETEARRAY(gPars.pbYUY2);
	pImage = gPars.pbDst; gPars.pbDst = NULL;
	// -----

	BITMAPINFOHEADER bih; 
	ZeroMemory(&bih, sizeof(bih));
	bih.biSize = sizeof(BITMAPINFOHEADER); 
	bih.biWidth = (LONG)lwidth; 
	bih.biHeight = (LONG)lheight; 		
	bih.biBitCount = 24;
	bih.biPlanes = 1;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = 0;		

	// 初始化Bitmap		
	g_lpStillBits = pImage;	 
	g_Stillihdr = bih;

	return TRUE; 
	}

BOOL CStillSampleGrabberCallback2::ImageConvert_9M()
	{ 
	ASSERT(NULL != m_CusBuffer.pBuffer0);
	ASSERT(NULL != m_CusBuffer.pBuffer1);
	if (NULL == m_CusBuffer.pBuffer0 || NULL == m_CusBuffer.pBuffer1)
		{
		SAFEDELETEARRAY(m_CusBuffer.pBuffer0);
		SAFEDELETEARRAY(m_CusBuffer.pBuffer1);
		return FALSE;
		}

	// +++++
	BYTE *pAllBuffer = NULL;	
	long lwidth = m_CusBuffer.sizeBuf0.cx * 2;
	long lheight = m_CusBuffer.sizeBuf0.cy * 2;
	long lAllBufferLen = lwidth * lheight;
	pAllBuffer = new BYTE[lAllBufferLen];
	if (NULL == pAllBuffer)
		{
		SAFEDELETEARRAY(m_CusBuffer.pBuffer0);
		SAFEDELETEARRAY(m_CusBuffer.pBuffer1);
		return FALSE;
		}

#if MySaveHalf9MRawDataToFile
	static int siCounter2 = 0;	
	CString strTmp2; 	

	siCounter2++;
	strTmp2.Format(_T("C:\\Half_9M-%d.raw"), siCounter2);

	CFile cfWrite2;
	cfWrite2.Open(strTmp2, CFile::modeCreate | CFile::modeWrite);
	cfWrite2.Write(m_CusBuffer.pBuffer0, lAllBufferLen/2);
	cfWrite2.Close();

	static int siCounter3 = 0;
	CString strTmp3;

	siCounter3++;
	strTmp3.Format(_T("C:\\Half2_9M-%d.raw"), siCounter3);

	CFile cfWrite3;
	cfWrite2.Open(strTmp3, CFile::modeCreate | CFile::modeWrite);
	cfWrite2.Write(m_CusBuffer.pBuffer1, lAllBufferLen/2);
	cfWrite2.Close();
#endif

	RGB24_GeomMirV(m_CusBuffer.pBuffer1, lheight/2, lwidth);
	memcpy(pAllBuffer, m_CusBuffer.pBuffer0, m_CusBuffer.ulBufLen0);	
	memcpy(pAllBuffer+m_CusBuffer.ulBufLen0, m_CusBuffer.pBuffer1, m_CusBuffer.ulBufLen1);
	SAFEDELETEARRAY(m_CusBuffer.pBuffer0);
	SAFEDELETEARRAY(m_CusBuffer.pBuffer1);
	// -----  

#if MySaveRawDataToFile
	static int siCounter = 0;
	CString strTmp;

	siCounter++;
	strTmp.Format(_T("%s\\9M-%d.raw"), gstrRawFileSavePath, siCounter);	

	CFile cfWrite;
	cfWrite.Open(strTmp, CFile::modeCreate | CFile::modeWrite);
	cfWrite.Write(pAllBuffer, lAllBufferLen);
	cfWrite.Close();
#endif

	// Raw Process
	// +++++
	BYTE *pImage = NULL;
	long lsize = lwidth * lheight * 3;	 	

	pImage = new BYTE[lsize];
	if (NULL == pImage)
		{
		SAFEDELETEARRAY(pAllBuffer);
		return FALSE;
		}	

	SAFEDELETEARRAY(gPars.pbSrc);
	SAFEDELETEARRAY(gPars.pbDst);
	SAFEDELETEARRAY(gPars.pbY);
	SAFEDELETEARRAY(gPars.pbU);
	SAFEDELETEARRAY(gPars.pbV);
	SAFEDELETEARRAY(gPars.pbYUY2); 

	gPars.uiWid = lwidth;
	gPars.uiHei = lheight;
	long lSizeY = gPars.uiWid * gPars.uiHei;
	long lSizeU = lSizeY / 2;
	long lSizeYUY2 = lSizeY * 2;

	gPars.pbSrc = pAllBuffer;	 pAllBuffer = NULL;
	gPars.pbDst = pImage;		 pImage = NULL;
	gPars.pbY = new BYTE[lSizeY];
	gPars.pbU = new BYTE[lSizeU];
	gPars.pbV = new BYTE[lSizeU];
	gPars.pbYUY2 = new BYTE[lSizeYUY2];
	if (NULL == gPars.pbY
		|| NULL == gPars.pbU
		|| NULL == gPars.pbV
		|| NULL == gPars.pbYUY2)
	{
	SAFEDELETEARRAY(gPars.pbSrc);
	SAFEDELETEARRAY(gPars.pbDst);
	SAFEDELETEARRAY(gPars.pbY);
	SAFEDELETEARRAY(gPars.pbU);
	SAFEDELETEARRAY(gPars.pbV);
	SAFEDELETEARRAY(gPars.pbYUY2);
	return FALSE;
	}

	gRawUtil.RawProcess(&gPars);	

	SAFEDELETEARRAY(gPars.pbSrc);
	SAFEDELETEARRAY(gPars.pbY);
	SAFEDELETEARRAY(gPars.pbU);
	SAFEDELETEARRAY(gPars.pbV);
	SAFEDELETEARRAY(gPars.pbYUY2);
	pImage = gPars.pbDst; gPars.pbDst = NULL;
	// -----	

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
	g_lpStillBits = pImage;	 
	g_Stillihdr = bih;	

	return TRUE;
	}

void CStillSampleGrabberCallback2::RGB24_GeomMirV( BYTE *pImage, const UINT iHeigh, const UINT iWidth )
	{
	RGB24_GeomMir(pImage, iHeigh, iWidth, FALSE);
	}


void CStillSampleGrabberCallback2::RGB24_GeomMirH( BYTE *pImage, const UINT iHeigh, const UINT iWidth )
	{
	RGB24_GeomMir((RGBPixel *)pImage, iHeigh, iWidth, TRUE);
	}

void CStillSampleGrabberCallback2::RGB24_GeomMir( RGBPixel *pImage, const UINT iHeigh, const UINT iWidth, BOOL bDiretion )
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

	SAFEDELETEARRAY(pLine);
	}

void CStillSampleGrabberCallback2::RGB24_GeomMir( BYTE *pImage, const UINT iHeigh, const UINT iWidth, BOOL bDiretion )
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

			memcpy(pLine, pDst, sizeof(BYTE)*iWidth);

			memcpy(pDst, pSrc, sizeof(BYTE)*iWidth);

			memcpy(pSrc, pLine, sizeof(BYTE)*iWidth);
			}

		}

	SAFEDELETEARRAY(pLine);
	}
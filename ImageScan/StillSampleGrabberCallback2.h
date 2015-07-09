#pragma once

//#include <qedit.h>
#include <streams.h>
#include "FormatConvertArithmetic.h"
#include "PicConver.h"
#include "RawPars.h"
#include "RawUtil.h"  
#include "RAW8ToRGB24.h"
#include "replaceqedit.h"


extern BYTE* g_lpStillBits;
extern BITMAPINFOHEADER g_Stillihdr;
extern HWND ghwndApp;  
extern CRawPars gPars;
extern CRawUtil gRawUtil;
extern CUSTOMDEVICETYPE g_cusDevType;

#if MySaveRawDataToFile
extern CString gstrRawFileSavePath;
#endif

class CStillSampleGrabberCallback2 :
	public ISampleGrabberCB
	{
	public:
		CStillSampleGrabberCallback2(void);
		~CStillSampleGrabberCallback2(void);

	private:
		AM_MEDIA_TYPE m_mtStillMediaType;	
		CFormatConvertArithmetic m_FmtConvAtc;
		BOOL m_bIsGetAMMEDIATYPE;		
		CUSBUFFER m_CusBuffer;

	private:
		void ReleaseCusBuffer();
		BOOL ImageConvert();
		BOOL ImageConvert_9M();
		BOOL ImageConvert_5M();
		void RGB24_GeomMirV( BYTE *pImage, const UINT iHeigh, const UINT iWidth );
		void RGB24_GeomMirH( BYTE *pImage, const UINT iHeigh, const UINT iWidth );
		void RGB24_GeomMir( RGBPixel *pImage, const UINT iHeigh, const UINT iWidth, BOOL bDiretion );
		void RGB24_GeomMir( BYTE *pImage, const UINT iHeigh, const UINT iWidth, BOOL bDiretion );

	public:		
		AM_MEDIA_TYPE * GetStillMediaType(void);
		// Fake referance counting.
		STDMETHODIMP_(ULONG) AddRef() { return 1; }
		STDMETHODIMP_(ULONG) Release() { return 2; }
		STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
			{
			if (NULL == ppvObject) return E_POINTER;
			if (riid == __uuidof(IUnknown))
				{
				*ppvObject = static_cast<IUnknown*>(this);
				return S_OK;
				}
			if (riid == __uuidof(ISampleGrabberCB))
				{
				*ppvObject = static_cast<ISampleGrabberCB*>(this);
				return S_OK;
				}
			return E_NOTIMPL;
			}
		STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
			{
			return E_NOTIMPL;
			}
		STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen)
			{		
			if ((m_mtStillMediaType.majortype != MEDIATYPE_Video) ||
				(m_mtStillMediaType.formattype != FORMAT_VideoInfo) ||
				(m_mtStillMediaType.cbFormat < sizeof(VIDEOINFOHEADER)) ||
				(m_mtStillMediaType.pbFormat == NULL))
				{
				return VFW_E_INVALIDMEDIATYPE;
				}		

			VIDEOINFOHEADER* pVih = NULL;
			if((m_mtStillMediaType.formattype == FORMAT_VideoInfo)&&
				(m_mtStillMediaType.cbFormat >= sizeof(VIDEOINFOHEADER))&&(m_mtStillMediaType.pbFormat != NULL))
				{
				pVih = (VIDEOINFOHEADER*)m_mtStillMediaType.pbFormat;
				}
			else
				{
				FreeMediaType(m_mtStillMediaType);
				return FALSE;
				}	

			// 释放上一幅照片资源
			SAFEDELETEARRAY(g_lpStillBits);

			long lwidth = pVih->bmiHeader.biWidth;
			long lheight = pVih->bmiHeader.biHeight;

			BOOL bFS_900Dev = (DEVTYPE_FS_900 == g_cusDevType) ? TRUE : FALSE;	
			if ((DEVTYPE_FS_900 == g_cusDevType)
				|| (DEVTYPE_FS_500 == g_cusDevType)
				|| (DEVTYPE_FS_300 == g_cusDevType))
				{
				// haifeng_liu 2010/11/12 add
				if (((lwidth*2)*lheight) != BufferLen)
					{
					AfxMessageBox(_T("Error, please switch video device driver to \"Microsoft Default Driver\"."), MB_OK | MB_ICONERROR);
					return FALSE;
					}
				}

			if (!bFS_900Dev)
				{ 
				BYTE *pTmp = NULL;
				pTmp = new BYTE[BufferLen];
				if (NULL == pTmp)
					{
					return FALSE;
					}
				memcpy(pTmp, pBuffer, BufferLen);
				ASSERT(NULL == m_CusBuffer.pBuffer0);
				ASSERT(NULL == m_CusBuffer.pBuffer1);
				m_CusBuffer.pBuffer0 = pTmp;
				m_CusBuffer.sizeBuf0.cx = lwidth;
				m_CusBuffer.sizeBuf0.cy = lheight;
				m_CusBuffer.ulBufLen0 = BufferLen;
				}
			else
				{
				BYTE *pTmp = NULL;
				pTmp = new BYTE[BufferLen];
				if (NULL == pTmp)
					{
					return FALSE;
					}
				memcpy(pTmp, pBuffer, BufferLen);
				if (NULL == m_CusBuffer.pBuffer0)
					{
					ASSERT(NULL == m_CusBuffer.pBuffer0);
					ASSERT(NULL == m_CusBuffer.pBuffer1);
					m_CusBuffer.pBuffer0 = pTmp;
					m_CusBuffer.sizeBuf0.cx = lwidth;
					m_CusBuffer.sizeBuf0.cy = lheight;
					m_CusBuffer.ulBufLen0 = BufferLen;
					}
				else
					{
					ASSERT(NULL == m_CusBuffer.pBuffer1);
					m_CusBuffer.pBuffer1 = pTmp;
					m_CusBuffer.sizeBuf1.cx = lwidth;
					m_CusBuffer.sizeBuf1.cy = lheight;
					m_CusBuffer.ulBufLen1 = BufferLen;
					}
				}

			// 900M 照片(900M照片Buffer设备端需要上传2次Buffer才能合成一幅照片(并且
			// 上传的第2个Buffer是垂直镜像的, 在做合成前必须将其翻转回来)
			if (bFS_900Dev)
				{
				if ( (NULL != m_CusBuffer.pBuffer0) && (NULL != m_CusBuffer.pBuffer1))
					{
					if (ImageConvert_9M())
						{
						SendMessage(ghwndApp, WM_GETSTILLBUFFER, 0, 0);	
						}
					ReleaseCusBuffer();
					}
				}
			// 其他大小照片
			else
				{  
				if (NULL != m_CusBuffer.pBuffer0)
					{
					if (DEVTYPE_FS_500 == g_cusDevType)
						{
						if (ImageConvert_5M())
							{
							SendMessage(ghwndApp, WM_GETSTILLBUFFER, 0, 0);	
							} 
						}
					else
						{
						if (ImageConvert())
							{
							SendMessage(ghwndApp, WM_GETSTILLBUFFER, 0, 0);	
							}
						}
					ReleaseCusBuffer();					
					}
				}

			return S_OK;
			}
	};

// SampleGrabberCallback.h: interface for the CSampleGrabberCallback2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEGRABBERCALLBACK2_H__A67961C2_B91A_4E69_B53F_647700CB9530__INCLUDED_)
#define AFX_SAMPLEGRABBERCALLBACK2_H__A67961C2_B91A_4E69_B53F_647700CB9530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <qedit.h>
#include "FormatConvertArithmetic.h"

extern BYTE* g_lpBits;
extern BITMAPFILEHEADER g_fhdr;
extern BITMAPINFOHEADER g_ihdr;
extern HWND ghwndApp;
extern CString g_strPictureFromWhichColorSpaceOrCompresor;

static int intTmp = 0;

class CSampleGrabberCallback2 : public ISampleGrabberCB  
	{
	private:
		AM_MEDIA_TYPE m_mtStillMediaType;	
		CFormatConvertArithmetic m_FmtConvAtc;
		BOOL m_bIsNeedSecondBuffer;
		CUSBUFFER m_CusBuffer;

	private:
		void ReleaseCusBuffer();
		BOOL ImageConvert();
		BOOL ImageConvert_900M();
		void RGB24_GeomMirV( BYTE *pImage, const UINT iHeigh, const UINT iWidth );
		void RGB24_GeomMirH( BYTE *pImage, const UINT iHeigh, const UINT iWidth );
		void RGB24_GeomMir( RGBPixel *pImage, const UINT iHeigh, const UINT iWidth, BOOL bDiretion );
		void RGB24_GeomMir( BYTE *pImage, const UINT iHeigh, const UINT iWidth, BOOL bDiretion );
	public:
		CSampleGrabberCallback2();
		virtual ~CSampleGrabberCallback2();	

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

			intTmp++;
			if (3 == intTmp)
			{
			intTmp = 0;
			return S_OK;
			}

			VIDEOINFOHEADER* pVih;
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

			SAFEDELETEARRAY(g_lpBits);

			long lwidth = 0, lheight = 0;
			lwidth = pVih->bmiHeader.biWidth;
			lheight = pVih->bmiHeader.biHeight;			

			// +++++
			if (_900M_SIZE_WID/2 == lwidth && _900M_SIZE_HEI/2 == lheight)
				{
				m_CusBuffer.bIsTwoBuffer = TRUE;
				}
			else
				{
				m_CusBuffer.bIsTwoBuffer = FALSE;
				}
			// -----

			if (!m_CusBuffer.bIsTwoBuffer)
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
					m_CusBuffer.sizeBuf0.cx = lwidth * 2;
					m_CusBuffer.sizeBuf0.cy = lheight * 2;
					m_CusBuffer.ulBufLen0 = BufferLen;
					}
				else
					{
					ASSERT(NULL == m_CusBuffer.pBuffer1);
					m_CusBuffer.pBuffer1 = pTmp;
					m_CusBuffer.sizeBuf1.cx = lwidth * 2;
					m_CusBuffer.sizeBuf1.cy = lheight * 2;
					m_CusBuffer.ulBufLen1 = BufferLen;
					}
				}

			// 900M 照片(900M照片Buffer设备端需要上传2次Buffer才能合成一幅照片(并且
			// 上传的第2个Buffer是垂直镜像的, 在做合成前必须将其翻转回来)
			if (m_CusBuffer.bIsTwoBuffer)
				{
				if ( (NULL != m_CusBuffer.pBuffer0) && (NULL != m_CusBuffer.pBuffer1))
					{
					if (ImageConvert_900M())
						{
						SendMessage(ghwndApp, WM_GETBUFFER, 0, (LPARAM)&(m_mtStillMediaType.subtype));	
						}
					ReleaseCusBuffer();
					}
				}
			// 其他大小照片
			else
				{
				if (NULL != m_CusBuffer.pBuffer0)
					{
					if (ImageConvert())
						{
						SendMessage(ghwndApp, WM_GETBUFFER, 0, (LPARAM)&(m_mtStillMediaType.subtype));	
						}
					ReleaseCusBuffer();					
					}
				}

			return S_OK;
			}		
	};


#endif // !defined(AFX_SAMPLEGRABBERCALLBACK2_H__A67961C2_B91A_4E69_B53F_647700CB9530__INCLUDED_)

// SampleGrabberCallback.h: interface for the CSampleGrabberCallback class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEGRABBERCALLBACK_H__A67961C2_B91A_4E69_B53F_647700CB9530__INCLUDED_)
#define AFX_SAMPLEGRABBERCALLBACK_H__A67961C2_B91A_4E69_B53F_647700CB9530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "replaceqedit.h"
#include <streams.h>
#include "FormatConvertArithmetic.h"
#include "PicConver.h"

extern CEvent*			g_event;	 
extern long				g_lBufferLen;
extern BYTE*			g_pBuffer;
extern AM_MEDIA_TYPE*	g_pmt;
extern BOOL				g_bIsProcFinished;

class CSampleGrabberCallback : public ISampleGrabberCB  
{
private:
	AM_MEDIA_TYPE m_mtStillMediaType;	
	CFormatConvertArithmetic m_FmtConvAtc;
	BOOL m_bIsGetAMMEDIATYPE;

public:
	CSampleGrabberCallback();
	virtual ~CSampleGrabberCallback();

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

		if (g_bIsProcFinished)
		{
			g_lBufferLen = BufferLen;
			g_pBuffer = pBuffer;
			g_pmt = &m_mtStillMediaType;		

			g_event->SetEvent();	
		}			

		return S_OK;
	}
	
};

#endif // !defined(AFX_SAMPLEGRABBERCALLBACK_H__A67961C2_B91A_4E69_B53F_647700CB9530__INCLUDED_)

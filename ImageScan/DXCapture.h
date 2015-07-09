// DXCapture.h: interface for the CDXCapture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXCAPTURE_H__445657FA_F82F_4718_924B_F8DB4B7DF3EA__INCLUDED_)
#define AFX_DXCAPTURE_H__445657FA_F82F_4718_924B_F8DB4B7DF3EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000  
#include <DShow.h>
#include "replaceqedit.h"
#include "SampleGrabberCallback.h"	
#include "StillSampleGrabberCallback2.h" 

#define SAFERELEASE(p)	{ if (NULL != p) { p->Release(); p = NULL;} }

class CDXCapture  
{
public:
	CDXCapture();
	virtual ~CDXCapture();	

private:
	IFilterGraph2			*m_pGraph;
	ICaptureGraphBuilder2	*m_pCapBuilder;
	IVideoWindow			*m_pVW;
	IMediaControl			*m_pMC;
	IMediaEventEx			*m_pME;	

	HWND					m_hwnd;

	BOOL					m_bIsUseSampleGrabberFilter;
	BOOL					m_bIsUseNullRenderer;
	BOOL					m_bIsWantStillPinGrabber;

	ISampleGrabber			*m_pSampleGrabber;
	ISampleGrabber			*m_pSampleGrabber_StillPin;

	CSampleGrabberCallback	m_SGC;

	CStillSampleGrabberCallback2 m_StillSGC;

	IBaseFilter				*m_pSrcFilter;

	DWORD					m_dwRegister;

	UINT					m_iMonikerIdx;			// 在查找设备时, 指定要查找的设备Idx  	

private:

	HRESULT GetInterfaces();	
	void InitInterfaces();
	void ReleaseInterfaces();

	HRESULT SetupVideoWindow();
	HRESULT CaptureVideoByMoniker();
	HRESULT FindCatureDeviceByMoniker(IMoniker **ppMoniker);
	HRESULT FindCatureDeviceByMoniker(IMoniker **ppMoniker, UINT iMonikerIdx);
	HRESULT AddCaptureMonikerToGraph(IMoniker *pMoniker);


	BOOL BuildGrabberGraph_Other(IBaseFilter *pSrcFilter);

	BOOL GetSubTypeNameList(const GUID *pCategory, IBaseFilter *pSrcFilter, CStringList& strMediaSubTypeNameList);
	BOOL GetSpecifySubTypeNameSizeList(const GUID *pCategory, IBaseFilter *pSrcFilter, const CString strSubTypeName, CStringList& strSubTypeSizeList);
	BOOL GetSpecifyFormat(const GUID *pCategory, IBaseFilter *pSrcFilter, const CString strSubTypeName, const SIZE size, AM_MEDIA_TYPE **ppmt);

	BOOL GetVideoPinSubTypeNameList(IBaseFilter *pSrcFilter, CStringList& strMediaSubTypeNameList);
	BOOL GetVideoPinSpecifySubTypeNameSizeList(IBaseFilter *pSrcFilter, const CString strSubTypeName, CStringList& strSubTypeSizeList);	
	BOOL GetVideoPinSpecifyFormat(IBaseFilter *pSrcFilter, const CString strSubTypeName, const SIZE size, AM_MEDIA_TYPE **ppmt);	

	BOOL GetStillPinSubTypeNameList(IBaseFilter *pSrcFilter, CStringList& strMediaSubTypeNameList);
	BOOL GetStillPinSpecifySubTypeNameSizeList(IBaseFilter *pSrcFilter, const CString strSubTypeName, CStringList& strSubTypeSizeList);	
	BOOL GetStillPinSpecifyFormat(IBaseFilter *pSrcFilter, const CString strSubTypeName, const SIZE size, AM_MEDIA_TYPE **ppmt);	

	void GetMaxOrMinSizeinSizeList(const CStringList& strSubTypeSizeList, CString& strSubTypeSize, BOOL bIsMax);
	BOOL FindSpecifySizeInSizeList(const CStringList& strSubTypeSizeList, const CString& strSubTypeSize);


	void TranslateGUID(GUID guid, WCHAR *wchColor, size_t numOfElem);

	HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void RemoveGraphFromRot(DWORD pdwRegister);

	BOOL GetCurrentPhotoSize(SIZE& curSize);
	BOOL GetCurrentSize(SIZE& curSize, const BOOL bIsVideo);

	void NukeDownstream(IBaseFilter *pf);
public:

	BOOL StartCapture();
	BOOL PauseCapture();
	BOOL StopCapture();
	void SetRenderHwnd(HWND hwnd);
	void ResizeVideoWindow();
	void ResizeVideoWindow(RECT rc);   // add [4/11/2011 haifeng_liu]
	void UseSampleGrabberFilter();
	void UseNullRenderer();
	void UseStillPinGrabber();
	ISampleGrabber * GetSampleGrabber();
	ISampleGrabber * GetSampleGrabber_StillPin();

	void TearDownGraph();
	void CloseInterfaces();	   

	BOOL StillPinSnapShot();	
	BOOL ShowFilterPinPropertyPage();
	HRESULT BuildPreviewGrpah();

	BOOL IsCurrentPreviewSizeSameWithStillPinPhotoSize();
	BOOL IsSpecifyPinExist(const GUID pIn);
	BOOL IsStillPinExist();
	IBaseFilter* GetSrcFilter();

	void SetMonikerIdx(UINT iMonikerIdx);

	BOOL GetCurrentPreviewSize(SIZE& curSize);
}; 

#endif // !defined(AFX_DXCAPTURE_H__445657FA_F82F_4718_924B_F8DB4B7DF3EA__INCLUDED_)

// DXCapture.cpp: implementation of the CDXCapture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlbase.h>
#include <DShow.h>
#include "DXCapture.h"
#include "SampleGrabberCallback.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if 1
CDXCapture::CDXCapture()
	{
	InitInterfaces();
	}

CDXCapture::~CDXCapture()
	{
	CloseInterfaces();
	}

void CDXCapture::InitInterfaces()
	{
	m_pGraph		= NULL;
	m_pCapBuilder	= NULL;
	m_pMC			= NULL;
	m_pME			= NULL;
	m_pVW			= NULL;	

	m_hwnd			= NULL;

	m_bIsUseSampleGrabberFilter = FALSE;
	m_bIsUseNullRenderer = FALSE;
	m_bIsWantStillPinGrabber = FALSE;	
	m_pSampleGrabber = NULL;
	m_pSampleGrabber_StillPin = NULL;	

	m_pSrcFilter = NULL;

	m_dwRegister = 0;

	m_iMonikerIdx = 0;
	}

void CDXCapture::ReleaseInterfaces()
	{
	SAFERELEASE(m_pGraph);
	SAFERELEASE(m_pCapBuilder);
	SAFERELEASE(m_pMC);
	SAFERELEASE(m_pME);
	SAFERELEASE(m_pVW);

	SAFERELEASE(m_pSampleGrabber);	
	SAFERELEASE(m_pSampleGrabber_StillPin);

	SAFERELEASE(m_pSrcFilter);
	}


HRESULT CDXCapture::GetInterfaces()
	{
	HRESULT hr = S_OK;

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
		IID_IFilterGraph2, (LPVOID *)&m_pGraph);
	if (FAILED(hr))
		{
		return hr;
		}

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2, (LPVOID *)&m_pCapBuilder);
	if (FAILED(hr))
		{
		return hr;
		}

	hr = m_pGraph->QueryInterface(IID_IMediaControl, (LPVOID *)&m_pMC);
	if (FAILED(hr))
		{
		return hr;
		}

	hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID *)&m_pME);
	if (FAILED(hr))
		{
		return hr;
		}

	hr = m_pGraph->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVW);
	if (FAILED(hr))
		{
		return hr;
		}
	
	return hr;
	}

void CDXCapture::CloseInterfaces()
	{
	TearDownGraph();

	ReleaseInterfaces();	
	}

HRESULT CDXCapture::CaptureVideoByMoniker()
	{
	HRESULT hr = S_OK;
	IMoniker *pMoniker = NULL;

	hr = GetInterfaces();
	if (FAILED(hr))
		{
		return hr;
		}

	hr = FindCatureDeviceByMoniker(&pMoniker, m_iMonikerIdx);
	if (FAILED(hr))
		{
		return hr;
		}

	hr = AddCaptureMonikerToGraph(pMoniker);
	if (FAILED(hr))
		{
		return hr;
		}	

	SAFERELEASE(pMoniker);

	return hr;
	}

// 默认获取设备列表中的第一个设备
HRESULT CDXCapture::FindCatureDeviceByMoniker(IMoniker **ppMoniker)
	{
	HRESULT hr = S_OK;
	ULONG cFetched;

	if (!ppMoniker)
		{
		return E_POINTER;
		}

	CComPtr <ICreateDevEnum> pDevEnum = NULL;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (LPVOID *)&pDevEnum);
	if (FAILED(hr))
		{
		return hr;
		}

	CComPtr <IEnumMoniker> pClassEnum = NULL;

	// 	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, NULL, CLSCTX_INPROC_SERVER,
	// 				IID_IEnumMoniker, &pClassEnum, 0);
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
	// haifeng_liu 2010/9/21 modify
	/*if (FAILED(hr))*/
	if (FAILED(hr) || NULL == pClassEnum)	
		{
		/*return hr;*/
		return E_FAIL;
		}

	if (S_OK == pClassEnum->Next(1, ppMoniker, &cFetched))
		{
		return S_OK;
		}
	else
		{
		return E_FAIL;
		}


	return hr;
	}

// 获取设备列表中由"iMonikerIdx"指定的设备
HRESULT CDXCapture::FindCatureDeviceByMoniker(IMoniker **ppMoniker, UINT iMonikerIdx)
	{
	HRESULT hr = S_OK;
	ULONG cFetched;

	if (!ppMoniker)
		{
		return E_POINTER;
		}

	CComPtr <ICreateDevEnum> pDevEnum = NULL;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (LPVOID *)&pDevEnum);
	if (FAILED(hr))
		{
		return hr;
		}

	CComPtr <IEnumMoniker> pClassEnum = NULL;

	// 	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, NULL, CLSCTX_INPROC_SERVER,
	// 				IID_IEnumMoniker, &pClassEnum, 0);
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
	// haifeng_liu 2010/9/21 modify
	/*if (FAILED(hr))*/
	if (FAILED(hr) || NULL == pClassEnum)	
		{
		/*return hr;*/
		return E_FAIL;
		}

	do
		{
		hr = pClassEnum->Next(1, ppMoniker, &cFetched);	 		
		} while (iMonikerIdx--);

	return hr;
	}


HRESULT CDXCapture::AddCaptureMonikerToGraph(IMoniker *pMoniker)
	{
	HRESULT hr = S_OK;	

	LPOLESTR strMonikerName = 0;
	hr = pMoniker->GetDisplayName(NULL, NULL, &strMonikerName);
	if (FAILED(hr))
		{
		return hr;
		}

	IBindCtx *pBindCtx = NULL;
	hr = CreateBindCtx(0, &pBindCtx);
	if (FAILED(hr))
		{
		return hr;
		}

	hr = m_pGraph->AddSourceFilterForMoniker(pMoniker, pBindCtx, strMonikerName, &m_pSrcFilter);
	if (FAILED(hr))
		{
		return hr;
		}

	hr = m_pCapBuilder->SetFiltergraph(m_pGraph);
	if (FAILED(hr))
		{
		return hr;
		}

	// 对于视频预览, 查询是否存在"VGA"格式, 如果存在以"VGA"格式连接;
	// 否则, 选择最小的"Size"格式进行连接
	// +++++
	CStringList strSubTypeNameList;
	CStringList strSubTypeSizeList;
	CString strSubTypeName;
	CString strSubTypeSize;
	AM_MEDIA_TYPE *pmt;

	IAMStreamConfig * pConfig = NULL;
	hr = m_pCapBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, 
		m_pSrcFilter, IID_IAMStreamConfig, (void **)&pConfig);	
	if (SUCCEEDED(hr))
		{
		VERIFY(GetVideoPinSubTypeNameList(m_pSrcFilter, strSubTypeNameList));

		// 选择查询到的第一个"SubType"
		POSITION headPos = strSubTypeNameList.GetHeadPosition();
		strSubTypeName = strSubTypeNameList.GetNext(headPos);			

		VERIFY(GetVideoPinSpecifySubTypeNameSizeList(m_pSrcFilter, strSubTypeName, strSubTypeSizeList));

		CString strSizeVGA;
		strSizeVGA.Format(_T("%d x %d"), 640, 480);
		if (FindSpecifySizeInSizeList(strSubTypeSizeList, strSizeVGA))
			{
			strSubTypeSize = strSizeVGA;
			}
		else
			{
			GetMaxOrMinSizeinSizeList(strSubTypeSizeList, strSubTypeSize, FALSE);
			}		

		// +++++
		CString strTmp;
		SIZE size;

		strTmp = strSubTypeSize.Left(strSubTypeSize.Find(_T('x')));
		strTmp.Trim();
		size.cx = _tstoi(strTmp);

		strTmp = strSubTypeSize.Right(strSubTypeSize.GetLength() - (strSubTypeSize.Find(_T('x'))) - 1);
		strTmp.Trim();
		size.cy = _tstoi(strTmp);	
		// -----

		VERIFY(GetVideoPinSpecifyFormat(m_pSrcFilter, strSubTypeName, size, &pmt));

		hr = pConfig->SetFormat(pmt);
		ASSERT(SUCCEEDED(hr));

		if (NULL != pmt)
			{
			DeleteMediaType(pmt);
			}

		SAFERELEASE(pConfig);
		}
	// -----


	// haifeng_liu 2010/8/8 add
	// +++++
	IBaseFilter *pMidFilter = NULL;			
	IBaseFilter *pRendererFilter = NULL;

	if (m_bIsUseSampleGrabberFilter)
		{
		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (LPVOID *)&pMidFilter);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			{
			return hr;
			}

		hr = m_pGraph->AddFilter(pMidFilter, L"VideoSampleGrabber");
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			{
			return hr;
			}
		}

	if (m_bIsUseNullRenderer)
		{
		hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (LPVOID *)&pRendererFilter);
		if (FAILED(hr))
			{
			return hr;
			}
		hr = m_pGraph->AddFilter(pRendererFilter, L"NullRenderer");
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			{
			return hr;
			}
		}	

	hr = m_pCapBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pSrcFilter, pMidFilter, pRendererFilter);
	if (FAILED(hr))
		{
		return hr;
		}

	if (m_bIsUseSampleGrabberFilter)
		{
		hr = pMidFilter->QueryInterface(IID_ISampleGrabber, (void **)&m_pSampleGrabber);
		hr = m_pSampleGrabber->SetOneShot(FALSE);
		hr = m_pSampleGrabber->SetBufferSamples(FALSE);
		hr = m_pSampleGrabber->SetCallback(&m_SGC, 1);
		hr = m_pSampleGrabber->GetConnectedMediaType(m_SGC.GetStillMediaType());
		ASSERT(SUCCEEDED(hr));
		}


	if (m_bIsWantStillPinGrabber)
		{
		BuildGrabberGraph_Other(m_pSrcFilter);
		}
	// -----	

	hr = AddGraphToRot(m_pGraph, &m_dwRegister);
	if(FAILED(hr))
		{		
		m_dwRegister = 0;
		}


	SAFERELEASE(pBindCtx);
	SAFERELEASE(pMidFilter);
	SAFERELEASE(pRendererFilter);

	return hr;
	}

BOOL CDXCapture::StartCapture()
	{
	HRESULT hr;
	hr = CaptureVideoByMoniker();
	if (FAILED(hr))
		{
		return FALSE;
		}

	hr = SetupVideoWindow();
	if (FAILED(hr))
		{
		return FALSE;
		}

	if (m_pMC)
		{
		hr = m_pMC->Run();
		}
	else
		{
		return FALSE;
		}

	return TRUE;
	}

BOOL CDXCapture::PauseCapture()
	{
	if (m_pMC)
		{
		m_pMC->Pause();
		}
	else
		{
		return FALSE;
		}

	return TRUE;
	}

BOOL CDXCapture::StopCapture()
	{
	if (m_pMC)
		{
		m_pMC->Stop();
		}
	else
		{
		return FALSE;
		}

	return TRUE;
	}


void CDXCapture::SetRenderHwnd(HWND hwnd)
	{
	m_hwnd = hwnd;
	}

HRESULT CDXCapture::SetupVideoWindow()
	{
	HRESULT hr = S_OK;

	if (NULL != m_hwnd)
		{
		if (m_pVW)
			{
			hr = m_pVW->put_Owner((OAHWND)m_hwnd);
			if (FAILED(hr))
				{
				return hr;
				}

			m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);

			ResizeVideoWindow();

			m_pVW->put_Visible(OATRUE);

			}
		}

	return hr;

	}

void CDXCapture::ResizeVideoWindow()
	{
	if (m_pVW)
		{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		m_pVW->SetWindowPosition(rc.left, rc.top, (rc.right-rc.left), (rc.bottom-rc.top));
		}
	}

void CDXCapture::ResizeVideoWindow(RECT rc)
{
	if (m_pVW)
	{
		m_pVW->SetWindowPosition(rc.left, rc.top, (rc.right-rc.left), (rc.bottom-rc.top));
		}
	}

void CDXCapture::UseSampleGrabberFilter()
	{
	m_bIsUseSampleGrabberFilter = TRUE;
	}



ISampleGrabber * CDXCapture::GetSampleGrabber()
	{
	return m_pSampleGrabber;
	}

void CDXCapture::UseNullRenderer()
	{
	m_bIsUseNullRenderer = TRUE;
	}


BOOL CDXCapture::BuildGrabberGraph_Other(IBaseFilter *pSrcFilter)
	{
	// 添加带stillpin照相功能	
	// Add the Sample Grabber filter to the graph
	HRESULT hr;
	IPin *pPin = NULL;
	BOOL bRet = FALSE;
	hr = m_pCapBuilder->FindPin(pSrcFilter,
		PINDIR_OUTPUT,
		&PIN_CATEGORY_STILL,
		0, FALSE,
		0, &pPin);
	if (SUCCEEDED(hr))
		{
		IBaseFilter *pSG_Filter;
		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&pSG_Filter);
		hr |= m_pGraph->AddFilter(pSG_Filter, L"StillSampleGrabber");

		IBaseFilter *pNull;
		hr |= CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&pNull);
		hr |= m_pGraph->AddFilter(pNull, L"NullReder");

		// 
		// 选择Raw输出格式进行连接, 因为Raw数据是从YUY2通的经过, 所以可以认为
		// YUY2下最大Resolution为Raw输出, 判断最大Resolution仅仅比较"Width"
		//
		// +++++
		CStringList strSubTypeNameList;
		CStringList strSubTypeSizeList;
		CString strSubTypeName;
		CString strSubTypeSize;
		AM_MEDIA_TYPE *pmt;

		IAMStreamConfig * pConfig = NULL;
		hr = m_pCapBuilder->FindInterface(&PIN_CATEGORY_STILL, &MEDIATYPE_Video, 
			pSrcFilter, IID_IAMStreamConfig, (void **)&pConfig);	
		if (SUCCEEDED(hr))
			{
			VERIFY(GetStillPinSubTypeNameList(pSrcFilter, strSubTypeNameList));

			// 
			// 拍照类型下可能有多种SubType(YUY2, RGB24, MJPG...), 先选出YUY2
			//
			// ++++++++++++++++++++++++++++++++++
			BOOL bFindYUY2 = FALSE;
			POSITION headPos = strSubTypeNameList.GetHeadPosition();
			for (int i = 0; i < strSubTypeNameList.GetCount(); i++)
			{
				strSubTypeName = strSubTypeNameList.GetNext(headPos);
				if (0 == strSubTypeName.CompareNoCase(_T("yuy2")))
				{
					bFindYUY2 = TRUE;
					break;
				}
			} 			
			// ----------------------------------

			if (!bFindYUY2)
			{ 				
				POSITION headPos = strSubTypeNameList.GetHeadPosition();
			strSubTypeName = strSubTypeNameList.GetNext(headPos);			
			}

			VERIFY(GetStillPinSpecifySubTypeNameSizeList(pSrcFilter, strSubTypeName, strSubTypeSizeList));

			GetMaxOrMinSizeinSizeList(strSubTypeSizeList, strSubTypeSize, TRUE);

			// +++++
			CString strTmp;
			SIZE size;

			strTmp = strSubTypeSize.Left(strSubTypeSize.Find(_T('x')));
			strTmp.Trim();
			size.cx = _tstoi(strTmp);

			strTmp = strSubTypeSize.Right(strSubTypeSize.GetLength() - (strSubTypeSize.Find(_T('x'))) - 1);
			strTmp.Trim();
			size.cy = _tstoi(strTmp);	
			// -----

			VERIFY(GetStillPinSpecifyFormat(pSrcFilter, strSubTypeName, size, &pmt));

			hr = pConfig->SetFormat(pmt);
			ASSERT(SUCCEEDED(hr));

			if (NULL != pmt)
				{
				DeleteMediaType(pmt);
				}

			SAFERELEASE(pConfig);
			}
		// -----

		if (SUCCEEDED(hr))
			{
			hr = m_pCapBuilder->RenderStream(
				&PIN_CATEGORY_STILL,
				&MEDIATYPE_Video,
				pSrcFilter,
				pSG_Filter,
				pNull);		


			if (SUCCEEDED(hr))
				{				
				hr = pSG_Filter->QueryInterface(IID_ISampleGrabber, (void**)&m_pSampleGrabber_StillPin);
				hr = m_pSampleGrabber_StillPin->SetOneShot(FALSE);
				hr = m_pSampleGrabber_StillPin->SetBufferSamples(TRUE);	
				hr = m_pSampleGrabber_StillPin->SetCallback(&m_StillSGC, 1);
				hr = m_pSampleGrabber_StillPin->GetConnectedMediaType(m_StillSGC.GetStillMediaType());
				ASSERT(SUCCEEDED(hr));			

				bRet = TRUE;
				}			
			}	
		SAFERELEASE(pSG_Filter);
		SAFERELEASE(pNull);
		} 

	SAFERELEASE(pPin);

	return bRet;
	}

ISampleGrabber * CDXCapture::GetSampleGrabber_StillPin()
	{
	return m_pSampleGrabber_StillPin;
	}

void CDXCapture::UseStillPinGrabber()
	{
	m_bIsWantStillPinGrabber = TRUE;
	}
#endif

BOOL CDXCapture::GetSubTypeNameList(const GUID *pCategory, IBaseFilter *pSrcFilter, CStringList& strMediaSubTypeNameList)
	{
	BOOL bRet = FALSE;

	if (NULL == pCategory || NULL == pSrcFilter || NULL == m_pCapBuilder)
		{
		return FALSE;
		}

	IAMStreamConfig *pConfig = NULL;
	HRESULT hr;		

	hr = m_pCapBuilder->FindInterface(pCategory, &MEDIATYPE_Video,
		pSrcFilter, IID_IAMStreamConfig, (void **)&pConfig);
	if (SUCCEEDED(hr))
		{
		int iCount = 0, iSize = 0;

		pConfig->GetNumberOfCapabilities(&iCount, &iSize);
		if (sizeof(VIDEO_STREAM_CONFIG_CAPS) == iSize)
			{
			int iFormatIdx = 0;
			for (iFormatIdx = 0; iFormatIdx < iCount; iFormatIdx++)
				{
				VIDEO_STREAM_CONFIG_CAPS scc;
				AM_MEDIA_TYPE *pmtConfig = NULL;

				hr = pConfig->GetStreamCaps(iFormatIdx, &pmtConfig, (BYTE*)&scc);
				if (SUCCEEDED(hr))
					{
					USES_CONVERSION;
					WCHAR wszSubTypeName[MAX_PATH] = {0};
					TranslateGUID(pmtConfig->subtype, wszSubTypeName, MAX_PATH);												
					CString strMediaSubTypeName(W2T(wszSubTypeName));

					// 防止重复添加相同的SubTypeName
					if (NULL == strMediaSubTypeNameList.Find(strMediaSubTypeName))
						{
						strMediaSubTypeNameList.AddHead(strMediaSubTypeName);
						}

					DeleteMediaType(pmtConfig);
					}
				else
					{						
					break;
					}
				}

			// iFormatIdx == iCount, 表明GetMediaSubTypeNameList过程中没有出错
			// 函数将返回TRUE; 否则, FLASE
			if (iFormatIdx == iCount)
				{
				bRet = TRUE;
				}
			}
		SAFERELEASE(pConfig);
		}		

	return bRet;
	}

BOOL CDXCapture::GetSpecifySubTypeNameSizeList(const GUID *pCategory, IBaseFilter *pSrcFilter, const CString strSubTypeName, CStringList& strSubTypeSizeList)
	{
	BOOL bRet = FALSE;

	if (NULL == pCategory || NULL == pSrcFilter || NULL == m_pCapBuilder)
		{
		return FALSE;
		}

	IAMStreamConfig *pConfig = NULL;
	HRESULT hr;
	CStringList strMediaSubTypeNameList;

	hr = m_pCapBuilder->FindInterface(pCategory, &MEDIATYPE_Video,
		pSrcFilter, IID_IAMStreamConfig, (void **)&pConfig);
	if (SUCCEEDED(hr))
		{
		int iCount = 0, iSize = 0;

		pConfig->GetNumberOfCapabilities(&iCount, &iSize);
		if (sizeof(VIDEO_STREAM_CONFIG_CAPS) == iSize)
			{
			int iFormatIdx = 0;
			for (iFormatIdx = 0; iFormatIdx < iCount; iFormatIdx++)
				{
				VIDEO_STREAM_CONFIG_CAPS scc;
				AM_MEDIA_TYPE *pmtConfig = NULL;

				hr = pConfig->GetStreamCaps(iFormatIdx, &pmtConfig, (BYTE*)&scc);
				if (SUCCEEDED(hr))
					{
					USES_CONVERSION;
					WCHAR wszSubTypeName[MAX_PATH] = {0};
					TranslateGUID(pmtConfig->subtype, wszSubTypeName, MAX_PATH);							
					CString strMediaSubTypeName(W2T(wszSubTypeName));

					if (strSubTypeName == strMediaSubTypeName)
						{							
						CString strSizeTmp;
						strSizeTmp.Format(_T("%d x %d"), scc.MinOutputSize.cx, scc.MinOutputSize.cy);

						// 防止重复添加相同的Size
						if (NULL == strSubTypeSizeList.Find(strSizeTmp))
							{
							// AddHead 添加的Size顺序为:160 x 120, 176 x 144, ..., 640 x 480
							// 现在需要如下顺序: 640 x 480, ... , 176 x 144, 160 x 120, 故使用
							// AddTail
							//strSubTypeSizeList.AddHead(strSizeTmp);
							strSubTypeSizeList.AddTail(strSizeTmp);
							}							
						}

					DeleteMediaType(pmtConfig);
					}
				else
					{						
					break;
					}
				}

			// iFormatIdx == iCount, 表明GetMediaSubTypeNameList过程中没有出错
			// 函数将返回TRUE; 否则, FLASE
			if (iFormatIdx == iCount)
				{
				bRet = TRUE;
				}				
			}
		SAFERELEASE(pConfig);
		}		

	return bRet;	
	}

// 获取StillPin端所支持的所有MediaSubTypes
BOOL CDXCapture::GetVideoPinSubTypeNameList(IBaseFilter *pSrcFilter, CStringList& strMediaSubTypeNameList)
	{	
	return GetSubTypeNameList(&PIN_CATEGORY_CAPTURE, pSrcFilter, strMediaSubTypeNameList);	
	}

// 获取StillPin端, 指定SubType所包含的所有Size
BOOL CDXCapture::GetVideoPinSpecifySubTypeNameSizeList(IBaseFilter *pSrcFilter, const CString strSubTypeName, CStringList& strSubTypeSizeList)
	{
	return GetSpecifySubTypeNameSizeList(&PIN_CATEGORY_CAPTURE, pSrcFilter, strSubTypeName, strSubTypeSizeList);
	}

// 获取StillPin端所支持的所有MediaSubTypes
BOOL CDXCapture::GetStillPinSubTypeNameList(IBaseFilter *pSrcFilter, CStringList& strMediaSubTypeNameList)
	{	
	return GetSubTypeNameList(&PIN_CATEGORY_STILL, pSrcFilter, strMediaSubTypeNameList);	
	}

// 获取StillPin端, 指定SubType所包含的所有Size
BOOL CDXCapture::GetStillPinSpecifySubTypeNameSizeList(IBaseFilter *pSrcFilter, const CString strSubTypeName, CStringList& strSubTypeSizeList)
	{
	return GetSpecifySubTypeNameSizeList(&PIN_CATEGORY_STILL, pSrcFilter, strSubTypeName, strSubTypeSizeList);
	}



BOOL CDXCapture::GetSpecifyFormat(const GUID *pCategory, IBaseFilter *pSrcFilter, const CString strSubTypeName, const SIZE size, AM_MEDIA_TYPE **ppmt)
	{
	BOOL bRet = FALSE;

	if (NULL == pCategory || NULL == pSrcFilter || NULL == ppmt || NULL == m_pCapBuilder)
		{
		return FALSE;
		}

	IAMStreamConfig *pConfig = NULL;
	HRESULT hr;		

	hr = m_pCapBuilder->FindInterface(pCategory, &MEDIATYPE_Video,
		pSrcFilter, IID_IAMStreamConfig, (void **)&pConfig);
	if (SUCCEEDED(hr))
		{
		int iCount = 0, iSize = 0;

		pConfig->GetNumberOfCapabilities(&iCount, &iSize);
		if (sizeof(VIDEO_STREAM_CONFIG_CAPS) == iSize)
			{
			int iFormatIdx = 0;
			for (iFormatIdx = 0; iFormatIdx < iCount; iFormatIdx++)
				{
				VIDEO_STREAM_CONFIG_CAPS scc;
				AM_MEDIA_TYPE *pmtConfig = NULL;

				hr = pConfig->GetStreamCaps(iFormatIdx, &pmtConfig, (BYTE*)&scc);
				if (SUCCEEDED(hr))
					{
					USES_CONVERSION;
					WCHAR wszSubTypeName[MAX_PATH] = {0};
					TranslateGUID(pmtConfig->subtype, wszSubTypeName, MAX_PATH);												
					CString strMediaSubTypeName(W2T(wszSubTypeName));

					if (strMediaSubTypeName == strSubTypeName
						&& size.cx == HEADER(pmtConfig->pbFormat)->biWidth
						&& size.cy == HEADER(pmtConfig->pbFormat)->biHeight)
						{
						// Is it VIDEOINFOHEADER and UYVY?
						if (pmtConfig->formattype == FORMAT_VideoInfo && 
							pmtConfig->subtype == MEDIASUBTYPE_UYVY)
							{
							// Find the smallest output size.
							LONG width = scc.MinOutputSize.cx;
							LONG height = scc.MinOutputSize.cy;
							LONG cbPixel = 2;  // Bytes per pixel in UYVY

							// Modify the format block.
							VIDEOINFOHEADER *pVih = 
								reinterpret_cast<VIDEOINFOHEADER*>(pmtConfig->pbFormat);
							pVih->bmiHeader.biWidth = width;
							pVih->bmiHeader.biHeight = height;

							// Set the sample size and image size.
							// (Round the image width up to a DWORD boundary.)
							pmtConfig->lSampleSize = pVih->bmiHeader.biSizeImage = 
								((width + 3) & ~3) * height * cbPixel; 
							}

						*ppmt = pmtConfig;
						bRet = TRUE;
						break;
						}

					DeleteMediaType(pmtConfig);											
					}					
				}				
			}
		SAFERELEASE(pConfig);
		}		

	return bRet;
	}

BOOL CDXCapture::GetVideoPinSpecifyFormat(IBaseFilter *pSrcFilter, const CString strSubTypeName, const SIZE size, AM_MEDIA_TYPE **ppmt)
	{
	return GetSpecifyFormat(&PIN_CATEGORY_CAPTURE, pSrcFilter, strSubTypeName, size, ppmt);
	}

BOOL CDXCapture::GetStillPinSpecifyFormat(IBaseFilter *pSrcFilter, const CString strSubTypeName, const SIZE size, AM_MEDIA_TYPE **ppmt)
	{
	return GetSpecifyFormat(&PIN_CATEGORY_STILL, pSrcFilter, strSubTypeName, size, ppmt);
	}

void CDXCapture::TranslateGUID(GUID guid, WCHAR *wchColor, size_t numOfElem)
	{
	if (MEDIASUBTYPE_RGB1 == guid)
		wcscpy_s(wchColor, numOfElem, L"RGB1");
	else if(MEDIASUBTYPE_RGB4 == guid)
		wcscpy_s(wchColor, numOfElem, L"RGB4");
	else if(MEDIASUBTYPE_RGB8 == guid)
		wcscpy_s(wchColor, numOfElem, L"RGB8");
	else if(MEDIASUBTYPE_RGB565 == guid)
		wcscpy_s(wchColor, numOfElem, L"RGB565");
	else if(MEDIASUBTYPE_RGB555 == guid)
		wcscpy_s(wchColor, numOfElem, L"RGB555");
	else if(MEDIASUBTYPE_RGB24 == guid)
		wcscpy_s(wchColor, numOfElem, L"RGB24");
	else if(MEDIASUBTYPE_RGB32 == guid)
		wcscpy_s(wchColor, numOfElem, L"RGB32");
	else if(MEDIASUBTYPE_ARGB32 == guid)
		wcscpy_s(wchColor, numOfElem, L"ARGB32");
	else if(MEDIASUBTYPE_YUY2 == guid)
		wcscpy_s(wchColor, numOfElem, L"YUY2");
	else if(MEDIASUBTYPE_YUYV == guid)
		wcscpy_s(wchColor, numOfElem, L"YUYV");
	else if(MEDIASUBTYPE_YVYU == guid)
		wcscpy_s(wchColor, numOfElem, L"YVYU");
	else if(MEDIASUBTYPE_UYVY == guid)
		wcscpy_s(wchColor, numOfElem, L"UYVY");
	else if(MEDIASUBTYPE_Y41P == guid)
		wcscpy_s(wchColor, numOfElem, L"Y41P");
	else if(MEDIASUBTYPE_Y211 == guid)
		wcscpy_s(wchColor, numOfElem, L"Y211");
	else if(MEDIASUBTYPE_IF09 == guid)
		wcscpy_s(wchColor, numOfElem, L"IF09");
	else if(MEDIASUBTYPE_IYUV == guid)
		wcscpy_s(wchColor, numOfElem, L"IYUV");
	else if(MEDIASUBTYPE_YV12 == guid)
		wcscpy_s(wchColor, numOfElem, L"YV12");
	else if(MEDIASUBTYPE_YVU9 == guid)
		wcscpy_s(wchColor, numOfElem, L"YVU9");
	else if(MEDIASUBTYPE_Y411 == guid)
		wcscpy_s(wchColor, numOfElem, L"Y411");
	else if(MEDIASUBTYPE_CLJR == guid)
		wcscpy_s(wchColor, numOfElem, L"CLJR");
	else if(MEDIASUBTYPE_CPLA == guid)
		wcscpy_s(wchColor, numOfElem, L"CPLA");
	else if(MEDIASUBTYPE_MJPG == guid)
		wcscpy_s(wchColor, numOfElem, L"MJPG");
	else if(MEDIASUBTYPE_TVMJ == guid)
		wcscpy_s(wchColor, numOfElem, L"TVMJ");
	else if(MEDIASUBTYPE_WAKE == guid)
		wcscpy_s(wchColor, numOfElem, L"WAKE");
	else if(MEDIASUBTYPE_CFCC == guid)
		wcscpy_s(wchColor, numOfElem, L"CFCC");
	else if(MEDIASUBTYPE_IJPG == guid)
		wcscpy_s(wchColor, numOfElem, L"IJPG");
	else if(MEDIASUBTYPE_Plum == guid)
		wcscpy_s(wchColor, numOfElem, L"Plum");
	else if(MEDIASUBTYPE_DVCS == guid)
		wcscpy_s(wchColor, numOfElem, L"DVCS");
	else if(MEDIASUBTYPE_MDVF == guid)
		wcscpy_s(wchColor, numOfElem, L"MDVF");
	else if(MEDIASUBTYPE_DVSD == guid)
		wcscpy_s(wchColor, numOfElem, L"DVSD");
	else if(MEDIASUBTYPE_CLPL == guid)
		wcscpy_s(wchColor, numOfElem, L"CLPL");
	else if(MEDIASUBTYPE_I420 == guid)
		wcscpy_s(wchColor, numOfElem, L"I420");
	else
		wcscpy_s(wchColor, numOfElem, L"Unknown");
	}

// 查询"strSubTypeSizeList"中最大或最小的"size", 并且由"strSubTypeSize"返回
void CDXCapture::GetMaxOrMinSizeinSizeList(const CStringList& strSubTypeSizeList, CString& strSubTypeSize, BOOL bIsMax)
	{
	CString strSize;
	CString strSizeWid;
	int iFirstSizeWid;
	int iSizeWid;

	strSize = strSubTypeSizeList.GetHead();
	strSizeWid = strSize.Left(strSize.Find(_T('x')));
	strSizeWid.Trim();
	iFirstSizeWid = _tstoi(strSizeWid);
	strSubTypeSize = strSize;

	if (bIsMax)
		{
		for (POSITION headPos = strSubTypeSizeList.GetHeadPosition(); NULL != headPos; )
			{
			strSize = strSubTypeSizeList.GetNext(headPos);
			strSizeWid = strSize.Left(strSize.Find(_T('x')));
			strSizeWid.Trim();
			iSizeWid = _tstoi(strSizeWid);

			if (iFirstSizeWid < iSizeWid)
				{
				strSubTypeSize = strSize;
				}
			}
		}
	else
		{
		for (POSITION headPos = strSubTypeSizeList.GetHeadPosition(); NULL != headPos; )
			{
			strSize = strSubTypeSizeList.GetNext(headPos);
			strSizeWid = strSize.Left(strSize.Find(_T('x')));
			strSizeWid.Trim();
			iSizeWid = _tstoi(strSizeWid);

			if (iFirstSizeWid > iSizeWid)
				{
				strSubTypeSize = strSize;
				}
			}
		}	
	}

// 在"strSubTypeSizeList"中查找由"strSubTypeSize"指定的"Size"是否存在, 如果存在返回TRUE; 否则
// 返回FALSE
BOOL CDXCapture::FindSpecifySizeInSizeList(const CStringList& strSubTypeSizeList, const CString& strSubTypeSize)
	{
	CString strSize;

	for (POSITION headPos = strSubTypeSizeList.GetHeadPosition(); NULL != headPos; )
		{
		strSize = strSubTypeSizeList.GetNext(headPos);

		if (0 == strSize.CompareNoCase(strSubTypeSize))
			{
			return TRUE;
			}

		}

	return FALSE;

	}

BOOL CDXCapture::IsSpecifyPinExist(const GUID pIn)
	{
	if (NULL == m_pCapBuilder || NULL == m_pSrcFilter)
		{
		return FALSE;
		}

	HRESULT hr;
	IPin *pPin = NULL;
	BOOL bRet = FALSE;
	hr = m_pCapBuilder->FindPin(m_pSrcFilter,
		PINDIR_OUTPUT,
		&pIn,
		0, FALSE,
		0, &pPin);
	if (SUCCEEDED(hr) && (NULL != pPin))
		{
		SAFERELEASE(pPin);
		bRet = TRUE;
		}

	return bRet; 
	}

BOOL CDXCapture::IsStillPinExist()
	{
	return	IsSpecifyPinExist(PIN_CATEGORY_STILL);
	}

BOOL CDXCapture::StillPinSnapShot()
	{
	if (NULL == m_pCapBuilder || NULL == m_pSrcFilter)
		{
		return FALSE;
		}

	HRESULT hr;
	IPin *pPin = NULL;
	BOOL bRet = FALSE;
	hr = m_pCapBuilder->FindPin(m_pSrcFilter,
		PINDIR_OUTPUT,
		&PIN_CATEGORY_STILL,
		0, FALSE,
		0, &pPin);
	if (SUCCEEDED(hr))
		{
		IAMVideoControl *pAMVideoControl = NULL;
		hr = m_pSrcFilter->QueryInterface(IID_IAMVideoControl, (void**)&pAMVideoControl);
		if (SUCCEEDED(hr))
			{							
			hr = pAMVideoControl->SetMode(pPin, VideoControlFlag_Trigger);	
			if (SUCCEEDED(hr))
				{
				bRet = TRUE;
				}
			SAFERELEASE(pAMVideoControl);
			}	

		SAFERELEASE(pPin);
		}

	return bRet;
	}

// Adds a DirectShow filter graph to the Running Object Table,
// allowing GraphEdit to "spy" on a remote filter graph.
HRESULT CDXCapture::AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
	{
	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	WCHAR wsz[128];
	HRESULT hr;

	if (!pUnkGraph || !pdwRegister)
		return E_POINTER;

	if(FAILED(GetRunningObjectTable(0, &pROT)))
		return E_FAIL;

	wsprintfW(wsz, L"FilterGraph %08x pid %08x\0", (DWORD_PTR)pUnkGraph, 
		GetCurrentProcessId());

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if(SUCCEEDED(hr))
		{
		// Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
		// to the object.  Using this flag will cause the object to remain
		// registered until it is explicitly revoked with the Revoke() method.
		//
		// Not using this flag means that if GraphEdit remotely connects
		// to this graph and then GraphEdit exits, this object registration 
		// will be deleted, causing future attempts by GraphEdit to fail until
		// this application is restarted or until the graph is registered again.
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, 
			pMoniker, pdwRegister);
		pMoniker->Release();
		}

	pROT->Release();
	return hr;
	}

// Removes a filter graph from the Running Object Table
void CDXCapture::RemoveGraphFromRot(DWORD pdwRegister)
	{
	IRunningObjectTable *pROT;

	if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
		{
		pROT->Revoke(pdwRegister);
		pROT->Release();
		}
	}

void CDXCapture::TearDownGraph()
	{
	if (m_pMC)
		{
		m_pMC->Stop();		
		}

	if (m_pVW)
		{
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
		}

	if (NULL != m_pSrcFilter)
		{
		NukeDownstream(m_pSrcFilter);
		}

	SAFERELEASE(m_pSampleGrabber);	
	SAFERELEASE(m_pSampleGrabber_StillPin);
	

	if(m_dwRegister)
		{
		RemoveGraphFromRot(m_dwRegister);
		m_dwRegister = 0;
		}

	m_bIsUseNullRenderer = FALSE;
	m_bIsUseSampleGrabberFilter = FALSE;
	m_bIsWantStillPinGrabber = FALSE;	
	}

void CDXCapture::NukeDownstream(IBaseFilter *pf)
	{

	IPin *pP=0, *pTo=0;
	ULONG u;
	IEnumPins *pins = NULL;
	PIN_INFO pininfo;

	if (!pf)
		return;

	HRESULT hr = pf->EnumPins(&pins);
	pins->Reset();

	while(hr == NOERROR)
		{
		hr = pins->Next(1, &pP, &u);
		if(hr == S_OK && pP)
			{
			pP->ConnectedTo(&pTo);
			if(pTo)
				{
				hr = pTo->QueryPinInfo(&pininfo);
				if(hr == NOERROR)
					{
					if(pininfo.dir == PINDIR_INPUT)
						{
						NukeDownstream(pininfo.pFilter);
						m_pGraph->Disconnect(pTo);
						m_pGraph->Disconnect(pP);
						m_pGraph->RemoveFilter(pininfo.pFilter);
						}
					pininfo.pFilter->Release();
					}
				pTo->Release();
				}
			pP->Release();
			}
		}

	if(pins)
		pins->Release();
	}

BOOL CDXCapture::ShowFilterPinPropertyPage()
	{
	if (!m_pSrcFilter)
		{
		return FALSE;
		}

	HRESULT hr;
	IEnumPins * pEnumpins = NULL;
	IPin * pPin = NULL;
	hr = m_pSrcFilter->EnumPins(&pEnumpins);
	while (pEnumpins->Next(1, &pPin, 0))
		{
		PIN_DIRECTION PinDirThis;
		pPin->QueryDirection(&PinDirThis);
		if (PINDIR_OUTPUT == PinDirThis)
			{
			pEnumpins->Release();
			break;
			}
		pPin->Release();
		}

	IAMStreamConfig * pPconfig = NULL;
	hr = pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pPconfig);
	ISpecifyPropertyPages * pSpecify;
	hr = pPconfig->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);

	if (SUCCEEDED(hr))
		{
		CAUUID   caGUID;
		pSpecify->GetPages(&caGUID);
		pSpecify->Release();

		OleCreatePropertyFrame(
			::GetActiveWindow(),        // Parent window
			0,                          // x (Reserved)
			0,                          // y (Reserved)
			NULL,                       // Caption for the dialog box
			1,                          // Number of filters
			(IUnknown **) &pPconfig,    // Pointer to the filter
			caGUID.cElems,              // Number of property pages
			caGUID.pElems,              // Pointer of property page CLSIDs
			0,                          // Locale identifier
			0,                          // Reserved
			NULL                        // Reserved
			);
		CoTaskMemFree(caGUID.pElems);
		return TRUE;
		}
	return FALSE;
	}

HRESULT CDXCapture::BuildPreviewGrpah()
	{	
	HRESULT hr;

	// haifeng_liu 2010/8/8 add
	// +++++
	IBaseFilter *pMidFilter = NULL;			
	IBaseFilter *pRendererFilter = NULL;

	if (m_bIsUseSampleGrabberFilter)
		{
		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (LPVOID *)&pMidFilter);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			{
			return hr;
			}

		hr = m_pGraph->AddFilter(pMidFilter, L"VideoSampleGrabber");
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			{
			return hr;
			}
		}

	if (m_bIsUseNullRenderer)
		{
		hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (LPVOID *)&pRendererFilter);
		if (FAILED(hr))
			{
			return hr;
			}
		hr = m_pGraph->AddFilter(pRendererFilter, L"NullRenderer");
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			{
			return hr;
			}
		}	

	hr = m_pCapBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pSrcFilter, pMidFilter, pRendererFilter);
	if (FAILED(hr))
		{
		return hr;
		}

	if (m_bIsUseSampleGrabberFilter)
		{
		hr = pMidFilter->QueryInterface(IID_ISampleGrabber, (void **)&m_pSampleGrabber);
		hr = m_pSampleGrabber->SetOneShot(FALSE);
		hr = m_pSampleGrabber->SetBufferSamples(FALSE);
		hr = m_pSampleGrabber->SetCallback(&m_SGC, 1);
		hr = m_pSampleGrabber->GetConnectedMediaType(m_SGC.GetStillMediaType());
		ASSERT(SUCCEEDED(hr));

		}

	if (m_bIsWantStillPinGrabber)
		{
		BuildGrabberGraph_Other(m_pSrcFilter);
		}
	// -----	

	hr = AddGraphToRot(m_pGraph, &m_dwRegister);
	if(FAILED(hr))
		{		
		m_dwRegister = 0;
		}
	
	SAFERELEASE(pMidFilter);
	SAFERELEASE(pRendererFilter);


	// haifeng_liu 2010/8/20 add
	// +++++
	hr = SetupVideoWindow();
	if (FAILED(hr))
		{
		return hr;
		}

	if (m_pMC)
		{
		hr = m_pMC->Run();
		if (FAILED(hr))
			{
			return hr;
			}
		}

	return S_OK;	
	// -----
	}

BOOL CDXCapture::GetCurrentPreviewSize(SIZE& curSize)
	{
	return GetCurrentSize(curSize, TRUE);
	}

BOOL CDXCapture::GetCurrentPhotoSize(SIZE& curSize)
	{
	return GetCurrentSize(curSize, FALSE);
	}

BOOL CDXCapture::GetCurrentSize(SIZE& curSize, const BOOL bIsVideo)
	{
	BOOL bRet = FALSE;
	if (NULL != m_pCapBuilder && NULL != m_pSrcFilter)
		{
		HRESULT hr;
		IAMStreamConfig *pConfig = NULL;
		GUID guid = bIsVideo ? PIN_CATEGORY_CAPTURE : PIN_CATEGORY_STILL;	

		hr = m_pCapBuilder->FindInterface(&guid, &MEDIATYPE_Video,
			m_pSrcFilter, IID_IAMStreamConfig, (void **)&pConfig);	
		if (SUCCEEDED(hr))
			{
			AM_MEDIA_TYPE *pmt = NULL;
			hr = pConfig->GetFormat(&pmt);
			if (SUCCEEDED(hr))
				{
				curSize.cx = HEADER(pmt->pbFormat)->biWidth;
				curSize.cy = HEADER(pmt->pbFormat)->biHeight;

				bRet = TRUE;

				DeleteMediaType(pmt);
				}
			SAFERELEASE(pConfig);
			}
		}
	return bRet;
	}


BOOL CDXCapture::IsCurrentPreviewSizeSameWithStillPinPhotoSize()
	{
	SIZE curPreviewSize;
	SIZE curStillPinPhotoSize;

	GetCurrentPreviewSize(curPreviewSize);
	GetCurrentPhotoSize(curStillPinPhotoSize);

	if (curPreviewSize.cx == curStillPinPhotoSize.cx
		&& curPreviewSize.cy == curStillPinPhotoSize.cy)
		{
		return TRUE;
		}
	else
		{
		return FALSE;
		}
	}

IBaseFilter* CDXCapture::GetSrcFilter()
	{
		return m_pSrcFilter;
	}

void CDXCapture::SetMonikerIdx( UINT iMonikerIdx )
	{
		m_iMonikerIdx = iMonikerIdx;
	}
#include "StdAfx.h"
#include "UDsUtils.h"

CUDsUtils::CUDsUtils(void)
	{
	}

CUDsUtils::~CUDsUtils(void)
	{
	}

// 显示SourceFilter属性页
BOOL CUDsUtils::ShowFilterPropertyPage(IBaseFilter * inFilter)
	{
	if (!inFilter)
		{
		return FALSE;
		}

	ISpecifyPropertyPages * pSpecify;
	HRESULT hr = inFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
	if (SUCCEEDED(hr))
		{
		CAUUID   caGUID;
		pSpecify->GetPages(&caGUID);
		pSpecify->Release();

		hr = OleCreatePropertyFrame(
			::GetActiveWindow(),        // Parent window
			0,                          // x (Reserved)
			0,                          // y (Reserved)
			NULL,                       // Caption for the dialog box
			1,                          // Number of filters
			(IUnknown **) &inFilter,    // Pointer to the filter
			caGUID.cElems,              // Number of property pages
			caGUID.pElems,              // Pointer of property page CLSIDs
			0,                          // Locale identifier
			0,                          // Reserved
			NULL                        // Reserved
			);
		CoTaskMemFree(caGUID.pElems);

		if (SUCCEEDED(hr))
		{
		return TRUE;
		}
		
		}
	return FALSE;
	}

// 显示SourceFilter 指定Pin属性页
BOOL CUDsUtils::ShowFilterPinPropertyPage(IBaseFilter * inFilter, const GUID * inpCategory)
	{
	if (!inFilter)
		{
		return FALSE;
		}

	HRESULT hr;
	IEnumPins * pEnumpins = NULL;
	IPin * pPin = NULL;
	hr = inFilter->EnumPins(&pEnumpins);
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

		hr = OleCreatePropertyFrame(
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

		if (SUCCEEDED(hr))
		{
		return TRUE;
		}
		
		}
	return FALSE;
	}

// 查询指定类型目录所有Filter的友好名称
BOOL CUDsUtils::QueryDeviceCategoryFriendlyName( const GUID inCategory, CStringList& strList )
	{
		HRESULT hr;
		ICreateDevEnum *pCreateEnum = NULL;
		hr = CoCreateInstance(CLSID_SystemDeviceEnum,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_ICreateDevEnum,
				(void **)&pCreateEnum);
		if (FAILED(hr))
		{
		return FALSE;
		}

		IEnumMoniker *pEm = NULL;
		hr = pCreateEnum->CreateClassEnumerator(inCategory, 
				&pEm, 0);
		if (FAILED(hr))
		{
		goto QueryFail;
		}

		pEm->Reset();

		TCHAR friendlyName[256] = {0};

		ULONG fetched = 0;
		IMoniker * moniker = NULL;		

		while(SUCCEEDED(pEm->Next(1, &moniker, &fetched)) && fetched)
			{
			if (NULL != moniker)
				{
				IPropertyBag * propertyBag = NULL;
				
				VARIANT        name;				

				hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propertyBag);

				if (SUCCEEDED(hr))
					{
					name.vt = VT_BSTR;
					// Get friendly name
					hr = propertyBag->Read(L"FriendlyName", &name, NULL);
					}

				if (SUCCEEDED(hr))
					{
					friendlyName[0] = 0;
#ifdef UNICODE
					_tcscpy_s(friendlyName, name.bstrVal);					
#else
					WideCharToMultiByte(CP_ACP, 0, name.bstrVal, -1,
						friendlyName, 256, NULL, NULL);
#endif					
					// 防止添加重复的Filter名称
					if (NULL == strList.Find(friendlyName, 0))
						{
						strList.AddTail(friendlyName);
						}
					}

				SAFERELEASE(propertyBag);
				SAFERELEASE(moniker);
				}
			}

		SAFERELEASE(pCreateEnum);
		SAFERELEASE(pEm);
		return TRUE;

QueryFail:
		SAFERELEASE(pCreateEnum);
		SAFERELEASE(pEm);

		return FALSE;
	}

// 根据指定目录Filter的友好名称, 获取此Filter
BOOL CUDsUtils::QueryDeviceCategoryFilterByFriendlyName(const GUID inCategory, TCHAR* pszName, IBaseFilter** ppCompressor)
	{
	if (!ppCompressor)
		{
		return FALSE;
		}	


	HRESULT hr = NOERROR;
	ICreateDevEnum * enumHardware = NULL;
	IBaseFilter *  filter = NULL;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_ALL,
		IID_ICreateDevEnum, (void**)&enumHardware);
	if (FAILED(hr))
		{
		return FALSE;
		}

	IEnumMoniker *  enumMoniker = NULL;
	hr = enumHardware->CreateClassEnumerator(CLSID_VideoCompressorCategory, &enumMoniker, 0);
	if (enumMoniker)
		{
		enumMoniker->Reset();	

		TCHAR friendlyName[256];

		ULONG fetched = 0;
		IMoniker * moniker = NULL;		

		while(SUCCEEDED(enumMoniker->Next(1, &moniker, &fetched)) && fetched)
			{
			if (moniker)
				{
				IPropertyBag * propertyBag = NULL;				
				VARIANT        name;				

				hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propertyBag);

				if (SUCCEEDED(hr))
					{
					name.vt = VT_BSTR;
					// Get friendly name
					hr = propertyBag->Read(L"FriendlyName", &name, NULL);

					// Release interfaces		
					SAFERELEASE(propertyBag);					

					}
				if (SUCCEEDED(hr))
					{
					friendlyName[0] = 0;
#ifdef UNICODE
					_tcscpy_s(friendlyName, name.bstrVal);					
#else
					WideCharToMultiByte(CP_ACP, 0, name.bstrVal, -1,
						friendlyName, 256, NULL, NULL);
#endif

					if (0 == _tcscmp(pszName, friendlyName))
						{
						hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&filter);
						if (SUCCEEDED(hr))
							{						
							SAFERELEASE(moniker);
							break;							
							}
						}					
					}

				SAFERELEASE(moniker);
				}
			}
		SAFERELEASE(enumMoniker);		
		}
	SAFERELEASE(enumHardware);	

	*ppCompressor = filter;

	return TRUE;
	}


// Adds a DirectShow filter graph to the Running Object Table,
// allowing GraphEdit to "spy" on a remote filter graph.
HRESULT CUDsUtils::AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
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
void CUDsUtils::RemoveGraphFromRot(DWORD pdwRegister)
	{
	IRunningObjectTable *pROT;

	if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
		{
		pROT->Revoke(pdwRegister);
		pROT->Release();
		}
	}

BOOL CUDsUtils::SetFilterMerit( const TCHAR *inClsid, const DWORD inMerit )
	{
	typedef struct
		{
		DWORD dwVersion;		// 版本号
		DWORD dwMerit;			// Merit值
		DWORD dwPinCount;		// Pin的数量
		DWORD dwReserved;		// 保留
		} FILTER_HEADER;

	const TCHAR *cRegistryEntry = _T("CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\");
	const long cMaxLength = 1024 * 16;
	BYTE filterData[cMaxLength] = {0};
	//	DWORD actualLength = 0;				// "actualLength" 设置为0, 会导致"ERROR_MORE_DATA"错误
	DWORD actualLength = cMaxLength;

	// 生成Filter信息注册部分的注册表入口
	TCHAR szEntry[1000];
	_tcscpy(szEntry, cRegistryEntry);
	_tcscat(szEntry, inClsid);

	HKEY hKey = NULL;
	LONG result = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, szEntry, 0, KEY_ALL_ACCESS, &hKey);
	BOOL bPass = (ERROR_SUCCESS == result);
	if (bPass)
		{
		// 读取FilterData的值
		result = ::RegQueryValueEx(hKey, _T("FilterData"), NULL, NULL, filterData, &actualLength);
		bPass = (result == ERROR_SUCCESS);
		}

	if (bPass)
		{
		// 修改FilterData中Merit部分, 然后写回到注册表
		FILTER_HEADER *filterHeader = (FILTER_HEADER *)filterData;
		filterHeader->dwMerit = inMerit;
		actualLength = cMaxLength;
		result = ::RegSetValueEx(hKey, _T("FilterData"), NULL, REG_BINARY, filterData, actualLength);
		bPass = (ERROR_SUCCESS == result);
		}

	if (hKey)
		{
		::RegCloseKey(hKey);
		}

	return bPass;	
	}

BOOL CUDsUtils::SetFilterMerit(const GUID inGuid, const DWORD inMerit)
	{
	TCHAR szClsid[MAX_PATH] = {0};

	_stprintf(szClsid, _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), 
		inGuid.Data1, inGuid.Data2, inGuid.Data3, 
		inGuid.Data4[0],
		inGuid.Data4[1],
		inGuid.Data4[2],
		inGuid.Data4[3],
		inGuid.Data4[4],
		inGuid.Data4[5],
		inGuid.Data4[6],
		inGuid.Data4[7]);	

	return SetFilterMerit(szClsid, inMerit);
	}

BOOL CUDsUtils::GetFilterMerit( const TCHAR *inClsid, DWORD& outMerit )
	{
	typedef struct
		{
		DWORD dwVersion;		// 版本号
		DWORD dwMerit;			// Merit值
		DWORD dwPinCount;		// Pin的数量
		DWORD dwReserved;		// 保留
		} FILTER_HEADER;

	const TCHAR *cRegistryEntry = _T("CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\");
	const long cMaxLength = 1024 * 16;
	BYTE filterData[cMaxLength] = {0};
//	DWORD actualLength = 0;				// "actualLength" 设置为0, 会导致"ERROR_MORE_DATA"错误
	DWORD actualLength = cMaxLength;

	// 生成Filter信息注册部分的注册表入口
	TCHAR szEntry[1000];
	_tcscpy(szEntry, cRegistryEntry);
	_tcscat(szEntry, inClsid);

	HKEY hKey = NULL;
	LONG result = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, szEntry, 0, KEY_ALL_ACCESS, &hKey);
	BOOL bPass = (ERROR_SUCCESS == result);
	if (bPass)
		{
		// 读取FilterData的值
		result = ::RegQueryValueEx(hKey, _T("FilterData"), NULL, NULL, filterData, &actualLength);		
		bPass = (result == ERROR_SUCCESS);
		}

	if (bPass)
		{
		// 修改FilterData中Merit部分, 然后写回到注册表
		FILTER_HEADER *filterHeader = (FILTER_HEADER *)filterData;
		outMerit = filterHeader->dwMerit;		
		}

	if (hKey)
		{
		::RegCloseKey(hKey);
		}

	return bPass;		
	}

BOOL CUDsUtils::GetFilterMerit(const GUID inGuid, DWORD& outMerit)
	{
	TCHAR szClsid[MAX_PATH] = {0};

	_stprintf(szClsid, _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), 
		inGuid.Data1, inGuid.Data2, inGuid.Data3, 
		inGuid.Data4[0],
		inGuid.Data4[1],
		inGuid.Data4[2],
		inGuid.Data4[3],
		inGuid.Data4[4],
		inGuid.Data4[5],
		inGuid.Data4[6],
		inGuid.Data4[7]);

	return GetFilterMerit(szClsid, outMerit);
	}

//
// 从给定的源字串中分析出对应的"vid"和"pid"字串// 如果给定的源字串
// 中"vid"和"pid"的格式必须如"usb\vid_0c45&pid_62c0", 如果不满足以上
// 规定或者源字串中不存在"vid"和"pid"字串, 则返回FALSE; 否则返回TRUE
//
BOOL CUDsUtils::ParseStringVidPidHex(const CString strSrc, CString& strVidHex, CString& strPidHex)
	{
	CString strSrcPrc = strSrc;
	CString strVidPrc, strPidPrc;

	int iPosVid = 0, iPosPid = 0;

	iPosVid = strSrcPrc.Find(_T("vid_"));
	if (-1 == iPosVid)
		{
		return FALSE;
		}		

	iPosPid = strSrcPrc.Find(_T("pid_"));
	if (-1 == iPosPid)
		{
		return FALSE;
		}

	strVidHex = strSrcPrc.Mid(iPosVid+4, 4);
	strPidHex = strSrcPrc.Mid(iPosPid+4, 4);

	return TRUE;
	}

BOOL CUDsUtils::GetCurDevDisplayName(IMoniker *pMoniker, CString& strDisplayName)
	{
	if (NULL == pMoniker)
	{
	return FALSE;
	}

	LPTSTR lpDisplayName = NULL;
	pMoniker->GetDisplayName(NULL, NULL, &lpDisplayName);
	strDisplayName = lpDisplayName;		

	IMalloc *pMalloc = NULL;
	HRESULT hr = CoGetMalloc(1, &pMalloc);
	if (SUCCEEDED(hr))
		{
		pMalloc->Free(lpDisplayName);
		pMalloc->Release();
		lpDisplayName = NULL;
		}

	return TRUE;	
	}

BOOL CUDsUtils::IsSpecifyDevVidPidExsit(IMoniker *pMoniker, const CString strVidHex, const CString strPidHex)
	{
	CString strDisplayName;
	CString strVidTmp;
	CString strPidTmp;

	if (!GetCurDevDisplayName(pMoniker, strDisplayName))
		{
		return FALSE;
		}

	if (!ParseStringVidPidHex(strDisplayName, strVidTmp, strPidTmp))
		{
		return FALSE;
		}

	if (0 == strVidHex.CompareNoCase(strVidTmp) && 0 == strPidHex.CompareNoCase(strPidTmp))
		{
		return TRUE;
		}
	else
		{
		return FALSE;
		}
	}
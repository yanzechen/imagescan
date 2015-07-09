#pragma once  

#include <streams.h>  

class CUDsUtils
	{
	public:
		CUDsUtils(void);
		~CUDsUtils(void);

	public:
		BOOL ShowFilterPropertyPage(IBaseFilter * inFilter);
		BOOL ShowFilterPinPropertyPage(IBaseFilter * inFilter, const GUID * inpCategory);

		BOOL QueryDeviceCategoryFriendlyName(const GUID inCategory, CStringList& strList);
		BOOL QueryDeviceCategoryFilterByFriendlyName(const GUID inCategory, TCHAR* pszName, IBaseFilter** ppCompressor);

		HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
		void RemoveGraphFromRot(DWORD pdwRegister);
		BOOL SetFilterMerit( const TCHAR *inClsid, const DWORD inMerit );
		BOOL SetFilterMerit(const GUID inGuid, const DWORD inMerit);
		BOOL GetFilterMerit( const TCHAR *inClsid, DWORD& outMerit );
		BOOL GetFilterMerit(const GUID inGuid, DWORD& outMerit);
		BOOL ParseStringVidPidHex(const CString strSrc, CString& strVidHex, CString& strPidHex);
		BOOL GetCurDevDisplayName(IMoniker *pMoniker, CString& strDisplayName);		
		BOOL IsSpecifyDevVidPidExsit(IMoniker *pMoniker, const CString strVidHex, const CString strPidHex);
	};

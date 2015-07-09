/********************************************************************
created:	2011/05/05	
filename: 	ReadWriteASIC	
author:		haifeng_liu

purpose:	
*********************************************************************/
#include "stdafx.h"	
#include <streams.h>			// add for "FreeMediyType()"  Declared in Mtype.h; include Streams.h 
#include <ks.h>				// ks.h must be included before ksmedia.h or ksproxy.h
#include <ksproxy.h>
#include "BaseFunc.h"
#include "ReadWriteASIC.h"

CReadWriteASIC::CReadWriteASIC(void)
{
	m_pSrcFilter = NULL;
}

CReadWriteASIC::~CReadWriteASIC(void)
{
	SAFERELEASE(m_pSrcFilter);
}

BOOL CReadWriteASIC::ReadFromRegister( ULONG id, PLONG pValue )
{
	IKsPropertySet*               pKsPropertySet;
	KSPROPERTY_CLIENT_PROP_S	  KsProperty;
	ULONG                         ulRtnCode;
	HRESULT                       hr;

	//vivi 2005/5/9 source code protection
	if(m_pSrcFilter == NULL)	return FALSE;

	hr = m_pSrcFilter->QueryInterface(IID_IKsPropertySet,(LPVOID*)&pKsPropertySet);
	if (hr != NOERROR)     return hr;

	ZeroMemory(&KsProperty,sizeof(KSPROPERTY_CLIENT_PROP_S));
	KsProperty.Property.Flags       = KSPROPERTY_TYPE_GET;

	// james 2007/01/03
	hr = pKsPropertySet->Get(PROPSETID_CLIENT_PROP_UVC_LIKE,id,&KsProperty,
		sizeof(KSPROPERTY_CLIENT_PROP_S),&KsProperty,
		sizeof(KSPROPERTY_CLIENT_PROP_S),&ulRtnCode);

	if(hr!=NOERROR)
	{
		hr = pKsPropertySet->Get(PROPSETID_CLIENT_PROP_ST50201,id,&KsProperty,
			sizeof(KSPROPERTY_CLIENT_PROP_S),&KsProperty,
			sizeof(KSPROPERTY_CLIENT_PROP_S),&ulRtnCode);
	}

	*pValue = KsProperty.value ;
	pKsPropertySet->Release();
	return hr;
}

BOOL CReadWriteASIC::WriteToRegister( ULONG id , PLONG pValue , ULONG index, ULONG valueSize, ULONG common )
{
	IKsPropertySet*               lpKsPropertySet;
	KSPROPERTY_CLIENT_PROP_S      KsProperty;
	HRESULT                       hr;

	//vivi 2005/5/9 source code protection
	if(m_pSrcFilter == NULL)	return FALSE;

	hr = m_pSrcFilter->QueryInterface(IID_IKsPropertySet,(LPVOID*)&lpKsPropertySet);
	if (hr != NOERROR)     return hr;

	ZeroMemory(&KsProperty,sizeof(KSPROPERTY_CLIENT_PROP_S));
	KsProperty.Property.Flags      = KSPROPERTY_TYPE_SET;
	KsProperty.index               = index;
	KsProperty.value               = *pValue;
	KsProperty.valueSize           = valueSize;
	KsProperty.common              = common;

	// james 2007/01/03
	hr = lpKsPropertySet->Set(PROPSETID_CLIENT_PROP_UVC_LIKE,id,&KsProperty,
		sizeof(KSPROPERTY_CLIENT_PROP_S),&KsProperty,
		sizeof(KSPROPERTY_CLIENT_PROP_S));

	TRACE("hr = %x", hr);

	if(hr!=NOERROR)
	{
		hr = lpKsPropertySet->Set(PROPSETID_CLIENT_PROP_ST50201,id,&KsProperty,
			sizeof(KSPROPERTY_CLIENT_PROP_S),&KsProperty,
			sizeof(KSPROPERTY_CLIENT_PROP_S));
		TRACE("hr = %x", hr);
	}

	lpKsPropertySet->Release();
	return hr;
}


BOOL CReadWriteASIC::AttachSrcFilter( IBaseFilter *pSrcFilter )
{
	if (NULL == pSrcFilter)
	{
		return FALSE;
	}

	SAFERELEASE(m_pSrcFilter);
	m_pSrcFilter = pSrcFilter;
	m_pSrcFilter->AddRef();
	return TRUE;
}

IBaseFilter* CReadWriteASIC::DetachSrcFilter()
{
	IBaseFilter *pSrcFilter = m_pSrcFilter;
	SAFERELEASE(m_pSrcFilter);

	return pSrcFilter;
}

BOOL CReadWriteASIC::ReadFromASIC( USHORT Addr, BYTE *pValue )
{
	IKsControl	*pKsControl;
	KSP_NODE	ExtensionProp;
	struct {
		USHORT	Addr;
		USHORT	Value;
	} ExtensionData;
	ULONG		ulBytesReturned;
	HRESULT		hr;

	TRACE0("ReadFromASIC=>\n");
	if(m_pSrcFilter == NULL)	return FALSE;

	hr = m_pSrcFilter->QueryInterface(IID_IKsControl,(LPVOID*)&pKsControl);
	TRACE("QueryInterface(IID_IKsControl):0x%08x\n", hr);
	if (hr != NOERROR)     return FALSE;

	// dummy write for setting address
	ExtensionProp.Property.Set = PROPSETID_VIDCAP_EXTENSION_UNIT;
	ExtensionProp.Property.Id = 1;//KSPROPERTY_EXTENSION_UNIT_INFO;
	ExtensionProp.Property.Flags = KSPROPERTY_TYPE_SET | 
		KSPROPERTY_TYPE_TOPOLOGY;
	ExtensionProp.NodeId = 1;

	ExtensionData.Addr = Addr;
	ExtensionData.Value = 0xFF00;

	hr = pKsControl->KsProperty(
		(PKSPROPERTY) &ExtensionProp,
		sizeof(ExtensionProp),
		(PVOID) &ExtensionData,
		sizeof(ExtensionData),
		&ulBytesReturned);
	TRACE("IKsControl->KsProperty(Set):0x%08x\n", hr);

	if (hr == NOERROR)
	{
		ExtensionProp.Property.Set = PROPSETID_VIDCAP_EXTENSION_UNIT;
		ExtensionProp.Property.Id = 1;//KSPROPERTY_EXTENSION_UNIT_INFO;
		ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET | 
			KSPROPERTY_TYPE_TOPOLOGY;
		ExtensionProp.NodeId = 1;

		ExtensionData.Addr = Addr;
		ExtensionData.Value = 0xFF00;
		memset(&ExtensionData, 0, sizeof(ExtensionData));

		hr = pKsControl->KsProperty(
			(PKSPROPERTY) &ExtensionProp,
			sizeof(ExtensionProp),
			(PVOID) &ExtensionData,
			sizeof(ExtensionData),
			&ulBytesReturned);
		TRACE("IKsControl->KsProperty(Get):0x%08x\n", hr);

		if (hr == NOERROR)
			*pValue = (BYTE)ExtensionData.Value;
	}
	else
	{
		// james 2007/01/03
		LONG Asic_data = 0;
		LONG Asic_address = Addr;
		if(NOERROR == WriteToRegister (KSPROPERTY_CLIENT_PROP_ASICREAD, &Asic_data, Asic_address, 0, 0))
		{
			if(NOERROR == ReadFromRegister (KSPROPERTY_CLIENT_PROP_ASICREAD, &Asic_data))
			{	
				*pValue = (BYTE)Asic_data;
				TRACE("ReadFromASIC Asic data = %x", Asic_data);
				hr = NOERROR;
			}

		}
		else
			hr = S_FALSE;
	}

	pKsControl->Release();

	return (hr == NOERROR);
}

BOOL CReadWriteASIC::WriteToASIC( USHORT Addr, BYTE Value )
{
	IKsControl	*pKsControl;
	KSP_NODE	ExtensionProp;
	struct {
		USHORT	Addr;
		USHORT	Value;
	} ExtensionData;
	ULONG		ulBytesReturned;
	HRESULT		hr;

	TRACE0("WriteToASIC=>\n");
	if(m_pSrcFilter == NULL)	return FALSE;

	hr = m_pSrcFilter->QueryInterface(IID_IKsControl,(LPVOID*)&pKsControl);
	TRACE("QueryInterface(IID_IKsControl):0x%08x\n", hr);
	if (hr != NOERROR)     return FALSE;

	ExtensionProp.Property.Set = PROPSETID_VIDCAP_EXTENSION_UNIT;
	ExtensionProp.Property.Id = 1;//KSPROPERTY_EXTENSION_UNIT_INFO;
	ExtensionProp.Property.Flags = KSPROPERTY_TYPE_SET | 
		KSPROPERTY_TYPE_TOPOLOGY;
	ExtensionProp.NodeId = 1;

	ExtensionData.Addr = Addr;
	ExtensionData.Value = Value;

	hr = pKsControl->KsProperty(
		(PKSPROPERTY) &ExtensionProp,
		sizeof(ExtensionProp),
		(PVOID) &ExtensionData,
		sizeof(ExtensionData),
		&ulBytesReturned);
	TRACE("IKsControl->KsProperty(Set):0x%08x\n", hr);
	pKsControl->Release();

	// james 2007/01/03
	if(hr != NOERROR)
	{
		LONG Asic_data = Value;
		LONG Asic_address = Addr;
		TRACE("AsicData = %x, AsicAddr = %x", Asic_data, Asic_address);
		hr = WriteToRegister (KSPROPERTY_CLIENT_PROP_ASICWRITE, &Asic_data, Asic_address, 0, 0);
	}

	return (hr == NOERROR);
}

BOOL CReadWriteASIC::ReadSlaveIDFromASIC( BYTE *pSlaveID )
{
	LONG lValTmp;
	BYTE value;
	BOOL bRet = FALSE;	
	if(NOERROR == WriteToRegister(KSPROPERTY_CLIENT_PROP_ASICREAD, &lValTmp, 0x10d1, 0, 0))		
	{
		if(NOERROR == ReadFromRegister(KSPROPERTY_CLIENT_PROP_ASICREAD, &lValTmp))	
		{
			value = (BYTE)lValTmp;
			bRet = TRUE;
		}			
	}
	else
	{
		ReadFromASIC(0x10d1, &value);	
		bRet = TRUE;
	}

	*pSlaveID = value;	 // add [3/11/2011 haifeng_liu]

	return bRet;		
}

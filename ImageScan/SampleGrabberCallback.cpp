// SampleGrabberCallback.cpp: implementation of the CSampleGrabberCallback class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxdlgs.h>
#include <streams.h>
#include "replaceqedit.h"
#include <stdio.h>							// for "sprintf"

#include "PicConver.h"
#include "SampleGrabberCallback.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSampleGrabberCallback::CSampleGrabberCallback()
{
	m_bIsGetAMMEDIATYPE = FALSE;
}

CSampleGrabberCallback::~CSampleGrabberCallback()
{	
	if (m_bIsGetAMMEDIATYPE)
	{
		FreeMediaType(m_mtStillMediaType);	
	}
	
}


AM_MEDIA_TYPE * CSampleGrabberCallback::GetStillMediaType()
{
	m_bIsGetAMMEDIATYPE = TRUE;

	return &m_mtStillMediaType;
}

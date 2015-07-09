// PicConver.cpp: implementation of the CPicConver class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxdlgs.h>	
#include <streams.h>
#include <dbt.h>
#include <mmreg.h>
#include "replaceqedit.h"
#include "PicConver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPicConver::CPicConver()
{

}

CPicConver::~CPicConver()
{

}


/************************************************************************/
/* 函数名称：
/* 参数列表：
/* 返回  值：
/* 功能描述：将原位图文件cbfBmp转换为流, 然后利用指定的类型的编码器对流
			 进行编码, 最后保存为strType指定类型的文件(MemoryBmpToMemoryImage)
/************************************************************************/
#if 1
BOOL CPicConver::MBmpToMImage( CMemFile& cbfBmp, CMemFile& cbfImage, CString strType )
{
	UINT iBmpSize = (UINT)cbfBmp.GetLength();
	HGLOBAL hMemBmp = GlobalAlloc(GMEM_FIXED, iBmpSize);
	if (NULL == hMemBmp)
	{
		return FALSE;
	}	

	IStream* pStmBmp = NULL;
	CreateStreamOnHGlobal(hMemBmp, FALSE, &pStmBmp);
	if (NULL == pStmBmp) 
	{
		GlobalFree(hMemBmp);
		return FALSE;
	}

	// 这段代码很重要, 如果删除就得不到转换图像
	// +++++
	BYTE* pbyBmp = (BYTE *)GlobalLock(hMemBmp);
	cbfBmp.SeekToBegin();
	cbfBmp.Read(pbyBmp, iBmpSize);
	// -----

	Image* imImage = NULL;
	imImage = Image::FromStream(pStmBmp, FALSE);
	if (NULL == imImage) 
	{
		GlobalUnlock(hMemBmp);
		GlobalFree(hMemBmp);
		return FALSE;
	}

	USES_CONVERSION;
	CLSID clImageClsid;

	CString szTmp = _T("");

	szTmp = _T("image/");
	szTmp += strType;

#ifdef UNICODE
	GetImageCLSID(szTmp, &clImageClsid);
#else
	GetImageCLSID(A2W(szTmp), &clImageClsid);
#endif	

	HGLOBAL hMemImage = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (NULL == hMemImage)
	{
		pStmBmp->Release();
		GlobalUnlock(hMemBmp);
		GlobalFree(hMemBmp);
		if (NULL != imImage) 
		{
			delete imImage;
		}
		return FALSE;
	}

	IStream* pStmImage = NULL;
	CreateStreamOnHGlobal(hMemImage, TRUE, &pStmImage);
	if (NULL == pStmImage)
	{
		pStmBmp->Release();
		GlobalUnlock(hMemBmp);
		GlobalFree(hMemBmp);
		GlobalFree(hMemImage);
		if (NULL != imImage) 
		{
			delete imImage;
		}
		return FALSE;
	}
	imImage->Save(pStmImage, &clImageClsid);
	if (NULL == pStmImage) 
	{
		pStmBmp->Release();
		pStmImage->Release();
		GlobalUnlock(hMemBmp);
		GlobalFree(hMemBmp);
		GlobalFree(hMemImage);
		if (NULL != imImage) 
		{
			delete imImage;
		}
		return FALSE;
	}
	LARGE_INTEGER liBegin = {0};
	pStmImage->Seek(liBegin, STREAM_SEEK_SET, NULL);
	BYTE* pbyImage = (BYTE *)GlobalLock(hMemImage);
	cbfImage.SeekToBegin();
	cbfImage.Write(pbyImage, GlobalSize(hMemImage));

	if (NULL != imImage) 
	{
		delete imImage;
	}

	pStmBmp->Release();
	pStmImage->Release();
	GlobalUnlock(hMemBmp);
	GlobalUnlock(hMemImage);
	GlobalFree(hMemBmp);
	GlobalFree(hMemImage);
	return TRUE;
}
#endif

 //得到格式为format的图像文件的编码器，得到该编码器的GUID值保存在pCLSID中
BOOL CPicConver::GetImageCLSID(const WCHAR* format, CLSID* pCLSID)
{
	UINT iNum	= 0;
	UINT iSize	= 0;
	UINT i		= 0;
	ImageCodecInfo	*pImageCodeInfo	= NULL;

	GetImageEncodersSize(&iNum, &iSize);

	if (iSize == 0)
	{
		return FALSE;
	}

	pImageCodeInfo = (ImageCodecInfo *)(malloc(iSize));
	if (pImageCodeInfo ==  NULL)
	{
		return FALSE;
	}

	// 获得系统中可用的编码器的所有信息
	GetImageEncoders(iNum, iSize, pImageCodeInfo);

	// 在可用的编码器中查找format格式是否被支持
	for (i = 0; i < iNum; i++)
	{		
		// MimeType: 编码方式的具体表述
		if (wcscmp(pImageCodeInfo[i].MimeType, format) == 0)
		{
			*pCLSID = pImageCodeInfo[i].Clsid;
			free(pImageCodeInfo);
			return TRUE;
		}
	}

	free(pImageCodeInfo);

	return FALSE;
}
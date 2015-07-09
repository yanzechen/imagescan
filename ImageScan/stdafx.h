// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes

#include <afxmt.h>			// haifeng_liu 2010/8/6 add for "CEvent"

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//#ifndef ULONG_PTR
//#define ULONG_PTR unsigned long*  
#include "GdiPlus.h"
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
//#endif

// Define MEDIASUBTYPE_I420
EXTERN_GUID(MEDIASUBTYPE_I420, 
			0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// custom message
#define WM_GETSTILLBUFFER (WM_USER + 2)	

// Define FS-9M, FS-5M, FS-3M AP最终输出Size
// 如果设备端输出Size小于AP端定义的Size, AP端会通过插值将其
// 放大到如下定义Size 
const int _9M_SIZE_WID = 3696; 
const int _9M_SIZE_HEI = 2464;  
const int _5M_SIZE_WID = 2520;	  
const int _5M_SIZE_HEI = 1680;	  
const int _3M_SIZE_WID = 2048;
const int _3M_SIZE_HEI = 1365;

const int _9M_RAW_SIZE_WID = 3200;
const int _9M_RAW_SIZE_HEI = 2112;
const int _5M_RAW_SIZE_WID = 2368;
const int _5M_RAW_SIZE_HEI = 1576;
const int _3M_RAW_SIZE_WID = 0;
const int _3M_RAW_SIZE_HEI = 0;

// Raw Process 
// +++++	
const int RGBGNODENUM = 24;
const int YGNODENUM = 18;
const int UVGAINNUM = 4;

// Lens Shading Compensation  
const int GAINTABLEWID = 26;
const int GAINTABLEHEI = 18;
const int GAINTABLERADIUS = 41;	 // (GAINTABLEWID/2 + 1)
const int RGBCOMPNUM = 20;
const int TABLENUM = 256;
// -----

const int STOREIMAGEMAXNUM = 12;		 // 内存中保存照片的最大数量   

// Useful Macro
#define SAFERELEASE(p)		{ if (NULL != p) { p->Release(); p = NULL;} }
#define SAFEDELETE(x)		{ if (NULL != x) { delete x; x = NULL;} }
#define SAFEDELETEARRAY(x)	{ if (NULL != x) { delete [] x; x = NULL;} }
#define MAX(x, y)			((x) > (y) ? (x) : (y))
#define MIN(x, y)			((x) < (y) ? (x) : (y))
#define	CLIP(x, y, b)		MIN(MAX(x, y), b)  

// Write ASIC to set film type
const USHORT ASIC_SETFILMTYPE_ADDR = 0x09AE;
const USHORT ASIC_SETFILMTYPE_FLAG_ADDR = 0x09AF;
const USHORT ASIC_SETFILMTYPE_FLAG_VAL = 0x01;
const USHORT ASIC_DEVTYPE_ADDR = 0x09C5;

const USHORT ASIC_RGBGAINEN_ADDR = 0x0A00;
const USHORT ASIC_GGAIN_ADDR = 0x09FD;
const USHORT ASIC_RGAIN_ADDR = 0x09FE;
const USHORT ASIC_BGAIN_ADDR = 0x09FF; 

const USHORT ASIC_YR_ADDR = 0x0A01;
const USHORT ASIC_YG_ADDR = 0x0A02;
const USHORT ASIC_YB_ADDR = 0x0A03;
const USHORT ASIC_URH_ADDR = 0x0A04;
const USHORT ASIC_URL_ADDR = 0x0A05;
const USHORT ASIC_UGH_ADDR = 0x0A06;
const USHORT ASIC_UGL_ADDR = 0x0A07;
const USHORT ASIC_UBH_ADDR = 0x0A08;
const USHORT ASIC_UBL_ADDR = 0x0A09;
const USHORT ASIC_VRH_ADDR = 0x0A0A;
const USHORT ASIC_VRL_ADDR = 0x0A0B;
const USHORT ASIC_VGH_ADDR = 0x0A0C;
const USHORT ASIC_VGL_ADDR = 0x0A0D;
const USHORT ASIC_VBH_ADDR = 0x0A0E;
const USHORT ASIC_VBL_ADDR = 0x0A0F;  

typedef enum {
	CUSTOM_SLIDE,				// 视频正常输出
	CUSTOM_NEGATIVE,			// 视频以底片方式输出
	CUSTOM_BW					// 视频以"黑白(灰度)方式输出
	} CUSTOMVIDEOOUTTYPE;

typedef enum {
	IMAGESAVE_JPEG,
	IMAGESAVE_BMP
	} CUSTOMIMAGESAVETYPE;

typedef enum {
	IMAGESCANQUALITY_1800dpi,
	IMAGESCANQUALITY_3600dpi
	} CUSTOMIMAGESCANQUALITY;

// Film Type
enum {
	SET_ASIC_Negative = 0,
	SET_ASIC_Slide,
	SET_ASIC_BW
	};

// Device Type
typedef enum{ 	
	DEVTYPE_FS_900 = 0,	 	
	DEVTYPE_FS_500,
	DEVTYPE_FS_300,
	DEVTYPE_FS_Ohter
	} CUSTOMDEVICETYPE;  

typedef struct  
	{
	BYTE *pBuffer0;
	BYTE *pBuffer1;
	SIZE sizeBuf0;
	SIZE sizeBuf1;
	ULONG ulBufLen0;
	ULONG ulBufLen1;
	BOOL bIsTwoBuffer;
	} CUSBUFFER;

// UI change conditions
typedef enum{
	UICOT_TargetVideoDevice,	
	UICOT_NoTargetVideoDevice,
	UICOT_NoVideoDevice,
	UICOT_OpenVideoFailed,
	UICOT_SnapshotStart,
	UICOT_SnapshotFinish,
	UICOT_ImageSaveStart,
	UICOT_ImageSaveFinish,
	UICOT_APWaitforExit,
	UICOT_TimeOut
	} UIChangeOperateType;	

typedef enum{
	YUV_YUV = 0,
	YUV_Y,
	YUV_U,
	YUV_V
	} YUVChannelType;

typedef enum{
	NEGATIVEIMP_ReverseRGBGamma = 0,
	NEGATIVEIMP_255SubRGBData
	} NEGATIVEIMPType;

typedef struct  
	{
	BYTE *pBitmap;
	CSize siBitmap;
	CString strImgSavePath;
	CUSTOMDEVICETYPE cusDevType;
	CUSTOMIMAGESCANQUALITY cusImgScanQuality;
	} IMAGETHREADINFO, *PIMAGETHREADINFO; 

typedef struct  
	{
	int CMt[3][3];
	} MyColorMatrix;

// Add for test
#define MyProcTimeRecord				0 
#define MyRawUtilParsUpdateOnlyOnce		0
#define My5MRawPreview					0  
#define MyEnableImageViewer				0
#define MySetFilmTypeHide				0	
#define MySaveRawDataToFile				0
#define MySaveYUY2ToFile				0 
#define MyInterpolation					1
#define MyNotifySetDlgUpdate			0	

#define MyKeepYEdgePars					1		// 按客户要求将Raw "YEdgeGain"和"YEdgeThd"固定为"63"和"8"

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



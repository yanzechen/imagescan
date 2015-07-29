// ImageScanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageScan.h"
#include "ImageScanDlg.h"
#include "PicConver.h"  
#include "ReadWriteASIC.h"
#include <math.h>

#define Timer_NotifyUpdate_ID	1 

DWORD WINAPI ImageThreadProc(LPVOID lpParamter);
void Expandimage(BYTE *pInBuf, BYTE *pOutBuf, int iInWidth, int iInHeight, int iOutWidth, int iOutHeight);	

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
//
// 
// global variable
// 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Video 
// +++++
BYTE* g_lpBits = NULL;
BITMAPINFOHEADER g_ihdr;
CEvent *g_event = new CEvent(FALSE, FALSE);		// 创建一个"初始无信号, 自动重置"的事件对象
BOOL g_bIsProcFinished = TRUE;					// 用于确认"CaptureThread()"是否处理完相应任务
// -----  

// Still
// +++++
BYTE* g_lpStillBits = NULL;
BITMAPINFOHEADER g_Stillihdr;
HWND ghwndApp = NULL;
CUSTOMDEVICETYPE g_cusDevType = DEVTYPE_FS_Ohter;
// -----

CRawPars gPars;
CRawUtil gRawUtil;

#if MySaveRawDataToFile
CString gstrRawFileSavePath;  
#endif

long g_lBufferLen = 0;
BYTE *g_pBuffer = NULL;
AM_MEDIA_TYPE *g_pmt = NULL;

// -----------------------------------------------------------------------

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CImageScanDlg dialog

CImageScanDlg::CImageScanDlg(CWnd* pParent /*=NULL*/)
: CDialog(CImageScanDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < STOREIMAGEMAXNUM; i++)
	{		
		m_ImgsStaticArray[i] = NULL;	
		m_ThumbImgsArray[i] = NULL;		
		m_ProcThreadHand[i] = NULL;
	}	

	m_iStoredImgNum			= 0;
	m_iSelectedImgIdx		= -1;
	g_lpBits				= NULL; 
	m_pVideoFrame			= NULL;	 
	m_pBKImg				= NULL;	 
	m_ImgSaveType			= IMAGESAVE_JPEG;	 
	m_bCBADlgFristShow		= TRUE;		   
	m_RawPars				= NULL;

	
}

CImageScanDlg::~CImageScanDlg()
{
	SAFEDELETEARRAY(g_lpBits);
	SAFEDELETEARRAY(g_lpStillBits);	
	SAFEDELETE(m_pBKImg);  
	for (int i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		SAFEDELETEARRAY(m_ThumbImgsArray[i]);
	} 
	DetachRawParsPointer();
}

void CImageScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_LIVEPREVIEW, m_LivePreviewWnd);
	DDX_Control(pDX, IDC_PIC_SNPSHOTPREVIEW, m_ImgPreviewWnd);
	DDX_Text(pDX, IDC_EDIT_IMAGESAVE, m_strImgSaveDir);
	DDX_Control(pDX, IDC_STATIC_PHOTOSBACK, m_BKImgStatic);
}

BEGIN_MESSAGE_MAP(CImageScanDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP	  	
	ON_WM_LBUTTONDOWN()	 	
	ON_WM_CLOSE()	  	 	
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUT_RAWPROPERTYPAGE, &CImageScanDlg::OnBnClickedButRawpropertypage)  
	ON_BN_CLICKED(IDC_BUT_SETSAVEPATH, &CImageScanDlg::OnBnClickedButSetsavepath)
	ON_BN_CLICKED(IDC_BUT_SNAPSHOT, &CImageScanDlg::OnBnClickedButSnapshot)
	ON_BN_CLICKED(IDC_BUT_DELETE, &CImageScanDlg::OnBnClickedButDelete)
	ON_BN_CLICKED(IDC_BUT_HMIRROR, &CImageScanDlg::OnBnClickedButHmirror) 
	ON_BN_CLICKED(IDC_BUT_RROTATE, &CImageScanDlg::OnBnClickedButRrotate)
	ON_BN_CLICKED(IDC_BUT_IMAGESAVE, &CImageScanDlg::OnBnClickedButImagesave)
	ON_BN_CLICKED(IDC_BUT_EXIT, &CImageScanDlg::OnBnClickedButExit)
	ON_BN_CLICKED(IDC_RADIO_DPI0, &CImageScanDlg::OnBnClickedRadioDpi0)
	ON_BN_CLICKED(IDC_RADIO_DPI1, &CImageScanDlg::OnBnClickedRadioDpi1)
	ON_BN_CLICKED(IDC_RADIO_PREVIEWTYPE_NEGATIVE, &CImageScanDlg::OnBnClickedRadioPreviewtypeNegative)
	ON_BN_CLICKED(IDC_RADIO_PREVIEWTYPE_SLIDE, &CImageScanDlg::OnBnClickedRadioPreviewtypeSlide)
	ON_BN_CLICKED(IDC_RADIO_PREVIEWTYPE_BW, &CImageScanDlg::OnBnClickedRadioPreviewtypeBw)
	ON_MESSAGE(WM_GETSTILLBUFFER, &CImageScanDlg::OnGetStillBuffer)	 	
	ON_BN_CLICKED(IDC_BUT_COLORBRIGHTNESSAJUST, &CImageScanDlg::OnBnClickedButColorbrightnessajust)
END_MESSAGE_MAP()


// CImageScanDlg message handlers

BOOL CImageScanDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here	
	Init_Create();
	Init_Paramters();
	Init_WndState(); 
	Init_VideoDevice();		

	UpdateData(FALSE);	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImageScanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImageScanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

		ImageRefresh();	
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CImageScanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CImageScanDlg::Init_Create()
{ 
	//创建白色画刷
	m_EditBKBrush.CreateSolidBrush(RGB(255, 255, 255));

	// 创建状态栏
	CreateStatusBar();

	// 创建图片预览窗口
#if MyEnableImageViewer
	m_ImgViewer.Create(IDD_IMAGEVIEWER_FRAME, this);	
#endif

	// 创建SettingDlg
	m_SettingDlg.Create(IDD_SETTING, this);
	m_SettingDlg.AttchRawParsPointer(&gPars);

	// 创建Brightness/Color Ajustment对话框
	m_CBADlg.Create(IDD_COLORBRIGHTNESSAJUSTMENT, this);
	m_CBADlg.AttchRawParsPointer(&gPars);  

	// 创建存放拍照照片的临时文件目录
	m_strTmpDirName.LoadString(IDS_IMAGETMPDIR);
	CreateTempDirectory(m_strTmpDirName, m_strTmpPath); 	
}

void CImageScanDlg::Init_Paramters()
{	
	AttchRawParsPointer(&gPars);
	InitRawPars();

	// 获取拍照预览窗口Size
	m_ImgPreviewWnd.GetClientRect(&m_rcImgPreviewWnd);

	// ImageSavPath 默认设置为"我的文档->收藏夹"	
	CString strSpecialPath;
	SHGetSpecialFolderPath(m_hWnd, strSpecialPath.GetBuffer(MAX_PATH*2), CSIDL_PERSONAL, FALSE);
	strSpecialPath.ReleaseBuffer();		
	strSpecialPath += _T("\\My Pictures");
	m_strImgSaveDir = strSpecialPath;  

#if MySaveRawDataToFile
	gstrRawFileSavePath = m_strImgSaveDir;
#endif

	// 获取ListImageStatic的指针 	
	m_ImgsStaticArray[0] = (CStatic *)GetDlgItem(IDC_PIC_LIST0);
	m_ImgsStaticArray[1] = (CStatic *)GetDlgItem(IDC_PIC_LIST1);
	m_ImgsStaticArray[2] = (CStatic *)GetDlgItem(IDC_PIC_LIST2);
	m_ImgsStaticArray[3] = (CStatic *)GetDlgItem(IDC_PIC_LIST3);
	m_ImgsStaticArray[4] = (CStatic *)GetDlgItem(IDC_PIC_LIST4);
	m_ImgsStaticArray[5] = (CStatic *)GetDlgItem(IDC_PIC_LIST5);
	m_ImgsStaticArray[6] = (CStatic *)GetDlgItem(IDC_PIC_LIST6);
	m_ImgsStaticArray[7] = (CStatic *)GetDlgItem(IDC_PIC_LIST7);
	m_ImgsStaticArray[8] = (CStatic *)GetDlgItem(IDC_PIC_LIST8);
	m_ImgsStaticArray[9] = (CStatic *)GetDlgItem(IDC_PIC_LIST9);
	m_ImgsStaticArray[10] = (CStatic *)GetDlgItem(IDC_PIC_LIST10);
	m_ImgsStaticArray[11] = (CStatic *)GetDlgItem(IDC_PIC_LIST11);

	m_pBKImg = Bitmap::FromResource((HINSTANCE)GetModuleHandle(NULL), L"PHOTOSBACKU");	  
	ghwndApp = m_hWnd; 
}

void CImageScanDlg::Init_WndState()
{ 	
	// Set AP title
	CString strMainWndTitle;
	strMainWndTitle.LoadString(IDS_APTITLE);
	SetWindowText(strMainWndTitle);

	// 图片ScanQuality默认为1800dpi
	CheckDlgButton(IDC_RADIO_DPI0, BST_CHECKED);
	m_ImgScanQuality = IMAGESCANQUALITY_1800dpi;

	// Bitmap Button
	m_BtnSnapshot.AutoLoad(IDC_BUT_SNAPSHOT, this);
	m_BtnDelete.AutoLoad(IDC_BUT_DELETE, this);
	m_BtnHMirror.AutoLoad(IDC_BUT_HMIRROR, this);
	m_BtnRRotate.AutoLoad(IDC_BUT_RROTATE, this);
	m_BtnExit.AutoLoad(IDC_BUT_EXIT, this);
	m_BtnImageSave.AutoLoad(IDC_BUT_IMAGESAVE, this);
	m_BtnBrowse1.AutoLoad(IDC_BUT_SETSAVEPATH, this);
	m_BtnBrowse2.AutoLoad(IDC_BUT_SETRAWSAVEPATH, this);
	m_BtnPhotosBack.AutoLoad(IDC_BUT_PHOTOSBACK, this);	
	m_BtnRawPage.AutoLoad(IDC_BUT_RAWPROPERTYPAGE, this);  

	// 移动"Static"到照片集背景图片框中
	// +++
	CRect rcPhotosBack;
	CRect rcPic0; 	
	const int iPBWid = 66;
	const int iPBHei = 48;
	const int iLeftOffset = 63;
	const int iTopOffset = 16;
	const int iBottomOffset = 14;
	const int iNextOffset = 5;	

	m_BKImgStatic.GetWindowRect(&rcPhotosBack);
	rcPic0 = rcPhotosBack;

	rcPic0.left += iLeftOffset;
	rcPic0.top += iTopOffset;
	rcPic0.right = rcPic0.left + iPBWid;
	rcPic0.bottom = rcPic0.top + iPBHei; 	
	ScreenToClient(&rcPic0);

	for (int i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		m_ImgsStaticArray[i]->MoveWindow(&rcPic0, TRUE);

		rcPic0.left += iPBWid + iNextOffset;
		rcPic0.right += iPBWid + iNextOffset;	  

		if (6 == i || 9 == i || 11 == i)
		{
			rcPic0.left -= 1;
			rcPic0.right -= 1;
		} 		
	}
	// ---		
}

void CImageScanDlg::Init_VideoDevice()
{
	// 选择指定设备
	// +++++
	const int MAXDETECTDEVNUM = 10;
	UINT iMonikerIdx = 0;
	IMoniker *pMoniker = NULL;
	CString strDevVid;
	CString strDevPid285;
	CString strDevPid289;
	BOOL bPid285 = FALSE;
	BOOL bPid289 = FALSE;

	strDevVid.LoadString(IDS_TARGETDEVICEVID);
	strDevPid285.LoadString(IDS_TARGETDEVICEPID285); 
	strDevPid289.LoadString(IDS_TARGETDEVICEPID289); 

	BOOL bFindVideoDevice = FALSE;
	BOOL bFindTargetVideoDevice = FALSE;	
	int i = 0;	

	// 查找指定视频采集设备
	for (i = 0; i < MAXDETECTDEVNUM; i++)
	{
		if (EnumVideoDevice(i, &pMoniker) && NULL != pMoniker)
		{
			bFindVideoDevice = TRUE;
			bPid285 = m_UDsUtils.IsSpecifyDevVidPidExsit(pMoniker, strDevVid, strDevPid285);
			bPid289 = m_UDsUtils.IsSpecifyDevVidPidExsit(pMoniker, strDevVid, strDevPid289);
			if (bPid285 || bPid289)
			{
				iMonikerIdx = i;
				bFindTargetVideoDevice = TRUE;
				SAFERELEASE(pMoniker);
				break;
			}
			SAFERELEASE(pMoniker);
		}
	}
	// -----

	// 视频设备不存在
	if (!bFindVideoDevice)
	{
		CString str;
		str.LoadString(IDS_STRING134);
		MessageBox(str, NULL, MB_OK | MB_ICONINFORMATION);
		UIChangeOperate(UICOT_NoVideoDevice);
		return;
	}

	// 如果目的设备存在则使用目的设备, 如果目的设备不存在, 则使用设备列表第一个设备
	if (!bFindTargetVideoDevice)
	{ 
		m_DevType = DEVTYPE_FS_Ohter; 
		CString strNotFindTargetDevText;
		strNotFindTargetDevText.LoadString(IDS_NOTFINDTARGETDEVTEXT);
		int iRet = MessageBox(strNotFindTargetDevText, NULL, MB_YESNO | MB_ICONINFORMATION);
		if (IDYES == iRet)
		{
			UIChangeOperate(UICOT_NoTargetVideoDevice);
			iMonikerIdx = 0;
		}
		else
		{
			UIChangeOperate(UICOT_NoVideoDevice);
			return;
		}
	}	

	// 设定要查找的设备Idx
	m_DXCapture.SetMonikerIdx(iMonikerIdx);
	// 构建Graph时, 插入SampleGrabberFilter
	m_DXCapture.UseSampleGrabberFilter();
	m_DXCapture.SetRenderHwnd(m_LivePreviewWnd.m_hWnd); 	
	// 构建StillPinGraph  
	m_DXCapture.UseStillPinGrabber();
	// 枚举系统视频设备, 并开启视频预览
	if (!m_DXCapture.StartCapture())
	{
		CString strText;
		strText.LoadString(IDS_OPENVIDEDEVFAIL_INFO);
		MessageBox(strText, NULL, MB_OK | MB_ICONINFORMATION);
		UIChangeOperate(UICOT_OpenVideoFailed);
		return;
	}

	if (bFindVideoDevice)
	{
		// 获取设备型号
		int iDevType = GetFirmwareCode(); 
		if (bFindTargetVideoDevice)
		{ 
			if (bPid285)
			{
				switch (iDevType)
				{				
				case 1:
					m_DevType = DEVTYPE_FS_500;
					break;
				case 2:
					m_DevType = DEVTYPE_FS_300;
					break;
				default:
					m_DevType = DEVTYPE_FS_Ohter;
					break; 
				} 
			}
			else
			{
				switch (iDevType)
				{				
				case 0:
					m_DevType = DEVTYPE_FS_900;
					break;				
				default:
					m_DevType = DEVTYPE_FS_Ohter;
					break; 
				} 				
			}

		}
		else
		{
			m_DevType = DEVTYPE_FS_Ohter;
		} 

		g_cusDevType = m_DevType; 
	} 

	// 检测到视频设备后需执行步骤
	// +++++ 
	// 如果当前使用的设备非"FS-300", "FS-500"或"FS-900", 则不能使用"FilmType"功能
	// 并且默认的输出方式为"Slide"
	if (DEVTYPE_FS_Ohter == m_DevType)
	{ 		
		UIChangeOperate(UICOT_NoTargetVideoDevice);
	}
	else
	{			
		UIChangeOperate(UICOT_TargetVideoDevice);
	}

	m_CBADlg.InitBrightnessAndRGBGain();		
	// -----
}

void CImageScanDlg::OnBnClickedButSnapshot()
{
	// TODO: Add your control notification handler code here 
	if (STOREIMAGEMAXNUM == m_iStoredImgNum)
	{
		CString str;
		str.LoadString(IDS_IMAGE_FULL);
		MessageBox(str, NULL, MB_OK | MB_ICONINFORMATION);
		return;
	}
	ASSERT(m_iStoredImgNum < STOREIMAGEMAXNUM);

	if (!IsFilePathExist(m_strTmpPath))
	{
		BOOL bRet = CreateTempDirectory(m_strTmpDirName, m_strTmpPath);
		if (!bRet)
		{
			MessageBox(_T("Create temporary file directory failed!"), NULL, MB_OK | MB_ICONERROR);
			return;
		}
	}

	if (!m_DXCapture.StillPinSnapShot())
	{
		MessageBox(_T("Snapshot failed!"), NULL, MB_OK | MB_ICONINFORMATION);
		return;
	}

	UIChangeOperate(UICOT_SnapshotStart);
	// yanze_chen changed to string table 2015-06-12
	CString str;
	str.LoadString(IDS_SNAPSHOT);
	m_StatusCtrl.SetText(str, 0, 0);
}

BOOL CImageScanDlg::ShowSelectedImageInImageList(int iIdx)
{
	if (iIdx > -1 && iIdx < STOREIMAGEMAXNUM)
	{
		Bitmap *pImage = m_ThumbImgsArray[iIdx];
		if (NULL != pImage)
		{
			Graphics gc(m_ImgsStaticArray[iIdx]->m_hWnd);
			CRect rcClt;
			m_ImgsStaticArray[iIdx]->GetClientRect(&rcClt);			 

			if (pImage->GetWidth() >= pImage->GetHeight())
			{  				
				gc.DrawImage(pImage, Rect(0, 0, rcClt.Width(), rcClt.Height())); 				
			}
			else
			{
				INT iWid = (INT)((rcClt.Height()) * (pImage->GetWidth()/(pImage->GetHeight()*1.0f)));
				INT iHei = rcClt.Height();
				INT iLeft = (INT)((rcClt.Width() - iWid) / 2);
				INT iTop = 0;

				gc.Clear(Color::Black);
				gc.DrawImage(pImage, Rect(iLeft, iTop, iWid, iHei));					
			}	
		}
	}
	return TRUE;
}

BOOL CImageScanDlg::ShowAllImagesInImageList()
{	
	for (int i = 0; i < STOREIMAGEMAXNUM; i++)
	{	
		ShowSelectedImageInImageList(i);	 		
	}
	return TRUE;
}  

BOOL CImageScanDlg::ShowSelectedImageInSnapshotPreview(int iIdx)
{
	if ( (iIdx >= 0)
		&& (iIdx < STOREIMAGEMAXNUM) 
		&& (!m_ImgPathArray[iIdx].IsEmpty()))
	{
		Bitmap *pImage = m_ThumbImgsArray[iIdx];
		if (NULL != pImage)
		{
			Graphics gc(m_ImgPreviewWnd.m_hWnd);				

			if (pImage->GetWidth() >= pImage->GetHeight())
			{ 				
				gc.DrawImage(pImage, Rect(0, 0, m_rcImgPreviewWnd.Width(), m_rcImgPreviewWnd.Height())); 					
			}
			else
			{ 				
				INT iWid = (INT) ( (m_rcImgPreviewWnd.Height()) * (pImage->GetWidth()/(pImage->GetHeight()*1.0f)) );
				INT iHei = m_rcImgPreviewWnd.Height();
				INT iLeft = (m_rcImgPreviewWnd.Width() - iWid) / 2;
				INT iTop = 0;

				gc.Clear(Color::Black);
				gc.DrawImage(pImage, Rect(iLeft, iTop, iWid, iHei));					
			}		 		
		}
	}
	return TRUE;
}

void CImageScanDlg::OnBnClickedButDelete()
{
	// TODO: Add your control notification handler code here
	if ( (m_iSelectedImgIdx >= 0)
		&& (m_iSelectedImgIdx < STOREIMAGEMAXNUM) 
		&& (!m_ImgPathArray[m_iSelectedImgIdx].IsEmpty()) )
	{	
		SAFEDELETE(m_ThumbImgsArray[m_iSelectedImgIdx]);

		// 文件有可能被用户在AP之外删除(或者文件还没有产生)
		if (IsFilePathExist(m_ImgPathArray[m_iSelectedImgIdx]))
		{
			DeleteFile(m_ImgPathArray[m_iSelectedImgIdx]);
		}
		m_ImgPathArray[m_iSelectedImgIdx].Empty(); 		

		// 删除的照片如果不是最后一个, "高亮"边框不需移动
		if (m_iSelectedImgIdx == (m_iStoredImgNum-1))
		{
			m_iSelectedImgIdx--;
		}

		// 删除一幅图片后, 其对应的处理线程句柄也要被关闭
		if (NULL != m_ProcThreadHand[m_iStoredImgNum])
		{
			CloseHandle(m_ProcThreadHand[m_iStoredImgNum]);
			m_ProcThreadHand[m_iStoredImgNum] = NULL;
		}

		m_iStoredImgNum--;		

		ListImageOrderAjust();		
		ImageRefresh();		
	}	
	else
	{
		MessageBox(_T("Please select one image!"), NULL, MB_OK | MB_ICONINFORMATION);
	}
}

void CImageScanDlg::OnBnClickedButHmirror()
{
	// TODO: Add your control notification handler code here
	RotateImage(RotateNoneFlipX);
	ImageRefresh_AfterRotate();	 	
}

void CImageScanDlg::OnBnClickedButRrotate()
{
	// TODO: Add your control notification handler code here
	RotateImage(Rotate90FlipNone);
	ImageRefresh_AfterRotate();	   	
}

void CImageScanDlg::OnBnClickedButImagesave()
{
	// TODO: Add your control notification handler code here
	// step1:
	// 确认"m_strImgSaveDir"指向的路径是否存在, 如果不存在提示
	// 用户选择正确的保存目录
	UpdateData(TRUE);

	CString strText;
	if (m_strImgSaveDir.IsEmpty())
	{
		strText = _T("Please select a valid path!");
		MessageBox(strText, NULL, MB_OK | MB_ICONINFORMATION);
		return;
	}
	else if (!IsFilePathExist(m_strImgSaveDir))
	{ 	
		strText.Format(_T("\"%s\" is not exist, \r\n\r\n please select a valid path!"), m_strImgSaveDir);
		MessageBox(strText, NULL, MB_OK | MB_ICONINFORMATION);
		return;
	}

	if (0 == m_iStoredImgNum)
	{
		CString str;
		str.LoadString(IDS_STRING135);
		MessageBox(str, NULL, MB_OK | MB_ICONINFORMATION);
		return;
	}

	WatiForPicturesProcFinished();

	UIChangeOperate(UICOT_ImageSaveStart);
	CString str;
	str.LoadString(IDS_SAVEIMAGE);
	m_StatusCtrl.SetText(str, 0, 0);
	m_StatusCtrl.SetRange(0, GetImagesCount()-1);
	m_StatusCtrl.SetPos(0);
	m_StatusCtrl.ShowProgressBar(SW_SHOW);

	// step2: 
	// 保存文件时, 为防止出现异常错误, 禁止用户进行其他按钮操作
	// 保存文件: 保存格式为jpg, 文件名称"当前日期+图片索引号"  	
	SaveImageToFile();	

	// step3:
	// 保存完后, 清除保存的图像指针以及当前显示
	AfterSaveImageClear();

	UIChangeOperate(UICOT_ImageSaveFinish);
	m_StatusCtrl.ShowProgressBar(SW_HIDE);
	str.LoadString(IDS_READY);
	m_StatusCtrl.SetText(str, 0, 0);
}

DWORD CImageScanDlg::WatiForPicturesProcFinished()
{
	//
	// 等待所有处理线程结束
	// 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int iValidThreadCnt = 0;
	int i = 0;
	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		if (NULL != m_ProcThreadHand[i])
		{
			iValidThreadCnt++;
		}
	}

	if (0 == iValidThreadCnt)
	{
		return 0;
	}

	HANDLE* pHandles = new HANDLE[iValidThreadCnt];
	ASSERT(NULL != pHandles);

	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		if (NULL != m_ProcThreadHand[i])
		{
			pHandles[i] = m_ProcThreadHand[i];
		}
	}

	CString str;
	str.LoadString(IDS_PICTUREPROC);
	m_StatusCtrl.SetText(str, 0, 0);

	DWORD dwRet = WaitForMultipleObjects(iValidThreadCnt, pHandles, TRUE, 5000);

	str.LoadString(IDS_PICTUREPROCFNI);
	m_StatusCtrl.SetText(str, 0, 0);

	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		if (NULL != m_ProcThreadHand[i])
		{
			CloseHandle(m_ProcThreadHand[i]);
			m_ProcThreadHand[i] = NULL;
		}
	}	
	// -------------------------------------------------------------

	return dwRet;
}

BOOL CImageScanDlg::IsFilePathExist(LPCTSTR lpFilePath)
{
	CFileFind fileFind;	 
	BOOL bRet = fileFind.FindFile(lpFilePath);	
	if (bRet)
	{
		fileFind.Close();
	}
	return bRet;
}

int CImageScanDlg::GetImagesCount()
{
	int iImagesCnt = 0;

	for (int i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		if (!m_ImgPathArray[i].IsEmpty() && IsFilePathExist(m_ImgPathArray[i]))
		{
			iImagesCnt++;
		}
	}

	return iImagesCnt;
}

void CImageScanDlg::SaveImageToFile()
{
	CString strSaveFileNamePrefix = GetCurrentDateString();

	CString strSaveFileName;
	CString strSaveFileNamePostfix;
	CString strSaveFilePath;	

	CLSID clImageClsid;

	CString strSaveType;
	CString strSaveFileExtName;
	CString strStatusBarText;

	if (IMAGESAVE_JPEG == m_ImgSaveType)
	{
		strSaveType = _T("image/jpeg");
		strSaveFileExtName = _T("jpg");
	}
	else
	{
		strSaveType = _T("image/bmp");
		strSaveFileExtName = _T("bmp");
	}

	CPicConver PicConver;
	BOOL bRet = PicConver.GetImageCLSID((LPCWSTR)strSaveType.AllocSysString(), &clImageClsid);
	if (!bRet)
	{
		CString strErr;
		strErr.Format(_T("GetImageCLSID failed!"));
		MessageBox(strErr, NULL, MB_OK | MB_ICONERROR);
		return;	
	}

	int iImagesCnt = GetImagesCount();
	int iSavedCnt = 0;

	int iSaveNum = 0;

	for (int i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		// haifeng_liu 2010/12/7 modify
		if (!m_ImgPathArray[i].IsEmpty() && IsFilePathExist(m_ImgPathArray[i]))
		{	
			// 在保存照片时, 处理窗口其他消息(如: WM_PAINT)
			PumpMessages();

RenameLable:
			if (iSaveNum > STOREIMAGEMAXNUM)
			{
				iSaveNum = 0;
			}
			iSaveNum++;
			strSaveFileNamePostfix.Format(_T("_%02d.%s"), iSaveNum, strSaveFileExtName);
			strSaveFileName = strSaveFileNamePrefix + strSaveFileNamePostfix;
			strSaveFilePath = m_strImgSaveDir + _T("\\") + strSaveFileName; 

			// 如果存在重名文件, 则重新生成新文件名(在原来基础上递增序列号)
			if (IsFilePathExist(strSaveFilePath))
			{
				goto RenameLable;
			}

			Bitmap *pImage = NULL;
			pImage = Bitmap::FromFile(m_ImgPathArray[i].AllocSysString());
			ASSERT(NULL != pImage);	
			if (NULL == pImage)
			{
				CString strErr;
				strErr.Format(_T("Save \"%s\" failed, \"pImge == NULL\""), strSaveFilePath);
				MessageBox(strErr, NULL, MB_OK | MB_ICONERROR);
				return;
			}

			// Rotate Image
			// +++++			
			pImage->RotateFlip(m_ImgRFRecArray[i].GetRecord());
			// -----

			Status st = pImage->Save((LPCWSTR)strSaveFilePath.AllocSysString(), &clImageClsid, NULL);
			if (Ok != st)
			{
				CString strErr;
				strErr.Format(_T("Save \"%s\" failed, Error code:%d"), strSaveFilePath, st);
				MessageBox(strErr, NULL, MB_OK | MB_ICONERROR);
			}
			SAFEDELETE(pImage);

			iSavedCnt++;
			strStatusBarText.Format(_T("Saved %d image to disk, left %d image......"), iSavedCnt, iImagesCnt-iSavedCnt);
			m_StatusCtrl.SetText(strStatusBarText, 0, 0, iSavedCnt-1);
		}
	}
}

void CImageScanDlg::AfterSaveImageClear()
{
	int i = 0;

	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		if (!m_ImgPathArray[i].IsEmpty())
		{			
			if (IsFilePathExist(m_ImgPathArray[i]))
			{
				DeleteFile(m_ImgPathArray[i]);
			}
			m_ImgPathArray[i].Empty();
		} 	

		SAFEDELETE(m_ThumbImgsArray[i]);
	}

	m_iStoredImgNum = 0;
	m_iSelectedImgIdx = -1;

	for (int i = 0; i < STOREIMAGEMAXNUM; i++)
	{		
		m_ImgRFRecArray[i].ResetRecord();
	}  

	Invalidate(TRUE);
	ImageRefresh();
} 

void CImageScanDlg::OnBnClickedButExit()
{
	// TODO: Add your control notification handler code here
	PostMessage(WM_CLOSE);		
}

void CImageScanDlg::OnBnClickedButSetsavepath()
{
	// TODO: Add your control notification handler code here
	CString str;
	str.LoadString(IDS_STRING133);
	CString strPath = BrowseDirectory(m_hWnd, str);

	if (!strPath.IsEmpty())
	{
		m_strImgSaveDir = strPath;
	} 

	UpdateData(FALSE);
}

LRESULT  CImageScanDlg::OnGetStillBuffer(WPARAM wparam, LPARAM lparam)
{ 
	if (STOREIMAGEMAXNUM == m_iStoredImgNum)
	{
		CString strMsg;
		strMsg.LoadString(IDS_IMAGE_FULL);
		MessageBox(strMsg, NULL, MB_OK | MB_ICONINFORMATION);
		return E_FAIL;
	}
	ASSERT(m_iStoredImgNum < STOREIMAGEMAXNUM);

	UIChangeOperate(UICOT_SnapshotFinish);
	CString str;
	str.LoadString(IDS_READY);
	m_StatusCtrl.SetText(str, 0, 0);  

	// CBADlg中"R/G/BGain"调节实际是通过修改F/W ColorMatrix值来实现的
	// 由于F/W的特殊做法, 拍完照后, F/W ColorMatrix值会变成F/W默认初始值
	// 所以AP端需要重新根据当前"R/G/BGain"值, 将新的ColorMatrix值重新下下去
	// 只有FS-900, FS-500, FS-300支持此操作
	if (DEVTYPE_FS_Ohter != m_DevType)
	{
		m_CBADlg.SetRGBGain();	
	}

	// 以".bmp"格式保存图片到临时缓冲区	  
	if (!IsFilePathExist(m_strTmpPath))
	{
		BOOL bRet = CreateTempDirectory(m_strTmpDirName, m_strTmpPath);
		if (!bRet)
		{
			SAFEDELETEARRAY(g_lpStillBits);			// 及时释放上一次占用内存
			CString strMsg;
			strMsg.LoadString(IDS_CREATETMPDIR_FAILED);
			MessageBox(strMsg, NULL, MB_OK | MB_ICONERROR); 			
			return E_FAIL;
		}
	}

	// haifeng_liu 2010/12/16 add 
	// +++++	 
	Bitmap bitmap((BITMAPINFO*)&g_Stillihdr, g_lpStillBits);				 

	CString strSaveFilePath;
	CString strSaveFileName;
	strSaveFileName.Format(_T("Tmp%d.bmp"), m_iStoredImgNum);
	strSaveFilePath = m_strTmpPath + _T("\\") + strSaveFileName;
	int iIdx = m_iStoredImgNum;
	m_ImgPathArray[iIdx] = strSaveFilePath;  

	SAFEDELETEARRAY(m_ThumbImgsArray[iIdx]); 	
	m_ThumbImgsArray[iIdx] = (Bitmap*)bitmap.GetThumbnailImage(m_rcImgPreviewWnd.Width(), m_rcImgPreviewWnd.Height(), NULL, NULL);		

	PIMAGETHREADINFO pImageInfo = new IMAGETHREADINFO;
	pImageInfo->pBitmap = g_lpStillBits;
	g_lpStillBits = NULL;

	pImageInfo->siBitmap.cx = g_Stillihdr.biWidth;
	pImageInfo->siBitmap.cy = g_Stillihdr.biHeight;

	pImageInfo->strImgSavePath = m_ImgPathArray[iIdx];
	pImageInfo->cusDevType = m_DevType;
	pImageInfo->cusImgScanQuality = m_ImgScanQuality;	 	

	// 拍得一幅照片, 照片计数加1
	m_iStoredImgNum++;

	// 确定那幅图片要在Snapshot Preview中显示
	m_iSelectedImgIdx = m_iStoredImgNum - 1;

	HANDLE handle = CreateThread(NULL, 0, ImageThreadProc, pImageInfo, 0, NULL); 
	m_ProcThreadHand[m_iSelectedImgIdx] = handle;

	// step2:
	// 在ImageList中显示所有的照片
	ShowAllImagesInImageList();

	// step3:
	// 在Snapshot Preview中显示选定的照片
	ShowSelectedImageInSnapshotPreview(m_iSelectedImgIdx);

	// Step4:
	// "高亮"当前拍得的照片的Static边框
	HighlightSelectedImageStatic(m_iSelectedImgIdx); 

	return 0;
}

CString CImageScanDlg::BrowseDirectory(HWND hwnd, LPCTSTR lpTitle)
{
	TCHAR szDirectory[MAX_PATH] = {0};

	BROWSEINFO browseInfo;
	browseInfo.hwndOwner = hwnd;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = szDirectory;
	browseInfo.lpszTitle = lpTitle;
	browseInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	browseInfo.lpfn = NULL;
	browseInfo.lParam = NULL;
	browseInfo.iImage = 0;

	SHGetPathFromIDList(SHBrowseForFolder(&browseInfo), szDirectory);

	return szDirectory;
}

void CImageScanDlg::RotateImage(RotateFlipType rt)
{
	if ( (m_iSelectedImgIdx >= 0)
		&& (m_iSelectedImgIdx < STOREIMAGEMAXNUM) 
		&& (!m_ImgPathArray[m_iSelectedImgIdx].IsEmpty())
		&& (NULL != m_ThumbImgsArray[m_iSelectedImgIdx]) )
	{		
		m_ThumbImgsArray[m_iSelectedImgIdx]->RotateFlip(rt);
		m_ImgRFRecArray[m_iSelectedImgIdx].RotateFilp(rt);
	}
}

// 当用户删除一幅照片后, 要将删除的照片之后的照片向前移动
// 确保删除操作后, 在ListImageStatic中, 所有的照片都是连续
// 显示的
void CImageScanDlg::ListImageOrderAjust()
{
	int i = 0;
	int iIdx = 0;	

	// Step1:

	// 定义一个临时的"Bitmap"指针数组  
	CString ImagePathArrayTmp[STOREIMAGEMAXNUM];  	
	CImageRotateFlipRecord ImgRFRecArrayTmp[STOREIMAGEMAXNUM];	  

	// 将"m_pImamgeArray"中的照片(不是"默认图片")按照顺序
	// 依次存入到"pImageArryTmp"中
	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		if (!m_ImgPathArray[i].IsEmpty())
		{
			ImagePathArrayTmp[iIdx] = m_ImgPathArray[i]; 			
			ImgRFRecArrayTmp[iIdx].SetRecord(m_ImgRFRecArray[i].GetRecord());
			iIdx++;
		}		
	}

	// 将排好顺序的图片指针(包括"照片"和"默认图片")存入到
	// "m_pImageArray"中
	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		m_ImgPathArray[i] = ImagePathArrayTmp[i];
		m_ImgRFRecArray[i] = ImgRFRecArrayTmp[i];
	}

	// Step2:  
	Bitmap *pThumbImgRrray[STOREIMAGEMAXNUM];
	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		pThumbImgRrray[i] = NULL;
	}

	iIdx = 0;
	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		if (NULL != m_ThumbImgsArray[i])
		{
			pThumbImgRrray[iIdx] = m_ThumbImgsArray[i];
			iIdx++;
		}
	}

	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		m_ThumbImgsArray[i] = pThumbImgRrray[i];
	}	
}

// 当用户选中"照片"时, 选中"照片"对应的Static的边框要高亮
void CImageScanDlg::HighlightSelectedImageStatic(int iIdx)
{
	ASSERT(iIdx < STOREIMAGEMAXNUM);

	// 在"高亮"此Static之前, 其他的"Static"可能也是"高亮"状态的,
	// 因此首先要去掉其他"Static"的"高亮"状态, 原本打算调用
	// "Invalidate(TRUE)", 重绘整个窗口, 就可以去掉Static"高亮"
	// 状态了, 但是"Invalidate(TRUE)"会使整个对话框出现"闪烁"现象,
	// 所以采用如下方法: 将"STOREIMAGEMAXNUM"的边框用"空画刷"绘制
	// 一遍, 既可达到目的又不会导致整个对话框出现"闪烁"现象
	for (int i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		CDC *pDC = m_ImgsStaticArray[i]->GetDC();
		CRect rcStatic;
		m_ImgsStaticArray[i]->GetClientRect(&rcStatic);
		rcStatic.InflateRect(1, 1, 1, 1);			
		pDC->FrameRect(&rcStatic, NULL);
		m_ImgsStaticArray[i]->ReleaseDC(pDC);
	}

	if ( (iIdx >= 0) && (iIdx < STOREIMAGEMAXNUM) )
	{
		CDC *pDC = m_ImgsStaticArray[iIdx]->GetDC();
		CRect rcStatic;
		m_ImgsStaticArray[iIdx]->GetClientRect(&rcStatic);
		rcStatic.InflateRect(1, 1, 1, 1);
		CBrush brush(RGB(0, 255, 0));
		pDC->FrameRect(&rcStatic, &brush);
		m_ImgsStaticArray[iIdx]->ReleaseDC(pDC);
	}
}
void CImageScanDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int i = 0;	

	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		CRect rcStatic;
		m_ImgsStaticArray[i]->GetWindowRect(&rcStatic);
		ScreenToClient(&rcStatic);
		if (PtInRect(&rcStatic, point)
			&& (!m_ImgPathArray[i].IsEmpty()))
		{ 				
			m_iSelectedImgIdx = i;
			HighlightSelectedImageStatic(m_iSelectedImgIdx);
			ShowSelectedImageInSnapshotPreview(m_iSelectedImgIdx);
			break;			
		}
	}	

	CDialog::OnLButtonDown(nFlags, point);
} 

void CImageScanDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default	
	WatiForPicturesProcFinished();

	// 保存"ImageSavePath"到"win.ini"文件中
	WriteProfileString(_T("Image Scan Tool"), _T("Image Save Path"), m_strImgSaveDir);

#if MyEnableImageViewer
	if (IsWindow(m_ImgViewer.m_hWnd))
	{
		m_ImgViewer.DestroyWindow();
	}
#endif	

	if (NULL != m_SettingDlg.m_hWnd)
	{
		m_SettingDlg.ShowWindow(SW_HIDE);
		m_SettingDlg.DestroyWindow();
	} 

	if (NULL != m_CBADlg.m_hWnd)
	{
		m_CBADlg.ShowWindow(SW_HIDE);
		m_CBADlg.DestroyWindow();
	}

	DestroyStatusBar();

	DeleteAllFilesAndDirectories(m_strTmpPath);

		

	CDialog::OnClose();
}


// 刷新所有图片
void CImageScanDlg::ImageRefresh()
{
	ShowImageListBackground();
	HighlightSelectedImageStatic(m_iSelectedImgIdx);
	ShowAllImagesInImageList();
	ShowSelectedImageInSnapshotPreview(m_iSelectedImgIdx);	
}

// 旋转图片时, 只需要刷新旋转的图片
void CImageScanDlg::ImageRefresh_AfterRotate()
{	
	ShowSelectedImageInImageList(m_iSelectedImgIdx);
	ShowSelectedImageInSnapshotPreview(m_iSelectedImgIdx);  
} 

void CImageScanDlg::OnBnClickedRadioPreviewtypeNegative()
{
	// TODO: Add your control notification handler code here   
	m_VideoOutType = CUSTOM_NEGATIVE;	 
	m_RawPars->bNegative = TRUE;
	m_RawPars->bBWMode = FALSE;
	if (!SetNegative())
	{
		MessageBox(_T("Set film type failed!"), NULL, MB_OK | MB_ICONINFORMATION);
	}
	 
	SetTimer(Timer_NotifyUpdate_ID, 100, NULL);
}

void CImageScanDlg::OnBnClickedRadioPreviewtypeSlide()
{
	// TODO: Add your control notification handler code here
	m_VideoOutType = CUSTOM_SLIDE;	
	m_RawPars->bNegative = FALSE;
	m_RawPars->bBWMode = FALSE;
	if (!SetSlide())
	{
		MessageBox(_T("Set film type failed!"), NULL, MB_OK | MB_ICONINFORMATION);
	}
	SetTimer(Timer_NotifyUpdate_ID, 100, NULL);
}

void CImageScanDlg::OnBnClickedRadioPreviewtypeBw()
{
	// TODO: Add your control notification handler code here
	m_VideoOutType = CUSTOM_BW; 	
	m_RawPars->bNegative = TRUE;
	m_RawPars->bBWMode = TRUE;
	if (!SetBW())
	{
		MessageBox(_T("Set film type failed!"), NULL, MB_OK | MB_ICONINFORMATION);
	}	

	SetTimer(Timer_NotifyUpdate_ID, 100, NULL);
}

void CImageScanDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
#if MyEnableImageViewer
	int i = 0;

	for (i = 0; i < STOREIMAGEMAXNUM; i++)
	{
		CRect rcStatic;
		m_ImgsStaticArray[i]->GetWindowRect(&rcStatic);
		ScreenToClient(&rcStatic);
		if (PtInRect(&rcStatic, point)
			&& (!m_ImgPathArray[i].IsEmpty()))
		{			
			HighlightSelectedImageStatic(i);
			ShowSelectedImageInSnapshotPreview(i);

			if (!m_ImgViewer.CusLoadImage(m_ImgPathArray[m_iSelectedImgIdx]))
			{
				MessageBox(_T("View image failed!"), NULL, MB_OK | MB_ICONINFORMATION);
				return;
			}
			m_ImgViewer.ShowWindow(SW_SHOW);

			break;			
		}
	}	
#endif

	CDialog::OnLButtonDblClk(nFlags, point);
} 

BOOL CImageScanDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class		

	return CDialog::PreTranslateMessage(pMsg);
}

void CImageScanDlg::OnBnClickedButRawpropertypage()
{
	// TODO: Add your control notification handler code here
	m_SettingDlg.ShowWindow(SW_SHOW);
}

// 按照自定义规则创建文件目录
// 指定要创建的目录名"lpDirName", 创建成功后, 返回创建的目录路径"strDirPath"
// 自定义规则: 先在系统目录"Program Files"下创建指定目录名的文件夹, 如果创建失败
// 则在程序当前目录创建
BOOL CImageScanDlg::CreateTempDirectory(LPCTSTR lpDirName, CString& strDirPath)
{
	ASSERT(NULL != lpDirName);
	if (NULL == lpDirName)
	{
		return FALSE;
	}

	//
	// 先在"系统目录->Program Files"下创建目录"lpDirName"
	//
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	BOOL bRet = FALSE;
	DWORD dwLastErr = 0;
	TCHAR szWindowsDir[MAX_PATH*2] = {0};
	TCHAR szDriver[MAX_PATH] = {0};

	GetWindowsDirectory(szWindowsDir, MAX_PATH*2);
	_tsplitpath_s(szWindowsDir, szDriver, MAX_PATH, NULL, 0, NULL, 0, NULL, 0);

	CString strDirToCreate;
	CString strTmp;

	strDirToCreate = szDriver;
	strDirToCreate += _T("\\");

	strDirToCreate += lpDirName;
	strDirPath = strDirToCreate;

	bRet = CreateDirectory(strDirToCreate, NULL);
	dwLastErr = GetLastError();	
	// 要创建的目录存在, 则直接返回"TRUE"
	if ( bRet || (ERROR_ALREADY_EXISTS == dwLastErr) )
	{
		SetFileAttributes(strDirPath, FILE_ATTRIBUTE_HIDDEN);
		return TRUE;
	}	
	// ---------------------------------------------------------

	//
	// 如果创建失败, 则在AP当前所在目录下创建"lpDirName"
	//
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	TCHAR szAPCurDir[MAX_PATH*2] = {0};
	TCHAR szAPFileName[MAX_PATH*2] = {0};

	GetModuleFileName(NULL, szAPFileName, MAX_PATH*2);

	strTmp = szAPFileName;
	strTmp = strTmp.Left(strTmp.ReverseFind(_T('\\'))+1);		// include "\"

	strDirToCreate = strTmp;
	strDirToCreate += lpDirName;
	strDirPath = strDirToCreate;

	bRet = CreateDirectory(strDirToCreate, NULL);
	dwLastErr = GetLastError();	
	// 要创建的目录存在, 则直接返回"TRUE"
	if ( bRet || (ERROR_ALREADY_EXISTS == dwLastErr) )
	{
		SetFileAttributes(strDirPath, FILE_ATTRIBUTE_HIDDEN);
		return TRUE;
	}
	// --------------------------------------------------------

	return bRet; 
}


// 递归删除指定目录中所有的文件及文件夹, 最后删除指定(根)目录
void CImageScanDlg::DeleteAllFilesAndDirectories(LPCTSTR lpStr)
{
	ASSERT(NULL != lpStr);
	if (NULL == lpStr)
	{
		return;
	}

	CFileFind cfFind;
	CString strWildcart(lpStr);
	strWildcart += _T("\\*.*");

	CString strTmp;
	BOOL bWorking = cfFind.FindFile(strWildcart); 
	while (bWorking)
	{ 
		bWorking = cfFind.FindNextFile();
		if (cfFind.IsDots())
		{ 			
			continue;
		}
		if (cfFind.IsDirectory())
		{
			strTmp = cfFind.GetFilePath();
			DeleteAllFilesAndDirectories(strTmp);
		}

		// You must call FindNextFile at least once before calling GetFilePath.
		strTmp = cfFind.GetFilePath();
		DeleteFile(strTmp);	
	}

	cfFind.Close();	

	// 释放CFileFind对文件夹引用后, 删除文件夹
	RemoveDirectory(lpStr);			  	
}

//   显示照片列表框背景图片
void CImageScanDlg::ShowImageListBackground()
{ 
	if (NULL != m_pBKImg)
	{	
		CRect rcGroup;
		m_BKImgStatic.GetWindowRect(&rcGroup);
		ScreenToClient(&rcGroup); 		
		Graphics gc(m_hWnd);
		gc.DrawImage(m_pBKImg, Rect(rcGroup.left, rcGroup.top, rcGroup.Width(), rcGroup.Height()));			
	} 	
}

void CImageScanDlg::OnBnClickedRadioDpi0()
{
	// TODO: Add your control notification handler code here
	m_ImgScanQuality = IMAGESCANQUALITY_1800dpi;
}

void CImageScanDlg::OnBnClickedRadioDpi1()
{
	// TODO: Add your control notification handler code here
	m_ImgScanQuality = IMAGESCANQUALITY_3600dpi;
} 

BOOL CImageScanDlg::ReadFromASIC(USHORT Addr, BYTE *pValue)
{
	CReadWriteASIC rwASIC;
	rwASIC.AttachSrcFilter(GetSrcFilter());
	return rwASIC.ReadFromASIC(Addr, pValue);	
}

BOOL CImageScanDlg::WriteToASIC(USHORT Addr, BYTE Value)
{
	CReadWriteASIC rwASIC;
	rwASIC.AttachSrcFilter(GetSrcFilter());
	return rwASIC.WriteToASIC(Addr, Value);	
}

BOOL CImageScanDlg::ReadSlaveIDFromASIC( BYTE *pSlaveID )
{
	CReadWriteASIC rwASIC;
	rwASIC.AttachSrcFilter(GetSrcFilter());
	return rwASIC.ReadSlaveIDFromASIC(pSlaveID);
}

BOOL CImageScanDlg::SetNegative()
{
	WriteToASIC(ASIC_SETFILMTYPE_ADDR, SET_ASIC_Negative);	
	return WriteToASIC(ASIC_SETFILMTYPE_FLAG_ADDR, ASIC_SETFILMTYPE_FLAG_VAL);
}

BOOL CImageScanDlg::SetSlide()
{
	WriteToASIC(ASIC_SETFILMTYPE_ADDR, SET_ASIC_Slide);	
	return WriteToASIC(ASIC_SETFILMTYPE_FLAG_ADDR, ASIC_SETFILMTYPE_FLAG_VAL);
}

BOOL CImageScanDlg::SetBW()
{
	WriteToASIC(ASIC_SETFILMTYPE_ADDR, SET_ASIC_BW); 	
	return WriteToASIC(ASIC_SETFILMTYPE_FLAG_ADDR, ASIC_SETFILMTYPE_FLAG_VAL);
}

int CImageScanDlg::GetFirmwareCode()
{
	// 0 : FS-900, 1: FS-500, 2: FS-300, 3: Other Device
	BYTE byVal = 3;	   	
	ReadFromASIC(ASIC_DEVTYPE_ADDR, &byVal);
	return (int)byVal;
}

BOOL CImageScanDlg::EnumVideoDevice(int iDevIndex, IMoniker ** ppMoniker)
{
	if (iDevIndex < 0 || NULL == ppMoniker)
	{
		return FALSE;
	}

	int iIndex = iDevIndex;
	HRESULT hr;
	ICreateDevEnum *pDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,
		(void **)&pDevEnum);
	if (FAILED(hr))
	{
		return FALSE;
	}

	IEnumMoniker *pEm = NULL;
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
		&pEm, 0);
	// haifeng_liu 2010/9/21 modify
	// 当PC未连接任何设备时, "pDevEnum->CreateClassEnumrator()"返回"S_FALSE",
	// 此时"TRUE == FAILED(hr)", "NULL == pEm", 在"pEm->Reset()"出会触发异常
	/*if (FAILED(hr))*/
	if (FAILED(hr) || NULL == pEm)
	{
		goto EnumFail;		
	}

	pEm->Reset();
	ULONG cFetched = 0;
	IMoniker *pM = NULL;
	BOOL bFindSpecifyDev = FALSE;

	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
	{
		if (0 == iIndex)
		{
			*ppMoniker = pM;
			bFindSpecifyDev = TRUE;
			break;
		}
		iIndex--;
		SAFERELEASE(pM);		
	}

	SAFERELEASE(pDevEnum);
	SAFERELEASE(pEm);

	return bFindSpecifyDev;

EnumFail:
	SAFERELEASE(pDevEnum);
	SAFERELEASE(pEm);

	*ppMoniker = NULL;
	return FALSE;	
}

void CImageScanDlg::UIChangeOperate(UIChangeOperateType uiChangeType)
{
	switch (uiChangeType)
	{
	case UICOT_TargetVideoDevice:

#if MySetFilmTypeHide
		CheckDlgButton(IDC_RADIO_PREVIEWTYPE_SLIDE, BST_CHECKED);
		OnBnClickedRadioPreviewtypeSlide();
#else
		CheckDlgButton(IDC_RADIO_PREVIEWTYPE_NEGATIVE, BST_CHECKED);
		OnBnClickedRadioPreviewtypeNegative();	
#endif	 						

		GetDlgItem(IDC_RADIO_PREVIEWTYPE_NEGATIVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_PREVIEWTYPE_SLIDE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_PREVIEWTYPE_BW)->EnableWindow(TRUE);
		break;   		 
	case UICOT_NoTargetVideoDevice:
		CheckDlgButton(IDC_RADIO_PREVIEWTYPE_SLIDE, BST_CHECKED);	

		GetDlgItem(IDC_RADIO_PREVIEWTYPE_NEGATIVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PREVIEWTYPE_SLIDE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PREVIEWTYPE_BW)->EnableWindow(FALSE);
		break;
	case UICOT_NoVideoDevice:
	case UICOT_OpenVideoFailed:
		CheckDlgButton(IDC_RADIO_PREVIEWTYPE_SLIDE, BST_CHECKED);			

		GetDlgItem(IDC_RADIO_PREVIEWTYPE_NEGATIVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PREVIEWTYPE_SLIDE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PREVIEWTYPE_BW)->EnableWindow(FALSE);	

		GetDlgItem(IDC_RADIO_DPI0)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_DPI1)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUT_SNAPSHOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_RROTATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_HMIRROR)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_IMAGESAVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_EXIT)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUT_SETSAVEPATH)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUT_COLORBRIGHTNESSAJUST)->EnableWindow(FALSE);
		break;
	case UICOT_SnapshotStart:
	case UICOT_ImageSaveStart:
	case UICOT_APWaitforExit:
		if (DEVTYPE_FS_Ohter != m_DevType)
		{
			GetDlgItem(IDC_RADIO_PREVIEWTYPE_NEGATIVE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PREVIEWTYPE_SLIDE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PREVIEWTYPE_BW)->EnableWindow(FALSE);	
		}			

		GetDlgItem(IDC_RADIO_DPI0)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_DPI1)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUT_SNAPSHOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_RROTATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_HMIRROR)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_IMAGESAVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_EXIT)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUT_SETSAVEPATH)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUT_COLORBRIGHTNESSAJUST)->EnableWindow(FALSE);

		break;
	case UICOT_SnapshotFinish:
	case UICOT_ImageSaveFinish:
	case UICOT_TimeOut:
		if (DEVTYPE_FS_Ohter != m_DevType)
		{
			GetDlgItem(IDC_RADIO_PREVIEWTYPE_NEGATIVE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_PREVIEWTYPE_SLIDE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_PREVIEWTYPE_BW)->EnableWindow(TRUE);	
		}	

		GetDlgItem(IDC_RADIO_DPI0)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_DPI1)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUT_SNAPSHOT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUT_DELETE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUT_RROTATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUT_HMIRROR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUT_IMAGESAVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUT_EXIT)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUT_SETSAVEPATH)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUT_COLORBRIGHTNESSAJUST)->EnableWindow(TRUE);

		break;
	}
}

BOOL CImageScanDlg::CreateStatusBar()
{
	BOOL bRet = FALSE; 

	bRet = m_StatusCtrl.Create(WS_CHILD|WS_VISIBLE|CCS_BOTTOM, CRect(0, 0, 0, 0), this, IDD_STATUSCTRL);
	m_StatusCtrl.SetRange(0, STOREIMAGEMAXNUM-1);
	CString str;
	str.LoadString(IDS_READY);
	m_StatusCtrl.SetText(str, 0, 0, 0);

	return bRet;
}

BOOL CImageScanDlg::DestroyStatusBar()
{
	BOOL bRet = TRUE;

	if (NULL != m_StatusCtrl)
	{
		bRet = m_StatusCtrl.DestroyWindow();
	}

	return bRet;
}

void CImageScanDlg::PumpMessages()
{
	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

IBaseFilter* CImageScanDlg::GetSrcFilter()
{
	return m_DXCapture.GetSrcFilter();
}

void CImageScanDlg::NotifyUpdate()
{
	NotifyUpdateRawParsFromHW2();	
	m_CBADlg.NotifyUpdateInColorMatrix(); 
	m_SettingDlg.NotifyUpdateRawPars();
}

CUSTOMDEVICETYPE CImageScanDlg::GetDeviceType()
{
	return m_DevType;
}

void CImageScanDlg::Timer_NotifyUpdate_EventHand()
{
	// 由于"NotifyUpdate()"执行的时间比较长, 所以利用"Timer(定时器)"在
	// 新线程中进行此操作
	KillTimer(1);
	NotifyUpdate();
}


void CImageScanDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case Timer_NotifyUpdate_ID:
		Timer_NotifyUpdate_EventHand();
		break;		
	}

	CDialog::OnTimer(nIDEvent);
}

void CImageScanDlg::AttchRawParsPointer( CRawPars *pPars )
{ 	
	m_RawPars = pPars;	
}

void CImageScanDlg::DetachRawParsPointer()
{
	m_RawPars = NULL;
}

void CImageScanDlg::InitRawPars()
{
	ASSERT(NULL != m_RawPars);

	// RGBGamma Ration
	if (DEVTYPE_FS_900 == m_DevType)
	{
		m_RawPars->dRGBGammaVal = 1.0;
		m_RawPars->dYGammaVal = 1.0;
	}
	else if (DEVTYPE_FS_500 == m_DevType)
	{
		m_RawPars->dRGBGammaVal = 1.0;
		m_RawPars->dYGammaVal = 1.0;
	}

	// RGBGamma & YGamma Enable
	m_RawPars->bRGBGammaEn = TRUE;
	m_RawPars->bYGammaEn = TRUE;

	// Negative Output
	m_RawPars->bNegative = FALSE;

	// Brightness
	m_RawPars->Brightness = 0;

	// Contras
	m_RawPars->Contrast = 32;
	m_RawPars->ContrastCenter = 160;
	m_RawPars->ContrastLmt = 20;

	// Color Gain
	if (DEVTYPE_FS_900 == m_DevType)
	{
		m_RawPars->BGain = 128;
		m_RawPars->GGain = 128;
		m_RawPars->RGain = 128;
	}
	else if (DEVTYPE_FS_500 == m_DevType)
	{
		m_RawPars->BGain = 128;
		m_RawPars->GGain = 128;
		m_RawPars->RGain = 128;
	}

	// Global Gain
	m_RawPars->GlobalGain = 0;

	// Color Offset
	m_RawPars->BOffset = 64;
	m_RawPars->BOffset = 64;
	m_RawPars->BOffset = 64;

	// UVGain
	m_RawPars->UVGain[0] = 64;
	m_RawPars->UVGain[1] = 64;
	m_RawPars->UVGain[3] = 64;
	m_RawPars->UVGain[4] = 64;
	m_RawPars->bUVGainEn = TRUE;

	// Definition
	m_RawPars->YEdgeGain = 64;
	m_RawPars->YEdgeHB = 0;
	m_RawPars->YEdgeLB = 0;
	m_RawPars->YEdgeThd = 6;
	m_RawPars->bYEdgeEnhanceEn = TRUE;

	// RGBGammaNode  
	int i = 0;
	BYTE RGBGammaValTmp[RGBGNODENUM] = {0};	
	if (DEVTYPE_FS_900 == m_DevType)
	{
		BYTE by9MRGBGammaValTmp[RGBGNODENUM] = {
			0, 5, 16, 42, 60, 78, 104, 125, 142, 154,
			164, 174, 181, 189, 197, 203, 206, 210,
			217, 225, 231, 239, 246, 255 			
		};

		for (i = 0; i < RGBGNODENUM; i++)
		{
			RGBGammaValTmp[i] = by9MRGBGammaValTmp[i];
		}
	}
	else if (DEVTYPE_FS_500 == m_DevType)
	{
		BYTE by5MRGBGammaValTmp[RGBGNODENUM] = {
			0, 12, 20, 38, 56, 73, 102, 124, 145, 162,
			178, 189, 195, 202, 208, 213, 216, 220, 223, 228,
			234, 239, 245, 255
		}; 
		for (i = 0; i < RGBGNODENUM; i++)
		{
			RGBGammaValTmp[i] = by5MRGBGammaValTmp[i];
		}
	}

	// YGammaNode
	BYTE YGammaValTmp[YGNODENUM] = {
		0, 8, 16, 24, 32, 48, 64, 80, 96, 128,
		160, 192, 208, 224, 232, 240, 248, 255 		
	};

	for (i = 0; i < RGBGNODENUM; i++)
	{
		m_RawPars->RGBGammaNode[i] =  (BYTE)(255*pow(((double)RGBGammaValTmp[i]/255.0), m_RawPars->dRGBGammaVal)); 
	}
	for (i = 0; i < YGNODENUM; i++)
	{
		m_RawPars->YGammaNode[i] = (BYTE)(255*pow(((double)YGammaValTmp[i]/255.0), m_RawPars->dYGammaVal));
	} 

	// Saturation
	m_RawPars->Sat = 1.0;

	// Hue
	m_RawPars->Hue = 0.0;

	// Color Cropping(Killer)
	m_RawPars->YHB = 0;
	m_RawPars->YLB = 0;

	// YUVProcEn
	m_RawPars->bYUVProcEn = TRUE;

	// Color Matrix
	m_RawPars->bUseDefColorMatrix = FALSE;  
	m_RawPars->InMatrix[0] = 19;
	m_RawPars->InMatrix[1] = 38;
	m_RawPars->InMatrix[2] = 7;
	m_RawPars->InMatrix[3] = -11;
	m_RawPars->InMatrix[4] = -21;
	m_RawPars->InMatrix[5] = 32;
	m_RawPars->InMatrix[6] = 32;
	m_RawPars->InMatrix[7] = -27;
	m_RawPars->InMatrix[8] = -5;  		 

	m_RawPars->OutMatrix[0] = 22;
	m_RawPars->OutMatrix[1] = 39;
	m_RawPars->OutMatrix[2] = 9;
	m_RawPars->OutMatrix[3] = -20;
	m_RawPars->OutMatrix[4] = -40;
	m_RawPars->OutMatrix[5] = 52;
	m_RawPars->OutMatrix[6] = 74;
	m_RawPars->OutMatrix[7] = -67;
	m_RawPars->OutMatrix[8] = 3;

	// YUV Display Channel
	m_RawPars->YUVChannel = YUV_YUV;		// YUV

	// Negative Outpu Implementation Type
	m_RawPars->negativeImpType = NEGATIVEIMP_ReverseRGBGamma;
}

// 需要从设备F/W端获取的值有:
// RGBGammaEnable, RGBGammaNode, YGammaEnable, YGammaNode, 
// UVGain1~4, ColorMatrix
// R/G/BGain, R/G/BOffset
// YHB, YLB
// YEdgeGain, YEdgeThd, YEdgeHB, YEdgeLB
void CImageScanDlg::NotifyUpdateRawParsFromHW2()
{
	int i = 0;

	const int AddrsNum1 = 18; 
	const USHORT AddrList1[AddrsNum1] = {
		0x11B7,					// RGBGammaEn
		0x1109,					// YGammaEn
		0x1191,					// BGain
		0x1192,					// GbGain
		0x1194,					// RGain
		0x118D,					// BOffset
		0x118E,					// GbOffset
		0x1190,					// ROffset
		0x110A,					// UVGain1
		0x110B,					// UVGain2
		0x110C,					// UVGain3
		0x110D,					// UVGain4
		0x1132,					// YEdgeGain
		0x1133,					// YEdgeThd
		0x1139,					// YEdgeHB
		0x113A,					// YEdgeLB 
		0x11D2,					// YHB
		0x11D4					// YLB
	};
	enum AddList1Name {
		RGBGammaEn = 0,
		YGammaEn,
		BGain,
		GbGain,
		RGain,
		BOffset,
		GbOffset,
		ROffset,
		UVGain1,
		UVGain2, 
		UVGain3,
		UVGain4,
		YEdgeGain,
		YEdgeThd,
		YEdgeHB,
		YEdgeLB	,
		YHB,
		YLB
	};
	BYTE Register1[AddrsNum1] = {0};
	for (i = 0; i < AddrsNum1; i++)
	{
		ReadFromASIC(AddrList1[i], &(Register1[i]));
	}

	// RGBGamma & YGamma Enable
	BOOL bRGBgammaEn = Register1[RGBGammaEn] & 0x01;
	BOOL bYGammaEn = Register1[YGammaEn] & 0x02;
	m_RawPars->bRGBGammaEn = bRGBgammaEn;
	m_RawPars->bYGammaEn = bYGammaEn;		 

	// Color Gain 	
	m_RawPars->BGain = Register1[BGain];
	m_RawPars->GGain = Register1[GbGain];
	m_RawPars->RGain = Register1[RGain];	

	// Color Offset	 
	m_RawPars->BOffset = Register1[BOffset];
	m_RawPars->GOffset = Register1[GbOffset];
	m_RawPars->ROffset = Register1[ROffset]; 

	// UVGain 
	m_RawPars->UVGain[0] = Register1[UVGain1];
	m_RawPars->UVGain[1] = Register1[UVGain2];
	m_RawPars->UVGain[2] = Register1[UVGain3];
	m_RawPars->UVGain[3] = Register1[UVGain4];  	

	// Y Edge
	m_RawPars->YEdgeGain = Register1[YEdgeGain];
	m_RawPars->YEdgeHB = Register1[YEdgeHB];
	m_RawPars->YEdgeLB = Register1[YEdgeLB];
	m_RawPars->YEdgeThd = Register1[YEdgeThd];

#if MyKeepYEdgePars
	m_RawPars->YEdgeGain = 63;
	m_RawPars->YEdgeThd = 8;
#endif

	// Color Cropping(Killer) 	
	m_RawPars->YHB = Register1[YHB];
	m_RawPars->YLB = Register1[YLB];

	// RGBGanna & YGamma
	const int AddrsNum2 = RGBGNODENUM;
	USHORT AddrList2[AddrsNum2] = {0};
	BYTE Register2[AddrsNum2] = {0};
	for (i = 0; i < AddrsNum2; i++)
	{
		AddrList2[i] = 0x11B8 + i;
	}
	for (i = 0; i < AddrsNum2; i++)
	{
		ReadFromASIC(AddrList2[i], &(Register2[i]));
	}
	for (i = 0; i < AddrsNum2; i++)
	{
		m_RawPars->RGBGammaNode[i] =  (BYTE)(255*pow(((double)Register2[i]/255.0), m_RawPars->dRGBGammaVal)); 
	}

	const int AddrsNum3 = YGNODENUM;
	USHORT AddrList3[AddrsNum3] = {0};
	BYTE Register3[AddrsNum3] = {0}; 
	for (i = 0; i < AddrsNum3; i++)
	{
		AddrList3[i] = 0x110E + i;
	}
	for (i = 0; i < AddrsNum3; i++)
	{
		ReadFromASIC(AddrList3[i], &(Register3[i]));
	}	
	for (i = 0; i < AddrsNum3; i++)
	{
		m_RawPars->YGammaNode[i] = (BYTE)(255*pow(((double)Register3[i]/255.0), m_RawPars->dYGammaVal));
	} 


	// Output ColorMatrix
	const int AddrsNum4 = 11;
	USHORT AddrList4[AddrsNum4] = {0};
	BYTE Register4[AddrsNum4] = {0};
	enum addrList4Name {
		YRMt = 0,
		YGMt,
		YBMt,
		URLMt,
		UGLMt,
		UBLMt,
		URGBHMt,
		VRLMt,
		VGLMt,
		VBLMt,
		VRGBHMt
	};
	for (i = 0; i < AddrsNum4; i++)
	{
		AddrList4[i] = 0x11D5 + i;
	}
	for (i = 0; i < AddrsNum4; i++)
	{
		ReadFromASIC(AddrList4[i], &(Register4[i]));
	}

	int iYRMt = Register4[YRMt];
	int iYGMt = Register4[YGMt];
	int iYBMt = Register4[YBMt]; 		

	int iURMt = Register4[URLMt] + ((Register4[URGBHMt]&0x03)<<8);
	iURMt = (iURMt > 511) ? (iURMt-1024) : iURMt;		

	int iUGMt = Register4[UGLMt] + (((Register4[URGBHMt]&0x0c)>>2)<<8);
	iUGMt = (iUGMt > 511) ? (iUGMt-1024) : iUGMt;

	int iUBMt = Register4[UBLMt] + (((Register4[URGBHMt]&0x30)>>4)<<8);
	iUBMt = (iUBMt > 511) ? (iUBMt-1024) : iUBMt; 		

	int iVRMt = Register4[VRLMt] + ((Register4[VRGBHMt]&0x03)<<8);
	iVRMt = (iVRMt > 511) ? (iVRMt-1024) : iVRMt;

	int iVGMt = Register4[VGLMt] + (((Register4[VRGBHMt]&0x0c)>>2)<<8);
	iVGMt = (iVGMt > 511) ? (iVGMt-1024) : iVGMt;

	int iVBMt = Register4[VBLMt] + (((Register4[VRGBHMt]&0x30)>>4)<<8);
	iVBMt = (iVBMt > 511) ? (iVBMt-1024) : iVBMt;

	m_RawPars->OutMatrix[0] = iYRMt;
	m_RawPars->OutMatrix[1] = iYGMt;
	m_RawPars->OutMatrix[2] = iYBMt;
	m_RawPars->OutMatrix[3] = iURMt;
	m_RawPars->OutMatrix[4] = iUGMt;
	m_RawPars->OutMatrix[5] = iUBMt;
	m_RawPars->OutMatrix[6] = iVRMt;
	m_RawPars->OutMatrix[7] = iVGMt;
	m_RawPars->OutMatrix[8] = iVBMt; 
}

void CImageScanDlg::OnBnClickedButColorbrightnessajust()
{
	// TODO: Add your control notification handler code here
	if (m_bCBADlgFristShow)
	{
		m_bCBADlgFristShow = FALSE;
		CRect rcWnd, rcCBA, rcMove;
		GetDlgItem(IDC_BUT_COLORBRIGHTNESSAJUST)->GetWindowRect(&rcWnd);
		m_CBADlg.GetWindowRect(&rcCBA);

		const int Offset = 10;	
		rcMove.left = rcWnd.left - (int)((rcCBA.Width()-rcWnd.Width())/2) + Offset;
		rcMove.top = rcWnd.top + 50;
		rcMove.right = rcMove.left + rcCBA.Width();
		rcMove.bottom = rcMove.top + rcCBA.Height();
		m_CBADlg.MoveWindow(&rcMove);
	}

	m_CBADlg.ShowWindow(SW_SHOW);
}

HBRUSH CImageScanDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// 如果用户修改了系统窗口背景或字体颜色, 那么Edit背景以及Edit字符串颜色
	// 也会被改变. 按要求, Edit窗口背景颜色要为固定为白色, 字体固定为黑色
	if (IDC_EDIT_IMAGESAVE == pWnd->GetDlgCtrlID())
	{
		pDC->SetBkColor(RGB(255, 255, 255));

		hbr = m_EditBKBrush;
	}

	pDC->SetTextColor(RGB(0, 0, 0)); 	

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

CString CImageScanDlg::GetCurrentDateString()
{
	CString strFileName;
	CTime ct = CTime::GetCurrentTime();
	strFileName.Format(_T("%04d%02d%02d%02d%02d%02d"),
		ct.GetYear(), 
		ct.GetMonth(),
		ct.GetDay(),
		ct.GetHour(),
		ct.GetMinute(),
		ct.GetSecond());

	return strFileName;
}


DWORD WINAPI ImageThreadProc(LPVOID lpParamter)
{ 
	PIMAGETHREADINFO pImageInfo = (PIMAGETHREADINFO)lpParamter;
	ASSERT(NULL != pImageInfo);	
	if (NULL == pImageInfo)
	{
		return 0;
	}

	BOOL bRet = FALSE;
	CFile cfWrite;
	BITMAPINFOHEADER ihdr;	
	BITMAPFILEHEADER	fhdr; 
	
	BYTE *pStillBits = pImageInfo->pBitmap;	 pImageInfo->pBitmap = NULL;
	CSize newSize = pImageInfo->siBitmap;
	ULONG ulSize = newSize.cx * newSize.cy * 3;

#if MyInterpolation	 	
	if ((DEVTYPE_FS_300 == pImageInfo->cusDevType)
		&& (newSize.cx < _3M_SIZE_WID || newSize.cy < _3M_SIZE_HEI))
	{
		BYTE *pTmp = pStillBits; pStillBits = NULL;
		CSize sizeTmp = newSize;

		newSize.cx = _3M_SIZE_WID;
		newSize.cy = _3M_SIZE_HEI;
		ulSize = newSize.cx * newSize.cy * 3;

		pStillBits = new BYTE[ulSize];
		ASSERT(NULL != pStillBits);
		if (NULL == pStillBits)
		{
			SAFEDELETEARRAY(pTmp);				
			SAFEDELETE(pImageInfo);
			AfxMessageBox(_T("Memory allocate failed!"), NULL, MB_OK | MB_ICONERROR);
			return 0;
		}
		Expandimage(pTmp, pStillBits, sizeTmp.cx, sizeTmp.cy, newSize.cx, newSize.cy);
		SAFEDELETEARRAY(pTmp);
	}
	else if ((DEVTYPE_FS_500 == pImageInfo->cusDevType)
		&& (newSize.cx < _5M_SIZE_WID || newSize.cy < _5M_SIZE_HEI))
	{
		BYTE *pTmp = pStillBits; pStillBits = NULL;
		CSize sizeTmp = newSize;

		newSize.cx = _5M_SIZE_WID;
		newSize.cy = _5M_SIZE_HEI;
		ulSize = newSize.cx * newSize.cy * 3;

		pStillBits = new BYTE[ulSize];
		ASSERT(NULL != pStillBits);
		if (NULL == pStillBits)
		{
			SAFEDELETEARRAY(pTmp);				
			SAFEDELETE(pImageInfo);
			AfxMessageBox(_T("Memory allocate failed!"), NULL, MB_OK | MB_ICONERROR);
			return 0;
		}
		Expandimage(pTmp, pStillBits, sizeTmp.cx, sizeTmp.cy, newSize.cx, newSize.cy);
		SAFEDELETEARRAY(pTmp); 
	}
	else if ((DEVTYPE_FS_900 == pImageInfo->cusDevType)
		&& (newSize.cy < _9M_SIZE_WID || newSize.cy < _9M_SIZE_HEI))
	{
		BYTE *pTmp = pStillBits; pStillBits = NULL;
		CSize sizeTmp = newSize;

		newSize.cx = _9M_SIZE_WID;
		newSize.cy = _9M_SIZE_HEI;
		ulSize = newSize.cx * newSize.cy * 3;

		pStillBits = new BYTE[ulSize];
		ASSERT(NULL != pStillBits);
		if (NULL == pStillBits)
		{
			SAFEDELETEARRAY(pTmp);				
			SAFEDELETE(pImageInfo);
			AfxMessageBox(_T("Memory allocate failed!"), NULL, MB_OK | MB_ICONERROR);
			return 0;
		}
		Expandimage(pTmp, pStillBits, sizeTmp.cx, sizeTmp.cy, newSize.cx, newSize.cy);
		SAFEDELETEARRAY(pTmp); 
	} 	

	// "3600dpi": 将图像缩放至原来的2倍(in Width and Height)
	if (IMAGESCANQUALITY_3600dpi == pImageInfo->cusImgScanQuality)
	{  
		BYTE *pTmp = pStillBits; pStillBits = NULL;
		CSize sizeTmp = newSize;
		newSize.cx *= 2;
		newSize.cy *= 2;
		ulSize = newSize.cx * newSize.cy * 3;

		pStillBits = new BYTE[ulSize];
		ASSERT(NULL != pStillBits);
		if (NULL == pStillBits)
		{
			SAFEDELETEARRAY(pTmp); 			
			SAFEDELETE(pImageInfo);
			AfxMessageBox(_T("Memory allocate failed!"), NULL, MB_OK | MB_ICONERROR);
			return 0;			
		}
		Expandimage(pTmp, pStillBits, sizeTmp.cx, sizeTmp.cy, newSize.cx, newSize.cy);	
		SAFEDELETEARRAY(pTmp);
	}
#endif

	ZeroMemory(&ihdr, sizeof(ihdr));
	ihdr.biSize = sizeof(BITMAPINFOHEADER); 
	ihdr.biWidth = newSize.cx; 
	ihdr.biHeight = newSize.cy; 		
	ihdr.biBitCount = 24;
	ihdr.biPlanes = 1;
	ihdr.biCompression = BI_RGB;
	ihdr.biSizeImage = 0;		

	ASSERT(!(pImageInfo->strImgSavePath.IsEmpty()));
	bRet = cfWrite.Open(pImageInfo->strImgSavePath, CFile::modeCreate | CFile::modeWrite, NULL);
	if (!bRet)
	{		
		SAFEDELETEARRAY(pStillBits);
		SAFEDELETE(pImageInfo);	
		AfxMessageBox(_T("Save iamge to temporary directory failed!"), NULL, MB_OK | MB_ICONERROR);
		return 0;		
	}

	fhdr.bfType		= MAKEWORD('B', 'M');	// always is "BM"
	fhdr.bfSize		= ulSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	fhdr.bfReserved1 = 0;
	fhdr.bfReserved2 = 0;
	fhdr.bfOffBits	 = (DWORD) (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

	cfWrite.Write(&fhdr, sizeof(BITMAPFILEHEADER));
	cfWrite.Write(&ihdr, sizeof(BITMAPINFOHEADER));
	cfWrite.Write(pStillBits, ulSize); 	
	cfWrite.Close(); 	 

	SAFEDELETEARRAY(pStillBits);
	SAFEDELETE(pImageInfo);	

	return 0;
} 

void Expandimage(BYTE *pInBuf, BYTE *pOutBuf, int iInWidth, int iInHeight, int iOutWidth, int iOutHeight)
{
	typedef struct tagRGBpix {
		BYTE B;
		BYTE G;
		BYTE R;
	} RGBpix, *pRGBpix;

	int x, y;
	double dMulX, dMulY, dStepX, dStepY;
	dMulX = (double)iOutWidth / iInWidth;
	dMulY = (double)iOutHeight / iInHeight;

	RGBpix *pInImg;
	RGBpix *pOutImg;
	RGBpix *pTmpImg;

	pInImg = (RGBpix*)pInBuf;
	pOutImg = (RGBpix*)pOutBuf;
	pTmpImg = new RGBpix[iOutWidth*iInHeight];

	for(y=0 ; y<iInHeight ; y++)
	{
		for(x=0 ; x<iOutWidth ; x++)
		{
			dStepX = (double)x/(iOutWidth-1)*(iInWidth-1);

			if( x == 0 || x == iOutWidth-1 )
			{
				pTmpImg[x + y*iOutWidth].B = pInImg[(long)dStepX + y*iInWidth].B;
				pTmpImg[x + y*iOutWidth].G = pInImg[(long)dStepX + y*iInWidth].G;
				pTmpImg[x + y*iOutWidth].R = pInImg[(long)dStepX + y*iInWidth].R;
			}
			else
			{
				pTmpImg[x + y*iOutWidth].B = pInImg[(long)dStepX + y*iInWidth].B*(1.0-(dStepX-(long)dStepX)) + pInImg[(long)dStepX+1 + y*iInWidth].B*(dStepX-(long)dStepX);
				pTmpImg[x + y*iOutWidth].G = pInImg[(long)dStepX + y*iInWidth].G*(1.0-(dStepX-(long)dStepX)) + pInImg[(long)dStepX+1 + y*iInWidth].G*(dStepX-(long)dStepX);
				pTmpImg[x + y*iOutWidth].R = pInImg[(long)dStepX + y*iInWidth].R*(1.0-(dStepX-(long)dStepX)) + pInImg[(long)dStepX+1 + y*iInWidth].R*(dStepX-(long)dStepX);
			}
		}
	}

	for(x=0 ; x<iOutWidth ; x++)
	{
		for(y=0 ; y<iOutHeight ; y++)
		{
			dStepY = (double)y/(iOutHeight-1)*(iInHeight-1);

			if( y == 0 || y == iOutHeight-1 )
			{
				pOutImg[x + y*iOutWidth].B = pTmpImg[x + (long)dStepY*iOutWidth].B;
				pOutImg[x + y*iOutWidth].G = pTmpImg[x + (long)dStepY*iOutWidth].G;
				pOutImg[x + y*iOutWidth].R = pTmpImg[x + (long)dStepY*iOutWidth].R;
			}
			else
			{
				pOutImg[x + y*iOutWidth].B = pTmpImg[x + (long)dStepY*iOutWidth].B*(1.0-(dStepY-(long)dStepY)) + pTmpImg[x + (long)(dStepY+1)*iOutWidth].B*(dStepY-(long)dStepY);
				pOutImg[x + y*iOutWidth].G = pTmpImg[x + (long)dStepY*iOutWidth].G*(1.0-(dStepY-(long)dStepY)) + pTmpImg[x + (long)(dStepY+1)*iOutWidth].G*(dStepY-(long)dStepY);
				pOutImg[x + y*iOutWidth].R = pTmpImg[x + (long)dStepY*iOutWidth].R*(1.0-(dStepY-(long)dStepY)) + pTmpImg[x + (long)(dStepY+1)*iOutWidth].R*(dStepY-(long)dStepY);
			}
		}
	}

	SAFEDELETEARRAY(pTmpImg);	
}



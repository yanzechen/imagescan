// ImageScanDlg.h : header file
// 
#pragma once
	
#include "DXCapture.h"	
#include "ImageViewerFrame.h"
#include "RawUtil.h"
#include "Setting.h"
#include "ColorBrightnessAjustment.h"
#include "UDsUtils.h"	
#include "ImageRotateFlipRecord.h"
#include "StatusBarCtrlwithProgressBar.h" 

// CImageScanDlg dialog
class CImageScanDlg : public CDialog
{
// Construction
public:
	CImageScanDlg(CWnd* pParent = NULL);	// standard constructor
	~CImageScanDlg();

// Dialog Data
	enum { IDD = IDD_IMAGESCAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	// Custom define 
	CUSTOMVIDEOOUTTYPE				m_VideoOutType;
	CUSTOMIMAGESAVETYPE				m_ImgSaveType;
	CUSTOMIMAGESCANQUALITY			m_ImgScanQuality;
	CUSTOMDEVICETYPE				m_DevType; 

	// Dialog
	CImageViewerFrame				m_ImgViewer; 
	CSetting						m_SettingDlg;
	CColorBrightnessAjustment		m_CBADlg;
	BOOL							m_bCBADlgFristShow; 
	CDialog						m_ProcDialog;
	BOOL							m_bProcDialogShow;

	// Control
	CBitmapButton m_BtnSnapshot;
	CBitmapButton m_BtnDelete;
	CBitmapButton m_BtnExit;
	CBitmapButton m_BtnRRotate;
	CBitmapButton m_BtnHMirror;
	CBitmapButton m_BtnBrowse1;
	CBitmapButton m_BtnBrowse2;
	CBitmapButton m_BtnImageSave;
	CBitmapButton m_BtnPhotosBack;
	CBitmapButton m_BtnRawPage;	

	CProgressCtrl* m_pProgress;

	CStatic							m_LivePreviewWnd;		// 视频显示窗口
	CStatic							m_ImgPreviewWnd;		// 视频拍照预览窗口
	CStatic							m_BKImgStatic; 
	CStatic*						m_ImgsStaticArray[STOREIMAGEMAXNUM];

	CStatusBarCtrlwithProgressBar	m_StatusCtrl;
	
	CRect							m_rcImgPreviewWnd;		// 视频拍照预览窗口区域	 	
	CBrush							m_EditBKBrush;

	Bitmap*							m_pBKImg;
	Bitmap*							m_pVideoFrame; 
	Bitmap*							m_ThumbImgsArray[STOREIMAGEMAXNUM];	
	CString							m_ImgPathArray[STOREIMAGEMAXNUM];
	CImageRotateFlipRecord			m_ImgRFRecArray[STOREIMAGEMAXNUM];

	CRawPars*						m_RawPars; 
	CDXCapture						m_DXCapture;			// 定义"CDXCapture", 用于构建VideoPlayCapture
	CUDsUtils						m_UDsUtils;  

	int								m_iStoredImgNum;		// 记录用户已经拍了多少在照片
	int								m_iSelectedImgIdx;		// 记录用户选择的照片Index	 
	
	CString							m_strImgSaveDir;		
	CString							m_strTmpPath;
	CString							m_strTmpDirName;

	HANDLE							m_ProcThreadHand[STOREIMAGEMAXNUM];



// Implementation
protected:
	HICON m_hIcon;	

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg); 	
	
	DECLARE_MESSAGE_MAP()
public:	 
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButSnapshot();
	afx_msg void OnBnClickedButDelete();
	afx_msg void OnBnClickedButHmirror();		
	afx_msg void OnBnClickedButRrotate();
	afx_msg void OnBnClickedButImagesave();
	afx_msg void OnBnClickedButExit();
	afx_msg void OnBnClickedRadioPreviewtypeNegative();
	afx_msg void OnBnClickedRadioPreviewtypeSlide();
	afx_msg void OnBnClickedRadioPreviewtypeBw();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButRawpropertypage(); 
	afx_msg void OnBnClickedRadioDpi0();
	afx_msg void OnBnClickedRadioDpi1();
	afx_msg void OnBnClickedButColorbrightnessajust(); 
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButSetsavepath();
	afx_msg LRESULT OnGetStillBuffer(WPARAM wparam, LPARAM lparam);	 
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


	protected:			
		BOOL CreateTempDirectory(LPCTSTR lpDirName, CString& strDirPath); 
		// 
		// displya image
		//
		BOOL ShowAllImagesInImageList();
		BOOL ShowSelectedImageInSnapshotPreview(int iIdx);

		void RotateImage(RotateFlipType rt);
		void ListImageOrderAjust();	
		void HighlightSelectedImageStatic(int iIdx);	 

		void SaveImageToFile();
		void AfterSaveImageClear();	  

		void ImageRefresh();	 		

		void ShowImageListBackground();	 	
		void ImageRefresh_AfterRotate(); 	
		BOOL ShowSelectedImageInImageList(int iIdx); 

		//
		// set file type
		//
		BOOL SetNegative();
		BOOL SetSlide();
		BOOL SetBW(); 

		int GetFirmwareCode();		// 0 : FS-900, 1: FS-500, 2: FS-300, 3: Other Device
		BOOL EnumVideoDevice(int iDevIndex, IMoniker ** ppMoniker);

		void UIChangeOperate(UIChangeOperateType uiChangeType);
		BOOL CreateStatusBar();
		BOOL DestroyStatusBar();
		int GetImagesCount();
		void PumpMessages(); 
		void Init_Create();
		void Init_Paramters();
		void Init_WndState();
		void Init_VideoDevice();	
		void AttchRawParsPointer(CRawPars *pPars);
		void DetachRawParsPointer();
		void InitRawPars();	
		void NotifyUpdateRawParsFromHW2();
		void NotifyUpdate();   

		DWORD WatiForPicturesProcFinished();
		void Timer_NotifyUpdate_EventHand();

		CString GetCurrentDateString();


		//
		// Util functions
		//
	public:
		CString BrowseDirectory(HWND hwnd, LPCTSTR lpTitle);
		BOOL IsFilePathExist(LPCTSTR lpFilePath);
		void DeleteAllFilesAndDirectories(LPCTSTR lpStr);

		// 
		// ASIC operation
		//
		BOOL ReadFromASIC(USHORT Addr, BYTE *pValue);
		BOOL WriteToASIC(USHORT Addr, BYTE Value);
		BOOL ReadSlaveIDFromASIC(BYTE *pSlaveID);

	public:
		IBaseFilter* GetSrcFilter();
		CUSTOMDEVICETYPE GetDeviceType();	
};

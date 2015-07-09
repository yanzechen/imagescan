#pragma once


// CStatusBarCtrlwithProgressBar

// 带进度条的单面板状态栏
class CStatusBarCtrlwithProgressBar : public CStatusBarCtrl
{
	DECLARE_DYNAMIC(CStatusBarCtrlwithProgressBar)

public:
	CStatusBarCtrlwithProgressBar();
	virtual ~CStatusBarCtrlwithProgressBar();

protected:
	DECLARE_MESSAGE_MAP()
private:
	CProgressCtrl m_ProgressCtrl;
	BOOL CreateProgressBar();
	BOOL DestroyProgressBar();
public:
	void SetRange( short nLower, short nUpper );
	void GetRange( int& nLower, int& nUpper );
	int SetPos( int nPos );
	int GetPos();
	BOOL SetText( LPCTSTR lpszText, int nPane, int nType, int nProgressPos = 0 );
	BOOL ShowProgressBar(int nCmdShow);

public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL DestroyWindow(); 
};



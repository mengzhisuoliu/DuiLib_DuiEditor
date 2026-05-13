#include "StdAfx.h"
#include "App.h"

CApp uiApp;

CApp::CApp(void)
{
}


CApp::~CApp(void)
{
}


bool CApp::InitInstance(int argc, LPSTR argv[])
{
	if(!__super::InitInstance(argc, argv))
		return false;

	CPaintManagerUI::ImportFontFile(GetAppPath() + _T("宋体.ttc"), _T("微软雅黑"), 12);

	//建立主窗口
	m_pMainWnd = new CMainFrame();
	if( m_pMainWnd == NULL ) return false;
	m_pMainWnd->Create(NULL, GetAppName(), UI_WNDSTYLE_FRAME, 0L, 0, 0, 0, 0);
	m_pMainWnd->CenterWindow();

	return true;
}

int CApp::ExitInstance()
{
	return __super::ExitInstance();
}
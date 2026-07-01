//#include "StdAfx.h"
#include "App.h"

CApp uiApp;

CApp::CApp(void)
{
}


CApp::~CApp(void)
{
}


bool CApp::InitInstance(int argc, char* argv[])
{
	if(!CUIApplication::InitInstance(argc, argv))
		return false;

	CPaintManagerUI::ImportFont(_T("/system/fonts/Roboto-Regular.ttf"), _T("微软雅黑"));
	
	//œšÁ¢Ö÷Ž°¿Ú
	m_pMainWnd = new CMainFrame();
	if( m_pMainWnd == NULL ) return false;
	m_pMainWnd->Create(NULL, GetAppName(), UI_WNDSTYLE_FRAME, 0L, 0, 0, 0, 0);
	m_pMainWnd->CenterWindow();

	return true;
}

int CApp::ExitInstance()
{
	return CUIApplication::ExitInstance();
}

void CApp::InitResource()
{
	CPaintManagerUI::SetResourceType(m_resType);
}


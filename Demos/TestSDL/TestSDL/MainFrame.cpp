#include "StdAfx.h"
#include "MainFrame.h"


CMainFrame::CMainFrame(void)
{
}


CMainFrame::~CMainFrame(void)
{

}

void CMainFrame::InitWindow()
{
	
}

bool CMainFrame::OnCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CMainFrame::OnMenuCommand(const MenuCmd *cmd)
{
	return false;
}

// bool CMainFrame::OnMenuUpdateCommandUI(CMenuCmdUI *cmdUI)
// {
// 	return false;
// }

void CMainFrame::OnNotifyClick(TNotifyUI& msg)
{
	if (IsControl(msg, _T("dpi_96")))
	{
		GetManager()->SetDPI(96);
		GetManager()->ResetDPIAssets();
	}
	if (IsControl(msg, _T("dpi_120")))
	{
		GetManager()->SetDPI(120);
		GetManager()->ResetDPIAssets();
	}
	if (IsControl(msg, _T("dpi_144")))
	{
		GetManager()->SetDPI(144);
		GetManager()->ResetDPIAssets();
	}
	if (IsControl(msg, _T("dpi_168")))
	{
		GetManager()->SetDPI(168);
		GetManager()->ResetDPIAssets();
	}
	if (IsControl(msg, _T("dpi_192")))
	{
		GetManager()->SetDPI(192);
		GetManager()->ResetDPIAssets();
	}
}
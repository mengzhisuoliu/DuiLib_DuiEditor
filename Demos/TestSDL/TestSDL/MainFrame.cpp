#include "StdAfx.h"
#include "MainFrame.h"

#include "DialogModal.h"

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
	CString str;
	str.Format(_T("Äãµã»÷ÁË²Ëµ¥: name=%s, text=%s"), cmd->szName, cmd->szText);
	return false;
}

bool CMainFrame::OnMenuUpdateCommandUI(CMenuCmdUI *cmdUI)
{
	return false;
}

void CMainFrame::OnNotifyClick(TNotifyUI& msg)
{
	if (IsControl(msg, _T("dpi_96")))
	{
		GetManager()->SetAllDPI(96);
		GetManager()->ResetDPIAssets();
	}
	if (IsControl(msg, _T("dpi_120")))
	{
		GetManager()->SetAllDPI(120);
		GetManager()->ResetDPIAssets();
	}
	if (IsControl(msg, _T("dpi_144")))
	{
		GetManager()->SetAllDPI(144);
		GetManager()->ResetDPIAssets();
	}
	if (IsControl(msg, _T("dpi_168")))
	{
		GetManager()->SetAllDPI(168);
		GetManager()->ResetDPIAssets();
	}
	if (IsControl(msg, _T("dpi_192")))
	{
		GetManager()->SetAllDPI(192);
		GetManager()->ResetDPIAssets();
	}


	if (IsControl(msg, _T("btn_domodal")))
	{
		CDialogModal dlg;
		dlg.DoModal(this);
		return;
	}

	if (IsControl(msg, _T("btn_show_dialog")))
	{
		CDialogModal *dlg = new CDialogModal;
		dlg->ShowDialog(this);
		return;
	}

	if (IsControl(msg, _T("windowmenubtn")))
	{
		CreateMenu(_T("Menu.xml"));
	}
}
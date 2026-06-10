#pragma once
#include "StdAfx.h"

#ifdef DUILIB_WIN32
namespace DuiLib{

CUIFrameWndWin32::CUIFrameWndWin32(void)
{
	
}

CUIFrameWndWin32::~CUIFrameWndWin32(void)
{
	
}

UIWND CUIFrameWndWin32::Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy)
{
	UIWND hWnd = CWindowWin32::Create(hwndParent, pstrName, dwStyle, dwExStyle, x, y, cx, cy);
	if(hWnd != NULL)
	{
		GetManager()->SetDPI(GetManager()->GetDPIObj()->GetMainMonitorDPI());
	}
	return hWnd;
}

LRESULT CUIFrameWndWin32::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRet = CUIFrameWndBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	if(IsHandleMessage()) return lRet;

	if(uMsg == UIMSG_INSERT_MSG)
	{
		CMsgWndUI *pMsgWindow = (CMsgWndUI *)wParam;
		GetManager()->SendNotify(pMsgWindow, _T("CMsgWndUI::InsertMsg"), wParam, lParam);
		return 0;
	}

	if(m_pApplication && uMsg == m_pApplication->m_UIAPP_SINGLEAPPLICATION_MSG)
	{
		::ShowWindow(GetHWND(), SW_SHOW);
		::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0); 
		::SetForegroundWindow(GetHWND());
		return 1;
	}

	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CUIFrameWndWin32::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CUIFrameWndWin32::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;
}

LRESULT CUIFrameWndWin32::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;
}

LRESULT CUIFrameWndWin32::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDuiPoint pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	CDuiRect rcClient;
	::GetClientRect(*this, &rcClient);

	if (!::IsZoomed(*this))
	{
		CDuiRect rcSizeBox = GetManager()->GetSizeBox();
		if (pt.y < rcClient.top + rcSizeBox.top)
		{
			if (pt.x < rcClient.left + rcSizeBox.left) return HTTOPLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
			return HTTOP;
		}
		else if (pt.y > rcClient.bottom - rcSizeBox.bottom)
		{
			if (pt.x < rcClient.left + rcSizeBox.left) return HTBOTTOMLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}

		if (pt.x < rcClient.left + rcSizeBox.left) return HTLEFT;
		if (pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
	}

	CDuiRect rcCaption = GetManager()->GetCaptionRect();
	rcCaption = GetManager()->GetDPIObj()->ScaleRect(rcCaption);
	if (-1 == rcCaption.bottom)
	{
		rcCaption.bottom = rcClient.bottom;
	}

	if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom)
	{
		CControlUI* pControl = GetManager()->FindControl(pt);
		if (IsInStaticControl(pControl))
		{
			return HTCAPTION;
		}
	}

	return HTCLIENT;
}

LRESULT CUIFrameWndWin32::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO Monitor = {};
	Monitor.cbSize = sizeof(Monitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &Monitor);
	CDuiRect rcWork = Monitor.rcWork;
	if( Monitor.dwFlags != MONITORINFOF_PRIMARY ) {
		rcWork.Offset(-rcWork.left, -rcWork.top);
	}

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x = rcWork.right - rcWork.left;
	lpMMI->ptMaxSize.y = rcWork.bottom - rcWork.top;
	lpMMI->ptMaxTrackSize.x = GetManager()->GetMaxInfo().cx == 0?rcWork.right - rcWork.left:GetManager()->GetMaxInfo().cx;
	lpMMI->ptMaxTrackSize.y = GetManager()->GetMaxInfo().cy == 0?rcWork.bottom - rcWork.top:GetManager()->GetMaxInfo().cy;
	lpMMI->ptMinTrackSize.x = GetManager()->GetMinInfo().cx;
	lpMMI->ptMinTrackSize.y = GetManager()->GetMinInfo().cy;

	bHandled = TRUE;
	return 0;
}

LRESULT CUIFrameWndWin32::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDuiSize szRoundCorner = GetManager()->GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
	if( !::IsIconic(*this) ) {
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
#endif
	bHandled = FALSE;
	return 0;
}

LRESULT CUIFrameWndWin32::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == SC_CLOSE)
	{
		bHandled = TRUE;
		::SendMessage(GetHWND(), WM_CLOSE, 0, 0);
		return 0;
	}
	BOOL bZoomed = GetManager()->IsZoomed();
	LRESULT lRes = ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
#if defined(WIN32) && !defined(UNDER_CE)
	if( GetManager()->IsZoomed() != bZoomed ) 
	{
		if( !bZoomed ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(GetManager()->FindControl(_T("windowmaxbtn")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(GetManager()->FindControl(_T("windowrestorebtn")));
			if( pControl ) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(GetManager()->FindControl(_T("windowmaxbtn")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(GetManager()->FindControl(_T("windowrestorebtn")));
			if( pControl ) pControl->SetVisible(false);
		}
	}
#endif
	return lRes;
}

LRESULT CUIFrameWndWin32::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 调整窗口样式
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	// 关联UI管理器
	GetManager()->Init(GetHWND(), GetManagerName(), this);
	// 注册PreMessage回调
	GetManager()->AddPreMessageFilter(this);

	// 创建主窗口
	CControlUI* pRoot=NULL;
	CDialogBuilder builder;
	CDuiString sSkinType = GetSkinType();
	if (!sSkinType.IsEmpty()) {
		STRINGorID xml(_ttoi(GetSkinFile().GetData()));
		pRoot = builder.Create(xml, sSkinType, this, GetManager());
	}
	else {
		pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, GetManager());
	}

	if (pRoot == NULL) {
		CDuiString sError = _T("Loading resource failed :");
		sError += GetSkinFile();
		MessageBox(NULL, sError, _T("Duilib") ,MB_OK|MB_ICONERROR);
		ExitProcess(1);
		return 0;
	}
	GetManager()->AttachDialog(pRoot);
	// 添加Notify事件接口
	GetManager()->AddNotifier(this);

	// 窗口初始化完毕
	__InitWindow();
	return 0;
}

} //namespace DuiLib{
#endif //#ifdef DUILIB_WIN32
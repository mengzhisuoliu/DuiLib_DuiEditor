#include "StdAfx.h"

#include <time.h>
#ifdef DUILIB_GTK
namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CPaintManagerGtkUI::CPaintManagerGtkUI()
	{
		
	}

	CPaintManagerGtkUI::~CPaintManagerGtkUI()
	{
		if(m_pLangManager){ delete m_pLangManager; m_pLangManager = NULL; }

		// Delete the control-tree structures
		for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
		m_aDelayedCleanup.Resize(0);
		for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
		m_aAsyncNotify.Resize(0);

		m_mNameHash.Resize(0);
		if( m_pRoot != NULL ) delete m_pRoot;

		//delete m_ResInfo.m_DefaultFontInfo;
		RemoveAllFonts();
		RemoveAllImages();
		RemoveAllStyle();
		RemoveAllDefaultAttributeList();
		RemoveAllWindowCustomAttribute();
		RemoveAllOptionGroups();
		RemoveAllTimers();
		RemoveAllDrawInfos();

#ifdef DUILIB_WIN32
		if( m_hwndTooltip != NULL ) {
			::DestroyWindow(m_hwndTooltip);
			m_hwndTooltip = NULL;
		}

		if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
#endif
		m_aPreMessages.Remove(m_aPreMessages.Find(this));


		// DPI管理对象
		if (m_pDPI != NULL) {
			delete m_pDPI;
			m_pDPI = NULL;
		}

		if(m_pRenderEngine)
		{
			//m_pRenderEngine->Release();
			m_pRenderEngine = NULL;
		}

		g_object_unref(m_hWndPaint);
	}

	void CPaintManagerGtkUI::Init(UIWND hWnd, LPCTSTR pstrName, CWindowWnd *pWindow)
	{
		CPaintManagerUI::Init(hWnd, pstrName, pWindow);
		if( m_hWndPaint != hWnd ) 
		{
			if(m_hWndPaint != NULL) g_object_unref(m_hWndPaint);
			m_hWndPaint = hWnd;
			m_pWindow = pWindow;
			g_object_ref(m_hWndPaint);
			m_aPreMessages.Add(this);
			Render()->Init(this, NULL);
		}
	}

	UIRender *CPaintManagerGtkUI::Render()
	{
		if(!m_pRenderEngine)
		{
			m_pRenderEngine = MakeRefPtr<UIRender>(UIGlobal::CreateRenderTarget());
			m_pRenderEngine->Init(this, NULL);
		}
		ASSERT(m_pRenderEngine);
		return m_pRenderEngine;
	}

	BOOL CPaintManagerGtkUI::InvalidateRect(UIWND hWnd, const RECT *lpRect, BOOL bErase)
	{
		if(!GTK_IS_WIDGET(m_hWndPaint)) return FALSE;
		GdkWindow *gdkWindow = gtk_widget_get_window(GTK_WIDGET(m_hWndPaint));
		if(lpRect == NULL)
		{
			gdk_window_invalidate_rect(gdkWindow, NULL, TRUE); 
		}
		else
		{
			GdkRectangle gdkRect;
			gdkRect.x = lpRect->left;
			gdkRect.y = lpRect->top;
			gdkRect.width = lpRect->right - lpRect->left;
			gdkRect.height = lpRect->bottom - lpRect->top;
			gdk_window_invalidate_rect(gdkWindow, &gdkRect, TRUE);
		}
		return TRUE;
	}

	void CPaintManagerGtkUI::SetCursor(int nCursor)
	{

	}

	BOOL CPaintManagerGtkUI::SetTimer(UINT uElapse, TIMERINFO* pTimer)
	{
		pTimer->uWinTimer = g_timeout_add((guint)uElapse, wrap_timer_event, pTimer);
		return pTimer->uWinTimer > 0;
	}

	BOOL CPaintManagerGtkUI::KillTimer(TIMERINFO* pTimer)
	{
		if (pTimer == NULL || pTimer->uWinTimer == 0) return FALSE;
		return g_source_remove(pTimer->uWinTimer);
	}

	BOOL CPaintManagerGtkUI::GetCursorPos(LPPOINT pt)
	{
		GdkDisplay* ddpy = gdk_display_get_default();   //获取默认的GdkDisplay
		GdkDeviceManager* device_manager = gdk_display_get_device_manager(ddpy);//获取gdk设备管理器
		GdkDevice* pointer = gdk_device_manager_get_client_pointer(device_manager);//获取gdk表示的鼠标设备
		gint x, y;
		gdk_device_get_position(pointer, NULL, &x, &y);//获取鼠标设备的全局坐标
		pt->x = x;
		pt->y = y;
		return TRUE;
	}

	BOOL CPaintManagerGtkUI::ScreenToClient(LPPOINT pt)
	{
		if (!pt) return FALSE;
		GtkWidget* widget = (GtkWidget*)GetPaintWindow();
		if (!widget || !gtk_widget_get_realized(widget)) return FALSE;

		gint client_x, client_y;
		// 将屏幕坐标 (pt->x, pt->y) 转换到 widget 的客户区坐标
		if (gtk_widget_translate_coordinates(NULL, widget, pt->x, pt->y, &client_x, &client_y)) {
			pt->x = client_x;
			pt->y = client_y;
			return TRUE;
		}
		return FALSE;
	}

	void CPaintManagerGtkUI::SetWndFocus()
	{
		GtkWidget *pGtkWidget = (GtkWidget *)m_hWndPaint;
		gtk_window_present(GTK_WINDOW(hWnd));
	}

	UIWND CPaintManagerGtkUI::GetWndFocus()
	{
		GList *windows = gtk_window_list_toplevels();
		GList *iter = windows;
		GtkWindow *focused = NULL;

		while (iter != NULL) {
			GtkWindow *win = GTK_WINDOW(iter->data);
			// 检查窗口是否处于活动顶层窗口链中
			if (gtk_window_is_active(win)) {
				focused = win;
				break;
			}
			iter = iter->next;
		}

		g_list_free(windows);
		return focused;
	}

	BOOL CPaintManagerGtkUI::IsKeyDown(UINT uKey)
	{
		return FALSE;
	}

	BOOL CPaintManagerGtkUI::IsKeyUp(UINT uKey)
	{
		return FALSE;
	}

	UINT CPaintManagerGtkUI::MapKeyState()
	{
		UINT uState = 0;
		if( IsKeyDown(VK_CONTROL) ) uState |= MK_CONTROL;
		if( IsKeyDown(VK_LBUTTON) ) uState |= MK_LBUTTON;
		if( IsKeyDown(VK_RBUTTON) ) uState |= MK_RBUTTON;
		if( IsKeyDown(VK_SHIFT) ) uState |= MK_SHIFT;
		if( IsKeyDown(VK_MENU) ) uState |= MK_ALT;
		return uState;
	}

	DWORD CPaintManagerGtkUI::GetTickCount()
	{
#ifdef WIN32
		return ::GetTickCount();
#else
#include <time.h>
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
	}

	void CPaintManagerGtkUI::GetLocalTime(SYSTEMTIME &st)
	{
#ifdef WIN32
		return ::GetLocalTime(&st);
#else
#include <time.h>
		time_t tmp = ::time(&tmp);
		struct tm *ptm;
		ptm = localtime(&tmp);
		st.wYear = ptm->tm_year;
		st.wMonth = ptm->tm_mon;
		st.wDay = ptm->tm_mday;
		st.wHour = ptm->tm_hour;
		st.wMinute = ptm->tm_min;
		st.wSecond = ptm->tm_sec;
		st.wDayOfWeek = ptm->tm_wday;
#endif
	}

	void CPaintManagerGtkUI::MessageLoop()
	{
		gtk_main();
	}

	bool CPaintManagerGtkUI::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes) 
	{
		return false;
	}

	bool CPaintManagerGtkUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes) 
	{
		switch( uMsg ) 
		{
		case WM_CLOSE:
			return OnClose(wParam, lParam, lRes);
			break;
		case WM_ERASEBKGND:
			{
				// We'll do the painting here...
				lRes = 1;
				return true;
				return OnEraseBkgnd(wParam, lParam, lRes); //预留，不调用
			}
			break;
		case WM_PAINT:
			return OnPaint(wParam, lParam, lRes);
			break;
		case WM_PRINTCLIENT:
			return false;
			return OnPrintClient(wParam, lParam, lRes);
			break;
		case WM_GETMINMAXINFO:
			return OnGetMaxMinInfo(wParam, lParam, lRes);
			break;
		case WM_SIZE:
			return OnSize(wParam, lParam, lRes);
			break;
		case WM_TIMER:
			return OnTimer(wParam, lParam, lRes);
			break;
		case WM_MOUSEHOVER:
			return OnMouseOver(wParam, lParam, lRes);
			break;
		case WM_MOUSELEAVE:
			return OnMouseLeave(wParam, lParam, lRes);
			break;
		case WM_MOUSEMOVE:
			return OnMouseMove(wParam, lParam, lRes);
			break;
		case WM_LBUTTONDOWN:
			return OnLButtonDown(wParam, lParam, lRes);
			break;
		case WM_LBUTTONDBLCLK:
			return OnLButtonDbClick(wParam, lParam, lRes);
			break;
		case WM_LBUTTONUP:
			return OnLButtonUp(wParam, lParam, lRes);
			break;
		case WM_RBUTTONDOWN:
			return OnRButtonDown(wParam, lParam, lRes);
			break;
		case WM_RBUTTONUP:
			return OnRButtonUp(wParam, lParam, lRes);
			break;
		case WM_MBUTTONDOWN:
			return OnMButtonDown(wParam, lParam, lRes);
			break;
		case WM_MBUTTONUP:
			return OnMButtonUp(wParam, lParam, lRes);
			break;
		case WM_CONTEXTMENU:
			return OnContextMenu(wParam, lParam, lRes);
			break;
		case WM_MOUSEWHEEL:
			return OnMouseWheel(wParam, lParam, lRes);
			break;
		case WM_CHAR:
			return OnChar(wParam, lParam, lRes);
			break;
		case WM_KEYDOWN:
			return OnKeyDown(wParam, lParam, lRes);
			break;
		case WM_KEYUP:
			return OnKeyUp(wParam, lParam, lRes);
			break;
		case WM_SETCURSOR:
			return OnSetCursor(wParam, lParam, lRes);
			break;
		case WM_SETFOCUS:
			return OnSetFocus(wParam, lParam, lRes);
			break;
		case WM_KILLFOCUS:
			return OnKillFocus(wParam, lParam, lRes);
			break;
		case WM_NOTIFY:
			return OnNotify(wParam, lParam, lRes);
			break;
		case WM_COMMAND:
			return OnCommand(wParam, lParam, lRes);
			break;
		case WM_CTLCOLOREDIT:
			return OnCtlColorEdit(wParam, lParam, lRes);
			break;
		case WM_CTLCOLORSTATIC:
			return OnCtlColorStatic(wParam, lParam, lRes);
			break;
		default:
			break;
		}
		return false;
	}

	bool CPaintManagerGtkUI::OnClose(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		if( m_pRoot == NULL )
		{
			return true;
		}

		CDuiRect rcClient;
// 		CWindowGtk *pGtkWindow = (CWindowGtk *)m_hWndPaint;
// 		UIWND hWnd = pGtkWindow->GetHWND();
		GetWindowRect(m_hWndPaint, &rcClient);
		if(rcClient.GetWidth() <= 0 || rcClient.GetHeight() <= 0)
			return true;

		CDuiRect rcPaint = rcClient;
		bool bNeedSizeMsg = false;

		SetPainting(true);
		if( m_bUpdateNeeded && !m_bLockUpdate) 
		{
			m_bUpdateNeeded = false;
			if( m_pRoot->IsUpdateNeeded() ) 
			{
				RECT rcRoot = rcClient;
				m_pRoot->SetPos(rcRoot, true);
				bNeedSizeMsg = true;
			}
			else 
			{
				//单独NeedUpdate某个控件时
				CControlUI* pControl = NULL;
				m_aFoundControls.Empty();
				m_pRoot->FindControl(__FindControlsFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST | UIFIND_UPDATETEST);
				for( int it = 0; it < m_aFoundControls.GetSize(); it++ ) {
					pControl = static_cast<CControlUI*>(m_aFoundControls[it]);
					//float控件不需要重新计算RelativePos，因为第一次SetPos时，已经把偏移计算好了，并且保存到m_rcItem。
					//if( !pControl->IsFloat() ) pControl->SetPos(pControl->GetPos(), true);
					//else pControl->SetPos(pControl->GetRelativePos(), true);
					pControl->SetPos(pControl->GetPos(), true);
				}
				bNeedSizeMsg = true;
			}

			//第一次绘制窗口之前，发送DUI_MSGTYPE_WINDOWINIT消息
			if( m_bFirstLayout ) 
			{
				m_bFirstLayout = false;
				SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWINIT,  0, 0, false);
			}
		}

		Render()->Resize(rcClient);
		Render()->ClearAlpha(rcPaint);

		cairo_t * cr = (cairo_t *)wParam;
		Render()->Init(this, cr);

		GetRoot()->Paint(Render(), rcPaint, NULL);

		for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
			CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
			pPostPaintControl->DoPostPaint(Render(), rcPaint);
		}
			

		SetPainting(false);
		if( m_bUpdateNeeded ) Invalidate();

		// 发送窗口大小改变消息
		if(bNeedSizeMsg) {
			this->SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWSIZE, 0, 0, true);
		}
		return true;
	}

	bool CPaintManagerGtkUI::OnEraseBkgnd(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnPrintClient(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnGetMaxMinInfo(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnSize(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		if( m_pFocus != NULL ) {
			TEventUI event = { 0 };
			event.Type = UIEVENT_WINDOWSIZE;
			event.pSender = m_pFocus;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
		}
		if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
		return true;
	}

	bool CPaintManagerGtkUI::OnTimer(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
			const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if(pTimer->hWnd == m_hWndPaint && 
				pTimer->uWinTimer == LOWORD(wParam) &&
				pTimer->bKilled == false)
			{
				TEventUI event = { 0 };
				event.Type = UIEVENT_TIMER;
				event.pSender = pTimer->pSender;
				event.dwTimestamp = GetTickCount();
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.wParam = pTimer->nLocalID;
				event.lParam = lParam;
				pTimer->pSender->Event(event);
				break;
			}
		}

		return false;
	}

	bool CPaintManagerGtkUI::OnLButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		// We alway set focus back to our app (this helps
		// when Win32 child windows are placed on the dialog
		// and we need to remove them on focus change).
		if (!m_bNoActivate) SetWndFocus(m_hWndPaint);
		if( m_pRoot == NULL ) return false;

		// 查找控件
		GdkEventButton *ev = (GdkEventButton *)wParam;
		CDuiPoint pt(ev->x, ev->y);
		m_ptLastMousePos = pt;
		CControlUI* pControl = FindControl(pt);
		if( pControl == NULL ) return false;
		if( pControl->GetManager() != this ) return false;
		m_pEventClick = pControl;
		pControl->SetFocus();

		TEventUI event = { 0 };
		event.Type = UIEVENT_BUTTONDOWN;
		event.pSender = pControl;
		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = pt;
		event.wKeyState = (WORD)wParam;
		event.dwTimestamp = GetTickCount();
		pControl->Event(event);

		return false;
	}

	bool CPaintManagerGtkUI::OnLButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		GdkEventButton *ev = (GdkEventButton *)wParam;
		CDuiPoint pt(ev->x, ev->y);
		m_ptLastMousePos = pt;
		if( m_pEventClick == NULL ) return false;
		ReleaseCapture();
		TEventUI event = { 0 };
		event.Type = UIEVENT_BUTTONUP;
		event.pSender = m_pEventClick;
		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = pt;
		event.wKeyState = (WORD)wParam;
		event.dwTimestamp = GetTickCount();

		CControlUI* pClick = m_pEventClick;
		m_pEventClick = NULL;
		pClick->Event(event);

		return false;
	}

	bool CPaintManagerGtkUI::OnLButtonDbClick(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnRButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnRButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnRButtonDbClick(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnMButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnMButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnMouseOver(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnMouseLeave(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		GdkEventMotion *ev = (GdkEventMotion *)wParam;
		CDuiPoint pt(ev->x, ev->y);

		// 记录鼠标位置
		m_ptLastMousePos = pt;
		CControlUI* pNewHover = FindControl(pt);
		if( pNewHover != NULL && pNewHover->GetManager() != this ) return false;

		TEventUI event = { 0 };
		event.ptMouse = pt;
		event.wParam = wParam;
		event.lParam = lParam;
		event.dwTimestamp = GetTickCount();
		event.wKeyState = MapKeyState();
		if( !IsCaptured() ) {
			pNewHover = FindControl(pt);
			if( pNewHover != NULL && pNewHover->GetManager() != this ) return false;
			if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
				event.Type = UIEVENT_MOUSELEAVE;
				event.pSender = m_pEventHover;

				CStdPtrArray aNeedMouseLeaveNeeded(m_aNeedMouseLeaveNeeded.GetSize());
				aNeedMouseLeaveNeeded.Resize(m_aNeedMouseLeaveNeeded.GetSize());
				memcpy(aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetSize() * sizeof(LPVOID));
				for( int i = 0; i < aNeedMouseLeaveNeeded.GetSize(); i++ ) {
					static_cast<CControlUI*>(aNeedMouseLeaveNeeded[i])->Event(event);
				}

				m_pEventHover->Event(event);
				m_pEventHover = NULL;
				//if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			}
			if( pNewHover != m_pEventHover && pNewHover != NULL ) {
				event.Type = UIEVENT_MOUSEENTER;
				event.pSender = pNewHover;
				pNewHover->Event(event);
				m_pEventHover = pNewHover;
			}
		}
		if( m_pEventClick != NULL ) {
			event.Type = UIEVENT_MOUSEMOVE;
			event.pSender = m_pEventClick;
			m_pEventClick->Event(event);
		}
		else if( pNewHover != NULL ) {
			event.Type = UIEVENT_MOUSEMOVE;
			event.pSender = pNewHover;
			pNewHover->Event(event);
		}

		return false;
	}

	bool CPaintManagerGtkUI::OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnContextMenu(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnChar(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnKeyDown(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnKeyUp(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnSetCursor(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return true;
	}

	bool CPaintManagerGtkUI::OnSetFocus(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnKillFocus(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnCommand(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnCtlColorEdit(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerGtkUI::OnCtlColorStatic(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

} // namespace DuiLib
#endif //#ifdef DUILIB_GTK

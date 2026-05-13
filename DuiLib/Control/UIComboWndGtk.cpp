#include "StdAfx.h"
#include "UIComboWndGtk.h"

#ifdef DUILIB_GTK
namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	void CComboWndGtk::Init(CComboUI* pOwner)
	{
		m_bHitItem = false;
		m_pOwner = pOwner;

		// Position the popup window in absolute space
		SIZE szDrop = m_pOwner->GetDropBoxSize();
		RECT rcOwner = pOwner->GetPos();
		CDuiRect rc = rcOwner;
		rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
		rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
		if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		int cyFixed = 0;
		for( int it = 0; it < pOwner->GetCount(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(it));
			if( !pControl->IsVisible() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;
		}
		cyFixed += 4;
		rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

  		Create(m_pOwner->GetManager()->GetPaintWindow(), _T(""), 0, 0, rc);
		ShowWindow(true, true);
//		gtk_widget_show_all(GTK_WIDGET(m_hWnd));
// 		GtkWidget *pWindow = (GtkWidget *)m_pOwner->GetManager()->GetPaintWindow();
// 		gtk_window_set_transient_for(GTK_WINDOW(m_hWnd), GTK_WINDOW(pWindow));
// 		gtk_window_set_decorated(GTK_WINDOW(m_hWnd), FALSE);
// 		gtk_window_set_type_hint(GTK_WINDOW(m_hWnd), GDK_WINDOW_TYPE_HINT_DIALOG);
// 		int toplevel_x, toplevel_y;
// 		GdkWindow *gdk_window = gtk_widget_get_window(pWindow);
// 		gdk_window_get_origin(gdk_window, &toplevel_x, &toplevel_y); //父窗口的屏幕坐标
// 		gtk_window_move(GTK_WINDOW(m_hWnd), rc.left+toplevel_x, rc.top+toplevel_y);
// 		gtk_window_set_default_size(GTK_WINDOW(m_hWnd), rc.GetWidth(), rc.GetHeight());
// 		gtk_widget_show_all(GTK_WIDGET(m_hWnd));

// 		GtkWidget *pWindow = (GtkWidget *)m_pOwner->GetManager()->GetPaintWindow();
// 		m_hWnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//		gtk_widget_set_app_paintable(GTK_WIDGET(m_hWnd), TRUE);
// 		gtk_window_set_transient_for(GTK_WINDOW(m_hWnd), GTK_WINDOW(pWindow));
// 		gtk_window_set_decorated(GTK_WINDOW(m_hWnd), FALSE);
// 		gtk_window_set_type_hint(GTK_WINDOW(m_hWnd), GDK_WINDOW_TYPE_HINT_DIALOG);
// 		RegisterSignal();
// 
// 		int toplevel_x, toplevel_y;
// 		GdkWindow *gdk_window = gtk_widget_get_window(pWindow);
// 		gdk_window_get_origin(gdk_window, &toplevel_x, &toplevel_y); //父窗口的屏幕坐标
// 		gtk_window_move(GTK_WINDOW(m_hWnd), rc.left+toplevel_x, rc.top+toplevel_y);
// 		gtk_window_set_default_size(GTK_WINDOW(m_hWnd), rc.GetWidth(), rc.GetHeight());
// 		gtk_widget_show_all(GTK_WIDGET(m_hWnd));
	}

	LRESULT CComboWndGtk::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if( uMsg == WM_CREATE ) 
		{
			m_pm.SetForceUseSharedRes(true);
			m_pm.Init(m_hWnd, NULL, this);

			m_pLayout = new CVerticalLayoutUI;
			m_pLayout->SetManager(&m_pm, NULL, true);
			LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
			if( pDefaultAttributes ) 
			{
				m_pLayout->ApplyAttributeList(pDefaultAttributes);
			}
			m_pLayout->SetInset(CDuiRect(2, 1, 2, 1));
			m_pLayout->SetBkColor(0xFFFFFFFF);
			m_pLayout->SetBorderColor(0xFFC6C7D2);
			m_pLayout->SetBorderSize(1);
			m_pLayout->SetAutoDestroy(false);
			m_pLayout->EnableScrollBar();
			m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());
			for( int i = 0; i < m_pOwner->GetCount(); i++ ) 
			{
				m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
			}
			CShadowUI *pShadow = m_pOwner->GetManager()->GetShadow();
			pShadow->CopyShadow(m_pm.GetShadow());
			m_pm.GetShadow()->ShowShadow(m_pOwner->IsShowShadow());
			m_pm.AttachDialog(m_pLayout);
			m_pm.AddNotifier(this);
			return 0;
		}

		if( uMsg == WM_CLOSE ) 
		{
			m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
			RECT rcNull = { 0 };
			for( int i = 0; i < m_pOwner->GetCount(); i++ ) static_cast<CControlUI*>(m_pOwner->GetItemAt(i))->SetPos(rcNull);
			m_pOwner->SetFocus();
		}
		else if(uMsg == WM_LBUTTONDOWN)
		{
			GdkEventButton *ev = (GdkEventButton *)wParam;
			CDuiPoint pt(ev->x, ev->y);
			m_bHitItem = IsHitItem(pt);
		}
		else if(uMsg == WM_LBUTTONUP)
		{
			GdkEventButton *ev = (GdkEventButton *)wParam;
			CDuiPoint pt(ev->x, ev->y);
			if(m_bHitItem && IsHitItem(pt)) 
			{
				Close();
			}
			m_bHitItem = false;
		}
		else if( uMsg == WM_KILLFOCUS ) 
		{
			Close();
		}
		LRESULT lRes = 0;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowGtk::HandleMessage(uMsg, wParam, lParam);
	}

	void CComboWndGtk::OnFinalMessage(UIWND hWnd)
	{
		m_pOwner->m_pWindow = NULL;
		m_pOwner->SetPushedState(false);
		m_pOwner->Invalidate();
		delete this;
	}

	void CComboWndGtk::Notify(TNotifyUI& msg)
	{

	}

	bool CComboWndGtk::IsHitItem(POINT ptMouse)
	{
		CControlUI* pControl = m_pm.FindControl(ptMouse);
		if(pControl != NULL) {
			LPVOID pInterface = pControl->GetInterface(DUI_CTR_SCROLLBAR);
			if(pInterface) return false;

			while(pControl != NULL) {
				IListItemUI* pListItem = (IListItemUI*)pControl->GetInterface(DUI_CTR_LISTITEM);
				if(pListItem != NULL ) {
					return true;
				}
				pControl = pControl->GetParent();
			}
		}

		return false;
	}
} // namespace DuiLib
#endif //#ifdef DUILIB_WIN32

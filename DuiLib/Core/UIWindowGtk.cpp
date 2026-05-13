#include "StdAfx.h"

#ifdef DUILIB_GTK
namespace DuiLib {

//////////////////////////////////////////////////////////////////////////
//
static void wrap_button_event(GtkWidget *widget, GdkEventButton *ev, CWindowGtk *pWindow)
{
	//不知道为什么会收到两次事件？
	static GdkEventButton localEv;
	if(localEv.type == ev->type &&
		localEv.button == ev->button &&
		localEv.x == ev->x &&
		localEv.y == ev->y)
	{
		return;
	}
	localEv = *ev;

	if (ev->type == GDK_BUTTON_PRESS)
	{
		DUITRACE(_T("GDK_BUTTON_PRESS: button=%d, x=%d, y=%d"), ev->button, (int)ev->x, (int)ev->y);
		if(ev->button == 1)
		{
			pWindow->HandleMessage(WM_LBUTTONDOWN, (WPARAM)ev, 0);
		}
		else if(ev->button == 2)
		{
			pWindow->HandleMessage(WM_RBUTTONDOWN, (WPARAM)ev, 0);
		}
		else if(ev->button == 3)
		{
			pWindow->HandleMessage(WM_MOUSEWHEEL, (WPARAM)ev, 0);
		}
	}
	else if (ev->type == GDK_BUTTON_RELEASE)
	{
		DUITRACE(_T("GDK_BUTTON_RELEASE: button=%d, x=%d, y=%d"), ev->button, (int)ev->x, (int)ev->y);
		if(ev->button == 1)
		{
			pWindow->HandleMessage(WM_LBUTTONUP, (WPARAM)ev, 0);
		}
		else if(ev->button == 2)
		{
			pWindow->HandleMessage(WM_RBUTTONUP, (WPARAM)ev, 0);
		}
		else if(ev->button == 3)
		{
			pWindow->HandleMessage(WM_MOUSEWHEEL, (WPARAM)ev, 0);
		}
	}
	else if (ev->type == GDK_DOUBLE_BUTTON_PRESS)
	{
		DUITRACE(_T("GDK_DOUBLE_BUTTON_PRESS: button=%d, x=%d, y=%d"), ev->button, (int)ev->x, (int)ev->y);
		if(ev->button == 1)
		{
			pWindow->HandleMessage(WM_LBUTTONDBLCLK, (WPARAM)ev, 0);
		}
	}
}

static gboolean wrap_motion_notify(GtkWidget *widget, GdkEventMotion *ev, CWindowGtk *pWindow)
{
	//DUITRACE(_T("GDK_MOTION_NOTIFY: x=%d, y=%d"), (int)ev->x, (int)ev->y);
	return (gboolean)pWindow->HandleMessage(WM_MOUSEMOVE, (WPARAM)ev, NULL);
}

static void dui_window_destroy(GtkWidget *widget, CWindowGtk *pWindow)
{
	pWindow->HandleMessage(WM_CLOSE, 0, 0);
	pWindow->OnFinalMessage(pWindow);
}

static void wrap_size(GtkWidget *widget, GdkRectangle *allocation, CWindowGtk *pWindow)
{
	DUITRACE(_T("wrap_size: x=%d, y=%d"), allocation->width, allocation->height);
	SIZE Size = {allocation->width, allocation->height};
	pWindow->HandleMessage(WM_SIZE, (WPARAM)&Size, NULL);
}


static gboolean wrap_scroll_event (GtkWidget *widget, GdkEventScroll *Event, CWindowGtk *pWindow)
{
	return (gboolean)pWindow->HandleMessage(WM_MOUSEWHEEL, (WPARAM)Event, NULL);
}

static gboolean wrap_enter_notify(GtkWidget *widget, GdkEventCrossing *event, CWindowGtk *pWindow)
{
	//return (gboolean)pWindow->HandleMessage(WM_MOUSEENTER, (WPARAM)event, NULL);
	return false;
}

static gboolean wrap_leave_notify(GtkWidget *widget, GdkEventCrossing *event, CWindowGtk *pWindow)
{
	return (gboolean)pWindow->HandleMessage(WM_MOUSELEAVE, (WPARAM)event, NULL);
}

static gboolean wrap_key_press(GtkWidget *widget, GdkEvent *event, CWindowGtk *pWindow)
{
	return (gboolean)pWindow->HandleMessage(WM_KEYDOWN, (WPARAM)event, NULL);
}

static gboolean wrap_key_release(GtkWidget *widget, GdkEvent *Event, CWindowGtk *pWindow)
{
	return (gboolean)pWindow->HandleMessage(WM_KEYUP, (WPARAM)Event, NULL);
}

static gboolean wrap_delete_event(GtkWidget *widget, GdkEvent  *Event, CWindowGtk *pWindow)
{
	return (gboolean)pWindow->HandleMessage(WM_CLOSE, (WPARAM)Event, NULL);
}

static gboolean wrap_draw(GtkWidget *widget, cairo_t *cr, CWindowGtk *pWindow)
{
	return (gboolean)pWindow->HandleMessage(WM_PAINT, (LPARAM)cr, NULL);
}

static gboolean wrap_timer_event(gpointer userdata)
{
	TIMERINFO *pTimer = (TIMERINFO *)userdata;
	//DUITRACE(_T("timer event\n"));
	CWindowGtk *pWindow = (CWindowGtk *)pTimer->pWindow;
	pWindow->HandleMessage(WM_TIMER, (WPARAM)pTimer->uWinTimer, 0);
	return true;
}

static void wrap_window_realize(GtkWidget *widget, gpointer user_data) 
{
	GdkWindow *window = gtk_widget_get_window(widget);
	if (window) 
	{
		CWindowGtk *pWindow = (CWindowGtk *)user_data;
		pWindow->HandleMessage(WM_CREATE, 0, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
CWindowGtk::CWindowGtk()
{
}

CWindowGtk::~CWindowGtk()
{
}

UIWND CWindowGtk::CreateDuiWindow(UIWND hwndParent, LPCTSTR pstrWindowName,DWORD dwStyle, DWORD dwExStyle)
{
	return Create(hwndParent,pstrWindowName,dwStyle,dwExStyle,0,0,0,0);
}

UIWND CWindowGtk::Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc)
{
	return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}

UIWND CWindowGtk::Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy)
{
	m_hWnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	GtkWidget *pGtkFixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(m_hWnd), pGtkFixed);
	gtk_widget_show(pGtkFixed);
	m_pGtkFixed = pGtkFixed;

	//隐藏标题栏
	gtk_window_set_decorated(GTK_WINDOW(m_hWnd), FALSE);
	
	//gtk_container_set_border_width(GTK_CONTAINER(m_hWnd), 8);
	//gtk_window_set_title(GTK_WINDOW(m_hWnd), "Hello World");

	//自绘窗口
	gtk_widget_set_app_paintable(GTK_WIDGET(m_hWnd), TRUE);

	if(hwndParent != NULL)
	{
		GtkWidget *pWindow = (GtkWidget *)hwndParent;
		gtk_window_set_transient_for(GTK_WINDOW(m_hWnd), GTK_WINDOW(pWindow));
		gtk_window_set_type_hint(GTK_WINDOW(m_hWnd), GDK_WINDOW_TYPE_HINT_DIALOG);
		int toplevel_x, toplevel_y;
		GdkWindow *gdk_window = gtk_widget_get_window(pWindow);
		gdk_window_get_origin(gdk_window, &toplevel_x, &toplevel_y); //父窗口的屏幕坐标
		gtk_window_move(GTK_WINDOW(m_hWnd), x+toplevel_x, y+toplevel_y);
	}

	gtk_window_set_default_size(GTK_WINDOW(m_hWnd), cx, cy);

	RegisterSignal();
	return m_hWnd;
}

void CWindowGtk::RegisterSignal()
{
	gtk_widget_add_events(GTK_WIDGET(m_hWnd),
		GDK_EXPOSURE_MASK               |
		GDK_SCROLL_MASK                 |
		GDK_POINTER_MOTION_MASK         |
		GDK_POINTER_MOTION_HINT_MASK    |
		GDK_BUTTON_MOTION_MASK          |
		GDK_BUTTON1_MOTION_MASK         |
		GDK_BUTTON2_MOTION_MASK         |
		GDK_BUTTON3_MOTION_MASK         |
		GDK_BUTTON_PRESS_MASK           |
		GDK_BUTTON_RELEASE_MASK         |
		GDK_KEY_PRESS_MASK              |
		GDK_KEY_RELEASE_MASK            |
		GDK_ENTER_NOTIFY_MASK           |
		GDK_LEAVE_NOTIFY_MASK           |
		GDK_FOCUS_CHANGE_MASK);

	//鼠标按下
	g_signal_connect(G_OBJECT(m_hWnd), "button-press-event", G_CALLBACK(wrap_button_event), this);
	//鼠标释放（弹起）
	g_signal_connect(G_OBJECT(m_hWnd), "button-release-event", G_CALLBACK(wrap_button_event), this);
	//鼠标移动
	g_signal_connect(G_OBJECT(m_hWnd), "motion-notify-event", G_CALLBACK(wrap_motion_notify), this);
	g_signal_connect(G_OBJECT(m_hWnd), "destroy", G_CALLBACK(dui_window_destroy), this);

	g_signal_connect(G_OBJECT(m_hWnd), "size-allocate", G_CALLBACK(wrap_size), this);
	g_signal_connect(G_OBJECT(m_hWnd), "scroll-event", G_CALLBACK(wrap_scroll_event), this);
	g_signal_connect(G_OBJECT(m_hWnd), "enter-notify-event", G_CALLBACK(wrap_enter_notify), this);
	g_signal_connect(G_OBJECT(m_hWnd), "leave-notify-event", G_CALLBACK(wrap_leave_notify), this);
	g_signal_connect(G_OBJECT(m_hWnd), "key-press-event", G_CALLBACK(wrap_key_press), this);
	g_signal_connect(G_OBJECT(m_hWnd), "key-release-event", G_CALLBACK(wrap_key_release), this);
	g_signal_connect(G_OBJECT(m_hWnd), "delete-event", G_CALLBACK(wrap_delete_event), this);
	g_signal_connect(G_OBJECT(m_hWnd), "draw", G_CALLBACK(wrap_draw), this);
	g_signal_connect(G_OBJECT(m_hWnd), "realize", G_CALLBACK(wrap_window_realize), this);
}

BOOL CWindowGtk::IsWindow(UIWND hWnd)
{
	return hWnd != NULL;
}

LRESULT CWindowGtk::SendMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0)
{
	return 0;
}

BOOL CWindowGtk::PostMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0)
{
	return FALSE;
}

BOOL CWindowGtk::SetWindowPos(int x, int y, int cx, int cy, UINT uFlags)
{
	gtk_window_set_default_size(GTK_WINDOW(hWnd), cx, cy);
	return TRUE;
}

BOOL CWindowGtk::GetWindowRect(LPRECT lpRect)
{
	GtkAllocation Allocation;
	gtk_widget_get_allocation(GTK_WIDGET(hWnd), &Allocation);
	lpRect->left = Allocation.x;
	lpRect->top = Allocation.y;
	lpRect->right = Allocation.x + Allocation.width;
	lpRect->bottom = Allocation.y + Allocation.height;

	//gtk_window_get_size
	return TRUE;
}

BOOL CWindowGtk::GetClientRect(LPRECT lpRect)
{
	return GetWindowRect(hWnd, lpRect);
}

void CWindowGtk::Close(UINT nRet)
{
	gtk_window_close(GTK_WINDOW(m_hWnd));
}

void CWindowGtk::SetCursor(int nCursor)
{
	GdkCursor *cursor;
	GdkCursor *lastcursor;

	GdkCursorType cursortype;
	switch (nCursor)
	{
	case DUI_ARROW: //           32512	
		cursortype = GDK_TOP_LEFT_ARROW;
		break;
	case DUI_IBEAM: //           32513
		cursortype = GDK_XTERM;
		break;
	case DUI_WAIT: //            32514
		cursortype = GDK_WATCH;
		break;
	case DUI_CROSS: //           32515
		cursortype = GDK_CROSS;
		break;
	case DUI_UPARROW: //         32516
		cursortype = GDK_SB_UP_ARROW;
		break;
	case DUI_SIZE: //            32640
		cursortype = GDK_FLEUR;
		break;
	case DUI_ICON: //            32641
		cursortype = GDK_ICON;
		break;
	case DUI_SIZENWSE: //        32642
		cursortype = GDK_TOP_LEFT_CORNER;
		break;
	case DUI_SIZENESW: //        32643
		cursortype = GDK_TOP_RIGHT_CORNER;
		break;
	case DUI_SIZEWE: //          32644
		cursortype = GDK_SB_H_DOUBLE_ARROW;
		break;
	case DUI_SIZENS: //          32645
		cursortype = GDK_SB_V_DOUBLE_ARROW;
		break;
	case DUI_SIZEALL: //         32646
		cursortype = GDK_SIZING;
		break;
	case DUI_NO: //              32648
		cursortype = GDK_SB_V_DOUBLE_ARROW;
		break;
	case DUI_HAND: //            32649
		cursortype = GDK_HAND2;
		break;
	default:
		cursortype = GDK_TOP_LEFT_ARROW;
		break;
	}

	cursor = gdk_cursor_new(cursortype);

	GdkWindow *gdkWindow = gtk_widget_get_window(GTK_WIDGET(m_hWnd));
	if(gdkWindow)
	{
		//lastcursor = gdk_window_get_cursor(gdkWindow);
		gdk_window_set_cursor(gdkWindow, cursor);
	}
	gdk_cursor_unref(cursor);
//	return lastcursor;
}

void CWindowGtk::ShowWindow(bool bShow, bool bTakeFocus)
{
	gtk_widget_show(GTK_WIDGET(m_hWnd));
}

void CWindowGtk::CenterWindow()	// 居中，支持扩展屏幕
{
	gtk_window_set_position(GTK_WINDOW(m_hWnd), GTK_WIN_POS_CENTER);
	gtk_widget_realize(GTK_WIDGET(m_hWnd));
	gtk_widget_show_all(GTK_WIDGET(m_hWnd));
}

PVOID CWindowGtk::GetFixedContainer()
{
	return m_pGtkFixed;
}

LRESULT CWindowGtk::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

} // namespace DuiLib
#endif //#ifdef DUILIB_GTK

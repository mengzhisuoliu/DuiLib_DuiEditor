#include "StdAfx.h"
#include "UIEditWndGtk.h"

#ifdef DUILIB_GTK
namespace DuiLib
{
//	GtkWidget *CEditWndGtk::m_pGtkFixed = NULL;

	CEditWndGtk::CEditWndGtk(CControlUI *pOwner) : CEditWnd(pOwner)
	{
		m_pGtkEntry = NULL;

		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_NOTHING);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DELETE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DESTROY);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_EXPOSE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_MOTION_NOTIFY);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_BUTTON_PRESS);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_2BUTTON_PRESS);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DOUBLE_BUTTON_PRESS);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_3BUTTON_PRESS);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_TRIPLE_BUTTON_PRESS);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_BUTTON_RELEASE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_KEY_PRESS);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_KEY_RELEASE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_ENTER_NOTIFY);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_LEAVE_NOTIFY);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_FOCUS_CHANGE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_CONFIGURE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_MAP);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_UNMAP);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_PROPERTY_NOTIFY);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_SELECTION_CLEAR);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_SELECTION_REQUEST);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_SELECTION_NOTIFY);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_PROXIMITY_IN);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_PROXIMITY_OUT);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DRAG_ENTER);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DRAG_LEAVE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DRAG_MOTION);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DRAG_STATUS);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DROP_START);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DROP_FINISHED);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_CLIENT_EVENT);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_VISIBILITY_NOTIFY);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_SCROLL);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_WINDOW_STATE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_SETTING);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_OWNER_CHANGE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_GRAB_BROKEN);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_DAMAGE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_TOUCH_BEGIN);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_TOUCH_UPDATE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_TOUCH_END);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_TOUCH_CANCEL);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_TOUCHPAD_SWIPE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_TOUCHPAD_PINCH);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_PAD_BUTTON_PRESS);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_PAD_BUTTON_RELEASE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_PAD_RING);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_PAD_STRIP);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_PAD_GROUP_MODE);
		MACROTOSTRINGMAP_ADD(m_eventStringMap, GDK_EVENT_LAST);
	}

	CEditWndGtk::~CEditWndGtk()
	{
// 		if(m_pGtkEntry) 
// 		{ 
// 			g_object_unref(m_pGtkEntry); m_pGtkEntry = NULL; 
// 		}
	}

	void CEditWndGtk::Init()
	{
		if(m_pGtkEntry != NULL)
			return;

		CDuiRect rc = CalPos();
		CDuiRect rcTextPadding = m_pOwner->GetTextPadding();

		GtkWidget *pWindow = (GtkWidget *)GetOwner()->GetManager()->GetPaintWindow();

// 		if(m_pGtkFixed == NULL)
// 		{
// 			m_pGtkFixed = gtk_fixed_new();
// 			//gtk_widget_set_size_request(m_pGtkFixed, rc.GetWidth(), rc.GetHeight());
// 			gtk_container_add(GTK_CONTAINER(pWindow), m_pGtkFixed);
// 			gtk_widget_show (m_pGtkFixed);
// 		}

		CWindowGtk *pWindowGtk = (CWindowGtk *)GetOwner()->GetManager()->GetWindow();
		LPVOID pFixedContainer = pWindowGtk->GetFixedContainer();

		m_pGtkEntry = gtk_entry_new();
		gtk_fixed_put(GTK_FIXED(pFixedContainer), m_pGtkEntry, rc.left, rc.top);
		gtk_widget_set_size_request(m_pGtkEntry, rc.GetWidth(), rc.GetHeight());
		//gtk_entry_set_has_frame(GTK_ENTRY(m_pGtkEntry), FALSE); //CUIApplicationGtk::InitInstance中设置css样式
		gtk_entry_set_max_length(GTK_ENTRY(m_pGtkEntry), GetMaxChar());
		gtk_editable_set_editable(GTK_EDITABLE(m_pGtkEntry), !GetOwner()->IsReadOnly());
		
		gtk_entry_set_text(GTK_ENTRY(m_pGtkEntry), GetOwner()->GetText().convert_to_auto());

		//gtk_widget_set_padding(m_pGtkEntry, rcTextPadding.left, rcTextPadding.top);

		g_signal_connect(G_OBJECT(m_pGtkEntry), "event", G_CALLBACK(wrap_event), this);
		g_signal_connect(G_OBJECT(m_pGtkEntry), "changed", G_CALLBACK(on_entry_changed), this);

		gtk_widget_show(m_pGtkEntry);
		gtk_window_set_focus(GTK_WINDOW(pWindow), m_pGtkEntry);
		gtk_editable_set_position(GTK_EDITABLE(m_pGtkEntry), -1);
	}

	bool CEditWndGtk::IsInit()
	{
		return m_pGtkEntry != NULL;
	}

	void CEditWndGtk::SetPos(CDuiRect rc)
	{
		if(m_pGtkEntry)
		{
			CWindowGtk *pWindow = (CWindowGtk *)GetOwner()->GetManager()->GetWindow();
			LPVOID pFixedContainer = pWindow->GetFixedContainer();
			gtk_fixed_move(GTK_FIXED(pFixedContainer), m_pGtkEntry, rc.left, rc.top);
			gtk_widget_set_size_request(m_pGtkEntry, rc.GetWidth(), rc.GetHeight());
		}
	}

	void CEditWndGtk::Move(SIZE szOffset)
	{
		
	}

	CDuiRect CEditWndGtk::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetClientPos();
		CControlUI* pParent = m_pOwner;
		RECT rcParent;
		while( pParent = pParent->GetParent() ) {
			if( !pParent->IsVisible() ) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
			rcParent = pParent->GetClientPos();
			//if( !::IntersectRect(&rcPos, &rcPos, &rcParent) ) {
			if( !rcPos.Intersect(rcPos, rcParent) ) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
		}

		return rcPos;
	}

	bool CEditWndGtk::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_SETFOCUS && m_pOwner->IsEnabled()) 
		{
			if(!IsInit())
			{
				if(!m_pOwner->GetClientPos().PtInRect(event.ptMouse))
					return false;
				Init();
			}

			if(!m_pGtkEntry)
				return false;
			
			GtkWidget *pWindow = (GtkWidget *)GetOwner()->GetManager()->GetPaintWindow();
			gtk_window_set_focus(GTK_WINDOW(pWindow), m_pGtkEntry);
			gtk_editable_set_position(GTK_EDITABLE(m_pGtkEntry), -1);
			m_pOwner->Invalidate();
			return true;		}

		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if(!m_pOwner->GetClientPos().PtInRect(event.ptMouse))
				return false;
			if( m_pOwner->IsEnabled()) 
			{
				m_pOwner->GetManager()->ReleaseCapture();
				if( m_pOwner->IsFocused() && !IsInit() )
				{
					Init();
				}
				else
				{
					if (!m_bAutoSelAll) 
					{
						//gtk_entry_grab_focus_without_selecting(GTK_ENTRY(m_pGtkEntry));
					}
				}
			}
			return true;
		}
		return false;
	}

	void CEditWndGtk::SetEditText(LPCTSTR sText)
	{
		CDuiString s(sText);
		gtk_entry_set_text(GTK_ENTRY(m_pGtkEntry), s.convert_to_auto());
	}

	bool CEditWndGtk::NeedPaintText()
	{
		return gtk_widget_is_visible(m_pGtkEntry) != TRUE; 
	}

	gboolean CEditWndGtk::wrap_event(GtkWidget* self, GdkEvent* ev, gpointer user_data)
	{
		CEditWndGtk *pWindow = (CEditWndGtk *)user_data;
		CDuiString sEventName = pWindow->m_eventStringMap.Lookup(ev->type);

		if(ev->type != GDK_MOTION_NOTIFY)
		{
			CDuiString s;
			s.Format(_T("CEditWndGtk::wrap_event, %s, entry=%p, self=%p\r\n"), 
				sEventName.toString(),
				pWindow->m_pGtkEntry,
				self);
			OutputDebugString(s);
		}

		if(ev->type == GDK_FOCUS_CHANGE) //改变焦点
		{
			if(ev->focus_change.in == 1) //获得焦点
			{
				//gtk_widget_show(pWindow->m_pGtkEntry);
				gtk_widget_set_visible(pWindow->m_pGtkEntry, true);
			}
			else //失去焦点
			{
				//gtk_widget_hide(pWindow->m_pGtkEntry);
				gtk_widget_set_visible(pWindow->m_pGtkEntry, false);
			}
			return false;
		}
		return false;
	}

	void CEditWndGtk::on_entry_changed(GtkEntry *entry, gpointer user_data) 
	{
		CEditWndGtk *pWindow = (CEditWndGtk *)user_data;
		CDuiString sText = gtk_entry_get_text(entry);
		pWindow->SetOwnerText(sText.convert_to_auto());
	}

}
#endif //#ifdef DUILIB_GTK

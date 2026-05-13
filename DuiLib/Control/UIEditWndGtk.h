#ifndef __UIEDIT_WND_GTK_H__
#define __UIEDIT_WND_GTK_H__

#pragma once

#ifdef DUILIB_GTK
namespace DuiLib
{
	class CEditWndGtk : public CEditWnd//, public CWindowGtk, 
	{
	public:
		CEditWndGtk(CControlUI *pOwner);
		virtual ~CEditWndGtk();

		virtual void Init() override;
		virtual bool IsInit() override;

		virtual void SetPos(CDuiRect rc) override;
		virtual void Move(SIZE szOffset) override;
		virtual CDuiRect CalPos() override;
		virtual bool DoEvent(TEventUI& event) override;

		virtual void SetEditText(LPCTSTR sText) override;
		virtual bool NeedPaintText() override;

		static gboolean wrap_event(GtkWidget* self, GdkEvent* ev, gpointer user_data);
		static void on_entry_changed(GtkEntry *entry, gpointer user_data);

		CMacroToStringMap m_eventStringMap;
	protected:
		//static GtkWidget *m_pGtkFixed;
		GtkWidget *m_pGtkEntry;
	};
} //namespace DuiLib
#endif //#ifdef DUILIB_GTK
#endif // __UIEDIT_H__
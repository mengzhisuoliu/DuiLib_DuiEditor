#ifndef __UICOMBO_GTK_H__
#define __UICOMBO_GTK_H__

#pragma once

#ifdef DUILIB_GTK
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class CComboWndGtk : public CWindowGtk, public INotifyUI
	{
	public:
		void Init(CComboUI* pOwner);
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		void OnFinalMessage(UIWND hWnd) override;

		void Notify(TNotifyUI& msg) override;

		bool IsHitItem(POINT ptMouse);
	protected:
		CComboUI* m_pOwner;
		CVerticalLayoutUI* m_pLayout;
		bool m_bHitItem;
	};

} // namespace DuiLib
#endif //#ifdef DUILIB_GTK

#endif // __UICOMBO_GTK_H__

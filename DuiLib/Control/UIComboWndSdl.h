#ifndef __UICOMBO_SDL_H__
#define __UICOMBO_SDL_H__

#pragma once

#ifdef DUILIB_SDL
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class CComboWndSDL : public CWindowSDL, public INotifyUI
	{
	public:
		void Init(CComboUI* pOwner);
		void OnFinalMessage(UIWND hWnd) override;
		void Notify(TNotifyUI& msg) override;
		bool IsHitItem(CDuiPoint ptMouse);

	protected:
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
	protected:
		CComboUI* m_pOwner;
		CVerticalLayoutUI* m_pLayout;
		bool m_bHitItem;
	};

} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

#endif // __UICOMBO_SDL_H__

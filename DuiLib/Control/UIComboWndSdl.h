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
#endif //#ifdef DUILIB_SDL

#endif // __UICOMBO_SDL_H__

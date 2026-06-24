#ifndef __UIHOTKEY_H__
#define __UIHOTKEY_H__
#pragma once

#ifdef DUILIB_WIN32
namespace DuiLib{
	class CHotKeyUI;

	class UILIB_API CHotKeyWnd : public CWindowWin32
	{
	public:
		CHotKeyWnd(void);

	public:
		void Init(CHotKeyUI * pOwner);
		CDuiRect CalPos();
		LPCTSTR GetWindowClassName() const;
		void OnFinalMessage(HWND hWnd);
		LPCTSTR GetSuperClassName() const;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
	public:
		void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);
		void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
		DWORD GetHotKey(void) const;
		CDuiString GetHotKeyName();
		void SetRules(WORD wInvalidComb, WORD wModifiers);
		CDuiString GetKeyName(UINT vk, uiBool fExtended);
	protected:
		CHotKeyUI * m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
		CStdRefPtr<UIFont> m_font;
	};

	class UILIB_API CHotKeyUI : public CLabelUI
	{
		DECLARE_DUICONTROL(CHotKeyUI)
		friend CHotKeyWnd;
	public:
		CHotKeyUI();

		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;
		virtual UINT GetControlFlags() const override;
		virtual void SetText(LPCTSTR pstrText) override;
		
		virtual void SetNativeBkColor(CDuiColor dwBkColor);
		virtual CDuiColor GetNativeBkColor() const;

		virtual void SetPos(CDuiRect rc, bool bNeedInvalidate = true) override;
		virtual void SetVisible(bool bVisible = true) override;
		virtual void SetInternVisible(bool bVisible = true) override;
		virtual CDuiSize EstimateSize(CDuiSize szAvailable) override;
		virtual void DoEvent(TEventUI& event) override;
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

	public:
		void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
		DWORD GetHotKey(void) const;
		void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);

	protected:
		CHotKeyWnd * m_pWindow;
		DWORD m_dwHotKeybkColor;

	protected:
		WORD m_wVirtualKeyCode;
		WORD m_wModifiers;
	};
}

#endif //#ifdef DUILIB_WIN32
#endif //#ifndef __UIHOTKEY_H__

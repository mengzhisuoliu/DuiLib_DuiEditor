#ifndef __UIEDIT_WND_WIN32_H__
#define __UIEDIT_WND_WIN32_H__

#pragma once

#ifdef DUILIB_WIN32
namespace DuiLib
{
	class CEditWndWin32 : public CWindowWin32, public CEditWnd
	{
	public:
		CEditWndWin32(CControlUI *pOwner);

		virtual void Init() override;
		virtual CDuiRect CalPos() override;

		virtual void SetEditText(LPCTSTR sText) override;

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		virtual void OnFinalMessage(HWND hWnd) override;

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);

	protected:
		enum { 
			DEFAULT_TIMERID = 20,
		};

		HBRUSH m_hBkBrush;
		bool m_bDrawCaret;
		CStdRefPtr<UIFont> m_font;
		CStdRefPtr<UIBitmap> m_bmpBrush;
		CStdRefPtr<UIBrush> m_brush;
	};
}
#endif // #ifdef DUILIB_WIN32
#endif // __UIEDIT_H__
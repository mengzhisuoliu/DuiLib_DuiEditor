#ifndef __UIDATETIME_WND_WIN32_H__
#define __UIDATETIME_WND_WIN32_H__

#pragma once

#ifdef DUILIB_WIN32
namespace DuiLib
{
	class CDateTimeWndWin32 : public CWindowWin32
	{
	public:
		CDateTimeWndWin32();

		void Init(CDateTimeUI* pOwner, CDuiRect rcBase, UINT uFormatStyle);
		CDuiRect CalPos(CDuiRect rcBase);


		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		CDateTimeUI* m_pOwner;
		UINT m_uFormatStyle;
		HBRUSH m_hBkBrush;
		bool m_bDropOpen;
		SYSTEMTIME m_oldSysTime;
		CStdRefPtr<UIFont> m_font;
	};
}
#endif // #ifdef DUILIB_WIN32
#endif // __UIDATETIME_H__

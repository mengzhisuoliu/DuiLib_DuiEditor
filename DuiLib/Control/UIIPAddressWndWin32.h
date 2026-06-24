#ifndef __UIIPADDRESS_WND_WIN32_H__
#define __UIIPADDRESS_WND_WIN32_H__

#pragma once

#ifdef DUILIB_WIN32
namespace DuiLib
{
#define IP_NONE   0
#define IP_UPDATE 1
#define IP_DELETE 2
#define IP_KEEP   3

class CIPAddressWndWin32 : public CWindowWin32
{
public:
	CIPAddressWndWin32();
	
	void Init(CIPAddressUI* pOwner);
	CDuiRect CalPos();

	LPCTSTR GetWindowClassName() const;
	LPCTSTR GetSuperClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);

	static DWORD GetLocalIpAddress();
protected:
	CIPAddressUI* m_pOwner;
	HBRUSH m_hBkBrush;
	bool m_bInit;
	CStdRefPtr<UIFont> m_font;
};

} //#ifdef DUILIB_WIN32
#endif // DUILIB_WIN32
#endif // __UIEDIT_WND_WIN32_H__
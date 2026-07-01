#include "StdAfx.h"
#include "UIIPAddressWndWin32.h"

#ifdef DUILIB_WIN32
#pragma comment( lib, "ws2_32.lib" )

namespace DuiLib
{

//////////////////////////////////////////////////////////////////////////
//
//
CIPAddressWndWin32::CIPAddressWndWin32() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
{
}

void CIPAddressWndWin32::Init(CIPAddressUI* pOwner)
{
	m_pOwner = pOwner;
	m_pOwner->m_nIPUpdateFlag = IP_NONE;

	if (m_hWnd == NULL)
	{
		INITCOMMONCONTROLSEX   CommCtrl;
		CommCtrl.dwSize = sizeof(CommCtrl);
		CommCtrl.dwICC = ICC_INTERNET_CLASSES;//指定Class
		if (InitCommonControlsEx(&CommCtrl))
		{
			CDuiRect rcPos = CalPos();
			UINT uStyle = WS_CHILD | WS_TABSTOP | WS_GROUP;
			Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight());

			LONG_PTR style = GetWindowLongPtr(GetHWND(), GWL_STYLE);
			style &= ~WS_BORDER;
			SetWindowLongPtr(GetHWND(), GWL_STYLE, style);
			LONG_PTR exStyle = GetWindowLongPtr(GetHWND(), GWL_EXSTYLE);
			exStyle &= ~WS_EX_CLIENTEDGE;
			SetWindowLongPtr(GetHWND(), GWL_EXSTYLE, exStyle);
			::SetWindowPos(GetHWND(), NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
		//不知何故，窗口关闭时，这个字体会被卸载掉。 有人知道请告诉我哈。
		//SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->hFont, TRUE);
		m_font = MakeRefPtr<UIFont>(m_pOwner->GetManager()->CloneFont(m_pOwner->GetFont()));
		::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)m_font->GetHFONT(m_pOwner->GetManager()), (LPARAM)uiTrue);
	}

	if (m_pOwner->GetText().IsEmpty())
		m_pOwner->m_dwIP = GetLocalIpAddress();
	::SendMessage(m_hWnd, IPM_SETADDRESS, 0, m_pOwner->m_dwIP);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SetFocus(m_hWnd);

	m_bInit = true;
}

CDuiRect CIPAddressWndWin32::CalPos()
{
	CDuiRect rcPos = m_pOwner->GetPos();
	rcPos.Deflate(1, 1, 1, 1);
	return rcPos;
}

LPCTSTR CIPAddressWndWin32::GetWindowClassName() const
{
	return _T("IPAddressWnd");
}

LPCTSTR CIPAddressWndWin32::GetSuperClassName() const
{
	return WC_IPADDRESS;
}

void CIPAddressWndWin32::OnFinalMessage(HWND /*hWnd*/)
{
	// Clear reference and die
	if (m_hBkBrush != NULL) ::DeleteObject(m_hBkBrush);
	m_pOwner->m_pWindow = NULL;
	delete this;
}

LRESULT CIPAddressWndWin32::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	uiBool bHandled = uiTrue;
	if (uMsg == WM_KILLFOCUS)
	{
		bHandled = uiTrue;
		return 0;
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
	}
	else if (uMsg == WM_KEYUP && (wParam == VK_DELETE || wParam == VK_BACK))
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		m_pOwner->m_nIPUpdateFlag = IP_DELETE;
		m_pOwner->UpdateText();
		::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		return lRes;
	}
	else if (uMsg == WM_KEYUP && wParam == VK_ESCAPE)
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		m_pOwner->m_nIPUpdateFlag = IP_KEEP;
		::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		return lRes;
	}
	else if (uMsg == OCM_COMMAND) {
		if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_KILLFOCUS)
		{
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
	}
	else bHandled = uiFalse;
	if (!bHandled) return DefaultWndProc(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CIPAddressWndWin32::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	HWND hWndFocus = GetFocus();
	while (hWndFocus)
	{
		if (GetFocus() == m_hWnd)
		{
			bHandled = uiTrue;
			return 0;
		}
		hWndFocus = GetParent(hWndFocus);
	}

	LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	if (m_pOwner->m_nIPUpdateFlag == IP_NONE)
	{
		::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM)&m_pOwner->m_dwIP);
		m_pOwner->m_nIPUpdateFlag = IP_UPDATE;
		m_pOwner->UpdateText();
	}

	::ShowWindow(m_hWnd, SW_HIDE);
	return lRes;
}


DWORD CIPAddressWndWin32::GetLocalIpAddress()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return 0;
	char local[255] = { 0 };
	gethostname(local, sizeof(local));
	hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return 0;
	in_addr addr;
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr));
	DWORD dwIP = MAKEIPADDRESS(addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
	return dwIP;
}

	
} //namespace DuiLib
#endif // DUILIB_WIN32


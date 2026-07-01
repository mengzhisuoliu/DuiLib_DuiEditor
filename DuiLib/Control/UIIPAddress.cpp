#include "StdAfx.h"

#include "UIIPAddressWndWin32.h"
#include "UIIPAddressWndSdl.h"

namespace DuiLib
{
//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DUICONTROL(CIPAddressUI)

CIPAddressUI::CIPAddressUI()
{
	m_dwIP = CIPAddressWnd::GetLocalIpAddress();
	m_pWindow = NULL;
	m_nIPUpdateFlag = IP_UPDATE;
	UpdateText();
	m_nIPUpdateFlag = IP_NONE;
}

LPCTSTR CIPAddressUI::GetClass() const
{
	return _T("IPAddressUI");
}

LPVOID CIPAddressUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, DUI_CTR_IPADDRESS) == 0) return static_cast<CIPAddressUI*>(this);
	return CLabelUI::GetInterface(pstrName);
}

void CIPAddressUI::DoInit()
{

}

DWORD CIPAddressUI::GetIP()
{
	return m_dwIP;
}

void CIPAddressUI::SetIP(DWORD dwIP)
{
	m_dwIP = dwIP;
	UpdateText();
}

void CIPAddressUI::SetText(LPCTSTR pstrText)
{
	LPTSTR pstr = NULL;
	BYTE s1 = (BYTE)_tcstol(pstrText, &pstr, 10);
	BYTE s2 = (BYTE)_tcstol(pstr + 1, &pstr, 10);
	BYTE s3 = (BYTE)_tcstol(pstr + 1, &pstr, 10);
	BYTE s4 = (BYTE)_tcstol(pstr + 1, &pstr, 10);
	SetIP(UIMAKEIPADDRESS(s1, s2, s3, s4));
}

CDuiString CIPAddressUI::GetText() const
{
	CDuiString s;
	WORD hiword = HIWORD(m_dwIP);
	WORD loword = LOWORD(m_dwIP);
	s.Format(_T("%d.%d.%d.%d"), HIBYTE(hiword), LOBYTE(hiword), HIBYTE(loword), LOBYTE(loword));
	return s;
}

int  CIPAddressUI::GetTextN()
{
	return m_dwIP;
}

void CIPAddressUI::SetTextN(int n)
{
	SetIP(n);
}

void CIPAddressUI::UpdateText()
{
// 	if (m_nIPUpdateFlag == IP_DELETE)
// 		SetText(_T(""));
// 	else if (m_nIPUpdateFlag == IP_UPDATE)
// 	{
// 		TCHAR szIP[MAX_PATH] = { 0 };
// 		in_addr addr;
// 		addr.S_un.S_addr = m_dwIP;
// 		_stprintf(szIP, _T("%d.%d.%d.%d"), addr.S_un.S_un_b.s_b4, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b1);
// 		SetText(szIP);
// 	}
}

void CIPAddressUI::SetPos(CDuiRect rc, bool bNeedInvalidate)
{
	CControlUI::SetPos(rc, bNeedInvalidate);
	if (m_pWindow != NULL)
	{
		CDuiRect rcPos = m_pWindow->CalPos();
		((CIPAddressWnd*)m_pWindow)->SetWindowPos(rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void CIPAddressUI::Move(CDuiSize szOffset, bool bNeedInvalidate)
{
	CControlUI::Move(szOffset, bNeedInvalidate);
	if (m_pWindow != NULL)
	{
		CDuiRect rcPos = m_pWindow->CalPos();
		((CIPAddressWnd*)m_pWindow)->SetWindowPos(rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE);
	}
}


void CIPAddressUI::SetVisible(bool bVisible)
{
	CControlUI::SetVisible(bVisible);
	if (!IsVisible() && m_pWindow != NULL) m_pManager->SetFocus(NULL);
}

void CIPAddressUI::SetInternVisible(bool bVisible)
{
	if (!IsVisible() && m_pWindow != NULL) m_pManager->SetFocus(NULL);
}

void CIPAddressUI::DoEvent(TEventUI & event)
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
		if (m_pParent != NULL) m_pParent->DoEvent(event);
		else CLabelUI::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_SETCURSOR && IsEnabled())
	{
		//::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
		GetManager()->SetCursor(DUI_IBEAM);
		return;
	}
	if (event.Type == UIEVENT_WINDOWSIZE)
	{
		if (m_pWindow != NULL) m_pManager->SetFocusNeeded(this);
	}
	if (event.Type == UIEVENT_SCROLLWHEEL)
	{
		if (m_pWindow != NULL) return;
	}
	if (event.Type == UIEVENT_SETFOCUS && IsEnabled())
	{
		if (m_pWindow)
		{
			return;
		}
		m_pWindow = new CIPAddressWnd();
		ASSERT(m_pWindow);
		m_pWindow->Init(this);
		m_pWindow->ShowWindow();
	}
	if (event.Type == UIEVENT_KILLFOCUS && IsEnabled())
	{
		Invalidate();
	}
	if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN)
	{
		if (IsEnabled()) {
			GetManager()->ReleaseCapture();
			if (IsFocused() && m_pWindow == NULL)
			{
				m_pWindow = new CIPAddressWnd();
				ASSERT(m_pWindow);
			}
			if (m_pWindow != NULL)
			{
				m_pWindow->Init(this);
				m_pWindow->ShowWindow();
			}
		}
		return;
	}
	if (event.Type == UIEVENT_MOUSEMOVE)
	{
		return;
	}
	if (event.Type == UIEVENT_BUTTONUP)
	{
		return;
	}
	if (event.Type == UIEVENT_CONTEXTMENU)
	{
		return;
	}
	if (event.Type == UIEVENT_MOUSEENTER)
	{
		return;
	}
	if (event.Type == UIEVENT_MOUSELEAVE)
	{
		return;
	}

	CLabelUI::DoEvent(event);
}

void CIPAddressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	CLabelUI::SetAttribute(pstrName, pstrValue);
}

} //namespace DuiLib




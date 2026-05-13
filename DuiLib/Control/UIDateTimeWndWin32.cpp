#include "StdAfx.h"
#include "UIDateTimeWndWin32.h"

//#include <atlbase.h> 
//#include <atlstr.h> 
//#include <atlconv.h>

#ifdef DUILIB_WIN32
namespace DuiLib
{
	CDateTimeWndWin32::CDateTimeWndWin32() : m_pOwner(NULL), m_hBkBrush(NULL), m_bDropOpen(false)
	{
		m_uFormatStyle = DTS_SHORTDATEFORMAT;
	}

	void CDateTimeWndWin32::Init(CDateTimeUI* pOwner, RECT rcBase, UINT uFormatStyle)
	{
		m_pOwner = pOwner;

		if (m_hWnd == NULL)
		{
			RECT rcPos = CalPos(rcBase);
			UINT uStyle = WS_CHILD;
			if(uFormatStyle == DTS_TIMEFORMAT)
			{
				uStyle |= DTS_TIMEFORMAT;
			}
			else if(uFormatStyle == DTS_SHORTDATEFORMAT)
			{
				uStyle |= DTS_SHORTDATEFORMAT;
			}
			m_uFormatStyle = uFormatStyle;

			Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
			m_font = MakeRefPtr<UIFont>(m_pOwner->GetManager()->CloneFont(m_pOwner->GetFont()));
			::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)m_font->GetHFONT(m_pOwner->GetManager()), (LPARAM)TRUE);
		}

		memcpy(&m_oldSysTime, &m_pOwner->m_sysTime, sizeof(SYSTEMTIME));
		::SendMessage(m_hWnd, DTM_SETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);
	}

	RECT CDateTimeWndWin32::CalPos(RECT rcBase)
	{
		CDuiRect rcPos = rcBase;//m_pOwner->GetPos();

		CControlUI* pParent = m_pOwner;
		RECT rcParent;
		while( pParent = pParent->GetParent() ) {
			if( !pParent->IsVisible() ) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
			rcParent = pParent->GetClientPos();
			//if( !::IntersectRect(&rcPos, &rcPos, &rcParent) ) {
			if( !rcPos.Intersect(rcPos, rcParent) ) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
		}

		return rcPos;
	}

	LPCTSTR CDateTimeWndWin32::GetWindowClassName() const
	{
		return _T("DateTimeWnd");
	}

	LPCTSTR CDateTimeWndWin32::GetSuperClassName() const
	{
		return DATETIMEPICK_CLASS;
	}

	void CDateTimeWndWin32::OnFinalMessage(HWND hWnd)
	{
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		//m_pOwner->GetManager()->RemoveNativeWindow(hWnd);
		if(m_uFormatStyle == DTS_SHORTDATEFORMAT)
			m_pOwner->m_pWindowDate = NULL;
		else if(m_uFormatStyle == DTS_TIMEFORMAT)
			m_pOwner->m_pWindowTime = NULL;
		delete this;
	}

	LRESULT CDateTimeWndWin32::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if( uMsg == WM_CREATE ) {
			//m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
			bHandled = FALSE;
		}
		else if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
		{
			if(m_uFormatStyle == DTS_SHORTDATEFORMAT)
			{
				m_pOwner->m_sysTime.wYear = m_oldSysTime.wYear;
				m_pOwner->m_sysTime.wMonth = m_oldSysTime.wMonth;
				m_pOwner->m_sysTime.wDay = m_oldSysTime.wDay;
				m_pOwner->m_sysTime.wDayOfWeek = m_oldSysTime.wDayOfWeek;
			}
			else if(m_uFormatStyle == DTS_TIMEFORMAT)
			{
				m_pOwner->m_sysTime.wHour = m_oldSysTime.wHour;
				m_pOwner->m_sysTime.wMinute = m_oldSysTime.wMinute;
				m_pOwner->m_sysTime.wSecond = m_oldSysTime.wSecond;
			}
			::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			return lRes;
		}
		else if(uMsg == OCM_NOTIFY)
		{
			NMHDR* pHeader=(NMHDR*)lParam;
			if(pHeader != NULL && pHeader->hwndFrom == m_hWnd) 
			{
				if(pHeader->code == DTN_DATETIMECHANGE) 
				{
					LPNMDATETIMECHANGE lpChage=(LPNMDATETIMECHANGE)lParam;
					SYSTEMTIME st;
					::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
					if(m_uFormatStyle == DTS_SHORTDATEFORMAT)
					{
						m_pOwner->m_sysTime.wYear = st.wYear;
						m_pOwner->m_sysTime.wMonth = st.wMonth;
						m_pOwner->m_sysTime.wDay = st.wDay;
						m_pOwner->m_sysTime.wDayOfWeek = st.wDayOfWeek;
					}
					else if(m_uFormatStyle == DTS_TIMEFORMAT)
					{
						m_pOwner->m_sysTime.wHour = st.wHour;
						m_pOwner->m_sysTime.wMinute = st.wMinute;
						m_pOwner->m_sysTime.wSecond = st.wSecond;
					}
					m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
				}
				else if(pHeader->code == DTN_DROPDOWN) 
				{
					m_bDropOpen = true;

				}
				else if(pHeader->code == DTN_CLOSEUP) 
				{
					SYSTEMTIME st;
					::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
					if(m_uFormatStyle == DTS_SHORTDATEFORMAT)
					{
						m_pOwner->m_sysTime.wYear = st.wYear;
						m_pOwner->m_sysTime.wMonth = st.wMonth;
						m_pOwner->m_sysTime.wDay = st.wDay;
						m_pOwner->m_sysTime.wDayOfWeek = st.wDayOfWeek;
					}
					else if(m_uFormatStyle == DTS_TIMEFORMAT)
					{
						m_pOwner->m_sysTime.wHour = st.wHour;
						m_pOwner->m_sysTime.wMinute = st.wMinute;
						m_pOwner->m_sysTime.wSecond = st.wSecond;
					}
					::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
					m_bDropOpen = false;
				}
			}
			bHandled = FALSE;
		}
		else if(uMsg == WM_KILLFOCUS)
		{
			if(!m_bDropOpen) {
				::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			}
			bHandled = FALSE;
		}
		else if( uMsg == WM_PAINT) {
			if (m_pOwner->GetManager()->IsLayered()) {
				//m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
			}
			bHandled = FALSE;
		}
		else if( uMsg == WM_SETCURSOR) 
		{
			//::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			SetCursor(DUI_IBEAM);
		}
		else bHandled = FALSE;
		if( !bHandled ) return CWindowWin32::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}
}
#endif // #ifdef DUILIB_WIN32



#include "StdAfx.h"
#include "UIDateTime.h"

#ifndef _WIN32
#include <locale.h>
#include <time.h>
#endif

#include "UIDateTimeWndWin32.h"
#include "UIDateTimeWndGtk.h"
#include "UIDateTimeWndSdl.h"

namespace DuiLib
{
	//////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(CDateTimeUI)

	CDateTimeUI::CDateTimeUI() 
	: m_uFormatStyle(0) //add by liqs99
	{
		m_pWindowDate = NULL;
		m_pWindowTime = NULL;
	}

	LPCTSTR CDateTimeUI::GetClass() const
	{
		return _T("DateTimeUI");
	}

	LPVOID CDateTimeUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_DATETIME) == 0 ) return static_cast<CDateTimeUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	void CDateTimeUI::DoInit()
	{
		GetManager()->GetLocalTime(m_sysTime);
	}

	SYSTEMTIME& CDateTimeUI::GetTime()
	{
		return m_sysTime;
	}

	void CDateTimeUI::SetTime(SYSTEMTIME* pst)
	{
		m_sysTime = *pst;
		Invalidate();
	}

	void CDateTimeUI::SetFormatStyle(UINT uStyle)
	{
		m_uFormatStyle = uStyle;
	}

	UINT CDateTimeUI::GetFormatStyle()
	{
		return m_uFormatStyle;
	}

	void CDateTimeUI::SetText(LPCTSTR lpszDate)
	{
#ifdef _WIN32
		CDuiStringW sUnicode = CDuiString(lpszDate);
		DATE dt = 0;
		LCID lcid = LOCALE_USER_DEFAULT;
		DWORD dwFlags = 0;
		HRESULT hr = VarDateFromStr(sUnicode.toString(), lcid, dwFlags, &dt);
		if(FAILED(hr)) return;
		::VariantTimeToSystemTime(dt, &m_sysTime);
#else
		// 保存当前 LC_TIME 环境
		char* old_locale = setlocale(LC_TIME, NULL);
		char* old_dup = old_locale ? strdup(old_locale) : NULL;

		// 设置为系统默认 locale（空字符串 "" 表示根据环境变量）
		if (setlocale(LC_TIME, "") == NULL) 
		{
			// 失败时回退到 "C" locale
			setlocale(LC_TIME, "C");
		}

		struct tm tm = {0};
		char* ret = strptime(str, format, &tm);
		// 恢复原有 locale
		if (old_dup) 
		{
			setlocale(LC_TIME, old_dup);
			free(old_dup);
		}

		if (ret == NULL || *ret != '\0') 
		{
			return;
		}

		// struct tm 中的 tm_year 是 1900 起，tm_mon 是 0-11）
		m_sysTime.wYear         = tm.tm_year + 1900;
		m_sysTime.wMonth        = tm.tm_mon + 1;
		m_sysTime.wDayOfWeek    = tm.tm_wday;  // 0=星期日, 注意与 Windows 一致（周日=0）
		m_sysTime.wDay          = tm.tm_mday;
		m_sysTime.wHour         = tm.tm_hour;
		m_sysTime.wMinute       = tm.tm_min;
		m_sysTime.wSecond       = tm.tm_sec;
		m_sysTime.wMilliseconds = 0; 
#endif
	}

	CDuiString CDateTimeUI::GetText() const
	{
		CDuiString sText;

		if(m_uFormatStyle == 0)
			sText.SmallFormat(_T("%4d - %02d - %02d"), m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
		else if(m_uFormatStyle == 1)
			sText.SmallFormat(_T("%02d : %02d : %02d"), m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond);
		else if(m_uFormatStyle == 2)
			sText.SmallFormat(_T("%4d - %02d - %02d %2d : %02d : %02d"), m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay, m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond);

		return sText;
	}

	void CDateTimeUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CLabelUI::SetPos(rc, bNeedInvalidate);

		if(m_uFormatStyle == 0)
		{
			if( m_pWindowDate != NULL ) 
			{
				RECT rcPos = m_pWindowDate->CalPos(rc);
				m_pWindowDate->SetWindowPos(rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
		else if(m_uFormatStyle == 1)
		{
			if( m_pWindowTime != NULL ) 
			{
				RECT rcPos = m_pWindowDate->CalPos(rc);
				m_pWindowTime->SetWindowPos(rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
		else if(m_uFormatStyle == 2)
		{		
			if( m_pWindowDate != NULL ) 
			{
				RECT rcBase = {rc.left, rc.top, rc.left + (rc.right-rc.left)/2, rc.bottom};
				RECT rcPos = m_pWindowDate->CalPos(rcBase);
				m_pWindowDate->SetWindowPos(rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			if( m_pWindowTime != NULL ) 
			{
				RECT rcBase = {rc.left + (rc.right-rc.left)/2, rc.top, rc.right, rc.bottom};
				RECT rcPos = m_pWindowDate->CalPos(rcBase);
				m_pWindowTime->SetWindowPos(rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
	}

	void CDateTimeUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CLabelUI::Move(szOffset, bNeedInvalidate);
		RECT rc = GetPos();
		if(m_uFormatStyle == 0)
		{
			if( m_pWindowDate != NULL ) 
			{
				RECT rcPos = m_pWindowDate->CalPos(rc);
				m_pWindowDate->SetWindowPos(rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
		else if(m_uFormatStyle == 1)
		{
			if( m_pWindowTime != NULL ) 
			{
				RECT rcPos = m_pWindowDate->CalPos(rc);
				m_pWindowTime->SetWindowPos(rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
		else if(m_uFormatStyle == 2)
		{
			if( m_pWindowDate != NULL ) 
			{
				RECT rcBase = {rc.left, rc.top, rc.left + (rc.right-rc.left)/2, rc.bottom};
				RECT rcPos = m_pWindowDate->CalPos(rcBase);
				m_pWindowDate->SetWindowPos(rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			if( m_pWindowTime != NULL ) 
			{
				RECT rcBase = {rc.left + (rc.right-rc.left)/2, rc.top, rc.right, rc.bottom};
				RECT rcPos = m_pWindowDate->CalPos(rcBase);
				m_pWindowTime->SetWindowPos(rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
	}

	void CDateTimeUI::SetVisible(bool bVisible)
	{
		CLabelUI::SetVisible(bVisible);
		if( !IsVisible() && m_pWindowDate != NULL && m_pWindowTime != NULL ) 
			m_pManager->SetFocus(NULL);
	}

	void CDateTimeUI::SetInternVisible(bool bVisible)
	{
		if( !IsVisible() && m_pWindowDate != NULL && m_pWindowTime != NULL ) 
			m_pManager->SetFocus(NULL);
	}

	void CDateTimeUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_WINDOWSIZE )
		{
			if( m_pWindowDate != NULL ) m_pManager->SetFocusNeeded(this);
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			if( m_pWindowDate != NULL ) return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			if(m_uFormatStyle == 0)
			{
				if( m_pWindowDate ) return;
				m_pWindowDate = new CDateTimeWnd();
				ASSERT(m_pWindowDate);
				if( m_pWindowDate)
				{
					m_pWindowDate->Init(this, GetPos(), DTS_SHORTDATEFORMAT);
					m_pWindowDate->ShowWindow();
				}
			}
			else if(m_uFormatStyle == 1)
			{
				if( m_pWindowTime ) return;
				m_pWindowTime = new CDateTimeWnd();
				ASSERT(m_pWindowTime);
				if( m_pWindowTime)
				{
					m_pWindowTime->Init(this, GetPos(), DTS_TIMEFORMAT);
					m_pWindowTime->ShowWindow();
				}
			}
			else //if(m_uFormatStyle == 2)
			{
				//不知道创建哪个。。。
			}
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if( IsEnabled() ) 
			{
				GetManager()->ReleaseCapture();
				if(IsFocused())
				{
					if(m_uFormatStyle == 0)
					{
						if(m_pWindowDate == NULL)
						{
							m_pWindowDate = new CDateTimeWnd();
							ASSERT(m_pWindowDate);
							if( m_pWindowDate)
							{
								m_pWindowDate->Init(this, GetPos(), DTS_SHORTDATEFORMAT);
								m_pWindowDate->ShowWindow();
							}
						}
					}
					else if(m_uFormatStyle == 1)
					{
						if(m_pWindowTime == NULL)
						{
							m_pWindowTime = new CDateTimeWnd();
							ASSERT(m_pWindowTime);
							if( m_pWindowTime)
							{
								m_pWindowTime->Init(this, GetPos(), DTS_TIMEFORMAT);
								m_pWindowTime->ShowWindow();
							}
						}
					}
					else //if(m_uFormatStyle == 2)
					{
						CDuiRect rc = GetPos();
						CDuiRect rcDate(rc.left, rc.top, rc.left + rc.GetWidth()/2, rc.bottom);
						CDuiRect rcTime(rc.left + rc.GetWidth()/2, rc.top, rc.right, rc.bottom);
						if(rcDate.PtInRect(event.ptMouse) && m_pWindowDate == NULL)
						{
							m_pWindowDate = new CDateTimeWnd();
							ASSERT(m_pWindowDate);
							if( m_pWindowDate)
							{
								m_pWindowDate->Init(this, rcDate, DTS_SHORTDATEFORMAT);
								m_pWindowDate->ShowWindow();
							}
						}
						else if(rcTime.PtInRect(event.ptMouse) && m_pWindowTime == NULL)
						{
							m_pWindowTime = new CDateTimeWnd();
							ASSERT(m_pWindowTime);
							if( m_pWindowTime)
							{
								m_pWindowTime->Init(this, rcTime, DTS_TIMEFORMAT);
								m_pWindowTime->ShowWindow();
							}
						}
					}
				}
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			return;
		}

		CLabelUI::DoEvent(event);
	}

	void CDateTimeUI::PaintText(UIRender *pRender)
	{
		if(m_uFormatStyle != 2)
			return CLabelUI::PaintText(pRender);

		DWORD dwColor = 0;
		int iFont = -1;
		CDuiRect rcTextPadding = GetTextPadding();

		if(IsPushedState())
		{
			CDuiRect rc = GetPushedTextPadding();
			if(!rc.IsNull()) rcTextPadding = rc;
		}

		//////////////////////////////////////////////////////////////////////////
		if( !IsEnabled() )
			iFont = GetDisabledFont();

		else if( IsSelectedState() )
			iFont = GetSelectedFont();

		else if( IsPushedState() )
			iFont = GetPushedFont();

		else if( IsHotState() )
			iFont = GetHotFont();

		else if( IsFocused() )
			iFont = GetFocusedFont();

		if(iFont == -1)
			iFont = GetFont();

		//////////////////////////////////////////////////////////////////////////
		if( !IsEnabled() )
			dwColor = GetDisabledTextColor();

		else if( IsSelectedState() )
			dwColor = GetSelectedTextColor();

		else if( IsPushedState() )
			dwColor = GetPushedTextColor();

		else if( IsHotState() )
			dwColor = GetHotTextColor();

		else if( IsFocused() )
			dwColor = GetFocusedTextColor();

		if(dwColor == 0)
			dwColor = GetTextColor();

		if(dwColor == 0 && m_pManager)
			dwColor = m_pManager->GetDefaultFontColor();

		CDuiRect rc = GetPos();
		CDuiRect rcDate(rc.left, rc.top, rc.left + rc.GetWidth()/2, rc.bottom);
		CDuiRect rcTime(rc.left + rc.GetWidth()/2, rc.top, rc.right, rc.bottom);

		CDuiString sTextDate, sTextTime;
		sTextDate.SmallFormat(_T("%4d - %02d - %02d"), m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
		sTextTime.SmallFormat(_T("%02d : %02d : %02d"), m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond);

		pRender->DrawText(rcDate, rcTextPadding, sTextDate, dwColor, iFont, GetTextStyle());
		pRender->DrawText(rcTime, rcTextPadding, sTextTime, dwColor, iFont, GetTextStyle());
		return;
	}

	void CDateTimeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		//add by liqs99
		if( _tcscmp(pstrName, _T("formatstyle")) == 0 ) 
		{
			if( _tcscmp(pstrValue, _T("date")) == 0)
				SetFormatStyle(0);
			else if( _tcscmp(pstrValue, _T("time")) == 0)
				SetFormatStyle(1);
			else if( _tcscmp(pstrValue, _T("datetime")) == 0)
				SetFormatStyle(2);
			else
				SetFormatStyle(0);
		}
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}
}


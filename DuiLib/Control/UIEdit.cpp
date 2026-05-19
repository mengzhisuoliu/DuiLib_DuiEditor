#include "StdAfx.h"
#include "UIEdit.h"

#include "UIEditWndWin32.h"
#include "UIEditWndGtk.h"
#include "UIEditWndSdl.h"

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(CEditUI)

	CEditUI::CEditUI()
	{
		SetTextPadding(CDuiRect(4, 3, 4, 3));
		SetBkColor(0xFFFFFFFF);
		
		m_uMaxChar = 255;
		m_bPasswordMode = false;
		m_bAutoSelAll = false;
		m_cPasswordChar = _T('*');
		m_bNumberOnly = false;
		m_bLowerCase = false;
		m_bUpperCase = false;
		m_bMultiLine = false;
		m_bWantReturn = false;

		m_sTipValue = _T("");
		m_dwTipValueColor = 0xFFBAC0C5;
		m_dwEditbkColor = 0xFFFFFFFF;
		m_dwEditTextColor = 0x00000000;

		m_pWindow = NULL;
	}

	CEditUI::~CEditUI()
	{
	}

	LPCTSTR CEditUI::GetClass() const
	{
		return _T("EditUI");
	}

	LPVOID CEditUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_EDIT) == 0 ) return static_cast<CEditUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CEditUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return CControlUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	void CEditUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
		{
			GetManager()->SetCursor(DUI_IBEAM);
			return;
		}
		if( event.Type == UIEVENT_WINDOWSIZE )
		{
			if( m_pWindow != NULL ) m_pManager->SetFocusNeeded(this);
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			if( m_pWindow != NULL ) return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			if( m_pWindow ) return;
			m_pWindow = new DuiLibEditWnd(this);
			ASSERT(m_pWindow);
			m_pWindow->Init();
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && m_pWindow == NULL )
				{
					m_pWindow = new DuiLibEditWnd(this);
					ASSERT(m_pWindow);
					m_pWindow->Init();

					if( m_rcItem.PtInRect(event.ptMouse) )
					{
						//int nSize = GetWindowTextLength(*m_pWindow);
						//if( nSize == 0 ) nSize = 1;
						//Edit_SetSel(*m_pWindow, 0, nSize);
					}
				}
				else if( m_pWindow != NULL )
				{
					if (!m_bAutoSelAll) {
						RECT rcTextPadding = GetTextPadding();
						POINT pt = event.ptMouse;
						pt.x -= m_rcItem.left + rcTextPadding.left;
						pt.y -= m_rcItem.top + rcTextPadding.top;
						//Edit_SetSel(*m_pWindow, 0, 0);
						UIWND hWnd = ((DuiLibEditWnd *)m_pWindow)->GetHWND();
						DuiLibWindowWnd::SendMessage(hWnd, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
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
			if( m_rcItem.PtInRect(event.ptMouse ) ) {
                if( IsEnabled() ) {
                    if( !IsHotState() ) {
                        SetHotState(true);
                        Invalidate();
                    }
                }
            }
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				SetHotState(false);
				Invalidate();
			}
			return;
		}
		CLabelUI::DoEvent(event);
	}

	CDuiString CEditUI::GetText() const
	{
		return m_sText;
	}

	void CEditUI::SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;
		if(m_pWindow) m_pWindow->SetEditText(pstrText);
		Invalidate();
	}

	void CEditUI::SetSel(long nStartChar, long nEndChar)
	{
// 		if (m_pWindow != NULL) //Edit_SetSel(*m_pWindow, nStartChar,nEndChar);
// 			m_pWindow->edit_SetSel(nStartChar, nEndChar);
	}

	void CEditUI::SetSelAll()
	{
		SetSel(0,-1);
	}

	void CEditUI::SetReplaceSel(LPCTSTR lpszReplace)
	{
// 		if (m_pWindow != NULL) //Edit_ReplaceSel(*m_pWindow, lpszReplace);
// 			m_pWindow->edit_ReplaceSel(lpszReplace);
	}
	
	void CEditUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		if( m_pWindow != NULL ) 
		{
			CDuiRect rcPos = m_pWindow->CalPos();
			((DuiLibEditWnd *)m_pWindow)->SetWindowPos(rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void CEditUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CControlUI::Move(szOffset, bNeedInvalidate);
		if( m_pWindow != NULL ) 
		{
			CDuiRect rcPos = m_pWindow->CalPos();
			((DuiLibEditWnd *)m_pWindow)->SetWindowPos(rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void CEditUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
		if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	}

	void CEditUI::SetInternVisible(bool bVisible)
	{
		if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	}

	SIZE CEditUI::EstimateSize(SIZE szAvailable)
	{
		if(IsAutoCalcHeight() && !IsMultiLine()) 
			return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontHeight(GetFont()) + 6);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("numberonly")) == 0 ) SetNumberOnly(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("autoselall")) == 0 ) SetAutoSelAll(_tcscmp(pstrValue, _T("true")) == 0);	
		else if( _tcsicmp(pstrName, _T("password")) == 0 ) SetPasswordMode(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("passwordchar")) == 0 ) SetPasswordChar(*pstrValue);
		else if( _tcsicmp(pstrName, _T("maxchar")) == 0 ) SetMaxChar(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("lowercase")) == 0 ) SetLowerCase(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("uppercase")) == 0 ) SetUpperCase(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("multiline")) == 0 ) SetMultiLine(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("wantreturn")) == 0 ) SetWantReturn(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("tipvalue")) == 0 ) SetTipValue(pstrValue);
		else if( _tcsicmp(pstrName, _T("tipvaluecolor")) == 0 ) 
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTipValueColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("nativetextcolor")) == 0 ) 
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetNativeEditTextColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("nativebkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetNativeEditBkColor(clrColor);
		}
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}


	void CEditUI::PaintText(UIRender *pRender)
	{
		if(m_pWindow)
			return;

		CDuiString sText = GetText();
		RECT rcText = m_rcItem;
		DWORD dwColor = 0;
		int iFont = -1;

		if(sText.IsEmpty())
		{
			sText = GetTipValue();
			dwColor = GetTipValueColor();
		}
		else 
		{
			if(IsPasswordMode())
			{
				int length = sText.GetLength();
				sText.Empty();
				for(int i=0; i<length; i++) 
					sText += GetPasswordChar();
			}

			if( !IsEnabled() )
				dwColor = GetDisabledTextColor();

			else if( IsHotState() )
				dwColor = GetHotTextColor();

			else if( IsPushedState() )
				dwColor = GetPushedTextColor();

			else if( IsFocused() )
				dwColor = GetFocusedTextColor();

			if(dwColor == 0)
				dwColor = GetTextColor();

			if(dwColor == 0 && m_pManager)
				dwColor = m_pManager->GetDefaultFontColor();
		}

		//////////////////////////////////////////////////////////////////////////
		if( !IsEnabled() )
			iFont = GetDisabledFont();

		else if( IsPushedState() )
			iFont = GetPushedFont();

		else if( IsHotState() )
			iFont = GetHotFont();

		else if( IsFocused() )
			iFont = GetFocusedFont();

		if(iFont == -1)
			iFont = GetFont();

		UINT uTextStyle = GetTextStyle();
		if(IsMultiLine())
			uTextStyle = DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EDITCONTROL;
		else
			uTextStyle |= DT_SINGLELINE;

		pRender->DrawText(rcText, GetTextPadding(), sText, dwColor, iFont, uTextStyle);
	}


	bool CEditUI::OnEnableResponseDefaultKeyEvent(WPARAM wParam)
	{
		//按回车键时，如果焦点在当前控件，不响应默认按键事件。
		if(wParam == VK_RETURN && IsWantReturn() && IsFocused() && m_pWindow != NULL )
			return false;
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	CEditWnd::CEditWnd(CControlUI *pOwner)
	{
		m_pOwner = pOwner;

	}

	CEditWnd::~CEditWnd()
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT)) { ((CEditUI *)m_pOwner)->m_pWindow = NULL; }
		else if(m_pOwner->GetInterface(DUI_CTR_COMBO)) { ((CComboUI *)m_pOwner)->m_pEditWindow = NULL; }
	}

	void CEditWnd::SetOwnerText(LPCTSTR sText)	
	{ 
		if(m_pOwner->GetInterface(DUI_CTR_EDIT)) { ((CEditUI *)m_pOwner)->m_sText = sText; }
		else if(m_pOwner->GetInterface(DUI_CTR_COMBO)) { ((CComboUI *)m_pOwner)->m_sText = sText; }
	}

	UINT CEditWnd::GetMaxChar()
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->GetMaxChar();
		return 9999;
	}

	bool CEditWnd::IsReadOnly() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->IsReadOnly();
		return false;
	}

	bool CEditWnd::IsPasswordMode() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->IsPasswordMode();
		return false;
	}

	TCHAR CEditWnd::GetPasswordChar() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->GetPasswordChar();
		return _T('*');
	}

	bool CEditWnd::IsNumberOnly() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->IsNumberOnly();
		return false;
	}

	bool CEditWnd::IsLowerCase() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->IsLowerCase();
		return false;
	}

	bool CEditWnd::IsUpperCase() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->IsUpperCase();
		return false;
	}

	bool CEditWnd::IsMultiLine() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->IsMultiLine();
		return false;
	}

	bool CEditWnd::IsWantReturn()
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->IsWantReturn();
		return false;
	}

	DWORD CEditWnd::GetNativeEditBkColor() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->GetNativeEditBkColor();
		//return UIARGB(0, 0, 0, 0);
		return UIARGB(255,255,255,255); //内嵌窗口的背景透明时，应该仿照WIN32的方式，暂时返回白色。
	}

	DWORD CEditWnd::GetNativeEditTextColor() const
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->GetNativeEditTextColor();
		return UIARGB(255,0,0,0);
	}

	bool CEditWnd::IsAutoSelAll()
	{
		if(m_pOwner->GetInterface(DUI_CTR_EDIT))
			return ((CEditUI *)m_pOwner)->IsAutoSelAll();
		return false;
	}
}


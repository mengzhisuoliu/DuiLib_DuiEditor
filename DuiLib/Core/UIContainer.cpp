#include "StdAfx.h"

namespace DuiLib
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(CContainerUI)

		CContainerUI::CContainerUI()
		: m_iChildPadding(0),
		m_iChildAlign(DT_LEFT),
		m_iChildVAlign(DT_TOP),
		m_bAutoDestroy(true),
		m_bDelayedDestroy(true),
		m_bMouseChildEnabled(true),
		m_pVerticalScrollBar(NULL),
		m_pHorizontalScrollBar(NULL),
		m_nScrollStepSize(0)
	{
		m_pCalcControl = NULL;
		m_bCalcResult = false;
	}

	CContainerUI::~CContainerUI()
	{
		m_bDelayedDestroy = false;
		RemoveAll();
		if( m_pVerticalScrollBar ) {
			delete m_pVerticalScrollBar;
			m_pVerticalScrollBar = NULL;
		}
		if( m_pHorizontalScrollBar ) {
			delete m_pHorizontalScrollBar;
			m_pHorizontalScrollBar = NULL;
		}
	}

	LPCTSTR CContainerUI::GetClass() const
	{
		return _T("ContainerUI");
	}

	LPVOID CContainerUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("IContainer")) == 0 ) return static_cast<IContainerUI*>(this);
		else if( _tcsicmp(pstrName, DUI_CTR_CONTAINER) == 0 ) return static_cast<CContainerUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	CControlUI* CContainerUI::GetItemAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return NULL;
		return static_cast<CControlUI*>(m_items[iIndex]);
	}

	int CContainerUI::GetItemIndex(CControlUI* pControl) const
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				return it;
			}
		}

		return -1;
	}

	bool CContainerUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				NeedUpdate();            
				m_items.Remove(it);
				return m_items.InsertAt(iIndex, pControl);
			}
		}

		return false;
	}

	int CContainerUI::GetCount() const
	{
		return m_items.GetSize();
	}

	bool CContainerUI::Add(CControlUI* pControl)
	{
		if( pControl == NULL) return false;

		if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.Add(pControl);   
	}

	bool CContainerUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if( pControl == NULL) return false;

		if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.InsertAt(iIndex, pControl);
	}

	bool CContainerUI::AddAt(CControlUI* pControl,CControlUI* _IndexControl)
	{
		int nIndex = GetItemIndex(_IndexControl);
		if(nIndex < 0) return false;
		return AddAt(pControl, nIndex);
	}

	bool CContainerUI::Remove(CControlUI* pControl, bool bDoNotDestroy )
	{
		if( pControl == NULL) return false;

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				NeedUpdate();
				if( !bDoNotDestroy && m_bAutoDestroy ) {
					if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(pControl);             
					else delete pControl;
				}
				return m_items.Remove(it);
			}
		}
		return false;
	}

	bool CContainerUI::RemoveAt(int iIndex, bool bDoNotDestroy )
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (pControl != NULL) {
			return CContainerUI::Remove(pControl);
		}

		return false;
	}

	void CContainerUI::RemoveAll()
	{
		for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
			CControlUI* pItem = static_cast<CControlUI*>(m_items[it]);
			if( m_bDelayedDestroy && m_pManager ) {
				m_pManager->AddDelayedCleanup(pItem);             
			}
			else {
				delete pItem;
				pItem = NULL;
			}
		}
		m_items.Empty();
		NeedUpdate();
	}

	bool CContainerUI::IsAutoDestroy() const
	{
		return m_bAutoDestroy;
	}

	void CContainerUI::SetAutoDestroy(bool bAuto)
	{
		m_bAutoDestroy = bAuto;
	}

	bool CContainerUI::IsDelayedDestroy() const
	{
		return m_bDelayedDestroy;
	}

	void CContainerUI::SetDelayedDestroy(bool bDelayed)
	{
		m_bDelayedDestroy = bDelayed;
	}

	CDuiRect CContainerUI::GetInset() const
	{
		if(m_pManager) return m_pManager->GetDPIObj()->ScaleRect(m_rcInset);
		return m_rcInset;
	}

	void CContainerUI::SetInset(RECT rcInset)
	{
		m_rcInset = rcInset;
		NeedUpdate();
	}

	int CContainerUI::GetChildPadding() const
	{
		if (m_pManager) return m_pManager->GetDPIObj()->ScaleInt(m_iChildPadding);
		return m_iChildPadding;
	}


	void CContainerUI::SetChildPadding(int iPadding)
	{
		m_iChildPadding = iPadding;
		NeedUpdate();
	}

	UINT CContainerUI::GetChildAlign() const
	{
		return m_iChildAlign;
	}

	void CContainerUI::SetChildAlign(UINT iAlign)
	{
		m_iChildAlign = iAlign;
		NeedUpdate();
	}

	UINT CContainerUI::GetChildVAlign() const
	{
		return m_iChildVAlign;
	}

	void CContainerUI::SetChildVAlign(UINT iVAlign)
	{
		m_iChildVAlign = iVAlign;
		NeedUpdate();
	}

	bool CContainerUI::IsMouseChildEnabled() const
	{
		return m_bMouseChildEnabled;
	}

	void CContainerUI::SetMouseChildEnabled(bool bEnable)
	{
		m_bMouseChildEnabled = bEnable;
	}

	void CContainerUI::SetVisible(bool bVisible)
	{
		if( m_bVisible == bVisible ) return;
		CControlUI::SetVisible(bVisible);
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
		}
	}

	// 逻辑上，对于Container控件不公开此方法
	// 调用此方法的结果是，内部子控件隐藏，控件本身依然显示，背景等效果存在
	void CContainerUI::SetInternVisible(bool bVisible)
	{
		CControlUI::SetInternVisible(bVisible);
		if( m_items.IsEmpty() ) return;
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			// 控制子控件显示状态
			// InternVisible状态应由子控件自己控制
			static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
		}
	}

	void CContainerUI::SetEnabled(bool bEnabled)
	{
		if( IsEnabled() == bEnabled ) return;

		m_state.SetEnabled(bEnabled);

		for( int it = 0; it < m_items.GetSize(); it++ ) 
		{
			static_cast<CControlUI*>(m_items[it])->SetEnabled(bEnabled);
		}
		Invalidate();
	}

	void CContainerUI::SetMouseEnabled(bool bEnabled)
	{
		if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetMouseEnabled(bEnabled);
		if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetMouseEnabled(bEnabled);
		CControlUI::SetMouseEnabled(bEnabled);
	}

	void CContainerUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			SetFocusState(true);
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			SetFocusState(false);
			return;
		}
		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled() )
		{
			if( event.Type == UIEVENT_KEYDOWN ) 
			{
				switch( event.chKey ) {
				case VK_DOWN:
					LineDown();
					return;
				case VK_UP:
					LineUp();
					return;
				case VK_NEXT:
					PageDown();
					return;
				case VK_PRIOR:
					PageUp();
					return;
				case VK_HOME:
					HomeUp();
					return;
				case VK_END:
					EndDown();
					return;
				}
			}
			else if( event.Type == UIEVENT_SCROLLWHEEL )
			{
				switch( LOWORD(event.wParam) ) {
				case SB_LINEUP:
					LineUp();
					return;
				case SB_LINEDOWN:
					LineDown();
					return;
				}
			}
		}
		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled() ) {
			if( event.Type == UIEVENT_KEYDOWN ) 
			{
				switch( event.chKey ) {
				case VK_DOWN:
					LineRight();
					return;
				case VK_UP:
					LineLeft();
					return;
				case VK_NEXT:
					PageRight();
					return;
				case VK_PRIOR:
					PageLeft();
					return;
				case VK_HOME:
					HomeLeft();
					return;
				case VK_END:
					EndRight();
					return;
				}
			}
			else if( event.Type == UIEVENT_SCROLLWHEEL )
			{
				switch( LOWORD(event.wParam) ) {
				case SB_LINEUP:
					LineLeft();
					return;
				case SB_LINEDOWN:
					LineRight();
					return;
				}
			}
		}
		CControlUI::DoEvent(event);
	}

	SIZE CContainerUI::GetScrollPos() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) sz.cy = m_pVerticalScrollBar->GetScrollPos();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) sz.cx = m_pHorizontalScrollBar->GetScrollPos();
		return sz;
	}

	SIZE CContainerUI::GetScrollRange() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) sz.cy = m_pVerticalScrollBar->GetScrollRange();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) sz.cx = m_pHorizontalScrollBar->GetScrollRange();
		return sz;
	}

	void CContainerUI::SetScrollPos(SIZE szPos, bool bMsg)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( cx == 0 && cy == 0 ) return;

		RECT rcPos;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;

			rcPos = pControl->GetPos();
			rcPos.left -= cx;
			rcPos.right -= cx;
			rcPos.top -= cy;
			rcPos.bottom -= cy;
			pControl->SetPos(rcPos);
		}

		Invalidate();

		if(m_pVerticalScrollBar)
		{
			// 发送滚动消息
			if( m_pManager != NULL && bMsg ) {
				int nPage = (m_pVerticalScrollBar->GetScrollPos() + m_pVerticalScrollBar->GetLineSize()) / m_pVerticalScrollBar->GetLineSize();
				m_pManager->SendNotify(this, DUI_MSGTYPE_SCROLL, (WPARAM)nPage);
			}
		}
	}

	void CContainerUI::SetScrollStepSize(int nSize)
	{
		if (nSize >0)
			m_nScrollStepSize = nSize;
	}

	int CContainerUI::GetScrollStepSize() const
	{
		if(m_pManager )return m_pManager->GetDPIObj()->ScaleInt(m_nScrollStepSize);

		return m_nScrollStepSize;
	}

	void CContainerUI::LineUp()
	{
		int cyLine = GetScrollStepSize();
		if (cyLine == 0) {
			cyLine = 8;
			if( m_pManager ) cyLine = m_pManager->GetFontHeight(-1) + 8;
		}

		SIZE sz = GetScrollPos();
		sz.cy -= cyLine;
		SetScrollPos(sz);
	}

	void CContainerUI::LineDown()
	{
		int cyLine = GetScrollStepSize();
		if (cyLine == 0) {
			cyLine = 8;
			if( m_pManager ) cyLine = m_pManager->GetFontHeight(-1) + 8;
		}

		SIZE sz = GetScrollPos();
		sz.cy += cyLine;
		SetScrollPos(sz);
	}

	void CContainerUI::PageUp()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy -= iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::PageDown()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy += iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::HomeUp()
	{
		SIZE sz = GetScrollPos();
		sz.cy = 0;
		SetScrollPos(sz);
	}

	void CContainerUI::EndDown()
	{
		SIZE sz = GetScrollPos();
		sz.cy = GetScrollRange().cy;
		SetScrollPos(sz);
	}

	void CContainerUI::LineLeft()
	{
		int nScrollStepSize = GetScrollStepSize();
		int cxLine = nScrollStepSize == 0 ? 8 : nScrollStepSize;

		SIZE sz = GetScrollPos();
		sz.cx -= cxLine;
		SetScrollPos(sz);
	}

	void CContainerUI::LineRight()
	{
		int nScrollStepSize = GetScrollStepSize();
		int cxLine = nScrollStepSize == 0 ? 8 : nScrollStepSize;

		SIZE sz = GetScrollPos();
		sz.cx += cxLine;
		SetScrollPos(sz);
	}

	void CContainerUI::PageLeft()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx -= iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::PageRight()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx += iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::HomeLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx = 0;
		SetScrollPos(sz);
	}

	void CContainerUI::EndRight()
	{
		SIZE sz = GetScrollPos();
		sz.cx = GetScrollRange().cx;
		SetScrollPos(sz);
	}

	void CContainerUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		if( bEnableVertical && !m_pVerticalScrollBar ) {
			m_pVerticalScrollBar = new CScrollBarUI;
			m_pVerticalScrollBar->SetOwner(this);
			m_pVerticalScrollBar->SetManager(m_pManager, NULL, false);
			if ( m_pManager ) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("VScrollBar"));
				if( pDefaultAttributes ) {
					m_pVerticalScrollBar->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableVertical && m_pVerticalScrollBar ) {
			delete m_pVerticalScrollBar;
			m_pVerticalScrollBar = NULL;
		}

		if( bEnableHorizontal && !m_pHorizontalScrollBar ) {
			m_pHorizontalScrollBar = new CScrollBarUI;
			m_pHorizontalScrollBar->SetHorizontal(true);
			m_pHorizontalScrollBar->SetOwner(this);
			m_pHorizontalScrollBar->SetManager(m_pManager, NULL, false);

			if ( m_pManager ) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("HScrollBar"));
				if( pDefaultAttributes ) {
					m_pHorizontalScrollBar->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableHorizontal && m_pHorizontalScrollBar ) {
			delete m_pHorizontalScrollBar;
			m_pHorizontalScrollBar = NULL;
		}

		NeedUpdate();
	}

	CScrollBarUI* CContainerUI::GetVerticalScrollBar() const
	{
		return m_pVerticalScrollBar;
	}

	CScrollBarUI* CContainerUI::GetHorizontalScrollBar() const
	{
		return m_pHorizontalScrollBar;
	}

	int CContainerUI::FindSelectable(int iIndex, bool bForward /*= true*/) const
	{
		// NOTE: This is actually a helper-function for the list/combo/ect controls
		//       that allow them to find the next enabled/available selectable item
		if( GetCount() == 0 ) return -1;
		iIndex = CLAMP(iIndex, 0, GetCount() - 1);
		if( bForward ) {
			for( int i = iIndex; i < GetCount(); i++ ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return -1;
		}
		else {
			for( int i = iIndex; i >= 0; --i ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return FindSelectable(0, true);
		}
	}

	RECT CContainerUI::GetClientPos() const
	{
		RECT rc = m_rcItem;
		RECT rcInset = GetInset();
		rc.left += rcInset.left;
		rc.top += rcInset.top;
		rc.right -= rcInset.right;
		rc.bottom -= rcInset.bottom;

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			rc.top -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom += m_pVerticalScrollBar->GetScrollRange();
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			rc.left -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right += m_pHorizontalScrollBar->GetScrollRange();
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}
		return rc;
	}

	void CContainerUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CControlUI::Move(szOffset, bNeedInvalidate);
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) m_pVerticalScrollBar->Move(szOffset, false);
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) m_pHorizontalScrollBar->Move(szOffset, false);
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( pControl != NULL && pControl->IsVisible() ) pControl->Move(szOffset, false);
		}
	}

	SIZE CContainerUI::EstimateSize(SIZE szAvailable)
	{	
		if(IsAnimationRunning(ANIMATION_ID_SHOW) || IsAnimationRunning(ANIMATION_ID_HIDE)) {
			return m_szAnimationCurrect;
		}

		if(!IsPaneVisible())
		{
			return m_szAnimationCurrect;
		}

		if(IsAutoCalcWidth() || IsAutoCalcHeight())
		{
			SIZE sz = {0};
			for (int it=0; it<GetCount(); it++)
			{
				SIZE szControl = {0};
				CControlUI *pControl = GetItemAt(it);
				if(!pControl->IsVisible()) continue;
				szControl = pControl->EstimateSize(szAvailable);
				RECT padding = pControl->GetPadding();
				if(IsAutoCalcWidth())
				{
					if (sz.cx < szControl.cx + padding.left + padding.right)
						sz.cx = szControl.cx + padding.left + padding.right;
				}

				if(IsAutoCalcHeight())
				{	
					if (sz.cy < szControl.cy + padding.top + padding.bottom)
					{
						sz.cy = szControl.cy + padding.top + padding.bottom;
					}
				}
			}

			RECT rcInset = GetInset();
			if(IsAutoCalcWidth())
			{
				sz.cx += rcInset.left + rcInset.right;
			}
			if(IsAutoCalcHeight())
			{
				sz.cy += rcInset.top + rcInset.bottom;
			}

			return sz;
		}
		return CControlUI::EstimateSize(szAvailable);
	}

	void CContainerUI::SetPos(RECT rc1, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc1, bNeedInvalidate);
		if( m_items.IsEmpty() ) return;

		CDuiRect rc = m_rcItem;
		rc.Deflate(GetInset());

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
		{
			rc.top -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom += m_pVerticalScrollBar->GetScrollRange();
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		{
			rc.left -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right += m_pHorizontalScrollBar->GetScrollRange();
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}

		for( int it = 0; it < m_items.GetSize(); it++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) 
				continue;
			if( pControl->IsFloat() ) 
			{
				SetFloatPos(it);
			}
			else 
			{ 
				CDuiSize sz(rc);
				localControl_AdjustMaxMinSize(sz, pControl);

				//铺满，不考虑pControl的padding
				CDuiRect rcCtrl(rc.LeftTop(), sz);
				if(m_pCalcControl) { m_rcCalcChild = rcCtrl; m_bCalcResult = true; return; }
				pControl->SetPos(rcCtrl, false);
			}
		}
	}

	bool CContainerUI::CalcPos(CControlUI *pChildControl, CDuiRect &rcChild)
	{
		m_pCalcControl = pChildControl;
		m_bCalcResult = false;

		SetPos(GetPos(), false);

		if(m_bCalcResult)
		{
			rcChild = m_rcCalcChild;
			m_rcCalcChild.Empty();
			m_pCalcControl = NULL;
			//CMsgWndUI::InsertMsgV(_T("CalcPos pControl=%p, rcChild=%s"), pChildControl, rcChild.ToString().toString());
			return true;
		}
		m_rcCalcChild.Empty();
		m_pCalcControl = NULL;
		return false;
	}

	void CContainerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("inset")) == 0 ) {
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetInset(rcInset);
		}
		else if( _tcsicmp(pstrName, _T("mousechild")) == 0 ) SetMouseChildEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("vscrollbar")) == 0 ) {
			EnableScrollBar(_tcsicmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);
		}
		else if( _tcsicmp(pstrName, _T("vscrollbarstyle")) == 0 ) {
			m_sVerticalScrollBarStyle = pstrValue;
			EnableScrollBar(TRUE, GetHorizontalScrollBar() != NULL);
			if( GetVerticalScrollBar() ) {
				LPCTSTR pStyle = m_pManager->GetStyle(m_sVerticalScrollBarStyle);
				if( pStyle ) {
					GetVerticalScrollBar()->ApplyAttributeList(pStyle);
				}
				else {
					GetVerticalScrollBar()->ApplyAttributeList(pstrValue);
				}
			}
		}
		else if( _tcsicmp(pstrName, _T("hscrollbar")) == 0 ) {
			EnableScrollBar(GetVerticalScrollBar() != NULL, _tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("hscrollbarstyle")) == 0 ) {
			m_sHorizontalScrollBarStyle = pstrValue;
			EnableScrollBar(TRUE, GetHorizontalScrollBar() != NULL);
			if( GetHorizontalScrollBar() ) {
				LPCTSTR pStyle = m_pManager->GetStyle(m_sHorizontalScrollBarStyle);
				if( pStyle ) {
					GetHorizontalScrollBar()->ApplyAttributeList(pStyle);
				}
				else {
					GetHorizontalScrollBar()->ApplyAttributeList(pstrValue);
				}
			}
		}
		else if( _tcsicmp(pstrName, _T("childpadding")) == 0 ) SetChildPadding(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("childalign")) == 0 ) {
			if( _tcscmp(pstrValue, _T("left")) == 0 ) m_iChildAlign = DT_LEFT;
			else if( _tcscmp(pstrValue, _T("center")) == 0 ) m_iChildAlign = DT_CENTER;
			else if( _tcscmp(pstrValue, _T("right")) == 0 ) m_iChildAlign = DT_RIGHT;
		}
		else if( _tcscmp(pstrName, _T("childvalign")) == 0 ) {
			if( _tcscmp(pstrValue, _T("top")) == 0 ) m_iChildVAlign = DT_TOP;
			else if( _tcscmp(pstrValue, _T("vcenter")) == 0 ) m_iChildVAlign = DT_VCENTER;
			else if( _tcscmp(pstrValue, _T("bottom")) == 0 ) m_iChildVAlign = DT_BOTTOM;
		}
		else if( _tcsicmp(pstrName, _T("scrollstepsize")) == 0 ) SetScrollStepSize(_ttoi(pstrValue));
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CContainerUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			static_cast<CControlUI*>(m_items[it])->SetManager(pManager, this, bInit);
		}

		if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetManager(pManager, this, bInit);
		if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetManager(pManager, this, bInit);
		CControlUI::SetManager(pManager, pParent, bInit);
	}

	CControlUI* CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
		if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
		//if( (uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
		if( (uFlags & UIFIND_HITTEST) != 0 && !m_rcItem.PtInRect(*(static_cast<LPPOINT>(pData))) ) return NULL;
		if( (uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL ) return NULL;

		CControlUI* pResult = NULL;
		if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
		}
		if( pResult == NULL && m_pVerticalScrollBar != NULL ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
		}
		if( pResult == NULL && m_pHorizontalScrollBar != NULL ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
		}
		if( pResult != NULL ) return pResult;

		if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled() ) {
			CDuiRect rc = m_rcItem;
			RECT rcInset = GetInset();
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
			if( (uFlags & UIFIND_TOP_FIRST) != 0 ) {
				for( int it = m_items.GetSize() - 1; it >= 0; it-- ) {
					pResult = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
					if( pResult != NULL ) {
						//if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
						if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !rc.PtInRect(*(static_cast<LPPOINT>(pData))) )
							continue;
						else 
							return pResult;
					}          
				}
			}
			else {
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					pResult = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
					if( pResult != NULL ) {
						//if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
						if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !rc.PtInRect(*(static_cast<LPPOINT>(pData))) )
							continue;
						else 
							return pResult;
					} 
				}
			}
		}

		pResult = NULL;
		if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
		}
		return pResult;
	}

	bool CContainerUI::DoPaint(UIRender *pRender, const RECT& rcPaint, CControlUI* pStopControl)
	{
		CDuiRect rcTemp;
		//if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return true;
		if( !rcTemp.Intersect(rcPaint, m_rcItem) ) return true;

		UIClip clip;
		clip.GenerateClip(pRender, rcTemp);
		CControlUI::DoPaint(pRender, rcPaint, pStopControl);

		if( m_items.GetSize() > 0 ) {
			RECT rcInset = GetInset();
			RECT rc = m_rcItem;
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

			//if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
			if( !rcTemp.Intersect(rcPaint, rc) ) {
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( pControl == pStopControl ) return false;
					if( !pControl->IsVisible() ) continue;
					//if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( !rcTemp.Intersect(rcPaint, pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						//if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						if( !rcTemp.Intersect(m_rcItem, pControl->GetPos()) ) continue;
						if( !pControl->Paint(pRender, rcPaint, pStopControl) ) return false;
					}
				}
			}
			else {
				UIClip childClip;
				childClip.GenerateClip(pRender, rcTemp);
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( pControl == pStopControl ) return false;
					if( !pControl->IsVisible() ) continue;
					//if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( !rcTemp.Intersect(rcPaint, pControl->GetPos()) ) continue;
					if( pControl->IsFloat() ) {
						//if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						if( !rcTemp.Intersect(m_rcItem, pControl->GetPos()) ) continue;
						childClip.UseOldClipBegin(pRender);
                        if( !pControl->Paint(pRender, rcPaint, pStopControl) ) return false;
						childClip.UseOldClipEnd(pRender);
					}
					else {
						//if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
						if( !rcTemp.Intersect(rc, pControl->GetPos()) ) continue;
                        if( !pControl->Paint(pRender, rcPaint, pStopControl) ) return false;
					}
				}
			}
		}

		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
			if( m_pVerticalScrollBar == pStopControl ) return false;
			//if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
			if( rcTemp.Intersect(rcPaint, m_pVerticalScrollBar->GetPos()) ) {
				if( !m_pVerticalScrollBar->Paint(pRender, rcPaint, pStopControl) ) return false;
			}
		}

		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
			if( m_pHorizontalScrollBar == pStopControl ) return false;
			//if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
			if( rcTemp.Intersect(rcPaint, m_pHorizontalScrollBar->GetPos()) ) {
				if( !m_pHorizontalScrollBar->Paint(pRender, rcPaint, pStopControl) ) return false;
			}
		}
		return true;
	}

	void CContainerUI::SetFloatPos(int iIndex)
	{
		// 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return;

		CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);

		if( !pControl->IsVisible() ) return;
		if( !pControl->IsFloat() ) return;

		SIZE szXY = pControl->GetFixedXY();
		SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};

		int nParentWidth = m_rcItem.right - m_rcItem.left;
		int nParentHeight = m_rcItem.bottom - m_rcItem.top;

		UINT uAlign = pControl->GetFloatAlign();
		TPercentInfo rcPercent = pControl->GetFloatPercent();
		if(uAlign != 0) 
		{
			//RECT rcCtrl = {0, 0, sz.cx, sz.cy};
			CDuiRect rcCtrl(0, 0, sz.cx, sz.cy);
			if((uAlign & DT_CENTER) != 0) {
				//::OffsetRect(&rcCtrl, (nParentWidth - sz.cx) / 2, 0);
				rcCtrl.Offset((nParentWidth - sz.cx) / 2, 0);
			}
			else if((uAlign & DT_RIGHT) != 0) {
				//::OffsetRect(&rcCtrl, nParentWidth - sz.cx, 0);
				rcCtrl.Offset(nParentWidth - sz.cx, 0);
			}
			else {
				//::OffsetRect(&rcCtrl, szXY.cx, 0);
				rcCtrl.Offset(szXY.cx, 0);
			}

			if((uAlign & DT_VCENTER) != 0) {
				//::OffsetRect(&rcCtrl, 0, (nParentHeight - sz.cy) / 2);
				rcCtrl.Offset(0, (nParentHeight - sz.cy) / 2);
			}
			else if((uAlign & DT_BOTTOM) != 0) {
				//::OffsetRect(&rcCtrl, 0, nParentHeight - sz.cy);
				rcCtrl.Offset(0, nParentHeight - sz.cy);
			}
			else {
				//::OffsetRect(&rcCtrl, 0, szXY.cy);
				rcCtrl.Offset(0, szXY.cy);
			}

			//::OffsetRect(&rcCtrl, m_rcItem.left, m_rcItem.top);
			rcCtrl.Offset(m_rcItem.left, m_rcItem.top);
			pControl->SetPos(rcCtrl, false);
		}
		else if(rcPercent.left !=0 || rcPercent.top != 0 || rcPercent.right != 0 || rcPercent.bottom != 0)
		{
			LONG width = m_rcItem.right - m_rcItem.left;
			LONG height = m_rcItem.bottom - m_rcItem.top;
			RECT rcCtrl = { 0 };
			rcCtrl.left = (LONG)(width*rcPercent.left) + szXY.cx+ m_rcItem.left;
			rcCtrl.top = (LONG)(height*rcPercent.top) + szXY.cy+ m_rcItem.top;
			rcCtrl.right = (LONG)(width*rcPercent.right) + szXY.cx + sz.cx+ m_rcItem.left;
			rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY.cy + sz.cy+ m_rcItem.top;
			pControl->SetPos(rcCtrl, false);
		}
		else 
		{
			RECT rcCtrl = { 0 };
			rcCtrl.left		= szXY.cx + m_rcItem.left;
			rcCtrl.top		= szXY.cy + m_rcItem.top;
			rcCtrl.right	= szXY.cx + sz.cx+ m_rcItem.left;
			rcCtrl.bottom	= szXY.cy + sz.cy+ m_rcItem.top;

			//根据百分比调整位置
			LONG width = m_rcItem.right - m_rcItem.left;
			LONG height = m_rcItem.bottom - m_rcItem.top;
			POINT ptPosition = pControl->GetFloatPosition();
			if(ptPosition.x != 0 && szXY.cx == 0)
			{
				rcCtrl.left  = width*ptPosition.x/100 + m_rcItem.left;
				rcCtrl.right = rcCtrl.left + sz.cx;
			}
			if(ptPosition.y != 0 && szXY.cy == 0)
			{
				rcCtrl.top  = height*ptPosition.y/100 + m_rcItem.top;
				rcCtrl.bottom = rcCtrl.top + sz.cy;
			}

			//根据百分比调整大小
			SIZE szFloat = { pControl->GetFixedWidthPercent(), pControl->GetFixedHeightPercent() };
			if(szFloat.cx != 0 && sz.cx == 0)
			{
				rcCtrl.right = width*szFloat.cx/100 + rcCtrl.left;
			}
			if(szFloat.cy != 0 && sz.cy == 0)
			{
				rcCtrl.bottom = height*szFloat.cy/100 + rcCtrl.top;
			}
			pControl->SetPos(rcCtrl, false);
		}
	}

	void CContainerUI::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
	{
		while (m_pHorizontalScrollBar)
		{
			// Scroll needed
			if (cxRequired > rc.right - rc.left && !m_pHorizontalScrollBar->IsVisible())
			{
				m_pHorizontalScrollBar->SetVisible(true);
				m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
				m_pHorizontalScrollBar->SetScrollPos(0);
				SetPos(m_rcItem);
				break;
			}

			// No scrollbar required
			if (!m_pHorizontalScrollBar->IsVisible()) break;

			// Scroll not needed anymore?
			int cxScroll = cxRequired - (rc.right - rc.left);
			if (cxScroll <= 0)
			{
				m_pHorizontalScrollBar->SetVisible(false);
				m_pHorizontalScrollBar->SetScrollPos(0);
				m_pHorizontalScrollBar->SetScrollRange(0);
				SetPos(m_rcItem);
			}
			else
			{
				RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
				m_pHorizontalScrollBar->SetPos(rcScrollBarPos);

				if (m_pHorizontalScrollBar->GetScrollRange() != cxScroll) 
				{
					int iScrollPos = m_pHorizontalScrollBar->GetScrollPos();
					m_pHorizontalScrollBar->SetScrollRange(::abs(cxScroll)); // if scrollpos>range then scrollpos=range
					if(iScrollPos > m_pHorizontalScrollBar->GetScrollPos()) 
					{
						SetPos(m_rcItem);
					}
				}
			}
			break;
		}

		while (m_pVerticalScrollBar)
		{
			// Scroll needed
			if (cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible()) 
			{
				m_pVerticalScrollBar->SetVisible(true);
				m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
				m_pVerticalScrollBar->SetScrollPos(0);
				SetPos(m_rcItem);
				break;
			}

			// No scrollbar required
			if (!m_pVerticalScrollBar->IsVisible()) break;

			// Scroll not needed anymore?
			int cyScroll = cyRequired - (rc.bottom - rc.top);
			if (cyScroll <= 0) 
			{
				m_pVerticalScrollBar->SetVisible(false);
				m_pVerticalScrollBar->SetScrollPos(0);
				m_pVerticalScrollBar->SetScrollRange(0);
				SetPos(m_rcItem);
				break;
			}

			RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
			m_pVerticalScrollBar->SetPos(rcScrollBarPos);

			if (m_pVerticalScrollBar->GetScrollRange() != cyScroll)
			{
				int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
				m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll)); // if scrollpos>range then scrollpos=range
				if(iScrollPos > m_pVerticalScrollBar->GetScrollPos()) 
				{
					SetPos(m_rcItem);
				}
			}
			break;
		}
	}

	bool CContainerUI::SetSubControlText( LPCTSTR pstrSubControlName,LPCTSTR pstrText )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetText(pstrText);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlFixedHeight( LPCTSTR pstrSubControlName,int cy )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetFixedHeight(cy);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlFixedWdith( LPCTSTR pstrSubControlName,int cx )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetFixedWidth(cx);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlUserData( LPCTSTR pstrSubControlName,LPCTSTR pstrText )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetUserData(pstrText);
			return TRUE;
		}
		else
			return FALSE;
	}

	DuiLib::CDuiString CContainerUI::GetSubControlText( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetText();
	}

	int CContainerUI::GetSubControlFixedHeight( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return -1;
		else
			return pSubControl->GetFixedHeight();
	}

	int CContainerUI::GetSubControlFixedWdith( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return -1;
		else
			return pSubControl->GetFixedWidth();
	}
	 
	const CDuiString CContainerUI::GetSubControlUserData( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetUserData();
	}

	CControlUI* CContainerUI::FindSubControl( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		if(m_pManager != NULL) pSubControl = static_cast<CControlUI*>(m_pManager->FindSubControlByName(this,pstrSubControlName));
		return pSubControl;
	}

	void CContainerUI::SetPosHorizontalLayout(RECT rc1, bool bNeedInvalidate)
	{
		if(!m_pCalcControl)
		{
			CControlUI::SetPos(rc1, bNeedInvalidate);
		}
		CDuiRect rc = m_rcItem;

		// Adjust for inset
		rc.Deflate(GetInset());

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
			rc.Deflate(0, 0, m_pVerticalScrollBar->GetFixedWidth(), 0);
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			rc.Deflate(0,0,0,m_pHorizontalScrollBar->GetFixedHeight());

		if( m_items.GetSize() == 0) 
		{
			if(m_pCalcControl)
			{
				m_bCalcResult = false;
			}
			else
			{
				ProcessScrollBar(rc, 0, 0);
			}
			return;
		}

		// Determine the minimum size
		//SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		CDuiSize szAvailable(rc);
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			szAvailable.Inflate(m_pHorizontalScrollBar->GetScrollRange(), 0);
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
			szAvailable.Inflate(0, m_pVerticalScrollBar->GetScrollRange());

		//第一次循环，计算哪些控件需要自动布局，以及剩余可自动布局的尺寸。
		int nAutoSizeCount = 0;	//需要自动布局的数量
		int nUsedSize = 0;		//已使用的尺寸
		int nEstimateCount = 0;	//需要布局的数量，Visible 和 非Float 的
		for (int i=0; i<m_items.GetSize(); i++) //注意这里一定要用m_Items，CListUI有重写GetCount()
		{
			CControlUI *pControl = static_cast<CControlUI*>(m_items.GetAt(i));//注意这里一定要用m_Items，CListUI有重写GetItemAt()
			if(m_pCalcControl)
			{
				if( !pControl->IsVisible() && !IsPaneVisible())
					continue;
			}
			else
			{
				if( !pControl->IsVisible() ) 
					continue;
			}

			if( pControl->IsFloat() ) 
			{
				if(m_pCalcControl)
				{
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = pControl->GetPos(); m_bCalcResult = true; return;
					}
				}
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
			CDuiSize szControlAvailable = szAvailable;
			LocalControl_GetAvailableMaxSize(szControlAvailable, pControl);
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			localControl_AdjustMaxMinSize(sz, pControl);
			if(sz.cx == 0 && pControl->GetFixedWidthPercent() > 0)
			{
				sz.cx = szAvailable.cx * pControl->GetFixedWidthPercent() / 100;
			}
			localControl_AdjustMaxMinSize(sz, pControl);

			if( sz.cx == 0 ) //控件的宽度为0，需要自动布局
			{ 
				nAutoSizeCount++;		
			}

			nUsedSize += sz.cx + rcPadding.left + rcPadding.right;
			nEstimateCount++;
		}
		nUsedSize += (nEstimateCount - 1) * GetChildPadding();

		//////////////////////////////////////////////////////////////////////////
		int xPos = rc.left;
		//计算布局的初始位置
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		{
			xPos -= m_pHorizontalScrollBar->GetScrollPos();
		}
		else 
		{	
			if(nAutoSizeCount == 0) //没有自动布局的控件，才支持横向对齐。
			{
				UINT iChildAlign = GetChildAlign(); 
				if (iChildAlign == DT_CENTER) 
				{
					xPos += (szAvailable.cx - nUsedSize) / 2;
				}
				else if (iChildAlign == DT_RIGHT) 
				{
					xPos += (szAvailable.cx - nUsedSize);
				}
			}
		}

		//第二次循环，设置子控件位置，剩余可自动布局的空间分给需要自动布局的子控件。
		int cxNeededEx = 0;
		int cyNeededEx = 0;
		int nIndex = 0;
		int nAdjustUsedSize = 0;
		for (int i=0; i<m_items.GetSize(); i++)
		{
			CControlUI *pControl = static_cast<CControlUI*>(m_items.GetAt(i));;
			if(m_pCalcControl)
			{
				if( !pControl->IsVisible() && !IsPaneVisible())
					continue;
			}
			else
			{
				if( !pControl->IsVisible() ) 
					continue;
			}
			if( pControl->IsFloat() ) 
			{
				if(m_pCalcControl)
				{
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = pControl->GetPos(); m_bCalcResult = true; return;
					}
				}
				else
				{
					SetFloatPos(i);
				}
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
			CDuiSize szControlAvailable = szAvailable;
			LocalControl_GetAvailableMaxSize(szControlAvailable, pControl);
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			localControl_AdjustMaxMinSize(sz, pControl);
			if(sz.cx == 0 && pControl->GetFixedWidthPercent() > 0)
			{
				sz.cx = szAvailable.cx * pControl->GetFixedWidthPercent() / 100;
			}
			if( sz.cx == 0 ) //控件的宽度为0，需要自动布局
			{ 
				sz.cx = (szAvailable.cx - nUsedSize - nAdjustUsedSize) / (nAutoSizeCount - nIndex);
				nIndex++;
				localControl_AdjustMaxMinSize(sz, pControl);
				nAdjustUsedSize += sz.cx;	
			}
			else
			{
				localControl_AdjustMaxMinSize(sz, pControl);
			}

			if(sz.cy == 0 && pControl->GetFixedHeightPercent() > 0) 
				sz.cy = szAvailable.cy * pControl->GetFixedHeightPercent() / 100;
			if( sz.cy == 0 ) 
				sz.cy = szAvailable.cy - rcPadding.top - rcPadding.bottom;
			if( sz.cy > szControlAvailable.cy ) 
				sz.cy = szControlAvailable.cy;
			localControl_AdjustMaxMinSize(sz, pControl);

			UINT iChildVAlign = GetChildVAlign(); 
			CDuiRect rcCtrl;
			if (iChildVAlign == DT_VCENTER) 
			{
				int yPos = rc.top + rc.GetHeight()/2;
				if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
				{
					yPos += m_pVerticalScrollBar->GetScrollRange() / 2;
					yPos -= m_pVerticalScrollBar->GetScrollPos();
				}
				rcCtrl.SetRect(xPos + rcPadding.left,
					yPos - sz.cy/2,
					xPos + rcPadding.left + sz.cx,
					yPos - sz.cy/2 + sz.cy);

				if(m_pCalcControl) 
				{ 
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = rcCtrl; m_bCalcResult = true; return;
					}
				}
				else
					pControl->SetPos(rcCtrl, false);
			}
			else if (iChildVAlign == DT_BOTTOM) 
			{
				int yPos = rc.bottom;
				if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
				{
					yPos += m_pVerticalScrollBar->GetScrollRange();
					yPos -= m_pVerticalScrollBar->GetScrollPos();
				}
				rcCtrl.SetRect(xPos + rcPadding.left,
					yPos - rcPadding.bottom - sz.cy,
					xPos + rcPadding.left + sz.cx,
					yPos - rcPadding.bottom);

				if(m_pCalcControl) 
				{ 
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = rcCtrl; m_bCalcResult = true; return;
					}
				}
				else
					pControl->SetPos(rcCtrl, false);
			}
			else //DT_TOP
			{
				int yPos = rc.top;
				if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
				{
					yPos -= m_pVerticalScrollBar->GetScrollPos();
				}
				rcCtrl.SetRect(xPos + rcPadding.left,
					yPos + rcPadding.top,
					xPos + rcPadding.left + sz.cx,
					yPos + rcPadding.top + sz.cy);

				if(m_pCalcControl) 
				{ 
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = rcCtrl; m_bCalcResult = true; return;
					}
				}
				else
					pControl->SetPos(rcCtrl, false);
			}

			xPos += sz.cx + GetChildPadding() + rcPadding.left + rcPadding.right;
			cxNeededEx += sz.cx + rcPadding.left + rcPadding.right;
			if(cyNeededEx < sz.cy + rcPadding.top + rcPadding.bottom)
				cyNeededEx = sz.cy + rcPadding.top + rcPadding.bottom;
		}
		cxNeededEx += (nEstimateCount - 1) * GetChildPadding();

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeededEx, cyNeededEx);
	}

	void CContainerUI::SetPosVerticalLayout(RECT rc1, bool bNeedInvalidate)
	{
		if(!m_pCalcControl)
		{
			CControlUI::SetPos(rc1, bNeedInvalidate);
		}
		CDuiRect rc = m_rcItem;

		// Adjust for inset
		rc.Deflate(GetInset());

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
			rc.Deflate(0, 0, m_pVerticalScrollBar->GetFixedWidth(), 0);
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			rc.Deflate(0, 0, 0, m_pHorizontalScrollBar->GetFixedHeight());

		if( m_items.GetSize() == 0) 
		{
			if(m_pCalcControl)
			{
				m_bCalcResult = false;
			}
			else
			{
				ProcessScrollBar(rc, 0, 0);
			}
			return;
		}

		// Determine the minimum size
		CDuiSize szAvailable(rc);
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			szAvailable.Inflate(m_pHorizontalScrollBar->GetScrollRange(), 0);
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
			szAvailable.Inflate(0, m_pVerticalScrollBar->GetScrollRange());

		//第一次循环，计算哪些控件需要自动布局，以及剩余可自动布局的尺寸。
		int nAutoSizeCount = 0;	//需要自动布局的数量
		int nUsedSize = 0;		//已使用的尺寸
		int nEstimateCount = 0;	//需要布局的数量，Visible 和 非Float 的
		for (int i=0; i<m_items.GetSize(); i++) //注意这里一定要用m_Items，CListUI有重写GetCount()
		{
			CControlUI *pControl = static_cast<CControlUI*>(m_items.GetAt(i));//注意这里一定要用m_Items，CListUI有重写GetItemAt()
 			if(m_pCalcControl)
 			{
				if( !pControl->IsVisible() && !IsPaneVisible())
					continue;
			}
			else
			{
				if( !pControl->IsVisible() ) 
					continue;
			}

			if( pControl->IsFloat() ) 
			{
				if(m_pCalcControl)
				{
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = pControl->GetPos(); m_bCalcResult = true; return;
					}
				}
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
			CDuiSize szControlAvailable = szAvailable;
			LocalControl_GetAvailableMaxSize(szControlAvailable, pControl);
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			localControl_AdjustMaxMinSize(sz, pControl);
			if(sz.cy == 0 && pControl->GetFixedHeightPercent() > 0)
			{
				sz.cy = szAvailable.cy * pControl->GetFixedHeightPercent() / 100;
			}
			localControl_AdjustMaxMinSize(sz, pControl);

			if( sz.cy == 0 ) //控件的高度为0，需要自动布局
			{ 
				nAutoSizeCount++;		
			}

			nUsedSize += sz.cy + rcPadding.top + rcPadding.bottom;
			nEstimateCount++;
		}
		nUsedSize += (nEstimateCount - 1) * GetChildPadding();

		//////////////////////////////////////////////////////////////////////////
		int yPos = rc.top;
		//计算布局的初始位置
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
		{
			yPos -= m_pVerticalScrollBar->GetScrollPos();
		}
		else 
		{	
			if(nAutoSizeCount == 0) //没有自动布局的控件，才支持横向对齐。
			{
				UINT iChildVAlign = GetChildVAlign(); 
				if (iChildVAlign == DT_VCENTER) 
				{
					yPos += (szAvailable.cy - nUsedSize) / 2;
				}
				else if (iChildVAlign == DT_BOTTOM) 
				{
					yPos += (szAvailable.cy - nUsedSize);
				}
			}
		}

		//第二次循环，设置子控件位置，剩余可自动布局的空间分给需要自动布局的子控件。
		int cyNeededEx = 0;
		int cxNeededEx = 0;
		int nIndex = 0;
		int nAdjustUsedSize = 0;
		for (int i=0; i<m_items.GetSize(); i++)
		{
			CControlUI *pControl = static_cast<CControlUI*>(m_items.GetAt(i));
 			if(m_pCalcControl)
 			{
				if( !pControl->IsVisible() && !IsPaneVisible())
					continue;
			}
			else
			{
				if( !pControl->IsVisible() ) 
					continue;
			}
			if( pControl->IsFloat() ) 
			{
				if(m_pCalcControl)
				{
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = pControl->GetPos(); m_bCalcResult = true; return;
					}
				}
				else
				{
					SetFloatPos(i);
				}
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
			CDuiSize szControlAvailable = szAvailable;
			LocalControl_GetAvailableMaxSize(szControlAvailable, pControl);
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			localControl_AdjustMaxMinSize(sz, pControl);
			if(sz.cy == 0 && pControl->GetFixedHeightPercent() > 0)
			{
				sz.cy = szAvailable.cy * pControl->GetFixedHeightPercent() / 100;
			}
			if( sz.cy == 0 ) //控件的高度为0，需要自动布局
			{ 
				sz.cy = (szAvailable.cy - nUsedSize - nAdjustUsedSize) / (nAutoSizeCount - nIndex);
				nIndex++;
				localControl_AdjustMaxMinSize(sz, pControl);
				nAdjustUsedSize += sz.cy;	
			}
			else
			{
				localControl_AdjustMaxMinSize(sz, pControl);
			}

			if(sz.cx == 0 && pControl->GetFixedWidthPercent() > 0) 
				sz.cx = szAvailable.cx * pControl->GetFixedWidthPercent() / 100;
			if( sz.cx == 0 ) 
				sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if( sz.cx > szControlAvailable.cx ) 
				sz.cx = szControlAvailable.cx;
			localControl_AdjustMaxMinSize(sz, pControl);

			UINT iChildAlign = GetChildAlign(); 
			CDuiRect rcCtrl;
			if (iChildAlign == DT_CENTER) 
			{
				int xPos = rc.left + rc.GetWidth()/2;
				if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
				{
					xPos += m_pHorizontalScrollBar->GetScrollRange() / 2;
					xPos -= m_pHorizontalScrollBar->GetScrollPos();
				}
				rcCtrl.SetRect(xPos - sz.cx/2,
					yPos + rcPadding.top,
					xPos - sz.cx/2 + sz.cx,
					yPos + rcPadding.top + sz.cy);

				if(m_pCalcControl) 
				{ 
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = rcCtrl; m_bCalcResult = true; return;
					}
				}
				else
					pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_RIGHT) 
			{
				int xPos = rc.right;
				if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
				{
					xPos += m_pHorizontalScrollBar->GetScrollRange();
					xPos -= m_pHorizontalScrollBar->GetScrollPos();
				}
				rcCtrl.SetRect(xPos - rcPadding.right - sz.cx,
					yPos + rcPadding.top,
					xPos - rcPadding.right,
					yPos + rcPadding.top + sz.cy);

				if(m_pCalcControl) 
				{ 
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = rcCtrl; m_bCalcResult = true; return;
					}
				}
				else
					pControl->SetPos(rcCtrl, false);
			}
			else //DT_LEFT
			{
				int xPos = rc.left;
				if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
				{
					xPos -= m_pHorizontalScrollBar->GetScrollPos();
				}
				rcCtrl.SetRect(xPos + rcPadding.left,
					yPos + rcPadding.top,
					xPos + rcPadding.left + sz.cx,
					yPos + rcPadding.top + sz.cy);

				if(m_pCalcControl) 
				{ 
					if(m_pCalcControl == pControl)
					{
						m_rcCalcChild = rcCtrl; m_bCalcResult = true; return;
					}
				}
				else
					pControl->SetPos(rcCtrl, false);
			}

			yPos += sz.cy + GetChildPadding() + rcPadding.top + rcPadding.bottom;
			cyNeededEx += sz.cy + rcPadding.top + rcPadding.bottom;
			if(cxNeededEx < sz.cx + rcPadding.left + rcPadding.right)
				cxNeededEx = sz.cx + rcPadding.left + rcPadding.right;
		}
		cyNeededEx += (nEstimateCount - 1) * GetChildPadding();

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeededEx, cyNeededEx);
	}
} // namespace DuiLib


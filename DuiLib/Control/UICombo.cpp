#include "StdAfx.h"

#include "UIComboWndWin32.h"
#include "UIComboWndSdl.h"

#include "UIEditWndWin32.h"
#include "UIEditWndSdl.h"
namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(CComboUI)

	CComboUI::CComboUI()
		: m_pWindow(NULL)
		, m_iCurSel(-1)
	{
		m_uTextStyle = DT_VCENTER | DT_SINGLELINE;
		m_szDropBox = CDuiSize(0, 150);
		m_rcTextPadding.left = 5;
		m_rcTextPadding.right = 5;

		m_dwTipValueColor = 0xFFBAC0C5;
		m_pEditWindow = NULL;
		m_type = CBS_DROPDOWNLIST;	
		m_szDropButtonSize.cx = 16;
		m_szDropButtonSize.cy = 16;
	}

	CComboUI::~CComboUI()
	{
		if(m_pWindow) { delete m_pWindow; m_pWindow = NULL; }
	}

	LPCTSTR CComboUI::GetClass() const
	{
		return _T("ComboUI");
	}

	LPVOID CComboUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_COMBO) == 0 ) return static_cast<CComboUI*>(this);
		if( _tcsicmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CComboUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP | UIFLAG_SETCURSOR;
	}

	void CComboUI::DoInit()
	{
	}

	UINT CComboUI::GetListType()
	{
		return LT_COMBO;
	}

	TListInfoUI* CComboUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	int CComboUI::GetCurSel() const
	{
		return m_iCurSel;
	}

	bool CComboUI::SelectItem(LPCTSTR pstrText, bool bTriggerEvent)
	{
		for( int it = 0; it < GetCount(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
			if( !pControl->IsVisible() ) continue;

			if(pControl->GetText() == pstrText)
			{
				SelectItem(it, false, bTriggerEvent);
				return true;
			}
		}
		return false;
	}

	bool CComboUI::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
	{
		if( iIndex == m_iCurSel ) return true;
		int iOldSel = m_iCurSel;
		if( m_iCurSel >= 0 ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			if( !pControl ) return false;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->Select(false);
			m_iCurSel = -1;
		}
		if( iIndex < 0 ) return false;
		if( m_items.GetSize() == 0 ) return false;
		if( iIndex >= m_items.GetSize() ) iIndex = m_items.GetSize() - 1;
		CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);
		//if( !pControl || !pControl->IsEnabled() ) return false;
		if( !pControl ) return false; //禁用状态必须是可以选择的
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;
		m_iCurSel = iIndex;
		if( m_pWindow != NULL || bTakeFocus ) pControl->SetFocus();
		pListItem->Select(true);

		//modify by liqs99
		SetText(pControl->GetText());

		if( m_pManager != NULL && bTriggerEvent) 
		{
			m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel, true);
		}
		Invalidate();


		return true;
	}

	bool CComboUI::SelectMultiItem(int iIndex, bool bTakeFocus)
	{
		return SelectItem(iIndex, bTakeFocus);
	}
	
	bool CComboUI::UnSelectItem(int iIndex, bool bOthers)
	{
		return false;
	}
		
	bool CComboUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		int iOrginIndex = GetItemIndex(pControl);
		if( iOrginIndex == -1 ) return false;
		if( iOrginIndex == iIndex ) return true;

		IListItemUI* pSelectedListItem = NULL;
		if( m_iCurSel >= 0 ) pSelectedListItem = 
			static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(_T("ListItem")));
		if( !CContainerUI::SetItemIndex(pControl, iIndex) ) return false;
		int iMinIndex = min(iOrginIndex, iIndex);
		int iMaxIndex = max(iOrginIndex, iIndex);
		for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
		return true;
	}

	bool CComboUI::Add(CControlUI* pControl)
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) 
		{
			pListItem->SetOwner(this);
			pListItem->SetIndex(m_items.GetSize());
			pControl->SetAutoCalcWidth(false);
		}
		return CContainerUI::Add(pControl);
	}

	bool CComboUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if (!CContainerUI::AddAt(pControl, iIndex)) return false;

		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
			pControl->SetAutoCalcWidth(false);
		}

		for(int i = iIndex + 1; i < GetCount(); ++i) {
			CControlUI* p = GetItemAt(i);
			pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( m_iCurSel >= iIndex ) m_iCurSel += 1;
		return true;
	}

	bool CComboUI::Remove(CControlUI* pControl, bool bDoNotDestroy )
	{
		int iIndex = GetItemIndex(pControl);
		if (iIndex == -1) return false;

		if (!CContainerUI::RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < GetCount(); ++i) {
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}

		if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
		return true;
	}

	bool CComboUI::RemoveAt(int iIndex, bool bDoNotDestroy )
	{
		if (!CContainerUI::RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < GetCount(); ++i) {
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->SetIndex(i);
		}

		if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
		return true;
	}

	void CComboUI::RemoveAll()
	{
		m_iCurSel = -1;
		CContainerUI::RemoveAll();
	}

	void CComboUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}

		if( event.Type == UIEVENT_BUTTONDOWN )
		{
			if( IsEnabled() ) 
			{
				CDuiRect rcButton = GetDropButtonRect();
				if(GetDropType() != CBS_DROPDOWN || rcButton.PtInRect(event.ptMouse))
				{
					Activate();
					SetCaptureState(true);
					SetPushedState(true);
				}
				else
				{
					m_pEditWindow = new DuiLibEditWnd(this);
					ASSERT(m_pEditWindow);
					m_pEditWindow->Init();
				}
			}
			return;
		}

		if( event.Type == UIEVENT_BUTTONUP )
		{
			CDuiRect rcButton = GetDropButtonRect();
			if(GetDropType() != CBS_DROPDOWN || rcButton.PtInRect(event.ptMouse))
			{
				if( IsCaptureState() ) 
				{
					SetCaptureState(false);
					Invalidate();
				}
			}
			return;
		}

		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			return;
		}
		if( event.Type == UIEVENT_KEYDOWN )
		{
			switch( event.chKey ) {
			case VK_F4:
				Activate();
				return;
			case VK_UP:
				SelectItem(FindSelectable(m_iCurSel - 1, false));
				return;
			case VK_DOWN:
				SelectItem(FindSelectable(m_iCurSel + 1, true));
				return;
			case VK_PRIOR:
				SelectItem(FindSelectable(m_iCurSel - 1, false));
				return;
			case VK_NEXT:
				SelectItem(FindSelectable(m_iCurSel + 1, true));
				return;
			case VK_HOME:
				SelectItem(FindSelectable(0, false));
				return;
			case VK_END:
				SelectItem(FindSelectable(GetCount() - 1, true));
				return;
			}
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			if(m_ListInfo.IsScrollSelect()) {
				bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
				SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			//if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
			if( m_rcItem.PtInRect(event.ptMouse ) ) {
				if( !IsHotState()  )
					SetHotState(true);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsHotState() ) {
				SetHotState(false);
				Invalidate();
			}
			return;
		}
		CControlUI::DoEvent(event);
	}

	CDuiSize CComboUI::EstimateSize(CDuiSize szAvailable)
	{
		if( IsAutoCalcHeight() ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontHeight(-1) + 12);
		return CControlUI::EstimateSize(szAvailable);
	}

	bool CComboUI::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_PREDROPDOWN);
		if( m_pWindow ) return true;
		m_pWindow = new CComboWnd();
		ASSERT(m_pWindow);
		m_pWindow->Init(this);
		if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_DROPDOWN);
		Invalidate();
		return true;
	}


	//modify by liqs99
// 	CDuiString CComboUI::GetText() const
// 	{
// 		if( m_iCurSel < 0 ) return _T("");
// 		CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
// 		return pControl->GetText();
// 	}

	CDuiString CComboUI::GetDropBoxAttributeList()
	{
		return m_sDropBoxAttributes;
	}

	void CComboUI::SetDropBoxAttributeList(LPCTSTR pstrList)
	{
		m_sDropBoxAttributes = pstrList;
	}

	CDuiSize CComboUI::GetDropBoxSize() const
	{
		return m_szDropBox;
	}

	void CComboUI::SetDropBoxSize(CDuiSize szDropBox)
	{
		m_szDropBox = szDropBox;
	}

	bool CComboUI::IsShowShadow()
	{
		return m_bShowShadow;
	}

	void CComboUI::SetShowShadow(bool bShow)
	{
		if( m_bShowShadow == bShow ) return;

		m_bShowShadow = bShow;
		Invalidate();
	}

	void CComboUI::SetPos(CDuiRect rc, bool bNeedInvalidate)
	{
		CDuiRect rcPos = rc;

		if(!rcPos.EqualRect(m_rcItem)) 
		{
			// 隐藏下拉窗口
			if(m_pWindow && DuiLibWindowWnd::IsWindow(m_pWindow->GetHWND())) m_pWindow->Close();
			// 所有元素大小置为0
			CDuiRect rcNull;
			for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<CControlUI*>(m_items[i])->SetPos(rcNull);
			// 调整位置
			CControlUI::SetPos(rc, bNeedInvalidate);
		}

		if( m_pWindow != NULL ) 
		{
			CDuiRect rcPos = GetClientPos();
			((DuiLibEditWnd *)m_pEditWindow)->SetWindowPos(rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void CComboUI::Move(CDuiSize szOffset, bool bNeedInvalidate)
	{
		CControlUI::Move(szOffset, bNeedInvalidate);
		if( m_pWindow != NULL ) 
		{
			CDuiRect rcPos = GetClientPos();
			((DuiLibEditWnd *)m_pEditWindow)->SetWindowPos(rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void CComboUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if(m_ListInfo.SetAttribute(pstrName, pstrValue)) 
			return;

		if( _tcsicmp(pstrName, _T("showshadow")) == 0 ) SetShowShadow(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("dropbox")) == 0 ) SetDropBoxAttributeList(pstrValue);
		else if( _tcsicmp(pstrName, _T("dropboxsize")) == 0)
		{
			SetDropBoxSize(pstrValue);
		}
		else if( _tcsicmp(pstrName, _T("dropbuttonsize")) == 0 )
		{
			m_szDropButtonSize = CDuiSize(pstrValue);
		}
		else if( _tcsicmp(pstrName, _T("dropbuttonnormalimage")) == 0 ) SetdbNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("dropbuttonhotimage")) == 0 ) SetdbHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("dropbuttonpushedimage")) == 0 ) SetdbPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("dropbuttonfocusedimage")) == 0 ) SetdbFocusedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("dropbuttondisabledimage")) == 0 ) SetdbDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("type")) == 0 ) 
		{
			if( _tcsicmp(pstrValue , _T("dropdown")) == 0)
				SetDropType(CBS_DROPDOWN);
			else
				SetDropType(CBS_DROPDOWNLIST);
		}
		else if( _tcsicmp(pstrName, _T("tipvalue")) == 0 ) SetTipValue(pstrValue);
		else if( _tcsicmp(pstrName, _T("tipvaluecolor")) == 0 ) SetTipValueColor(pstrValue);
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	bool CComboUI::DoPaint(UIRender *pRender, const CDuiRect& rcPaint, CControlUI* pStopControl)
	{
		if(!CControlUI::DoPaint(pRender, rcPaint, pStopControl))
			return false;

		if( !IsEnabled() ) {
			if( !m_dbDisabledImage.IsEmpty() ) {
				if( DrawDropButtonImage(pRender, (LPCTSTR)m_dbDisabledImage) )
					return true;
			}
		}
		else if( IsPushedState() ) {
			if( !m_dbPushedImage.IsEmpty() ) {
				if( DrawDropButtonImage(pRender, (LPCTSTR)m_dbPushedImage) )
					return true;
			}
		}
		else if( IsHotState() ) {
			if( !m_dbHotImage.IsEmpty() ) {
				if( DrawDropButtonImage(pRender, (LPCTSTR)m_dbHotImage) ) 
					return true;
			}
		}
		else if( IsFocused() ) {
			if( !m_dbFocusedImage.IsEmpty() ) {
				if( DrawDropButtonImage(pRender, (LPCTSTR)m_dbFocusedImage) )
					return true;
			}
		}

		if( !m_dbNormalImage.IsEmpty() ) {
			if( DrawDropButtonImage(pRender, (LPCTSTR)m_dbNormalImage) )
				return true;
		}

		return true;
	}

	bool CComboUI::DrawDropButtonImage(UIRender *pRender, LPCTSTR pStrImage, LPCTSTR pStrModify)
	{
		CDuiRect rcButton = GetDropButtonRect();
		return pRender->DrawImageString(rcButton, rcButton, pStrImage, pStrModify, m_instance);
	}

	void CComboUI::PaintText(UIRender *pRender)
	{
		if(m_pEditWindow)
			return;

// 		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
// 		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
// 
// 		CDuiRect rc = m_rcItem;
// 		rc.left += m_rcTextPadding.left;
// 		rc.right -= m_rcTextPadding.right;
// 		rc.top += m_rcTextPadding.top;
// 		rc.bottom -= m_rcTextPadding.bottom;
// 
// 		CDuiString sText = GetText();
// 		DWORD dwTextColor = m_dwTextColor;
// 
// 		if( sText.IsEmpty() ) 
// 		{
// 			return;
// 		}
// 
// 		pRender->DrawText(rc, CDuiRect(0,0,0,0), sText, dwTextColor, GetFont(), GetTextStyle());

		CDuiString sText = GetText();
		if(sText.IsEmpty()) return;

		CDuiRect rcText = m_rcItem;
		CDuiColor dwColor = 0;
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

		pRender->DrawText(rcText, rcTextPadding, sText, dwColor, iFont, GetTextStyle());
		return;
	}

	void CComboUI::SetText(LPCTSTR pstrText)
	{
		if( m_sText == pstrText ) return;
		m_sText = pstrText;

		for( int it = 0; it < GetCount(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
			if( !pControl->IsVisible() ) continue;

			if(pControl->GetText() == pstrText)
			{
				SelectItem(it);
				return;
			}
		}
		SelectItem(-1);
	}

	CDuiRect CComboUI::GetClientPos()
	{
		CDuiRect rcPos = m_rcItem;
		CDuiRect rcButton = GetDropButtonRect();
		rcPos.right = rcButton.left;
		rcPos.right -= 1;
		return rcPos;
	}

	CControlUI *CComboUI::AddString(LPCTSTR pstrText, UINT_PTR pItemData)
	{
		CListLabelElementUI *pLabel = new CListLabelElementUI;
		pLabel->SetTag(pItemData);
		pLabel->SetText(pstrText);
		if(!Add(pLabel))
		{
			delete pLabel;
			return NULL;
		}
		pLabel->SetFixedWidth(m_rcItem.right - m_rcItem.left);
		return pLabel;
	}

	bool CComboUI::DeleteString(LPCTSTR pstrText)
	{
		for( int it = 0; it < GetCount(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
			if( !pControl->IsVisible() ) continue;

			if(pControl->GetText() == pstrText)
			{
				if(m_iCurSel  == it)
				{
					SetCurSel(-1);
					SetText(_T(""));
				}
				Remove(pControl);
				return true;
			}
		}
		return false;
	}

	bool CComboUI::DeleteString_byItemData(UINT_PTR pItemData)
	{
		for( int it = 0; it < GetCount(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
			if( !pControl->IsVisible() ) continue;

			if(pControl->GetTag() == pItemData)
			{
				if(m_iCurSel  == it)
				{
					SetCurSel(-1);
					SetText(_T(""));
				}
				Remove(pControl);
				return true;
			}
		}
		return false;
	}

	bool CComboUI::SelectString(LPCTSTR pstrText)
	{
		return SelectItem(pstrText);
	}

	bool CComboUI::SetCurSel(int iIndex, bool bTakeFocus)
	{
		bool bRet = SelectItem(iIndex, bTakeFocus);
		if(!bRet)
		{
			m_sText.Empty();
		}
		return bRet;
	}

	bool CComboUI::SetCurSelFromItemData(UINT_PTR ptrItemData)
	{
		for( int it = 0; it < GetCount(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
			if( !pControl->IsVisible() ) continue;

			if(pControl->GetTag() == ptrItemData)
			{
				SelectItem(it);
				return true;
			}
		}
		return false;
	}

	bool CComboUI::SetItemData(CControlUI *pControl, UINT_PTR ptrItemData)
	{
		pControl->SetTag(ptrItemData);
		return true;
	}

	UINT_PTR CComboUI::GetCurSelItemData()
	{
		if(m_iCurSel < 0) return 0;
		CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
		return pControl->GetTag();
	}

	LPCTSTR CComboUI::GetdbNormalImage() const
	{
		return m_dbNormalImage;
	}

	void CComboUI::SetdbNormalImage(LPCTSTR pStrImage)
	{
		m_dbNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CComboUI::GetdbHotImage() const
	{
		return m_dbHotImage;
	}

	void CComboUI::SetdbHotImage(LPCTSTR pStrImage)
	{
		m_dbHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CComboUI::GetdbPushedImage() const
	{
		return m_dbPushedImage;
	}

	void CComboUI::SetdbPushedImage(LPCTSTR pStrImage)
	{
		m_dbPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CComboUI::GetdbFocusedImage() const
	{
		return m_dbFocusedImage;
	}

	void CComboUI::SetdbFocusedImage(LPCTSTR pStrImage)
	{
		m_dbFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CComboUI::GetdbDisabledImage() const
	{
		return m_dbDisabledImage;
	}

	void CComboUI::SetdbDisabledImage(LPCTSTR pStrImage)
	{
		m_dbDisabledImage = pStrImage;
		Invalidate();
	}

	int CComboUI::GetDropType() const
	{
		return m_type;
	}

	void CComboUI::SetDropType(int type)
	{
		m_type = type;
	}

	void CComboUI::SetTipValue( LPCTSTR pStrTipValue )
	{
		m_sTipValue	= pStrTipValue;
	}

	CDuiString CComboUI::GetTipValue()
	{
		if (IsResourceText()) 
		{
			CDuiString s = CResourceManager::GetInstance()->GetText(m_sTipValue);
			if(!s.IsEmpty()) return s;
		}

		CLangPackageUI *pkg = GetLangPackage();
		if(pkg && GetResourceID() > 0)
		{
			LPCTSTR s = pkg->GetTipValue(GetResourceID());
			if(s && *s!='\0') return s; 
		}
		else if (IsResourceText())
		{
			CDuiString s = CLangManagerUI::LoadString(m_sTipValue);
			if(!s.IsEmpty()) return s;
		}
		return m_sTipValue;
	}

	void CComboUI::SetTipValueColor(CDuiColor dwColor )
	{
		m_dwTipValueColor = dwColor;
	}

	CDuiColor CComboUI::GetTipValueColor()
	{
		return m_dwTipValueColor;
	}

	CDuiRect CComboUI::GetDropButtonRect()
	{
		CDuiRect rc = m_rcItem;
		rc.top++;
		rc.bottom--;
		rc.right--;
		rc.left = rc.right - (rc.bottom - rc.top);

		CDuiSize sz = m_szDropButtonSize;
		CDuiRect rcButton;
		rcButton.left = rc.left + (rc.right - rc.left)/2 - sz.cx/2;
		rcButton.right = rcButton.left + sz.cx;
		rcButton.top = rc.top + (rc.bottom - rc.top)/2 - sz.cy/2;
		rcButton.bottom = rcButton.top + sz.cy;

		return rcButton;
	}
} // namespace DuiLib


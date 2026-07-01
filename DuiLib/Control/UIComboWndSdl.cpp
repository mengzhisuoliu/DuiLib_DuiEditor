#include "StdAfx.h"
#include "UIComboWndSdl.h"

#ifdef DUILIB_SDL
namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	void CComboWndSDL::Init(CComboUI* pOwner)
	{
		m_bHitItem = false;
		m_pOwner = pOwner;

		// Position the popup window in absolute space
		CDuiSize szDrop = m_pOwner->GetDropBoxSize();
		CDuiRect rcOwner = pOwner->GetPos();
		CDuiRect rc = rcOwner;
		rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
		rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
		if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

		CDuiSize szAvailable = rc;
		int cyFixed = 0;
		for( int it = 0; it < pOwner->GetCount(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(it));
			if( !pControl->IsVisible() ) continue;
			CDuiSize sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;
		}
		cyFixed += 4;
		rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

		SDL_Window* pParentWnd = m_pOwner->GetManager()->GetPaintWindow();
		UIWND pWindow = Create(pParentWnd, NULL, UI_WNDSTYLE_CHILD, 0, rc.left, rc.top, rc.GetWidth(), rc.GetHeight());
		if (!pWindow) return;
	}

	void CComboWndSDL::OnFinalMessage(UIWND hWnd)
	{
		m_pOwner->m_pWindow = NULL;
		m_pOwner->SetPushedState(false);
		m_pOwner->Invalidate();
		delete this;
	}

	LRESULT CComboWndSDL::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
	{
		m_pm.SetForceUseSharedRes(true);
		m_pm.Init(m_hWnd, NULL, this);

		m_pLayout = new CVerticalLayoutUI;
		m_pLayout->SetManager(&m_pm, NULL, true);
		LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
		if (pDefaultAttributes)
		{
			m_pLayout->ApplyAttributeList(pDefaultAttributes);
		}
		m_pLayout->SetInset(CDuiRect(2, 1, 2, 1));
		m_pLayout->SetBkColor(0xFFFFFFFF);
		m_pLayout->SetBorderColor(0xFFC6C7D2);
		m_pLayout->SetBorderSize(1);
		m_pLayout->SetAutoDestroy(false);
		m_pLayout->EnableScrollBar();
		m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());
		for (int i = 0; i < m_pOwner->GetCount(); i++)
		{
			m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
		}
		CShadowUI* pShadow = m_pOwner->GetManager()->GetShadow();
		pShadow->CopyShadow(m_pm.GetShadow());
		m_pm.GetShadow()->ShowShadow(m_pOwner->IsShowShadow());
		m_pm.AttachDialog(m_pLayout);
		m_pm.AddNotifier(this);

		ShowAndActivateChildWindow();
		return 0;
	}

	LRESULT CComboWndSDL::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
	{
		m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
		CDuiRect rcNull;
		for (int i = 0; i < m_pOwner->GetCount(); i++)
		{
			static_cast<CControlUI*>(m_pOwner->GetItemAt(i))->SetPos(rcNull);
		}
		m_pOwner->SetFocus();
		bHandled = uiFalse;
		return 0;
	}

	LRESULT CComboWndSDL::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
	{
		CDuiPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		m_bHitItem = IsHitItem(pt);
		bHandled = uiFalse;
		return 0;
	}

	LRESULT CComboWndSDL::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
	{
		CDuiPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (m_bHitItem && IsHitItem(pt))
		{
			Close();
		}
		m_bHitItem = false;
		//SetHandleMessage(FALSE);
		return 0;
	}

	LRESULT CComboWndSDL::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
	{
		Close();
		//SetHandleMessage(FALSE);
		return 0;
	}

	void CComboWndSDL::Notify(TNotifyUI& msg)
	{

	}

	bool CComboWndSDL::IsHitItem(CDuiPoint ptMouse)
	{
		CControlUI* pControl = m_pm.FindControl(ptMouse);
		if(pControl != NULL) {
			LPVOID pInterface = pControl->GetInterface(DUI_CTR_SCROLLBAR);
			if(pInterface) return false;

			while(pControl != NULL) {
				IListItemUI* pListItem = (IListItemUI*)pControl->GetInterface(DUI_CTR_LISTITEM);
				if(pListItem != NULL ) {
					return true;
				}
				pControl = pControl->GetParent();
			}
		}

		return false;
	}
} // namespace DuiLib
#endif //#ifdef DUILIB_WIN32


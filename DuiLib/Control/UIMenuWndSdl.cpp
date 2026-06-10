#include "StdAfx.h"

#include "UIMenuWndSdl.h"

#ifdef DUILIB_SDL
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	CMenuWndSDL::CMenuWndSDL():
	m_pOwner(NULL),
		m_pLayout(),
		m_xml(_T("")),
		isClosing(false),
		m_bAutoDestroy(false)
	{
		m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
	}

	CMenuWndSDL::~CMenuWndSDL()
	{
		
	}

	void CMenuWndSDL::Close(UINT nRet)
	{
		CWindowSDL::Close(nRet);
		isClosing = true;
	}


	BOOL CMenuWndSDL::Receive(ContextMenuParam param)
	{
		switch (param.wParam)
		{
		case 1:
			Close();
			break;
		case 2:
			{
				UIWND hParent = GetParentWindow(m_hWnd);
				while (hParent != NULL)
				{
					if (hParent == param.hWnd)
					{
						Close();
						break;
					}
					hParent = GetParentWindow(hParent);
				}
			}
			break;
		default:
			break;
		}

		return TRUE;
	}

	CMenuWndSDL* CMenuWndSDL::CreateMenu(CMenuElementUI* pOwner, STRINGorID xml, CDuiPoint point, CPaintManagerUI* pMainPaintManager, CStdStringPtrMap* pMenuCheckInfo /*= NULL*/, DWORD dwAlignment /*= eMenuAlignment_Left | eMenuAlignment_Top*/)
	{
		CMenuWndSDL* pMenu = new CMenuWndSDL;
		pMenu->Init(pOwner, xml, point, pMainPaintManager, pMenuCheckInfo, dwAlignment);
		return pMenu;
	}

	void CMenuWndSDL::DestroyMenu()
	{
		CStdStringPtrMap* mCheckInfos = CMenuWndSDL::GetGlobalContextMenuObserver().GetMenuCheckInfo();
		if (mCheckInfos != NULL)
		{
			for(int i = 0; i < mCheckInfos->GetSize(); i++) {
				MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(mCheckInfos->GetAt(i));
				if(pItemInfo != NULL) {
					delete pItemInfo;
					pItemInfo = NULL;
				}
			}
			mCheckInfos->Resize(0);
			CMenuWndSDL::GetGlobalContextMenuObserver().SetMenuCheckInfo(NULL);
		}
	}
	
	MenuItemInfo* CMenuWndSDL::SetMenuItemInfo(LPCTSTR pstrName, bool bChecked)
	{
		if(pstrName == NULL || _tcslen(pstrName) <= 0) return NULL;

		CStdStringPtrMap* mCheckInfos = CMenuWndSDL::GetGlobalContextMenuObserver().GetMenuCheckInfo();
		if (mCheckInfos != NULL)
		{
			MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(pstrName);
			if(pItemInfo == NULL) {
				pItemInfo = new MenuItemInfo;
				_tcscpy(pItemInfo->szName, pstrName);
				pItemInfo->bChecked = bChecked;
				mCheckInfos->Insert(pstrName, pItemInfo);
			}
			else {
				pItemInfo->bChecked = bChecked;
			}

			return pItemInfo;
		}
		return NULL;
	}

	void CMenuWndSDL::Init(CMenuElementUI* pOwner, STRINGorID xml, CDuiPoint point,
		CPaintManagerUI* pMainPaintManager, CStdStringPtrMap* pMenuCheckInfo/* = NULL*/,
		DWORD dwAlignment/* = eMenuAlignment_Left | eMenuAlignment_Top*/)
	{

		m_BasedPoint = point;
		m_pOwner = pOwner;
		m_pLayout = NULL;
		m_xml = xml;
		m_dwAlignment = dwAlignment;

		// 如果是一级菜单的创建
		if (pOwner == NULL)
		{
			ASSERT(pMainPaintManager != NULL);
			CMenuWndSDL::GetGlobalContextMenuObserver().SetManger(pMainPaintManager);
			if (pMenuCheckInfo != NULL)
				CMenuWndSDL::GetGlobalContextMenuObserver().SetMenuCheckInfo(pMenuCheckInfo);
		}

		CMenuWndSDL::GetGlobalContextMenuObserver().AddReceiver(this);

		UIWND hParentWnd = NULL;
		if (m_pOwner == NULL)
			hParentWnd = pMainPaintManager->GetPaintWindow();
		else
			hParentWnd = m_pOwner->GetManager()->GetPaintWindow();
		Create(hParentWnd, NULL, WS_POPUP , WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0,0,0,0);
	}

	LPCTSTR CMenuWndSDL::GetWindowClassName() const
	{
		return _T("DuiMenuWnd");
	}


	void CMenuWndSDL::Notify(TNotifyUI& msg)
	{
		if( CMenuWndSDL::GetGlobalContextMenuObserver().GetManager() != NULL) 
		{
			if( msg.sType == _T("click") || msg.sType == _T("valuechanged") ) 
			{
				CMenuWndSDL::GetGlobalContextMenuObserver().GetManager()->SendNotify(msg, false);
			}
		}

	}

	CControlUI* CMenuWndSDL::CreateControl( LPCTSTR pstrClassName )
	{
		if (_tcsicmp(pstrClassName, _T("Menu")) == 0)
		{
			return new CMenuUI();
		}
		else if (_tcsicmp(pstrClassName, _T("MenuElement")) == 0)
		{
			return new CMenuElementUI();
		}
		return NULL;
	}


	void CMenuWndSDL::OnFinalMessage(UIWND hWnd)
	{
		RemoveObserver();
		if( m_pOwner != NULL ) {
			for( int i = 0; i < m_pOwner->GetCount(); i++ ) {
				if( static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement"))) != NULL ) {
					(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pOwner->GetParent());
					(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetVisible(false);
					(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
				}
			}
			m_pOwner->m_pWindow = NULL;
			m_pOwner->SetPushedState(false);
			m_pOwner->Invalidate();

			// 内部创建的内部删除
			delete this;
		}
		else
		{
			if(m_bAutoDestroy)
			{
				DestroyMenu();
				// 外部创建的也删除
				delete this;
			}
		}
	}

	LRESULT CMenuWndSDL::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bool bShowShadow = false;
		if( m_pOwner != NULL) 
		{
			CDuiRect rcClient;
			GetClientRect(&rcClient);
			SetWindowPos(rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
				rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

			m_pm.Init(m_hWnd, NULL, this);
			m_pm.GetDPIObj()->SetScale(m_pOwner->GetManager()->GetDPIObj()->GetDPI());
			// The trick is to add the items to the new container. Their owner gets
			// reassigned by this operation - which is why it is important to reassign
			// the items back to the righfull owner/manager when the window closes.
			m_pLayout = new CMenuUI();
			m_pm.SetForceUseSharedRes(true);
			m_pLayout->SetManager(&m_pm, NULL, true);
			LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("Menu"));
			if( pDefaultAttributes ) {
				m_pLayout->ApplyAttributeList(pDefaultAttributes);
			}
			m_pLayout->GetList()->SetAutoDestroy(false);

			for( int i = 0; i < m_pOwner->GetCount(); i++ ) 
			{
				if(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL )
				{
					CMenuElementUI *pElementUI = static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i));
					pElementUI->SetOwner(m_pLayout);
					m_pLayout->Add(static_cast<CControlUI*>(pElementUI));
					pElementUI->OnUpdateCommandUI();
				}
			}

			CShadowUI *pShadow = m_pOwner->GetManager()->GetShadow();
			pShadow->CopyShadow(m_pm.GetShadow());
			bShowShadow = m_pm.GetShadow()->IsShowShadow();
			m_pm.GetShadow()->ShowShadow(false);
			m_pm.SetLayered(m_pOwner->GetManager()->IsLayered());
			m_pm.AttachDialog(m_pLayout);
			m_pm.AddNotifier(this);

			ResizeSubMenu();
		}
		else {
			m_pm.Init(m_hWnd, NULL, this);
			m_pm.GetDPIObj()->SetScale(CMenuWndSDL::GetGlobalContextMenuObserver().GetManager()->GetDPIObj()->GetDPI());
			CDialogBuilder builder;

			CControlUI* pRoot = builder.Create(m_xml, NULL, this, &m_pm);
			bShowShadow = m_pm.GetShadow()->IsShowShadow();
			m_pm.GetShadow()->ShowShadow(false);
			m_pm.AttachDialog(pRoot);
			m_pm.AddNotifier(this);

			if(GetMenuUI())
			{
				for (int i=0; i<GetMenuUI()->GetCount(); i++)
				{
					CMenuElementUI *pElementUI = static_cast<CMenuElementUI*>(GetMenuUI()->GetItemAt(i));
					pElementUI->OnUpdateCommandUI();
				}
			}

			ResizeMenu();
		}
		GetMenuUI()->m_pWindow = this;
		m_pm.GetShadow()->ShowShadow(bShowShadow);
		m_pm.GetShadow()->Create(&m_pm);
		return 0;
	}

	CMenuUI* CMenuWndSDL::GetMenuUI()
	{
		return dynamic_cast<CMenuUI*>(m_pm.GetRoot());
	}

	/*
	void CMenuWndSDL::ResizeMenu()
	{
		CControlUI* pRoot = m_pm.GetRoot();

#if defined(WIN32) && !defined(UNDER_CE)
		MONITORINFO oMonitor = {}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
#else
		CDuiRect rcWork;
		GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
		SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
		szAvailable = pRoot->EstimateSize(szAvailable);

		//szAvailable.cx += 20; //add by liqs99 菜单右边显示不全

		m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

		//必须是Menu标签作为xml的根节点
		CMenuUI *pMenuRoot = static_cast<CMenuUI*>(pRoot);
		ASSERT(pMenuRoot);

		SIZE szInit = m_pm.GetInitSize();
		CDuiRect rc;
		CDuiPoint point = m_BasedPoint;
		rc.left = point.x;
		rc.top = point.y;
		rc.right = rc.left + szInit.cx;
		rc.bottom = rc.top + szInit.cy;

		int nWidth = rc.GetWidth();
		int nHeight = rc.GetHeight();

		if (m_dwAlignment & eMenuAlignment_Right)
		{
			rc.right = point.x;
			rc.left = rc.right - nWidth;
		}

		if (m_dwAlignment & eMenuAlignment_Bottom)
		{
			rc.bottom = point.y;
			rc.top = rc.bottom - nHeight;
		}

		//////////////////////////////////////////////////////////////////////////add by liqs99
		//调整在屏幕中的位置，主要是触碰边界
		//int screenX = GetSystemMetrics(SM_CXSCREEN);//获取整个屏幕右下角X坐标   
		//int screenY = GetSystemMetrics(SM_CYSCREEN);//屏幕Y坐标 
		if(rc.right > rcWork.GetWidth())
		{
			rc.left -= rc.right - rcWork.GetWidth();
			rc.right -= rc.right - rcWork.GetWidth();
		}
		if(rc.bottom > rcWork.GetHeight())
		{
			rc.top = point.y - rc.GetHeight();
			rc.bottom = point.y;
		}
		//////////////////////////////////////////////////////////////////////////

		SetForegroundWindow(m_hWnd);
		MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
		SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight() + pMenuRoot->GetInset().bottom + pMenuRoot->GetInset().top, SWP_SHOWWINDOW);
	}
	*/
	void CMenuWndSDL::ResizeMenu()
	{
		CMenuUI* pMenuRoot = static_cast<CMenuUI*>(m_pm.GetRoot());
		if (!pMenuRoot)
			return;

		// 获取鼠标点所在的显示器工作区
		SDL_Point ptBase = { m_BasedPoint.x, m_BasedPoint.y };
		SDL_DisplayID displayID = SDL_GetDisplayForPoint(&ptBase);
		if (displayID == 0)
			displayID = SDL_GetPrimaryDisplay();

		SDL_Rect rcWork;
		if (!SDL_GetDisplayUsableBounds(displayID, &rcWork)) {
			// 获取失败，回退到整个显示器边界
			SDL_GetDisplayBounds(displayID, &rcWork);
		}

		// 计算菜单所需尺寸
		CDuiSize szAvailable = rcWork;
		szAvailable = pMenuRoot->EstimateSize(szAvailable);
		CDuiRect rcInset = pMenuRoot->GetInset();
		szAvailable.cx += rcInset.left + rcInset.right;
		szAvailable.cy += rcInset.top + rcInset.bottom;
		m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

		int nWidth = m_pm.GetInitSize().cx;
		int nHeight = m_pm.GetInitSize().cy;

		// 调整基点坐标（确保菜单位于工作区内）
		int x = m_BasedPoint.x;
		int y = m_BasedPoint.y;

		if (x < rcWork.x)
			x = rcWork.x;
		else if (x + nWidth > rcWork.x + rcWork.w)
			x = rcWork.x + rcWork.w - nWidth;

		if (y + nHeight > rcWork.y + rcWork.h)
			y = rcWork.y + rcWork.h - nHeight;
		if (y < rcWork.y)
			y = rcWork.y;

		// 应用窗口位置与尺寸
		SetWindowPos(x, y, nWidth, nHeight, SWP_SHOWWINDOW);
	}

	void CMenuWndSDL::ResizeSubMenu()
	{
		// 获取父窗口 (SDL_Window*)
		SDL_Window* pParentWnd = m_pOwner->GetManager()->GetPaintWindow();
		if (!pParentWnd) return;

		// 父窗口屏幕位置与尺寸
		int parentX, parentY, parentW, parentH;
		SDL_GetWindowPosition(pParentWnd, &parentX, &parentY);
		SDL_GetWindowSize(pParentWnd, &parentW, &parentH);

		// 控件在父窗口客户区中的相对位置，转为屏幕坐标
		CDuiRect rcOwner = m_pOwner->GetPos();
		CDuiRect rcOwnerScreen(
			rcOwner.left + parentX,
			rcOwner.top + parentY,
			rcOwner.right + parentX,
			rcOwner.bottom + parentY);

		// 基于触发点获取显示器工作区
		SDL_Point ptBase = { rcOwnerScreen.left, rcOwnerScreen.top };
		SDL_DisplayID displayID = SDL_GetDisplayForPoint(&ptBase);
		if (displayID == 0) displayID = SDL_GetPrimaryDisplay();

		SDL_Rect rcWork;
		SDL_GetDisplayUsableBounds(displayID, &rcWork);   // 工作区（不含任务栏）

		CDuiSize szAvailable = rcWork;

		// 计算子菜单所需尺寸
		int cxFixed = 0, cyFixed = 0;
		for (int it = 0; it < m_pOwner->GetCount(); ++it) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
			if (pControl->GetInterface(_T("MenuElement")) != nullptr) {
				CDuiSize sz = pControl->EstimateSize(szAvailable);
				cyFixed += sz.cy;
				if (cxFixed < sz.cx) cxFixed = sz.cx;
			}
		}

		// 父窗口屏幕矩形
		CDuiRect rcWindow = { parentX, parentY, parentX + parentW, parentY + parentH };

		// 初始菜单位置：右侧对齐父窗口，顶部对齐控件顶部
		CDuiRect rc = rcOwnerScreen;
		rc.bottom = rc.top + cyFixed;
		rc.left = rcWindow.right;
		rc.right = rc.left + cxFixed + 2;   // +2 为边距

		// 与已打开菜单窗口的位置冲突检测
		bool bReachBottom = false, bReachRight = false;
		MenuObserverImpl::Iterator iterator(CMenuWndSDL::GetGlobalContextMenuObserver());
		MenuMenuReceiverImplBase* pReceiver = iterator.next();
		while (pReceiver) {
			auto pContextMenu = dynamic_cast<CMenuWndSDL*>(pReceiver);
			if (pContextMenu) {
				SDL_Rect rcPre;
				SDL_GetWindowPosition(pContextMenu->m_hWnd, &rcPre.x, &rcPre.y);
				SDL_GetWindowSize(pContextMenu->m_hWnd, &rcPre.w, &rcPre.h);
				CDuiRect rcPreWindow(rcPre.x, rcPre.y, rcPre.x + rcPre.w, rcPre.y + rcPre.h);

				bReachRight = (rcPreWindow.left >= rcWindow.right);
				bReachBottom = (rcPreWindow.top >= rcWindow.bottom);
				if (pContextMenu->m_hWnd == (UIWND)pParentWnd || bReachBottom || bReachRight)
					break;
			}
			pReceiver = iterator.next();
		}

		if (bReachBottom) {
			rc.bottom = rcWindow.top;
			rc.top = rc.bottom - cyFixed;
		}
		if (bReachRight) {
			rc.right = rcWindow.left;
			rc.left = rc.right - cxFixed;
		}

		// 边界裁剪（不超出显示器工作区）
		if (rc.bottom > rcWork.y + rcWork.h) {
			rc.bottom = rc.top;
			rc.top = rc.bottom - cyFixed;
		}
		if (rc.right > rcWork.x + rcWork.w) {
			rc.right = rcWindow.left;
			rc.left = rc.right - cxFixed;
		}
		if (rc.top < rcWork.y) {
			rc.top = rcOwnerScreen.top;
			rc.bottom = rc.top + cyFixed;
		}
		if (rc.left < rcWork.x) {
			rc.left = rcWindow.right;
			rc.right = rc.left + cxFixed;
		}

		// 应用窗口位置与尺寸（加上 Inset 高度）
		int finalHeight = (rc.bottom - rc.top) + m_pLayout->GetInset().top + m_pLayout->GetInset().bottom;
		SDL_SetWindowPosition(m_hWnd, rc.left, rc.top);
		SDL_SetWindowSize(m_hWnd, rc.right - rc.left, finalHeight);
	}

	void CMenuWndSDL::setDPI(int DPI) 
	{
		m_pm.SetDPI(DPI);
	}

	LRESULT CMenuWndSDL::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//SetHandleMessage(FALSE);

		UIWND hFocusWnd = (UIWND)wParam;

		BOOL bInMenuWindowList = FALSE;
		ContextMenuParam param;
		param.hWnd = GetHWND();

		MenuObserverImpl::Iterator iterator(CMenuWndSDL::GetGlobalContextMenuObserver());
		MenuMenuReceiverImplBase* pReceiver = iterator.next();
		while( pReceiver != NULL ) {
			CMenuWndSDL* pContextMenu = dynamic_cast<CMenuWndSDL*>(pReceiver);
			if( pContextMenu != NULL && pContextMenu->GetHWND() ==  hFocusWnd ) {
				bInMenuWindowList = TRUE;
				break;
			}
			pReceiver = iterator.next();
		}

		if( !bInMenuWindowList ) {
			param.wParam = 1;
			CMenuWndSDL::GetGlobalContextMenuObserver().RBroadcast(param);
			return 0;
		}
		return 0;
	}

	LRESULT CMenuWndSDL::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
// 		SIZE szRoundCorner = m_pm.GetRoundCorner();
// 		if( !::IsIconic(*this) ) {
// 			CDuiRect rcWnd;
// 			::GetWindowRect(*this, &rcWnd);
// 			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
// 			rcWnd.right++; rcWnd.bottom++;
// 			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
// 			::SetWindowRgn(*this, hRgn, TRUE);
// 			::DeleteObject(hRgn);
// 		}
//		SetHandleMessage(FALSE);
		return 0;
	}

	LRESULT CMenuWndSDL::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == VK_ESCAPE || wParam == VK_LEFT)
		{
			Close();
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT CMenuWndSDL::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_pOwner != NULL)
		{
			m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
			m_pOwner->SetPos(m_pOwner->GetPos());
			m_pOwner->SetFocus();
		}

		bHandled = FALSE;
		return 0;
	}

} // namespace DuiLib
#endif // #ifdef DUILIB_WIN32



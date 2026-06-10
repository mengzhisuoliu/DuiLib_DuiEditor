#include "StdAfx.h"

#include "../Render/UIRender_Sdl.h"

#ifdef DUILIB_SDL
namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	// 定时器回调（运行在独立线程）
	static Uint32 SDLCALL Callback_SDL_Timer(void* userdata, SDL_TimerID timerID, Uint32 interval)
	{
		TIMERINFO* pTimer = (TIMERINFO*)userdata;

		// 创建自定义事件
		SDL_Event ev;
		SDL_zero(ev);
		ev.type = SDL_EVENT_USER;
		ev.window.windowID = SDL_GetWindowID(pTimer->hWnd);
		ev.user.code = WM_TIMER;
		ev.user.data1 = userdata;
		ev.user.data2 = NULL;

		if (ev.window.windowID == 0)
		{
			return 0;
		}

		// 推送到主线程事件队列（线程安全）
		if (!SDL_PushEvent(&ev)) {
			SDL_Log("PushEvent failed: %s", SDL_GetError());
		}

		// 返回相同的间隔，让定时器继续运行
		return interval;
	}

	CPaintManagerSDLUI::CPaintManagerSDLUI()
	{
	}

	CPaintManagerSDLUI::~CPaintManagerSDLUI()
	{
		if(m_pLangManager){ delete m_pLangManager; m_pLangManager = NULL; }

		// Delete the control-tree structures
		for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
		m_aDelayedCleanup.Resize(0);
		for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
		m_aAsyncNotify.Resize(0);

		m_mNameHash.Resize(0);
		if( m_pRoot != NULL ) delete m_pRoot;

		//delete m_ResInfo.m_DefaultFontInfo;
		RemoveAllFonts();
		RemoveAllImages();
		RemoveAllStyle();
		RemoveAllDefaultAttributeList();
		RemoveAllWindowCustomAttribute();
		RemoveAllOptionGroups();
		RemoveAllTimers();
		RemoveAllDrawInfos();

		m_aPreMessages.Remove(m_aPreMessages.Find(this));

		// DPI管理对象
		if (m_pDPI != NULL) {
			delete m_pDPI;
			m_pDPI = NULL;
		}

		if(m_pRenderEngine)
		{
			//m_pRenderEngine->Release();
			m_pRenderEngine = NULL;
		}
	}

	void CPaintManagerSDLUI::Init(UIWND hWnd, LPCTSTR pstrName, CWindowWnd *pWindow)
	{
		CPaintManagerUI::Init(hWnd, pstrName, pWindow); 
		
		if( m_hWndPaint != hWnd ) 
		{
			m_hWndPaint = hWnd;
			m_pWindow = pWindow;
			m_aPreMessages.Add(this);
			Render()->Init(this, (PVOID)m_hWndPaint);
		}
	}

	UIRender *CPaintManagerSDLUI::Render()
	{
		if(!m_pRenderEngine)
		{
			m_pRenderEngine = MakeRefPtr<UIRender>(UIGlobal::CreateRenderTarget());
			//m_pRenderEngine->Init(this, NULL);
		}
		ASSERT(m_pRenderEngine);
		return m_pRenderEngine;
	}

	void CPaintManagerSDLUI::ClearImageTexture(UIRender* pRender)
	{
		if (!pRender) return;

		// 获取 SDL_Renderer*
		SDL_Renderer* pSDLRenderer = static_cast<SDL_Renderer*>(pRender->GetHandle());
		if (!pSDLRenderer) return;

		// 遍历非共享图片资源
		for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); ++i) {
			LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i);
			if (!key) continue;
			UIImage* pImage = static_cast<UIImage*>(m_ResInfo.m_ImageHash.Find(key));
			if (pImage && pImage->bitmap) {
				UIBitmap_SDL* pBitmapSDL = dynamic_cast<UIBitmap_SDL*>(pImage->bitmap);
				if (pBitmapSDL) {
					pBitmapSDL->DeleteTexture(pSDLRenderer);
				}
			}
		}

		// 遍历共享图片资源
		for (int i = 0; i < m_SharedResInfo.m_ImageHash.GetSize(); ++i) {
			LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i);
			if (!key) continue;
			UIImage* pImage = static_cast<UIImage*>(m_SharedResInfo.m_ImageHash.Find(key));
			if (pImage && pImage->bitmap) {
				UIBitmap_SDL* pBitmapSDL = dynamic_cast<UIBitmap_SDL*>(pImage->bitmap);
				if (pBitmapSDL) {
					pBitmapSDL->DeleteTexture(pSDLRenderer);
				}
			}
		}
	}

	BOOL CPaintManagerSDLUI::InvalidateRect(UIWND hWnd, const CDuiRect*lpRect, BOOL bErase)
	{
		if (m_pWindow)
		{
			if (lpRect && lpRect->left == 0 && lpRect->top == 0 && lpRect->right == 0 && lpRect->bottom == 0)
				return FALSE;

			UIRender_Sdl* pRender = (UIRender_Sdl*)Render();
			pRender->InvalidRect(lpRect);

			Uint64 flags = SDL_GetWindowFlags(m_hWndPaint);
			if (!(flags & SDL_WINDOW_MINIMIZED) && !(flags & SDL_WINDOW_HIDDEN)) 
			{
				SDL_Event ev;
				SDL_zero(ev);
				ev.type = SDL_EVENT_USER;
				ev.window.windowID = SDL_GetWindowID(GetPaintWindow());
				ev.user.code = WM_PAINT;
				ev.user.data1 = NULL;
				ev.user.data2 = NULL;
				SDL_PushEvent(&ev);
			}	
		}
		return TRUE;
	}

	void CPaintManagerSDLUI::SetCursor(int nCursor)
	{
		if (m_pWindow)
		{
			m_pWindow->SetCursor(nCursor);
		}
	}

	void CPaintManagerSDLUI::SetCapture()
	{
		m_bMouseCapture = true;
		SDL_CaptureMouse(true);
	}

	void CPaintManagerSDLUI::ReleaseCapture()
	{
		m_bMouseCapture = false;
		SDL_CaptureMouse(false);
	}

	bool CPaintManagerSDLUI::IsCaptured()
	{
		return m_bMouseCapture;
	}

	BOOL CPaintManagerSDLUI::SetTimer(UINT uElapse, TIMERINFO* pTimer)
	{
		pTimer->uWinTimer = SDL_AddTimer(uElapse, Callback_SDL_Timer, pTimer);
		return pTimer->uWinTimer != 0;
	}

	BOOL CPaintManagerSDLUI::KillTimer(TIMERINFO* pTimer)
	{
		if (pTimer == NULL || pTimer->uWinTimer == 0) return FALSE;
		if (SDL_RemoveTimer(pTimer->uWinTimer))
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL CPaintManagerSDLUI::GetCursorPos(LPPOINT pt)
	{
		float x, y;
// 		SDL_GetMouseState(&x, &y);
		SDL_GetGlobalMouseState(&x, &y);
		pt->x = (int)x;
		pt->y = (int)y;
		return TRUE;
	}

	BOOL CPaintManagerSDLUI::ScreenToClient(LPPOINT pt)
	{
		int wx, wy;
		SDL_GetWindowPosition(m_hWndPaint, &wx, &wy);
		pt->x -= wx;
		pt->y -= wy;
		return TRUE;
	}

	void CPaintManagerSDLUI::SetWndFocus()
	{
		SDL_RaiseWindow(m_hWndPaint);
	}

	UIWND CPaintManagerSDLUI::GetWndFocus()
	{
		return NULL;
	}

	BOOL CPaintManagerSDLUI::IsZoomed()
	{
		Uint64 flags = SDL_GetWindowFlags(m_hWndPaint);
		return (flags & SDL_WINDOW_MAXIMIZED) == SDL_WINDOW_MAXIMIZED;
	}

	BOOL CPaintManagerSDLUI::IsIconic()
	{
		Uint64 flags = SDL_GetWindowFlags(m_hWndPaint);
		return (flags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED;
	}

	BOOL CPaintManagerSDLUI::SetWindowPos(int x, int y, int cx, int cy, UINT uFlags)
	{
		if (!IsWindow()) return FALSE;
		return m_pWindow->SetWindowPos(x, y, cx, cy, uFlags);
	}

	BOOL CPaintManagerSDLUI::GetWindowRect(LPRECT lpRect)
	{
		if (!IsWindow()) return FALSE;
		return m_pWindow->GetWindowRect(lpRect);
	}

	BOOL CPaintManagerSDLUI::GetClientRect(LPRECT lpRect)
	{
		if (!IsWindow()) return FALSE;
		return m_pWindow->GetClientRect(lpRect);
	}

	BOOL CPaintManagerSDLUI::IsKeyDown(UINT uKey)
	{
		auto it = DuiLibWindowWnd::m_keyWin32ToSdl.find(uKey);
		if (it == DuiLibWindowWnd::m_keyWin32ToSdl.end())
			return FALSE;
		const bool* state = SDL_GetKeyboardState(NULL);
		SDL_Scancode scancode = SDL_GetScancodeFromKey((SDL_Keycode)it->second, NULL);
		return state[scancode];
	}

	BOOL CPaintManagerSDLUI::IsKeyUp(UINT uKey)
	{
		return !IsKeyDown(uKey);
	}

	BOOL CPaintManagerSDLUI::IsCtrlKeyDown()		
	{ 
		const bool* state = SDL_GetKeyboardState(NULL);
		return (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]);
	}

	BOOL CPaintManagerSDLUI::IsAltKeyDown()			
	{ 
		const bool* state = SDL_GetKeyboardState(NULL);
		return (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]);
	}

	BOOL CPaintManagerSDLUI::IsShiftKeyDown()			
	{ 
		const bool* state = SDL_GetKeyboardState(NULL);
		return (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]);
	}

	BOOL CPaintManagerSDLUI::IsCapsLockKeyOn()		
	{ 
		//const bool* state = SDL_GetKeyboardState(NULL);
		//return state[SDL_SCANCODE_CAPSLOCK];
		//上面代码只是获取当前的按下状态,而不是锁状态
		//跨平台方案暂不实现
		return FALSE;
	}

	BOOL CPaintManagerSDLUI::IsNumberLockKeyOn()	
	{ 
		//const bool* state = SDL_GetKeyboardState(NULL);
		//return state[SDL_SCANCODE_NUMLOCKCLEAR];
		//上面代码只是获取当前的按下状态,而不是锁状态
		//跨平台方案暂不实现
		return FALSE;
	}

	UINT CPaintManagerSDLUI::MapKeyState()
	{
		UINT uState = 0;
		const bool* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL])
			uState |= MK_CONTROL;
		if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT])
			uState |= MK_SHIFT;
		if (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT])
			uState |= MK_ALT;
		Uint32 mouse = SDL_GetMouseState(NULL, NULL);
		if (mouse & SDL_BUTTON_LMASK)
			uState |= MK_LBUTTON;
		if (mouse & SDL_BUTTON_RMASK)
			uState |= MK_RBUTTON;
		if (mouse & SDL_BUTTON_MMASK)
			uState |= MK_MBUTTON;
		return uState;
	}

	DWORD CPaintManagerSDLUI::GetTickCount()
	{
		return SDL_GetTicks();
	}

	void CPaintManagerSDLUI::GetLocalTime(SYSTEMTIME& st)
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		st.wYear = timeinfo->tm_year + 1900;
		st.wMonth = timeinfo->tm_mon + 1;
		st.wDayOfWeek = timeinfo->tm_wday;
		st.wDay = timeinfo->tm_mday;
		st.wHour = timeinfo->tm_hour;
		st.wMinute = timeinfo->tm_min;
		st.wSecond = timeinfo->tm_sec;
		st.wMilliseconds = 0;
	}

	void CPaintManagerSDLUI::MessageLoop()
	{
		SDL_Event ev;
		MSG msg = { 0 };
		while (CWindowSDL::GetMessage(&ev, &msg))
		{
			if (!CWindowSDL::TranslateMessage(&ev, &msg))
				break;

			if (!CPaintManagerUI::TranslateMessage(&msg))
				CWindowSDL::DispatchMessage(&ev, &msg);
		}
	}

	bool CPaintManagerSDLUI::OnClose(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		TEventUI event = { 0 };
		event.ptMouse = m_ptLastMousePos;
		event.wKeyState = MapKeyState();
		event.dwTimestamp = GetTickCount();
		if (m_pEventHover != NULL) {
			event.Type = UIEVENT_MOUSELEAVE;
			event.pSender = m_pEventHover;
			m_pEventHover->Event(event);
		}
		if (m_pEventClick != NULL) {
			event.Type = UIEVENT_BUTTONUP;
			event.pSender = m_pEventClick;
			m_pEventClick->Event(event);
		}

		SDL_Event ev;
		SDL_zero(ev);
		ev.type = SDL_EVENT_USER;
		ev.window.windowID = SDL_GetWindowID(GetPaintWindow());
		ev.user.code = WM_DESTROY;
		ev.user.data1 = NULL;
		ev.user.data2 = NULL;
		SDL_PushEvent(&ev);
		return false;
	}

	bool CPaintManagerSDLUI::OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		if (m_pRoot == NULL)
		{
			return true;
		}

		CDuiRect rcClient;
		GetClientRect(&rcClient);
		if (rcClient.IsEmpty())
			return true;

		UIRender_Sdl* pRender = (UIRender_Sdl*)Render();
		CDuiRect rcPaint = pRender->GetInvalidRect();
		if (rcPaint.IsEmpty())
			rcPaint = rcClient;

		bool bNeedSizeMsg = false;

		SetPainting(true);
		if (m_bUpdateNeeded && !m_bLockUpdate)
		{
			m_bUpdateNeeded = false;
			if (m_pRoot->IsUpdateNeeded())
			{
				CDuiRect rcRoot = rcClient;
				m_pRoot->SetPos(rcRoot, true);
				bNeedSizeMsg = true;
			}
			else
			{
				//单独NeedUpdate某个控件时
				CControlUI* pControl = NULL;
				m_aFoundControls.Empty();
				m_pRoot->FindControl(__FindControlsFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST | UIFIND_UPDATETEST);
				for (int it = 0; it < m_aFoundControls.GetSize(); it++) {
					pControl = static_cast<CControlUI*>(m_aFoundControls[it]);
					//float控件不需要重新计算RelativePos，因为第一次SetPos时，已经把偏移计算好了，并且保存到m_rcItem。
					//if( !pControl->IsFloat() ) pControl->SetPos(pControl->GetPos(), true);
					//else pControl->SetPos(pControl->GetRelativePos(), true);
					pControl->SetPos(pControl->GetPos(), true);
				}
				bNeedSizeMsg = true;
			}

			//第一次绘制窗口之前，发送DUI_MSGTYPE_WINDOWINIT消息
			if (m_bFirstLayout)
			{
				m_bFirstLayout = false;
				SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWINIT, 0, 0, false);
			}
		}

		Render()->BeginPaint();
		Render()->Resize(rcClient);
 		//Render()->ClearAlpha(rcPaint);
		GetRoot()->Paint(Render(), rcPaint, NULL);

		for (int i = 0; i < m_aPostPaintControls.GetSize(); i++) {
			CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
			pPostPaintControl->DoPostPaint(Render(), rcPaint);
		}

 		Render()->EndPaint();

		SetPainting(false);
		if (m_bUpdateNeeded) Invalidate();

		// 发送窗口大小改变消息
		if (bNeedSizeMsg) {
			this->SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWSIZE, 0, 0, true);
		}
		return true;
	}

	bool CPaintManagerSDLUI::OnGetMaxMinInfo(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

	bool CPaintManagerSDLUI::OnMouseOver(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		m_bMouseTracking = false;
		CDuiPoint pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		CControlUI* pHover = FindControl(pt);
		if (pHover == NULL) return false;
		// Generate mouse hover event
		if (m_pEventHover != NULL) {
			TEventUI event = { 0 };
			event.Type = UIEVENT_MOUSEHOVER;
			event.pSender = m_pEventHover;
			event.wParam = wParam;
			event.lParam = lParam;
			event.dwTimestamp = GetTickCount();
			event.ptMouse = pt;
			event.wKeyState = MapKeyState();
			m_pEventHover->Event(event);
			event.Type = UIEVENT_SETCURSOR;
			m_pEventHover->Event(event);
		}
		return true;
	}

	bool CPaintManagerSDLUI::OnMouseLeave(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		if (m_bMouseTracking) 
		{
			SendMessage(WM_MOUSEMOVE, 0, (LPARAM)-1);
		}
		m_bMouseTracking = false;
		return false;
	}

	bool CPaintManagerSDLUI::OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		// Generate the appropriate mouse messages
		CDuiPoint pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		m_ptLastMousePos = pt;
		CControlUI* pNewHover = FindControl(pt);
		if (pNewHover != NULL && pNewHover->GetManager() != this) return false;

		TEventUI event = { 0 };
		event.ptMouse = pt;
		event.wParam = wParam;
		event.lParam = lParam;
		event.dwTimestamp = GetTickCount();
		event.wKeyState = MapKeyState();
		if (!IsCaptured()) {
			pNewHover = FindControl(pt);
			if (pNewHover != NULL && pNewHover->GetManager() != this) return false;
			if (pNewHover != m_pEventHover && m_pEventHover != NULL) {
				event.Type = UIEVENT_MOUSELEAVE;
				event.pSender = m_pEventHover;

				CStdPtrArray aNeedMouseLeaveNeeded(m_aNeedMouseLeaveNeeded.GetSize());
				aNeedMouseLeaveNeeded.Resize(m_aNeedMouseLeaveNeeded.GetSize());
				memcpy(aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetSize() * sizeof(LPVOID));
				for (int i = 0; i < aNeedMouseLeaveNeeded.GetSize(); i++) {
					static_cast<CControlUI*>(aNeedMouseLeaveNeeded[i])->Event(event);
				}

				m_pEventHover->Event(event);
				m_pEventHover = NULL;
			}
			if (pNewHover != m_pEventHover && pNewHover != NULL) 
			{
				event.Type = UIEVENT_MOUSEENTER;
				event.pSender = pNewHover;
				pNewHover->Event(event);
				m_pEventHover = pNewHover;
			}
		}
		if (m_pEventClick != NULL) 
		{
			event.Type = UIEVENT_MOUSEMOVE;
			event.pSender = m_pEventClick;
			m_pEventClick->Event(event);
		}
		else if (pNewHover != NULL) 
		{
			event.Type = UIEVENT_MOUSEMOVE;
			event.pSender = pNewHover;
			pNewHover->Event(event);

			event.Type = UIEVENT_SETCURSOR;
			pNewHover->Event(event);
		}

		return false;
	}

	bool CPaintManagerSDLUI::OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		return false;
	}

} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

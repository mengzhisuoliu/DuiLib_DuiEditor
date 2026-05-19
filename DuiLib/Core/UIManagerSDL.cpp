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
		ev.window.windowID = SDL_GetWindowID((SDL_Window*)pTimer->hWnd);
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

#ifdef _DEBUG
	CMacroToStringMap CPaintManagerSDLUI::m_sdlEventString;
#endif
	std::map<UINT, WORD> CPaintManagerSDLUI::m_keySdlToWin32;
	std::map<WORD, UINT> CPaintManagerSDLUI::m_keyWin32ToSdl;

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

	BOOL CPaintManagerSDLUI::InvalidateRect(UIWND hWnd, const RECT *lpRect, BOOL bErase)
	{
		if (m_pWindow)
		{
			UIRender_Sdl* pRender = (UIRender_Sdl*)Render();
			pRender->InvalidRect(lpRect);

			SDL_Event event;
			event.type = SDL_EVENT_WINDOW_EXPOSED;
			event.window.windowID = SDL_GetWindowID((SDL_Window*)m_hWndPaint);
			SDL_PushEvent(&event);
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
		SDL_RemoveTimer(pTimer->uWinTimer);
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
		SDL_GetWindowPosition((SDL_Window *)m_pWindow, &wx, &wy);
		pt->x -= wx;
		pt->y -= wy;
		return TRUE;
	}

	void CPaintManagerSDLUI::SetWndFocus()
	{
		SDL_RaiseWindow((SDL_Window *)m_hWndPaint);
	}

	UIWND CPaintManagerSDLUI::GetWndFocus()
	{
		return NULL;
	}

	BOOL CPaintManagerSDLUI::IsZoomed()
	{
		Uint64 flags = SDL_GetWindowFlags((SDL_Window *)m_hWndPaint);
		return (flags & SDL_WINDOW_MAXIMIZED) == SDL_WINDOW_MAXIMIZED;
	}

	BOOL CPaintManagerSDLUI::IsIconic()
	{
		Uint64 flags = SDL_GetWindowFlags((SDL_Window*)m_hWndPaint);
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
		auto it = m_keyWin32ToSdl.find(uKey);
		if (it == m_keyWin32ToSdl.end())
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
		const bool* state = SDL_GetKeyboardState(NULL);
		return state[SDL_SCANCODE_CAPSLOCK];
	}

	BOOL CPaintManagerSDLUI::IsNUmberLockKeyOn()	
	{ 
		const bool* state = SDL_GetKeyboardState(NULL);
		return state[SDL_SCANCODE_NUMLOCKCLEAR];
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

	//翻译兼容windows的消息
	static void TranslateSdlMessage(SDL_Event &ev, UINT &uMsg, WPARAM &wparam, LPARAM &lparam)
	{
		switch (ev.type)
		{
			case SDL_EVENT_CLIPBOARD_UPDATE://	SDL_ClipboardEvent	clipboard
				break;
			case SDL_EVENT_DROP_BEGIN:
			case SDL_EVENT_DROP_FILE: 
			case SDL_EVENT_DROP_TEXT: //, etc	SDL_DropEvent	drop
				break;
			case SDL_EVENT_KEY_DOWN:
				{
					uMsg = WM_KEYDOWN;
					auto it = CPaintManagerSDLUI::m_keySdlToWin32.find(ev.key.key);
					if(it != CPaintManagerSDLUI::m_keySdlToWin32.end())
						wparam = it->second;
					break;
				}
			case SDL_EVENT_KEY_UP: //	SDL_KeyboardEvent	key
				{
					uMsg = WM_KEYUP;
					auto it = CPaintManagerSDLUI::m_keySdlToWin32.find(ev.key.key);
					if (it != CPaintManagerSDLUI::m_keySdlToWin32.end())
						wparam = it->second;
					break;
				}
			case SDL_EVENT_MOUSE_MOTION: //	SDL_MouseMotionEvent	motion
				{
					uMsg = WM_MOUSEMOVE;
					lparam = MAKELPARAM(ev.button.x, ev.button.y);
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				{
					if(ev.button.button == SDL_BUTTON_LEFT)
						uMsg = WM_LBUTTONDOWN;
					if (ev.button.button == SDL_BUTTON_RIGHT)
						uMsg = WM_RBUTTONDOWN;
					if (ev.button.button == SDL_BUTTON_MIDDLE)
						uMsg = WM_MBUTTONDOWN;
					lparam = MAKELPARAM(ev.button.x, ev.button.y);
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP: //	SDL_MouseButtonEvent	button
				{
					if (ev.button.button == SDL_BUTTON_LEFT)
						uMsg = WM_LBUTTONUP;
					if (ev.button.button == SDL_BUTTON_RIGHT)
						uMsg = WM_RBUTTONUP;
					if (ev.button.button == SDL_BUTTON_MIDDLE)
						uMsg = WM_MBUTTONUP;
					lparam = MAKELPARAM(ev.button.x, ev.button.y);
				}
				break;
			case SDL_EVENT_MOUSE_WHEEL: //	SDL_MouseWheelEvent	wheel
				{
					uMsg = WM_MOUSEWHEEL;
					lparam = MAKELPARAM(ev.button.x, ev.button.y);
				}
				break;
			case SDL_EVENT_QUIT: //	SDL_QuitEvent	quit
				{
					uMsg = WM_CLOSE;
				}
				break;
			case SDL_EVENT_WINDOW_RESIZED: 
			case SDL_EVENT_WINDOW_MOVED: //, etc	SDL_WindowEvent	window
				{
					uMsg = WM_SIZE; 
				}
				break;
			case SDL_EVENT_WINDOW_MOUSE_ENTER: //,       /**< Window has gained mouse focus */
				{
					uMsg = WM_MOUSEHOVER;
					lparam = MAKELPARAM(ev.button.x, ev.button.y);
				}
				break;
			case SDL_EVENT_WINDOW_MOUSE_LEAVE: //,       /**< Window has lost mouse focus */
				{
					uMsg = WM_MOUSELEAVE;
					lparam = MAKELPARAM(ev.button.x, ev.button.y);
				}
				break;
			case SDL_EVENT_WINDOW_EXPOSED:
				{
					uMsg = WM_PAINT; 
				}
				break;
			case SDL_EVENT_WINDOW_FOCUS_LOST:
				{
					uMsg = WM_KILLFOCUS;
				}
				break;
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
				{
					uMsg = WM_SETFOCUS;
				}
				break;
// 			case SDL_EVENT_WINDOW_DESTROYED:
// 				{
// 				}
// 				break;
			case SDL_EVENT_USER: //	SDL_UserEvent	user
				{
					if (ev.user.code == WM_TIMER)
					{
						TIMERINFO* pTimer = (TIMERINFO*)ev.user.data1;
						uMsg = ev.user.code; 
						wparam = MAKEWPARAM(pTimer->uWinTimer, 0);
					}
					else if (ev.user.code == WM_DESTROY)
					{
						uMsg = ev.user.code;
					}
				}
			default: // Other events	SDL_CommonEvent	common
				{
					
				}
				break;
		}
	}

	//模拟WM_CHAR消息
	static bool TranslateSdlCharMessage(SDL_Event& ev, UINT& uMsg, WPARAM& wparam, LPARAM& lparam)
	{
		uMsg = WM_CHAR;
		TCHAR ret = 0;
		// 字母键：根据 Shift 和 CapsLock 决定大小写
		if (ev.key.key >= SDLK_A && ev.key.key <= SDLK_Z) 
		{
			TCHAR base = _T('a') + (ev.key.key - SDLK_A);
			bool upper = (ev.key.mod & SDL_KMOD_SHIFT) != 0;
			bool caps = (ev.key.mod & SDL_KMOD_CAPS) != 0;
			TCHAR ret = base;
			if (upper && caps) //输出小写
				ret = base;
			else if (upper && !caps) //输出大写
				ret = base - 32;
			else if (!upper && caps) //输出大写
				ret = base - 32;
			else if (!upper && !caps) //输出小写
				ret = base;
			else
				ret = base;
			wparam = ret;
			return true;
		}

		//主键盘的数字键
		static const std::map<SDL_Keycode, TCHAR> digitShiftMap = 
		{
			{SDLK_1, _T('!')}, {SDLK_2, _T('@')}, {SDLK_3, _T('#')}, {SDLK_4, _T('$')},
			{SDLK_5, _T('%')}, {SDLK_6, _T('^')}, {SDLK_7, _T('&')}, {SDLK_8, _T('*')},
			{SDLK_9, _T('(')}, {SDLK_0, _T(')')}
		};
		if (ev.key.key >= SDLK_0 && ev.key.key <= SDLK_9)
		{
			if (ev.key.mod & SDL_KMOD_SHIFT)
			{
				auto it = digitShiftMap.find(ev.key.key);
				if (it != digitShiftMap.end())
				{
					wparam = it->second; 
					return true;
				}
			}
			wparam = _T('0') + (ev.key.key - SDLK_0);
			return true;
		}

		// 标点符号键（主键盘区，Shift 产生不同字符）
		static const std::map<SDL_Keycode, std::pair<TCHAR, TCHAR>> punctMap = 
		{
			{SDLK_SPACE,		{ _T(' '),  _T(' ')}},
			{SDLK_MINUS,		{ _T('-'),  _T('_')}},
			{SDLK_EQUALS,		{ _T('='),  _T('+')}},
			{SDLK_LEFTBRACKET,	{ _T('['),  _T('{')}},
			{SDLK_RIGHTBRACKET,	{ _T(']'),  _T('}')}},
			{SDLK_BACKSLASH,	{ _T('\\'),  _T('|')}},
			{SDLK_SEMICOLON,	{ _T(';'),  _T(':')}},
			{SDLK_APOSTROPHE,   { _T('\''),  _T('"')}},
			{SDLK_COMMA,		{ _T(','),  _T('<')}},
			{SDLK_PERIOD,		{ _T('.'),  _T('>')}},
			{SDLK_SLASH,		{ _T('/'),  _T('?')}},
			{SDLK_GRAVE,		{ _T('`'),  _T('~')}}
		};
		auto it = punctMap.find(ev.key.key);
		if (it != punctMap.end()) 
		{
			if (ev.key.mod & SDL_KMOD_SHIFT)
				ret = it->second.second;
			else
				ret = it->second.first;
			wparam = ret;
			return true;
		}
		
		//////////////////////////////////////////////////////////////////////////
		// 数字键盘区域
		if (ev.key.key >= SDLK_KP_0 && ev.key.key <= SDLK_KP_9)
		{
			if (ev.key.mod & SDL_KMOD_NUM) //NumLock键已经按下，输出数字字符
			{
				ret = _T('0') + (ev.key.key - SDLK_KP_0);
				wparam = ret;
				return true;
			}
			return false;
		}
		// 小数点键（KP_PERIOD
		if (ev.key.key == SDLK_KP_PERIOD)
		{
			if (ev.key.mod & SDL_KMOD_NUM)
			{
				wparam = _T('.');
				return true;
			}
			return false;
		}
		// 小键盘其他键值
		if (ev.key.key == SDLK_KP_DIVIDE)	{ wparam = _T('/'); return true; }
		if (ev.key.key == SDLK_KP_MULTIPLY) { wparam = _T('*'); return true; }
		if (ev.key.key == SDLK_KP_MINUS)	{ wparam = _T('-'); return true; }
		if (ev.key.key == SDLK_KP_PLUS)		{ wparam = _T('+'); return true; }
		if (ev.key.key == SDLK_KP_ENTER)	{ wparam = _T('\r'); return true; }


		return false;
	}

	void CPaintManagerSDLUI::MessageLoop()
	{
		_init_sdl_defined();

		SDL_Event ev;
		while (SDL_WaitEvent(&ev))
		{
		#if defined _DEBUG //&& 0
			static UINT lastWindowID = 0;
			static UINT lastEventType = 0;
			if (lastWindowID != ev.window.windowID || lastEventType != ev.type)
			{
				CDuiString tmp;

				if (ev.type == CWindowSDL::m_EVENT_SEND_MESSAGE)
				{
					CWindowSDL::SdlSendMessage* pMsg = (CWindowSDL::SdlSendMessage*)ev.user.data1;
					tmp.Format(_T("winowID=%d, SDL_Event: %s, code=%s, wparam=%p, lparam=%p"),
						ev.window.windowID, m_sdlEventString.Lookup(ev.type).toString(),
						m_wmEventString.Lookup(pMsg->uMsg).toString(), pMsg->wParam, pMsg->lParam);
				}
				else if (ev.type == SDL_EVENT_USER)
				{
					if (ev.user.code != WM_TIMER)
					{
						tmp.Format(_T("winowID=%d, SDL_Event: %s, code=%s, wparam=%p, lparam=%p"),
							ev.window.windowID, m_sdlEventString.Lookup(ev.type).toString(),
							m_wmEventString.Lookup(ev.user.code).toString(), ev.user.data1, ev.user.data2);
					}
				}
				else if(ev.type != SDL_EVENT_WINDOW_EXPOSED)
				{
					tmp.Format(_T("winowID=%d, SDL_Event: %s"), 
						ev.window.windowID, m_sdlEventString.Lookup(ev.type).toString());
				}

				if (!tmp.IsEmpty())
				{
					lastWindowID = ev.window.windowID;
					lastEventType = ev.type;
					DUITRACE(tmp.toString());
				}
			}
		#endif

			if (ev.type == SDL_EVENT_WINDOW_DESTROYED)
			{
				CWindowSDL::WindowInfo info;
				if (!CWindowSDL::FindWindowInfo(ev.window.windowID, &info))
				{
					assert(false);
					break;
				}
				CWindowSDL::UnRegisterWindow(ev.window.windowID);
				info.pDuiWindow->OnFinalMessage((UIWND)info.sdlWindow);
				if (info.isMainWindow)
					break; //是主窗口，退出消息循环
				continue;
			}

			UINT uMsg = 0;
			WPARAM wparam = 0;
			LPARAM lparam = 0;

			if (ev.type == CWindowSDL::m_EVENT_SEND_MESSAGE)
			{
				CWindowSDL::SdlSendMessage* pMsg = (CWindowSDL::SdlSendMessage*)ev.user.data1;
				uMsg = pMsg->uMsg;
				wparam = pMsg->wParam;
				lparam = pMsg->lParam;
			}
			else
			{
				TranslateSdlMessage(ev, uMsg, wparam, lparam);
			}

			SDL_Window* win = SDL_GetWindowFromID(ev.window.windowID);
			if (!win) continue;

			MSG msg = { 0 };
// 			#ifdef WIN32
// 			msg.hwnd = (HWND)win;
// 			#else
// 			msg.hwnd = (UIWND)win;
// 			#endif
			msg.hwnd = reinterpret_cast<decltype(msg.hwnd)>(win);
			msg.wParam = wparam;
			msg.lParam = lparam;
			if (TranslateMessage(&msg))
				continue;

			// 查找对应的PaintManager
			for (int i = 0; i < m_aPreMessages.GetSize(); i++)
			{
				CPaintManagerSDLUI* pMgr = static_cast<CPaintManagerSDLUI*>(m_aPreMessages[i]);
				CWindowSDL* pWindow = (CWindowSDL*)pMgr->GetWindow();
				if (pWindow->GetHWND() == (UIWND)win)
				{
					if (pWindow->OnSdlEvent(&ev))
					{
						break;
					}

					//调用窗口过程函数
					LRESULT lRet = pWindow->HandleMessage(uMsg, wparam, lparam);

					if (ev.type == CWindowSDL::m_EVENT_SEND_MESSAGE)
					{
						CWindowSDL::SdlSendMessage* pMsg = (CWindowSDL::SdlSendMessage*)ev.user.data1;
						pMsg->result = lRet;
						// 通知发送线程完成
						SDL_LockMutex((SDL_Mutex*)pMsg->mutex);
						pMsg->processed = true;
						SDL_SignalCondition((SDL_Condition*)pMsg->cond);
						SDL_UnlockMutex((SDL_Mutex*)pMsg->mutex);
					}

					if (uMsg == WM_DESTROY)
					{
						pMgr->ReleaseRender();
						SDL_DestroyWindow((SDL_Window*)pWindow->GetHWND());
					}

					if (uMsg == WM_KEYDOWN)
					{
						UINT uMsgChar = 0;
						WPARAM wparamChar = 0;
						LPARAM lparamChar = 0;
						if (TranslateSdlCharMessage(ev, uMsgChar, wparamChar, lparamChar))
						{
							pWindow->HandleMessage(uMsgChar, wparamChar, lparamChar);
						}
					}
					break;
				}
			}
		}
	return;
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
		ev.window.windowID = SDL_GetWindowID((SDL_Window*)GetPaintWindow());
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
		if (rcClient.GetWidth() <= 0 || rcClient.GetHeight() <= 0)
			return true;

		CDuiRect rcPaint = rcClient;
		bool bNeedSizeMsg = false;

		SetPainting(true);
		if (m_bUpdateNeeded && !m_bLockUpdate)
		{
			m_bUpdateNeeded = false;
			if (m_pRoot->IsUpdateNeeded())
			{
				RECT rcRoot = rcClient;
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
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
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
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
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

#define MAPKEY_WIN32_SDL(sdl, win32)	\
 do {									\
	m_keySdlToWin32[sdl] = win32;		\
	m_keyWin32ToSdl[win32] = sdl;		\
 } while(0)
	void CPaintManagerSDLUI::_init_sdl_defined()
	{
	#ifdef _DEBUG
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_FIRST);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_QUIT);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_TERMINATING);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_LOW_MEMORY);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WILL_ENTER_BACKGROUND);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DID_ENTER_BACKGROUND);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WILL_ENTER_FOREGROUND);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DID_ENTER_FOREGROUND);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_LOCALE_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_SYSTEM_THEME_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DISPLAY_ORIENTATION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DISPLAY_ADDED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DISPLAY_REMOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DISPLAY_MOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DISPLAY_USABLE_BOUNDS_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_SHOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_HIDDEN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_EXPOSED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_MOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_RESIZED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_METAL_VIEW_RESIZED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_MINIMIZED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_MAXIMIZED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_RESTORED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_MOUSE_ENTER);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_MOUSE_LEAVE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_FOCUS_GAINED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_FOCUS_LOST);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_CLOSE_REQUESTED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_HIT_TEST);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_ICCPROF_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_DISPLAY_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_SAFE_AREA_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_OCCLUDED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_ENTER_FULLSCREEN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_LEAVE_FULLSCREEN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_DESTROYED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_WINDOW_HDR_STATE_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_KEY_DOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_KEY_UP);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_TEXT_EDITING);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_TEXT_INPUT);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_KEYMAP_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_KEYBOARD_ADDED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_KEYBOARD_REMOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_TEXT_EDITING_CANDIDATES);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_SCREEN_KEYBOARD_SHOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_SCREEN_KEYBOARD_HIDDEN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_MOUSE_MOTION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_MOUSE_BUTTON_DOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_MOUSE_BUTTON_UP);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_MOUSE_WHEEL);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_MOUSE_ADDED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_MOUSE_REMOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_AXIS_MOTION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_BALL_MOTION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_HAT_MOTION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_BUTTON_DOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_BUTTON_UP);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_ADDED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_REMOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_BATTERY_UPDATED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_JOYSTICK_UPDATE_COMPLETE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_AXIS_MOTION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_BUTTON_DOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_BUTTON_UP);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_ADDED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_REMOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_REMAPPED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_TOUCHPAD_UP);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_SENSOR_UPDATE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_UPDATE_COMPLETE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_FINGER_DOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_FINGER_UP);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_FINGER_MOTION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_FINGER_CANCELED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PINCH_BEGIN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PINCH_UPDATE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PINCH_END);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_CLIPBOARD_UPDATE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DROP_FILE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DROP_TEXT);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DROP_BEGIN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DROP_COMPLETE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_DROP_POSITION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_AUDIO_DEVICE_ADDED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_AUDIO_DEVICE_REMOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_SENSOR_UPDATE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PEN_PROXIMITY_IN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PEN_PROXIMITY_OUT);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PEN_DOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PEN_UP);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PEN_BUTTON_DOWN);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PEN_BUTTON_UP);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PEN_MOTION);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PEN_AXIS);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_CAMERA_DEVICE_ADDED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_CAMERA_DEVICE_REMOVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_CAMERA_DEVICE_APPROVED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_CAMERA_DEVICE_DENIED);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_RENDER_TARGETS_RESET);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_RENDER_DEVICE_RESET);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_RENDER_DEVICE_LOST);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PRIVATE0);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PRIVATE1);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PRIVATE2);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_PRIVATE3);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_POLL_SENTINEL);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_USER);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_LAST);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, SDL_EVENT_ENUM_PADDING);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, CWindowSDL::m_EVENT_SEND_MESSAGE);
		MACROTOSTRINGMAP_ADD(m_sdlEventString, CWindowSDL::m_EVENT_POST_MESSAGE);
	#endif

		// 字母键
		MAPKEY_WIN32_SDL(SDLK_A, 'A');
		MAPKEY_WIN32_SDL(SDLK_B, 'B');
		MAPKEY_WIN32_SDL(SDLK_C, 'C');
		MAPKEY_WIN32_SDL(SDLK_D, 'D');
		MAPKEY_WIN32_SDL(SDLK_E, 'E');
		MAPKEY_WIN32_SDL(SDLK_F, 'F');
		MAPKEY_WIN32_SDL(SDLK_G, 'G');
		MAPKEY_WIN32_SDL(SDLK_H, 'H');
		MAPKEY_WIN32_SDL(SDLK_I, 'I');
		MAPKEY_WIN32_SDL(SDLK_J, 'J');
		MAPKEY_WIN32_SDL(SDLK_K, 'K');
		MAPKEY_WIN32_SDL(SDLK_L, 'L');
		MAPKEY_WIN32_SDL(SDLK_M, 'M');
		MAPKEY_WIN32_SDL(SDLK_N, 'N');
		MAPKEY_WIN32_SDL(SDLK_O, 'O');
		MAPKEY_WIN32_SDL(SDLK_P, 'P');
		MAPKEY_WIN32_SDL(SDLK_Q, 'Q');
		MAPKEY_WIN32_SDL(SDLK_R, 'R');
		MAPKEY_WIN32_SDL(SDLK_S, 'S');
		MAPKEY_WIN32_SDL(SDLK_T, 'T');
		MAPKEY_WIN32_SDL(SDLK_U, 'U');
		MAPKEY_WIN32_SDL(SDLK_V, 'V');
		MAPKEY_WIN32_SDL(SDLK_W, 'W');
		MAPKEY_WIN32_SDL(SDLK_X, 'X');
		MAPKEY_WIN32_SDL(SDLK_Y, 'Y');
		MAPKEY_WIN32_SDL(SDLK_Z, 'Z');

		// 数字键
		MAPKEY_WIN32_SDL(SDLK_0, '0');
		MAPKEY_WIN32_SDL(SDLK_1, '1');
		MAPKEY_WIN32_SDL(SDLK_2, '2');
		MAPKEY_WIN32_SDL(SDLK_3, '3');
		MAPKEY_WIN32_SDL(SDLK_4, '4');
		MAPKEY_WIN32_SDL(SDLK_5, '5');
		MAPKEY_WIN32_SDL(SDLK_6, '6');
		MAPKEY_WIN32_SDL(SDLK_7, '7');
		MAPKEY_WIN32_SDL(SDLK_8, '8');
		MAPKEY_WIN32_SDL(SDLK_9, '9');

		// 功能键 F1 ~ F12
		MAPKEY_WIN32_SDL(SDLK_F1, VK_F1);
		MAPKEY_WIN32_SDL(SDLK_F2, VK_F2);
		MAPKEY_WIN32_SDL(SDLK_F3, VK_F3);
		MAPKEY_WIN32_SDL(SDLK_F4, VK_F4);
		MAPKEY_WIN32_SDL(SDLK_F5, VK_F5);
		MAPKEY_WIN32_SDL(SDLK_F6, VK_F6);
		MAPKEY_WIN32_SDL(SDLK_F7, VK_F7);
		MAPKEY_WIN32_SDL(SDLK_F8, VK_F8);
		MAPKEY_WIN32_SDL(SDLK_F9, VK_F9);
		MAPKEY_WIN32_SDL(SDLK_F10, VK_F10);
		MAPKEY_WIN32_SDL(SDLK_F11, VK_F11);
		MAPKEY_WIN32_SDL(SDLK_F12, VK_F12);

		// 方向与编辑键
		MAPKEY_WIN32_SDL(SDLK_UP, VK_UP);
		MAPKEY_WIN32_SDL(SDLK_DOWN, VK_DOWN);
		MAPKEY_WIN32_SDL(SDLK_LEFT, VK_LEFT);
		MAPKEY_WIN32_SDL(SDLK_RIGHT, VK_RIGHT);
		MAPKEY_WIN32_SDL(SDLK_HOME, VK_HOME);
		MAPKEY_WIN32_SDL(SDLK_END, VK_END);
		MAPKEY_WIN32_SDL(SDLK_PAGEUP, VK_PRIOR);
		MAPKEY_WIN32_SDL(SDLK_PAGEDOWN, VK_NEXT);
		MAPKEY_WIN32_SDL(SDLK_INSERT, VK_INSERT);
		MAPKEY_WIN32_SDL(SDLK_DELETE, VK_DELETE);

		// 控制键（左右 Ctrl/Alt/Shift/Win）
		MAPKEY_WIN32_SDL(SDLK_LCTRL, VK_LCONTROL);
		MAPKEY_WIN32_SDL(SDLK_RCTRL, VK_RCONTROL);
		MAPKEY_WIN32_SDL(SDLK_LALT, VK_LMENU);
		MAPKEY_WIN32_SDL(SDLK_RALT, VK_RMENU);
		MAPKEY_WIN32_SDL(SDLK_LSHIFT, VK_LSHIFT);
		MAPKEY_WIN32_SDL(SDLK_RSHIFT, VK_RSHIFT);
		MAPKEY_WIN32_SDL(SDLK_LGUI, VK_LWIN);
		MAPKEY_WIN32_SDL(SDLK_RGUI, VK_RWIN);

		// 常用控制键（空格、回车、ESC、Tab、退格）
		MAPKEY_WIN32_SDL(SDLK_SPACE, VK_SPACE);
		MAPKEY_WIN32_SDL(SDLK_RETURN, VK_RETURN);
		MAPKEY_WIN32_SDL(SDLK_ESCAPE, VK_ESCAPE);
		MAPKEY_WIN32_SDL(SDLK_TAB, VK_TAB);
		MAPKEY_WIN32_SDL(SDLK_BACKSPACE, VK_BACK);

		// 标点符号 (Windows 虚拟键码使用 OEM 宏)
		MAPKEY_WIN32_SDL(SDLK_COMMA, VK_OEM_COMMA);        // ,
		MAPKEY_WIN32_SDL(SDLK_PERIOD, VK_OEM_PERIOD);      // .
		MAPKEY_WIN32_SDL(SDLK_MINUS, VK_OEM_MINUS);        // -
		MAPKEY_WIN32_SDL(SDLK_EQUALS, VK_OEM_PLUS);        // =
		MAPKEY_WIN32_SDL(SDLK_SEMICOLON, VK_OEM_1);        // ;
		MAPKEY_WIN32_SDL(SDLK_SLASH, VK_OEM_2);            // /
		MAPKEY_WIN32_SDL(SDLK_GRAVE, VK_OEM_3);            // `
		MAPKEY_WIN32_SDL(SDLK_LEFTBRACKET, VK_OEM_4);      // [
		MAPKEY_WIN32_SDL(SDLK_BACKSLASH, VK_OEM_5);        // \ (反斜线)
		MAPKEY_WIN32_SDL(SDLK_RIGHTBRACKET, VK_OEM_6);     // ]
		MAPKEY_WIN32_SDL(SDLK_APOSTROPHE, VK_OEM_7);       // '

		// 数字键盘区
		MAPKEY_WIN32_SDL(SDLK_KP_0, VK_NUMPAD0);
		MAPKEY_WIN32_SDL(SDLK_KP_1, VK_NUMPAD1);
		MAPKEY_WIN32_SDL(SDLK_KP_2, VK_NUMPAD2);
		MAPKEY_WIN32_SDL(SDLK_KP_3, VK_NUMPAD3);
		MAPKEY_WIN32_SDL(SDLK_KP_4, VK_NUMPAD4);
		MAPKEY_WIN32_SDL(SDLK_KP_5, VK_NUMPAD5);
		MAPKEY_WIN32_SDL(SDLK_KP_6, VK_NUMPAD6);
		MAPKEY_WIN32_SDL(SDLK_KP_7, VK_NUMPAD7);
		MAPKEY_WIN32_SDL(SDLK_KP_8, VK_NUMPAD8);
		MAPKEY_WIN32_SDL(SDLK_KP_9, VK_NUMPAD9);
		MAPKEY_WIN32_SDL(SDLK_KP_PERIOD, VK_DECIMAL);
		MAPKEY_WIN32_SDL(SDLK_KP_DIVIDE, VK_DIVIDE);
		MAPKEY_WIN32_SDL(SDLK_KP_MULTIPLY, VK_MULTIPLY);
		MAPKEY_WIN32_SDL(SDLK_KP_MINUS, VK_SUBTRACT);
		MAPKEY_WIN32_SDL(SDLK_KP_PLUS, VK_ADD);
		MAPKEY_WIN32_SDL(SDLK_KP_ENTER, VK_RETURN);
	}
} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

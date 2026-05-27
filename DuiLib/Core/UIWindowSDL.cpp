#include "StdAfx.h"

#ifdef DUILIB_SDL


namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

UINT CWindowSDL::m_EVENT_SEND_MESSAGE = 0;
UINT CWindowSDL::m_EVENT_POST_MESSAGE = 0;

CMacroToStringMap CWindowSDL::m_sdlEventString;
std::map<UINT, WORD> CWindowSDL::m_keySdlToWin32;
std::map<WORD, UINT> CWindowSDL::m_keyWin32ToSdl;

CWindowSDL::CWindowSDL()
{
	m_id = 0;
	m_uOwnerThread = 0;
	m_lastCursor = NULL;
	m_nCurrentCursor = -1;

	while (m_EVENT_SEND_MESSAGE == 0 || m_EVENT_SEND_MESSAGE == SDL_EVENT_USER)
	{
		m_EVENT_SEND_MESSAGE = SDL_RegisterEvents(1);
	}
	while (m_EVENT_POST_MESSAGE == 0 || m_EVENT_POST_MESSAGE == SDL_EVENT_USER)
	{
		m_EVENT_POST_MESSAGE = SDL_RegisterEvents(1);
	}

	if (m_sdlEventString.GetSize() <= 0)
	{
		_init_sdl_defined();
	}
}

CWindowSDL::~CWindowSDL()
{
	if (m_lastCursor)
	{
		SDL_DestroyCursor((SDL_Cursor*)m_lastCursor); m_lastCursor = NULL;
	}
}

UIWND CWindowSDL::CreateDuiWindow(UIWND hwndParent, LPCTSTR pstrWindowName,DWORD dwStyle, DWORD dwExStyle)
{
	return Create(hwndParent,pstrWindowName,dwStyle,dwExStyle,0,0,0,0);
}

UIWND CWindowSDL::Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc)
{
	return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}

UIWND CWindowSDL::Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy)
{
	CDuiStringUtf8 strNameUtf8(pstrName);

	// 创建SDL窗口
	Uint32 flags = SDL_WINDOW_BORDERLESS;// SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS;
	if(dwStyle & WS_SIZEBOX) flags |= SDL_WINDOW_RESIZABLE;

	SDL_Window *pWindow = SDL_CreateWindow(strNameUtf8.toString(), cx, cy, flags);
	if (!pWindow) return NULL;

	if (hwndParent)
	{
		if (dwStyle & WS_POPUP) //弹出窗口
		{
			SDL_SetWindowPosition(pWindow, x, y);
			SDL_SetWindowParent(pWindow, (SDL_Window*)hwndParent);
		}
		else if (dwStyle & WS_CHILD) //内部窗口
		{
			int x1 = 0, y1 = 0;
			SDL_GetWindowPosition((SDL_Window*)hwndParent, &x1, &y1);
			SDL_SetWindowPosition(pWindow, x + x1, y + y1); //内部窗口的坐标是相对于父窗口的偏移
			SDL_SetWindowParent(pWindow, (SDL_Window*)hwndParent);
		}
	}

	m_hWnd = (UIWND)pWindow;
	m_id = SDL_GetWindowID(pWindow);
	m_uOwnerThread = SDL_GetThreadID(NULL);
	RegisterWindow((UINT_PTR)pWindow, this);
	HandleMessage(WM_CREATE, 0, 0);
	return m_hWnd;
}

void CWindowSDL::Close(UINT nRet)
{
	SDL_Event ev;
	SDL_zero(ev);
	ev.type = SDL_EVENT_USER;
	ev.window.windowID = SDL_GetWindowID((SDL_Window*)GetHWND());
	ev.user.code = WM_CLOSE;
	ev.user.data1 = (void *)nRet;
	ev.user.data2 = 0;
	SDL_PushEvent(&ev);
}

void CWindowSDL::SetCursor(int nCursor)
{
	//DUITRACE(_T("CWindowSDL::SetCursor(%d)"), nCursor);

	// 如果光标类型没有变化，直接返回，避免无意义的操作
	if (nCursor == m_nCurrentCursor)
		return;

	SDL_Cursor* cursor = NULL;

	switch (nCursor)
	{
		case DUI_ARROW:           // 32512 默认箭头
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT); break;
		case DUI_IBEAM:           // 32513 文本输入I型
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT); break;
		case DUI_WAIT:            // 32514 等待（沙漏/转圈）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT); break;
		case DUI_CROSS:           // 32515 十字准星
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR); break;
		case DUI_UPARROW:         // 32516 向上箭头（SDL无直接对应，使用默认箭头）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT); break;
		case DUI_SIZE:            // 32640 四向移动（对应SDL四向箭头）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE); break;
		case DUI_ICON:            // 32641 标准箭头（与ARROW相同）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT); break;
		case DUI_SIZENWSE:        // 32642 对角线调整大小（西北-东南）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE); break;
		case DUI_SIZENESW:        // 32643 对角线调整大小（东北-西南）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE); break;
		case DUI_SIZEWE:          // 32644 水平调整大小
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE); break;
		case DUI_SIZENS:          // 32645 垂直调整大小
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE); break;
		case DUI_SIZEALL:         // 32646 四向移动（与SIZE相同）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE); break;
		case DUI_NO:              // 32648 不可用（禁止符号）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED); break;
		case DUI_HAND:            // 32649 手型（可点击）
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER); break;
		default:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT); break;
	}

	if (!cursor)
		return;

	// 销毁之前的光标（如果存在）
	if (m_lastCursor)
		SDL_DestroyCursor((SDL_Cursor *)m_lastCursor);

	// 记录新光标并设置为当前
	m_lastCursor = cursor;
	SDL_SetCursor(cursor);

	// 更新当前光标类型缓存
	m_nCurrentCursor = nCursor;
}

void CWindowSDL::ShowWindow(bool bShow, bool bTakeFocus)
{
	if (bShow)
	{
		SDL_ShowWindow((SDL_Window*)m_hWnd);
		SDL_RaiseWindow((SDL_Window*)m_hWnd);
	}
	else
	{
		SDL_HideWindow((SDL_Window*)m_hWnd);
	}
}

UINT CWindowSDL::ShowModal()
{
	// 获取父窗口（必须在创建时已设置）
	SDL_Window* pParent = SDL_GetWindowParent((SDL_Window*)m_hWnd);
	if (!pParent) {
		// 没有父窗口，则作为普通窗口显示
		ShowWindow(true, true);
		return 0;
	}

	// 保存父窗口 Dui 对象，用于后续恢复焦点
	WindowInfo parentInfo;
	if (!FindWindowInfoByPtr((UINT_PTR)pParent, &parentInfo)) {
		parentInfo.pDuiWindow = nullptr;
	}

	// 设置为模态窗口（SDL3 原生支持）
	if (!SDL_SetWindowModal((SDL_Window*)m_hWnd, true)) 
	{
		// 设置失败，可能平台不支持，但继续使用手动事件过滤
	}

	// 显示窗口并确保获得焦点
	ShowWindow(true, true);

	// 进入模态消息循环
	SDL_Event ev;
	UINT nRet = 0;
	bool bQuit = false;
	while (!bQuit && SDL_WaitEvent(&ev)) {
		// 将 SDL 事件转换为 MSG（复用现有 TranslateMessage）
		MSG msg = { 0 };
		if (!TranslateMessage(&ev, &msg)) {
			continue;
		}

		// 确定消息目标窗口
		SDL_Window* targetWin = SDL_GetWindowFromID(ev.window.windowID);
		if (targetWin != (SDL_Window*)m_hWnd && !IsChildWindow((UIWND)targetWin)) {
			// 消息不属于模态窗口或其子窗口
			// 对于鼠标/键盘事件，直接忽略（模拟模态行为）
			if (msg.message == WM_LBUTTONDOWN || msg.message == WM_LBUTTONUP ||
				msg.message == WM_RBUTTONDOWN || msg.message == WM_RBUTTONUP ||
				msg.message == WM_MBUTTONDOWN || msg.message == WM_MBUTTONUP ||
				msg.message == WM_MOUSEMOVE ||
				msg.message == WM_KEYDOWN || msg.message == WM_KEYUP ||
				msg.message == WM_CHAR) {
				continue;
			}
			// 对于其他消息（如重绘、定时器），仍然分发，保证父窗口刷新
		}

		// 分发消息到窗口过程
		LRESULT lRet = DispatchMessage(&ev, &msg);

		if (msg.message == WM_CLOSE) 
		{
			if (m_id == ev.window.windowID)
			{
				nRet = (UINT)msg.wParam;
				SDL_SetWindowModal((SDL_Window*)m_hWnd, false); // 清除模态状态
			}
		}
		else if (msg.message == WM_QUIT)
		{
			if (m_id == ev.window.windowID)
			{
				bQuit = true;
				break;
			}
		}
	}

	// 恢复父窗口焦点
	if (parentInfo.pDuiWindow) {
		SDL_RaiseWindow(pParent);
	}

	return nRet;
}

void CWindowSDL::CenterWindow()	// 居中，支持扩展屏幕
{
	SDL_SetWindowPosition((SDL_Window *)m_hWnd, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

BOOL CWindowSDL::OnSdlEvent(const void* pEvent)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
std::map<UINT, CWindowSDL::WindowInfo> CWindowSDL::m_smap;
CDuiLock CWindowSDL::m_smap_lock;

BOOL CWindowSDL::RegisterWindow(UINT_PTR pSdlWindow, CWindowWnd* pDuiWindow)
{
	CDuiInnerLock lock(&m_smap_lock);

	if (!pSdlWindow) return FALSE;
	SDL_WindowID id = SDL_GetWindowID((SDL_Window *)pSdlWindow);
	if (id == 0) return FALSE; // 无效ID

	// 如果已经存在，先注销
	auto it = m_smap.find(id);
	if (it != m_smap.end())
	{
		//ASSERT(false);
		m_smap.erase(it);
	}

	WindowInfo info;
	info.windowID = id;
	info.sdlWindow = pSdlWindow;
	info.pDuiWindow = pDuiWindow;

	//认为第一个窗口即为主窗口
	if (m_smap.size() == 0)
	{
		info.isMainWindow = TRUE;
	}
	else
	{
		info.isMainWindow = FALSE;
	}

	DUITRACE(_T("SDL: RegisterWindow SDL_Window=%p, SDL_WindowID=%d"), info.sdlWindow, info.windowID);

	m_smap[id] = info;
	return TRUE;
}

void CWindowSDL::UnRegisterWindow(UINT id)
{
	CDuiInnerLock lock(&m_smap_lock);
	DUITRACE(_T("SDL: UnRegisterWindow SDL_WindowID=%d"), id);
	m_smap.erase(id);
}

BOOL CWindowSDL::FindWindowInfo(UINT id, WindowInfo* pInfo)
{
	CDuiInnerLock lock(&m_smap_lock);

	auto it = m_smap.find(id);
	if (it != m_smap.end()) 
	{
		if (pInfo)
			memcpy(pInfo, &it->second, sizeof(WindowInfo));
		return TRUE;
	}
	return FALSE;
}

BOOL CWindowSDL::FindWindowInfoByPtr(UINT_PTR window, WindowInfo* pInfo)
{
	CDuiInnerLock lock(&m_smap_lock);

	if (!window) return FALSE;
	for (auto& pair : m_smap) 
	{
		if (pair.second.sdlWindow == window) 
		{
			if (pInfo)
				memcpy(pInfo, &pair.second, sizeof(WindowInfo));
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CWindowSDL::FindWindowInfoByPtr(CWindowWnd* pDuiWindow, WindowInfo* pInfo)
{
	CDuiInnerLock lock(&m_smap_lock);

	if (!pDuiWindow) return FALSE;
	for (auto& pair : m_smap)
	{
		if (pair.second.pDuiWindow == pDuiWindow)
		{
			if(pInfo)
				memcpy(pInfo, &pair.second, sizeof(WindowInfo));
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CWindowSDL::IsMainWindow(UINT windowID)
{
	WindowInfo info;
	if (!FindWindowInfo(windowID, &info))
		return FALSE;
	return info.isMainWindow;
}

BOOL CWindowSDL::SetMainWindow(UINT windowID)
{
	CDuiInnerLock lock(&m_smap_lock);

	for (auto& pair : m_smap)
	{
		if (pair.second.windowID == windowID)
		{
			pair.second.isMainWindow = TRUE;
		}
		else
		{
			pair.second.isMainWindow = FALSE;
		}
	}
	return FALSE;
}

BOOL CWindowSDL::IsWindow(UIWND hWnd)
{
	CDuiInnerLock lock(&m_smap_lock);

	return FindWindowInfoByPtr((UINT_PTR)hWnd, NULL);
}

BOOL CWindowSDL::IsChildWindow(UIWND hWnd)
{
	return SDL_GetWindowParent((SDL_Window*)hWnd) != NULL;
}

UIWND CWindowSDL::GetParentWindow(UIWND hWnd)
{
	return (UIWND)SDL_GetWindowParent((SDL_Window*)hWnd);
}

BOOL CWindowSDL::SetForeground(UIWND hWnd)
{
	return SDL_RaiseWindow((SDL_Window*)hWnd);
}

LRESULT CWindowSDL::SendMessage(UIWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//注意这个函数没有加锁
	//加锁的风险是容易死锁, 接收方访问任何m_smap相关内容都会死锁.
	//不加锁的风险是有可能崩溃, 应该避免在SendMessage过程中(发送消息得到返回值是一个过程)销毁窗口.

	WindowInfo info;
	if (!FindWindowInfoByPtr((UINT_PTR)hWnd, &info))
	{
		return -1;
	}

	CWindowSDL* pUiWindow = (CWindowSDL*)info.pDuiWindow;
	if (pUiWindow->m_uOwnerThread == SDL_GetThreadID(NULL))
	{
		//如果到这里窗口已经销毁了, 崩溃
		return info.pDuiWindow->HandleMessage(uMsg, wParam, lParam);
	}

	//跨线程同步消息
	SdlSendMessage payload;
	payload.uMsg = uMsg;
	payload.wParam = wParam;
	payload.lParam = lParam;
	payload.result = 0;
	payload.mutex = SDL_CreateMutex();
	payload.cond = SDL_CreateCondition();
	payload.processed = false;

	// 创建自定义事件
	SDL_Event ev;
	SDL_zero(ev);
	ev.window.windowID = info.windowID;
	ev.type = m_EVENT_SEND_MESSAGE;    // 使用注册的事件类型
	ev.user.data1 = &payload; 

	if (!SDL_PushEvent(&ev)) 
	{
		SDL_DestroyMutex((SDL_Mutex*)payload.mutex);
		SDL_DestroyCondition((SDL_Condition *)payload.cond);
		return -1;
	}

	// 等待处理完成
	SDL_LockMutex((SDL_Mutex*)payload.mutex);
	while (!payload.processed) 
	{
		//如果到这里窗口已经销毁了, 永远等不到应答
		SDL_WaitCondition((SDL_Condition*)payload.cond, (SDL_Mutex*)payload.mutex);
	}
	SDL_UnlockMutex((SDL_Mutex *)payload.mutex);

	SDL_DestroyMutex((SDL_Mutex*)payload.mutex);
	SDL_DestroyCondition((SDL_Condition*)payload.cond);
	return payload.result;
}

BOOL CWindowSDL::PostMessage(UIWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowInfo info;
	if (!FindWindowInfoByPtr((UINT_PTR)hWnd, &info))
		return FALSE;

	SDL_Event ev;
	ev.type = SDL_EVENT_USER;
	ev.window.windowID = info.windowID;
	ev.user.code = uMsg;
	ev.user.data1 = (void*)(uintptr_t)wParam;
	ev.user.data2 = (void*)(uintptr_t)lParam;
	if (SDL_PushEvent(&ev))
		return TRUE;
	
	return FALSE;
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
	if (ev.key.key == SDLK_KP_DIVIDE) { wparam = _T('/'); return true; }
	if (ev.key.key == SDLK_KP_MULTIPLY) { wparam = _T('*'); return true; }
	if (ev.key.key == SDLK_KP_MINUS) { wparam = _T('-'); return true; }
	if (ev.key.key == SDLK_KP_PLUS) { wparam = _T('+'); return true; }
	if (ev.key.key == SDLK_KP_ENTER) { wparam = _T('\r'); return true; }


	return false;
}

static void PrintSdlMessageInfo(SDL_Event& ev)
{
	static UINT lastWindowID = 0;
	static UINT lastEventType = 0;
	if (lastWindowID != ev.window.windowID || lastEventType != ev.type)
	{
		CDuiString tmp;

		if (ev.type == CWindowSDL::m_EVENT_SEND_MESSAGE)
		{
			CWindowSDL::SdlSendMessage* pMsg = (CWindowSDL::SdlSendMessage*)ev.user.data1;
			tmp.Format(_T("winowID=%d, SDL_Event: %s, code=%s, wparam=%p, lparam=%p"),
				ev.window.windowID,
				DuiLibWindowWnd::m_sdlEventString.Lookup(ev.type).toString(),
				DuiLibWindowWnd::m_wmEventString.Lookup(pMsg->uMsg).toString(),
				pMsg->wParam,
				pMsg->lParam);
		}
		else if (ev.type == SDL_EVENT_USER)
		{
			if (ev.user.code != WM_TIMER)
			{
				tmp.Format(_T("winowID=%d, SDL_Event: %s, code=%s, wparam=%p, lparam=%p"),
					ev.window.windowID,
					DuiLibWindowWnd::m_sdlEventString.Lookup(ev.type).toString(),
					DuiLibWindowWnd::m_wmEventString.Lookup(ev.user.code).toString(),
					ev.user.data1,
					ev.user.data2);
			}
		}
		else
		{
			tmp.Format(_T("winowID=%d, SDL_Event: %s"),
				ev.window.windowID,
				DuiLibWindowWnd::m_sdlEventString.Lookup(ev.type).toString());
		}

		if (!tmp.IsEmpty())
		{
			lastWindowID = ev.window.windowID;
			lastEventType = ev.type;
			DUITRACE(tmp.toString());
		}
	}
}

BOOL CWindowSDL::GetMessage(PVOID ev, MSG* msg)
{
	return SDL_WaitEvent((SDL_Event*)ev);
}

BOOL CWindowSDL::TranslateMessage(PVOID ev1, MSG* msg)
{
	SDL_Event &ev = *(SDL_Event*)ev1;

	#if defined _DEBUG //&& 0
	PrintSdlMessageInfo(ev);
	#endif

	SDL_Window* win = SDL_GetWindowFromID(ev.window.windowID);
	if(win) msg->hwnd = reinterpret_cast<decltype(msg->hwnd)>(win);

	if (ev.type == SDL_EVENT_WINDOW_DESTROYED)
	{
		CWindowSDL::WindowInfo info;
		if (!CWindowSDL::FindWindowInfo(ev.window.windowID, &info))
		{
			return FALSE;
		}
		CWindowSDL::UnRegisterWindow(ev.window.windowID);
		info.pDuiWindow->OnFinalMessage((UIWND)info.sdlWindow);
		if (info.isMainWindow)
			return FALSE; //是主窗口，退出消息循环
	}

	if (ev.type == CWindowSDL::m_EVENT_SEND_MESSAGE)
	{
		CWindowSDL::SdlSendMessage* pMsg = (CWindowSDL::SdlSendMessage*)ev.user.data1;
		msg->message = pMsg->uMsg;
		msg->wParam = pMsg->wParam;
		msg->lParam = pMsg->lParam;
	}
	else if (ev.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED)
	{
		CWindowSDL::WindowInfo info;
		if (CWindowSDL::FindWindowInfo(ev.window.windowID, &info))
		{
			msg->message = WM_DPICHANGED;

			float scale = SDL_GetWindowDisplayScale((SDL_Window*)info.sdlWindow);
			int dpi = (int)(96.0f * scale + 0.5f);
			msg->wParam = MAKEWPARAM(dpi, 0);
		}
	}
	else if (ev.type == SDL_EVENT_KEY_DOWN)
	{
		msg->message = WM_KEYDOWN;
		auto it = DuiLibWindowWnd::m_keySdlToWin32.find(ev.key.key);
		if (it != DuiLibWindowWnd::m_keySdlToWin32.end())
			msg->wParam = it->second;
	}
	else if (ev.type == SDL_EVENT_KEY_UP) //	SDL_KeyboardEvent	key
	{
		msg->message = WM_KEYUP;
		auto it = DuiLibWindowWnd::m_keySdlToWin32.find(ev.key.key);
		if (it != DuiLibWindowWnd::m_keySdlToWin32.end())
			msg->wParam = it->second;
	}
	else if (ev.type == SDL_EVENT_MOUSE_MOTION) //	SDL_MouseMotionEvent	motion
	{
		msg->message = WM_MOUSEMOVE;
		msg->lParam = MAKELPARAM(ev.button.x, ev.button.y);
	}
	else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		if (ev.button.button == SDL_BUTTON_LEFT)
			msg->message = WM_LBUTTONDOWN;
		if (ev.button.button == SDL_BUTTON_RIGHT)
			msg->message = WM_RBUTTONDOWN;
		if (ev.button.button == SDL_BUTTON_MIDDLE)
			msg->message = WM_MBUTTONDOWN;
		msg->lParam = MAKELPARAM(ev.button.x, ev.button.y);
	}
	else if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP) //	SDL_MouseButtonEvent	button
	{
		if (ev.button.button == SDL_BUTTON_LEFT)
			msg->message = WM_LBUTTONUP;
		if (ev.button.button == SDL_BUTTON_RIGHT)
			msg->message = WM_RBUTTONUP;
		if (ev.button.button == SDL_BUTTON_MIDDLE)
			msg->message = WM_MBUTTONUP;
		msg->lParam = MAKELPARAM(ev.button.x, ev.button.y);
	}
	else if (ev.type == SDL_EVENT_MOUSE_WHEEL) //	SDL_MouseWheelEvent	wheel
	{
		msg->message = WM_MOUSEWHEEL;
		msg->lParam = MAKELPARAM(ev.button.x, ev.button.y);
	}
// 	else if (ev.type == SDL_EVENT_QUIT) //	SDL_QuitEvent	quit
// 	{
// 		msg->message = WM_CLOSE; //这样不对。当收到SDL_EVENT_QUIT，窗口已经不存在了。
// 	}
	else if (ev.type == SDL_EVENT_WINDOW_RESIZED || ev.type == SDL_EVENT_WINDOW_MOVED) //, etc	SDL_WindowEvent	window
	{
		msg->message = WM_SIZE;
	}
	else if (ev.type == SDL_EVENT_WINDOW_MOUSE_ENTER) //,       /**< Window has gained mouse focus */
	{
		msg->message = WM_MOUSEHOVER;
		msg->lParam = MAKELPARAM(ev.button.x, ev.button.y);
	}
	else if (ev.type == SDL_EVENT_WINDOW_MOUSE_LEAVE) //,       /**< Window has lost mouse focus */
	{
		msg->message = WM_MOUSELEAVE;
		msg->lParam = MAKELPARAM(ev.button.x, ev.button.y);
	}
	else if (ev.type == SDL_EVENT_WINDOW_EXPOSED)
	{
		msg->message = WM_PAINT;
	}
	else if (ev.type == SDL_EVENT_WINDOW_FOCUS_LOST)
	{
		msg->message = WM_KILLFOCUS;
	}
	else if (ev.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
	{
		msg->message = WM_SETFOCUS;
	}
	else if (ev.type == SDL_EVENT_USER) //	SDL_UserEvent	user
	{
		if (ev.user.code == WM_TIMER)
		{
			TIMERINFO* pTimer = (TIMERINFO*)ev.user.data1;
			msg->message = ev.user.code;
			msg->wParam = MAKEWPARAM(pTimer->uWinTimer, 0);
		}
		else if (ev.user.code == WM_CLOSE)
		{
			msg->message = ev.user.code;
			msg->wParam = (WPARAM)ev.user.data1;
		}
		else if (ev.user.code == WM_DESTROY)
		{
			msg->message = ev.user.code;
		}
		else if (ev.user.code == WM_QUIT)
		{
			msg->message = ev.user.code;
		}
		else if (ev.user.code == WM_PAINT)
		{
			msg->message = ev.user.code;
		}
	}

	return TRUE;
}

LRESULT CWindowSDL::DispatchMessage(PVOID ev1, MSG* msg)
{
	SDL_Event& ev = *(SDL_Event*)ev1;

	WindowInfo info;
	if (!FindWindowInfoByPtr((UINT_PTR)msg->hwnd, &info))
	{
		return -1;
	}

	CWindowSDL* pWindow = (CWindowSDL*)info.pDuiWindow;

	if (pWindow->OnSdlEvent(ev1))
		return 0;

	LRESULT lRet = pWindow->HandleMessage(msg->message, msg->wParam, msg->lParam);

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

	if (msg->message == WM_DESTROY)
	{
		pWindow->GetManager()->ReleaseRender();
		SDL_DestroyWindow((SDL_Window*)pWindow->GetHWND());

		SDL_Event ev2;
		SDL_zero(ev2);
		ev2.type = SDL_EVENT_USER;
		ev2.window.windowID = ev.window.windowID;
		ev2.user.code = WM_QUIT;
		ev2.user.data1 = 0;
		ev2.user.data2 = 0;
		SDL_PushEvent(&ev2);
		return lRet;
	}

	if (msg->message == WM_KEYDOWN)
	{
		UINT uMsgChar = 0;
		WPARAM wparamChar = 0;
		LPARAM lparamChar = 0;
		if (TranslateSdlCharMessage(ev, uMsgChar, wparamChar, lparamChar))
		{
			pWindow->HandleMessage(uMsgChar, wparamChar, lparamChar);
		}
	}

	return lRet;
}

void CWindowSDL::Invalidate()
{
// 	SDL_Event event;
// 	event.type = SDL_EVENT_WINDOW_EXPOSED;
// 	event.window.windowID = SDL_GetWindowID((SDL_Window*)m_hWnd);
// 	SDL_PushEvent(&event);
 	SDL_Event ev;
	SDL_zero(ev);
	ev.type = SDL_EVENT_USER;
	ev.window.windowID = SDL_GetWindowID((SDL_Window*)m_hWnd);
	ev.user.code = WM_PAINT;
	ev.user.data1 = NULL;
	ev.user.data2 = NULL;
	SDL_PushEvent(&ev);
}

BOOL CWindowSDL::SetWindowPos(int x, int y, int cx, int cy, UINT uFlags)
{
	if (!SDL_SetWindowPosition((SDL_Window*)m_hWnd, x, y))
		return FALSE;
	if (!SDL_SetWindowSize((SDL_Window*)m_hWnd, cx, cy))
		return FALSE;
	return TRUE;
}

BOOL CWindowSDL::GetWindowRect(LPRECT lpRect)
{
	int x, y, w, h;
	if (!SDL_GetWindowPosition((SDL_Window*)m_hWnd, &x, &y))
		return FALSE;
	if (!SDL_GetWindowSize((SDL_Window*)m_hWnd, &w, &h))
		return FALSE;
	lpRect->left = x;
	lpRect->top = y;
	lpRect->right = x + w;
	lpRect->bottom = y + h;
	return TRUE;
}

BOOL CWindowSDL::GetClientRect(LPRECT lpRect)
{
	int w, h;
	if (!SDL_GetWindowSize((SDL_Window*)m_hWnd, &w, &h))
		return FALSE;
	lpRect->left = 0;
	lpRect->top = 0;
	lpRect->right = w;
	lpRect->bottom = h;
	return TRUE;
}

#define MAPKEY_WIN32_SDL(sdl, win32)	\
 do {									\
	m_keySdlToWin32[sdl] = win32;		\
	m_keyWin32ToSdl[win32] = sdl;		\
 } while(0)
void CWindowSDL::_init_sdl_defined()
{
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

#include "StdAfx.h"

#ifdef DUILIB_SDL


namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

UINT CWindowSDL::m_EVENT_SEND_MESSAGE = 0;
UINT CWindowSDL::m_EVENT_POST_MESSAGE = 0;

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
	Uint32 flags = SDL_WINDOW_HIDDEN;
// 	if (dwStyle & WS_POPUP) flags |= SDL_WINDOW_BORDERLESS;
// 	if (dwStyle & WS_SIZEBOX) flags |= SDL_WINDOW_RESIZABLE;
	flags |= SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE;
	SDL_Window *pWindow = SDL_CreateWindow(strNameUtf8.toString(), 200, 200, flags);
	if (!pWindow) return NULL;
	m_hWnd = (UIWND)pWindow;
	m_id = SDL_GetWindowID(pWindow);
	m_uOwnerThread = SDL_GetThreadID(NULL);
	RegisterWindow((UINT_PTR)pWindow, this);
	HandleMessage(WM_CREATE, 0, 0);
	return m_hWnd;
}

void CWindowSDL::Close(UINT nRet)
{
	SDL_Event event;
	SDL_zero(event);
	event.window.windowID = SDL_GetWindowID((SDL_Window*)GetHWND());
	event.type = SDL_EVENT_QUIT;
	event.user.code = nRet;
	SDL_PushEvent(&event);
}

void CWindowSDL::SetCursor(int nCursor)
{
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
	return SDL_GetWindowParent((SDL_Window*)hWnd) == NULL;
}

UIWND CWindowSDL::GetParentWindow(UIWND hWnd)
{
	return (UIWND)SDL_GetWindowParent((SDL_Window*)hWnd);
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

void CWindowSDL::Invalidate()
{
	SDL_Event event;
	event.type = SDL_EVENT_WINDOW_EXPOSED;
	event.window.windowID = SDL_GetWindowID((SDL_Window*)m_hWnd);
	SDL_PushEvent(&event);
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

} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

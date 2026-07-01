#ifndef __UI_WINDOW_SDL_H__
#define __UI_WINDOW_SDL_H__


#pragma once

#ifdef DUILIB_SDL
namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CWindowSDL : public CWindowWnd
{
public:
	struct WindowInfo {
		UINT windowID;			 // SDL_WindowID windowID;
		UIWND sdlWindow;		 // SDL_Window* window;
		CWindowWnd* pDuiWindow;  // 关联的CWindowSDL*
		uiBool isMainWindow;       // 是否为主窗口
	};
	struct SdlSendMessage
	{
		UINT uMsg;
		WPARAM wParam;
		LPARAM lParam;
		LRESULT result;         //返回值
		PVOID mutex;			//SDL_Mutex*
		PVOID cond;				//SDL_Condition*
		bool processed;         // 处理完成标志
		bool isSync;            // 是否为同步消息（用于接收端判断）
	};

public:
	CWindowSDL();
	virtual ~CWindowSDL();

	static UINT m_EVENT_SEND_MESSAGE;
	static UINT m_EVENT_POST_MESSAGE;

	virtual UIWND Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = 0, int y = 0, int cx = 0, int cy = 0);

	virtual void Close(UINT nRet = IDOK) override;
	virtual void SetCursor(int nCursor) override;
	void ShowWindow(bool bShow = true, bool bTakeFocus = true);
	UINT ShowModal();

	void CenterWindow();	// 居中，支持扩展屏幕

	void ShowAndActivateChildWindow();

	virtual uiBool OnSdlEvent(const void *pEvent);

	virtual void Invalidate() override;
	virtual uiBool SetWindowPos(int x, int y, int cx, int cy, UINT uFlags) override;
	virtual uiBool GetWindowRect(LPRECT lpRect) override;
	virtual uiBool GetClientRect(LPRECT lpRect) override;

protected:
	DWORD m_dwStyle;
	UINT m_id;
	uiUInt64 m_uOwnerThread;
	PVOID m_lastCursor; //SDL_Cursor
	int   m_nCurrentCursor; // 当前光标类型，初始化为 -1 或一个无效值

public:
	// 注册窗口，必须在窗口创建成功后调用
	static uiBool RegisterWindow(UIWND pSdlWindow, CWindowWnd *pDuiWindow);

	// 注销窗口
	static void UnRegisterWindow(UINT id);

	// 根据窗口ID查找窗口信息，SDL_WindowID
	static uiBool FindWindowInfo(UINT id, WindowInfo *pInfo);

	// 根据窗口指针查找窗口信息, SDL_Window *
	static uiBool FindWindowInfoByPtr(UIWND window, WindowInfo* pInfo);

	// 根据窗口指针查找窗口信息, CWindowWnd *
	static uiBool FindWindowInfoByPtr(CWindowWnd* pDuiWindow, WindowInfo* pInfo);

	// 判断指定ID的窗口是否为主窗口
	static uiBool IsMainWindow(UINT windowID);

	// 设定主窗口
	static uiBool SetMainWindow(UINT windowID);

	static uiBool IsWindow(UIWND hWnd);
	static uiBool IsChildWindow(UIWND hWnd);
	static UIWND GetParentWindow(UIWND hWnd);
	static uiBool SetForeground(UIWND hWnd);
	static LRESULT SendMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
	static uiBool PostMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);

	static uiBool GetMessage(PVOID ev, MSG* msg);
	static uiBool TranslateMessage(PVOID ev, MSG* msg);
	static LRESULT DispatchMessage(PVOID ev, MSG* msg);
protected:
	static std::map<UINT, WindowInfo> m_smap;
	static CDuiLock m_smap_lock; 

public:
	static void _init_sdl_defined();
	static CMacroToStringMap m_sdlEventString;
	static std::map<UINT, WORD> m_keySdlToWin32;
	static std::map<WORD, UINT> m_keyWin32ToSdl;
};	

} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

#endif // __UIBASE_H__


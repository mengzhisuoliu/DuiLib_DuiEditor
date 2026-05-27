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
		UINT windowID;		 // SDL_WindowID windowID;
		UINT_PTR sdlWindow;		 // SDL_Window* window;
		CWindowWnd* pDuiWindow;  // 关联的CWindowSDL*
		BOOL isMainWindow;       // 是否为主窗口
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

	virtual UIWND Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc);
	virtual UIWND Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = 0, int y = 0, int cx = 0, int cy = 0);
	UIWND CreateDuiWindow(UIWND hwndParent, LPCTSTR pstrWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);

	virtual void Close(UINT nRet = IDOK) override;
	virtual void SetCursor(int nCursor) override;
	void ShowWindow(bool bShow = true, bool bTakeFocus = true);
	UINT ShowModal();

	void CenterWindow();	// 居中，支持扩展屏幕

	virtual BOOL OnSdlEvent(const void *pEvent);

	virtual void Invalidate() override;
	virtual BOOL SetWindowPos(int x, int y, int cx, int cy, UINT uFlags) override;
	virtual BOOL GetWindowRect(LPRECT lpRect) override;
	virtual BOOL GetClientRect(LPRECT lpRect) override;

protected:
	UINT m_id;
	UInt64 m_uOwnerThread;
	PVOID m_lastCursor; //SDL_Cursor
	int   m_nCurrentCursor; // 当前光标类型，初始化为 -1 或一个无效值

public:
	// 注册窗口，必须在窗口创建成功后调用
	static BOOL RegisterWindow(UINT_PTR pSdlWindow, CWindowWnd *pDuiWindow);

	// 注销窗口
	static void UnRegisterWindow(UINT id);

	// 根据窗口ID查找窗口信息，SDL_WindowID
	static BOOL FindWindowInfo(UINT id, WindowInfo *pInfo);

	// 根据窗口指针查找窗口信息, SDL_Window *
	static BOOL FindWindowInfoByPtr(UINT_PTR window, WindowInfo* pInfo);

	// 根据窗口指针查找窗口信息, CWindowWnd *
	static BOOL FindWindowInfoByPtr(CWindowWnd* pDuiWindow, WindowInfo* pInfo);

	// 判断指定ID的窗口是否为主窗口
	static BOOL IsMainWindow(UINT windowID);

	// 设定主窗口
	static BOOL SetMainWindow(UINT windowID);

	static BOOL IsWindow(UIWND hWnd);
	static BOOL IsChildWindow(UIWND hWnd);
	static UIWND GetParentWindow(UIWND hWnd);
	static BOOL SetForeground(UIWND hWnd);
	static LRESULT SendMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
	static BOOL PostMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);

	static BOOL GetMessage(PVOID ev, MSG* msg);
	static BOOL TranslateMessage(PVOID ev, MSG* msg);
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

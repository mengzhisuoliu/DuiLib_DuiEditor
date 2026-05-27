#ifndef __UIWINDOW_WIN32_H__
#define __UIWINDOW_WIN32_H__


#pragma once

#ifdef DUILIB_WIN32
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CWindowWin32 : public CWindowWnd
	{
	public:
		CWindowWin32();

		bool RegisterWindowClass();
		bool RegisterSuperclass();

		UIWND CreateDuiWindow(UIWND hwndParent, LPCTSTR pstrWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);
		virtual HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc);
		virtual HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT);
		
		HWND Subclass(HWND hWnd);
		void Unsubclass();
		void ShowWindow(bool bShow = true, bool bTakeFocus = true);
		UINT ShowModal();

		static BOOL IsWindow(UIWND hWnd);
		static BOOL IsChildWindow(UIWND hWnd);
		static UIWND GetParentWindow(UIWND hWnd);
		static BOOL SetForeground(UIWND hWnd);
		static LRESULT SendMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
		static BOOL PostMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);

		virtual void Invalidate() override;
		virtual BOOL SetWindowPos(int x, int y, int cx, int cy, UINT uFlags) override;
		virtual BOOL GetWindowRect(LPRECT lpRect) override;
		virtual BOOL GetClientRect(LPRECT lpRect) override;

		virtual void Close(UINT nRet = IDOK) override;
		virtual void SetCursor(int nCursor) override;
		
		void CenterWindow();	// 居中，支持扩展屏幕
		void SetIcon(UINT nRes);

		void ResizeClient(int cx = -1, int cy = -1);
	protected:
		virtual LPCTSTR GetWindowClassName() const = 0;
		virtual LPCTSTR GetSuperClassName() const;
		virtual UINT GetClassStyle() const;

		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static BOOL DoTouchInformation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		WNDPROC m_OldWndProc;
		bool m_bSubclassed;
	};

} // namespace DuiLib
#endif //#ifdef DUILIB_WIN32
#endif // __UIWINDOW_WIN32_H__


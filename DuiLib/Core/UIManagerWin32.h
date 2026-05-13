#ifndef __UIMANAGER_WIN32_H__
#define __UIMANAGER_WIN32_H__
#pragma once

#ifdef DUILIB_WIN32
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CPaintManagerWin32UI : public CPaintManagerUI
	{
	public:
		CPaintManagerWin32UI();
		virtual ~CPaintManagerWin32UI();

		virtual void Init(HWND hWnd, LPCTSTR pstrName = NULL, CWindowWnd *pWindow=NULL) override;
		virtual UIRender *Render() override;

		virtual BOOL InvalidateRect(UIWND hWnd, const RECT *lpRect, BOOL bErase) override;
		virtual HDC GetPaintDC() const override;

		virtual void SetLayered(bool bLayered) override;

		virtual void SetCursor(int nCursor) override;

		virtual void SetCapture() override;
		virtual void ReleaseCapture() override;
		virtual bool IsCaptured() override;

		virtual BOOL SetTimer(UINT uElapse, TIMERINFO* pTimer) override;
		virtual BOOL KillTimer(TIMERINFO* pTimer) override;
		virtual BOOL GetCursorPos(LPPOINT pt) override;
		virtual BOOL ScreenToClient(LPPOINT pt) override;
		virtual void SetWndFocus() override;
		virtual UIWND GetWndFocus() override;
		virtual BOOL IsZoomed() override;
		virtual BOOL IsIconic() override;
		virtual BOOL SetWindowPos(int x, int y, int cx, int cy, UINT uFlags) override;
		virtual BOOL GetWindowRect(LPRECT lpRect) override;
		virtual BOOL GetClientRect(LPRECT lpRect) override;
		virtual BOOL IsKeyDown(UINT uKey) override;
		virtual BOOL IsKeyUp(UINT uKey) override;
		virtual BOOL IsCtrlKeyDown() override;
		virtual BOOL IsAltKeyDown() override;
		virtual BOOL IsShiftKeyDown() override;
		virtual BOOL IsCapsLockKeyOn() override;
		virtual BOOL IsNUmberLockKeyOn() override;
		virtual UINT MapKeyState() override;
		virtual DWORD GetTickCount() override;
		virtual void GetLocalTime(SYSTEMTIME &st) override;

		int GetNativeWindowCount() const;
		RECT GetNativeWindowRect(HWND hChildWnd);
		bool AddNativeWindow(CControlUI* pControl, HWND hChildWnd);
		bool RemoveNativeWindow(HWND hChildWnd);

		static void MessageLoop();

		virtual bool OnClose(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnEraseBkgnd(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnPrintClient(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnGetMaxMinInfo(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseOver(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseLeave(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnContextMenu(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnNotify(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnCommand(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnCtlColorEdit(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnCtlColorStatic(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;

	public:
		//创建控件的图标，比如拖拽时的图片
		virtual UIBitmap* CreateControlBitmap(CControlUI* pControl, DWORD dwFilterColor = 0, CControlUI* pStopControl = NULL) override;

		//构造内部窗口的背景画刷
		virtual UIBitmap* CreateControlBackBitmap(CControlUI* pControl, const RECT &rcWnd, DWORD dwFilterColor) override;

	protected:
		HDC m_hDcPaint;
		TOOLINFO m_ToolTip;
		UINT m_uTimerID;
	};


	//先锁定窗口更新，然后强制刷新整个窗口
	class UILIB_API CLockWindowUpdateUI
	{
	public:
		CLockWindowUpdateUI(CPaintManagerUI* pManager);
		~CLockWindowUpdateUI();
	private:
		CPaintManagerUI* m_pManager;
	};
} // namespace DuiLib
#endif //#ifdef DUILIB_WIN32

#endif // __UIMANAGER_H__

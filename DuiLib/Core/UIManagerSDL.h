#ifndef __UIMANAGER_SDL_H__
#define __UIMANAGER_SDL_H__
#pragma once

#ifdef DUILIB_SDL
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CPaintManagerSDLUI : public CPaintManagerUI
	{
	public:
		CPaintManagerSDLUI();
		virtual ~CPaintManagerSDLUI();

		virtual void Init(UIWND hWnd, LPCTSTR pstrName = NULL, CWindowWnd *pWindow=NULL) override;
		virtual UIRender *Render() override;

		void ClearImageTexture(UIRender *pRender);

		virtual uiBool InvalidateRect(UIWND hWnd, const CDuiRect*lpRect, uiBool bErase) override;
		virtual void SetCursor(int nCursor) override;

		virtual void SetCapture() override;
		virtual void ReleaseCapture() override;
		virtual bool IsCaptured() override;

		virtual uiBool SetTimer(UINT uElapse, TIMERINFO* pTimer) override;
		virtual uiBool KillTimer(TIMERINFO* pTimer) override;
		virtual uiBool GetCursorPos(LPPOINT pt) override;
		virtual uiBool ScreenToClient(LPPOINT pt) override;
		virtual void SetWndFocus() override;
		virtual UIWND GetWndFocus() override;
		virtual uiBool IsZoomed() override;
		virtual uiBool IsIconic() override;
		virtual uiBool SetWindowPos(int x, int y, int cx, int cy, UINT uFlags) override;
		virtual uiBool GetWindowRect(LPRECT lpRect) override;
		virtual uiBool GetClientRect(LPRECT lpRect) override;
		virtual uiBool IsKeyDown(UINT uKey) override; // π”√WIN32–Èƒ‚º¸¬Î∑√Œ 
		virtual uiBool IsKeyUp(UINT uKey) override;	// π”√WIN32–Èƒ‚º¸¬Î∑√Œ 
		virtual uiBool IsCtrlKeyDown() override;
		virtual uiBool IsAltKeyDown() override;
		virtual uiBool IsShiftKeyDown() override;
		virtual uiBool IsCapsLockKeyOn() override;
		virtual uiBool IsNumberLockKeyOn() override;
		virtual UINT MapKeyState() override;
		virtual DWORD GetTickCount() override;
		virtual void GetLocalTime(SYSTEMTIME& st) override;

		static void MessageLoop();

		virtual bool OnClose(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnGetMaxMinInfo(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseOver(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseLeave(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;

	public:
	};
} // namespace DuiLib

#endif //#ifdef DUILIB_SDL
#endif // __UIMANAGER_SDL_H__

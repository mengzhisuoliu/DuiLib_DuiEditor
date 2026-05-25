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

		virtual BOOL InvalidateRect(UIWND hWnd, const RECT *lpRect, BOOL bErase) override;
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
		virtual BOOL IsKeyDown(UINT uKey) override; // π”√WIN32–Èƒ‚º¸¬Î∑√Œ 
		virtual BOOL IsKeyUp(UINT uKey) override;	// π”√WIN32–Èƒ‚º¸¬Î∑√Œ 
		virtual BOOL IsCtrlKeyDown() override;
		virtual BOOL IsAltKeyDown() override;
		virtual BOOL IsShiftKeyDown() override;
		virtual BOOL IsCapsLockKeyOn() override;
		virtual BOOL IsNUmberLockKeyOn() override;
		virtual UINT MapKeyState() override;
		virtual DWORD GetTickCount() override;
		virtual void GetLocalTime(SYSTEMTIME& st) override;

		static void MessageLoop();

		virtual bool OnClose(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnGetMaxMinInfo(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseOver(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseLeave(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;

	private:
		static void _init_sdl_defined();
	public:
		static CMacroToStringMap m_sdlEventString;
		static std::map<UINT, WORD> m_keySdlToWin32;
		static std::map<WORD, UINT> m_keyWin32ToSdl;
	};
} // namespace DuiLib

#endif //#ifdef DUILIB_SDL
#endif // __UIMANAGER_SDL_H__

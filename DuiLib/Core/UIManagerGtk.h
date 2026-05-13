#ifndef __UIMANAGER_GTK_H__
#define __UIMANAGER_GTK_H__
#pragma once

#ifdef DUILIB_GTK
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CPaintManagerGtkUI : public CPaintManagerUI
	{
	public:
		CPaintManagerGtkUI();
		virtual ~CPaintManagerGtkUI();

		virtual void Init(UIWND hWnd, LPCTSTR pstrName = NULL, CWindowWnd *pWindow=NULL) override;
		virtual UIRender *Render() override;

		virtual BOOL InvalidateRect(UIWND hWnd, const RECT *lpRect, BOOL bErase) override;
		virtual void SetCursor(int nCursor) override;

		virtual BOOL SetTimer(UINT uElapse, TIMERINFO* pTimer) override;
		virtual BOOL KillTimer(TIMERINFO* pTimer) override;
		virtual BOOL GetCursorPos(LPPOINT pt) override;
		virtual BOOL ScreenToClient(LPPOINT pt) override;
		virtual void SetWndFocus() override;
		virtual UIWND GetWndFocus() override;
		virtual BOOL IsKeyDown(UINT uKey) override;
		virtual BOOL IsKeyUp(UINT uKey) override;
		virtual UINT MapKeyState() override;
		virtual DWORD GetTickCount() override;
		virtual void GetLocalTime(SYSTEMTIME &st) override;

		static void MessageLoop();

		virtual bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;
		virtual bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes) override;

		virtual bool OnClose(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnEraseBkgnd(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnPrintClient(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnGetMaxMinInfo(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnSize(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnTimer(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnLButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnLButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnLButtonDbClick(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnRButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnRButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnRButtonDbClick(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnMButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnMButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnMouseOver(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnMouseLeave(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnContextMenu(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnChar(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnKeyDown(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnKeyUp(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnSetCursor(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnSetFocus(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnKillFocus(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnNotify(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnCommand(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnCtlColorEdit(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		virtual bool OnCtlColorStatic(WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	};
} // namespace DuiLib

#endif //#ifdef DUILIB_GTK
#endif // __UIMANAGER_GTK_H__

#ifndef __UIEDIT_WND_SDL_H__
#define __UIEDIT_WND_SDL_H__

#pragma once

#ifdef DUILIB_SDL
namespace DuiLib
{
	class CEditWndSDL : public CEditWnd, public CWindowSDL
	{
	public:
		CEditWndSDL(CControlUI* pOwner);
		virtual ~CEditWndSDL();

		virtual void Init() override;
		virtual CDuiRect CalPos() override;
		virtual void SetEditText(LPCTSTR sText) override;

		virtual void OnFinalMessage(UIWND hWnd) override;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		BOOL OnSdlEvent(const void* pEvent) override;

	protected:
		// 消息处理
		void OnPaint();
		void OnKeyDown(WPARAM wParam, LPARAM lParam);
		void OnChar(WPARAM wParam, LPARAM lParam);
		void OnLButtonDown(WPARAM wParam, LPARAM lParam);
		void OnLButtonUp(WPARAM wParam, LPARAM lParam);
		void OnMouseMove(WPARAM wParam, LPARAM lParam);
		void OnSetFocus(WPARAM wParam, LPARAM lParam);
		void OnKillFocus(WPARAM wParam, LPARAM lParam);
		void OnTimer(WPARAM wParam, LPARAM lParam);

		// 编辑操作
		void InsertText(const CDuiString& text);
		void DeleteBackward();
		void DeleteForward();
		void DeleteSelection();
		void MoveCursor(int delta, bool extendSelection);
		void MoveCursorToHome(bool extendSelection);
		void MoveCursorToEnd(bool extendSelection);
		void SelectAll();
		void CopyToClipboard();
		void CutToClipboard();
		void PasteFromClipboard();

		// 辅助函数
		int GetCharPosFromPoint(const POINT& pt);
		int GetCharXPos(int charIndex);
		CDuiRect GetCaretPos();
		bool IsFocused();

	private:
		CDuiString m_sText;    // 当前编辑的文本
		int m_cursorPos;       // 光标位置（字符索引）
		int m_selStart;        // 选择起点
		int m_selEnd;          // 选择终点
		bool m_bDrawCaret;     // 光标是否显示（闪烁）
		UINT m_caretTimerID;   // 光标闪烁定时器 ID
		bool m_bDragging;
	};
}
#endif // DUILIB_SDL
#endif // __UIEDIT_WND_SDL_H__
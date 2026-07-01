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
		uiBool OnSdlEvent(const void* pEvent) override;

	protected:
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
		virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;

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
		int GetCharPosFromPoint(const CDuiPoint& pt);
		int GetCharXPos(int charIndex);
		CDuiRect GetCaretPos();

	private:
		CDuiString m_sText;    // 当前编辑的文本
		int m_cursorPos;       // 光标位置（字符索引）
		int m_selStart;        // 选择起点
		int m_selEnd;          // 选择终点
		bool m_bDrawCaret;     // 光标是否显示（闪烁）
		UINT m_caretTimerID;   // 光标闪烁定时器 ID
		bool m_bDragging;

		CDuiString m_sComposition;  // 当前 IME 组合字符串（预编辑文本）
		int m_compositionCursor;    // 组合字符串中的光标位置
	};
}
#endif // DUILIB_SDL
#endif // __UIEDIT_WND_SDL_H__

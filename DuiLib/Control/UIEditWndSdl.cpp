#include "StdAfx.h"
#include "UIEditWndSdl.h"

#ifdef DUILIB_SDL
#include <algorithm>

namespace DuiLib
{
	 /**
	 * 计算输入法候选窗口的合理位置
	 * @param window   SDL窗口指针
	 * @param caretRect 输入框相对于窗口客户区的矩形（屏幕坐标需自行转换）
	 * @param offset   光标相对于输入框左侧的像素偏移（通常为0或光标X坐标）
	 * @return 返回一个SDL_Rect，表示输入区域（用于SDL_SetTextInputArea）
	 *         同时该矩形的屏幕坐标版本可用于平台API手动定位
	 */
	static SDL_Rect CalculateCandidatePosition(SDL_Window* window, const SDL_Rect* caretRect, int offset) {
		// 获取窗口在屏幕上的位置
		int winX, winY;
		SDL_GetWindowPosition(window, &winX, &winY);

		// 获取窗口大小（像素）
		int winW, winH;
		SDL_GetWindowSize(window, &winW, &winH);

		// 计算输入框在屏幕上的坐标（假设caretRect已经是窗口客户区坐标）
		SDL_Rect screenRect;
		screenRect.x = winX + caretRect->x + offset;
		screenRect.y = winY + caretRect->y + caretRect->h; // 候选窗默认放在输入框下方
		screenRect.w = caretRect->w;
		screenRect.h = 1; // height不影响候选窗，可忽略

		// 获取显示器工作区域（避免候选窗超出屏幕）
		SDL_DisplayID display = SDL_GetDisplayForWindow(window);
		SDL_Rect displayRect;
		SDL_GetDisplayUsableBounds(display, &displayRect);

		// 边界修正：优先放在下方，如果空间不足则放在上方
		int candidateHeight = 150; // 假设候选窗高度约150像素（可根据实际情况调整）
		if (screenRect.y + candidateHeight > displayRect.y + displayRect.h) {
			// 下方空间不足，改放在输入框上方
			screenRect.y = winY + caretRect->y - candidateHeight;
			if (screenRect.y < displayRect.y) {
				screenRect.y = displayRect.y;
			}
		}

		// 横向不超出屏幕边界
		if (screenRect.x + caretRect->w > displayRect.x + displayRect.w) {
			screenRect.x = displayRect.x + displayRect.w - caretRect->w;
		}
		if (screenRect.x < displayRect.x) {
			screenRect.x = displayRect.x;
		}

		// 转换为窗口坐标（用于SDL_SetTextInputArea）
		SDL_Rect inputArea;
		inputArea.x = screenRect.x - winX;
		inputArea.y = caretRect->y;          // 由于候选窗位置由输入法根据输入区域自动放置，
		inputArea.w = caretRect->w;          // 但我们提供整个输入框的区域即可。
		inputArea.h = caretRect->h;

		return inputArea;
	}

	CEditWndSDL::CEditWndSDL(CControlUI* pOwner)
		: CEditWnd(pOwner)
		, m_cursorPos(0)
		, m_selStart(0)
		, m_selEnd(0)
		, m_bDrawCaret(true)
		, m_caretTimerID(100)
		, m_bDragging(false)
		, m_compositionCursor(0)
	{
	}

	CEditWndSDL::~CEditWndSDL()
	{
			
	}

	void CEditWndSDL::Init()
	{
		CDuiRect rc = CalPos();
		rc.Deflate(1, 1, 1, 1);
		CWindowSDL* pParentWnd = (CWindowSDL*)GetOwner()->GetManager()->GetWindow();
		SDL_Window* hParentWnd = (SDL_Window*)pParentWnd->GetHWND();

		// SDL3 创建弹出窗口（无边框，隐藏）
		Uint32 flags = SDL_WINDOW_POPUP_MENU | SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN;
		SDL_Window* pWindow = SDL_CreatePopupWindow(hParentWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), flags);
		if (!pWindow) return;

		m_hWnd = (UIWND)pWindow;
		m_id = SDL_GetWindowID(pWindow);
		m_uOwnerThread = SDL_GetThreadID(NULL);
		RegisterWindow((UINT_PTR)pWindow, this);

		// 初始化文本
		m_sText = GetOwner()->GetText();
		m_cursorPos = m_sText.GetLength();
		m_selStart = m_selEnd = m_cursorPos;

		// 自动全选
		if (IsAutoSelAll() && !m_sText.IsEmpty())
		{
			m_selStart = 0;
			m_selEnd = m_sText.GetLength();
			m_cursorPos = m_selEnd;
		}

		m_pm.SetForceUseSharedRes(true);
		m_pm.Init(m_hWnd, NULL, this);

		// 显示窗口并设置焦点
		ShowWindow();

		// 强制重绘
		Invalidate();
	}

	CDuiRect CEditWndSDL::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetClientPos();

// 		if (!IsMultiLine())
// 		{
// 			int lineHeight = m_font ? m_font->GetHeight(m_pOwner->GetManager()) : 20;
// 			if (lineHeight < rcPos.GetHeight())
// 			{
// 				rcPos.top += (rcPos.GetHeight() - lineHeight) / 2;
// 				rcPos.bottom = rcPos.top + lineHeight;
// 			}
// 		}

		CControlUI* pParent = m_pOwner;
		while (pParent = pParent->GetParent())
		{
			if (!pParent->IsVisible())
			{
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
			RECT rcParent = pParent->GetClientPos();
			if (!rcPos.Intersect(rcPos, rcParent))
			{
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
		}
		return rcPos;
	}

	void CEditWndSDL::SetEditText(LPCTSTR sText)
	{
		m_sText = sText;
		m_cursorPos = MIN(m_cursorPos, m_sText.GetLength());
		m_selStart = MIN(m_selStart, m_sText.GetLength());
		m_selEnd = MIN(m_selEnd, m_sText.GetLength());

		SetOwnerText(sText);
		GetOwner()->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
		Invalidate();
	}

	void CEditWndSDL::OnFinalMessage(UIWND hWnd)
	{
		SetOwnerText(m_sText);
		m_pOwner->Invalidate();
		delete this;
	}

	LRESULT CEditWndSDL::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_PAINT:
				OnPaint();
				return 0;
			case WM_KEYDOWN:
				OnKeyDown(wParam, lParam);
				return 0;
			case WM_CHAR:
				OnChar(wParam, lParam);
				return 0;
			case WM_LBUTTONDOWN:
				OnLButtonDown(wParam, lParam);
				return 0;
			case WM_LBUTTONUP:
				OnLButtonUp(wParam, lParam);
				return 0;
			case WM_MOUSEMOVE:
				OnMouseMove(wParam, lParam);
				return 0;
			case WM_SETFOCUS:
				OnSetFocus(wParam, lParam);
				break;
			case WM_KILLFOCUS:
				OnKillFocus(wParam, lParam);
				break;
			case WM_TIMER:
				OnTimer(wParam, lParam);
				break;
			default:
				break;
		}
		LRESULT lRes = 0;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowSDL::HandleMessage(uMsg, wParam, lParam);
	}

	BOOL CEditWndSDL::OnSdlEvent(const void* pEvent)
	{
		SDL_Event* ev = (SDL_Event*)pEvent;
		// 处理预编辑文本（IME 组合状态）
		if (ev->type == SDL_EVENT_TEXT_EDITING)
		{
			// 获取组合文本和光标位置
			m_sComposition = CDuiStringUtf8(ev->edit.text);
			m_compositionCursor = ev->edit.start;  // 光标在组合字符串中的偏移（字符数）
			// 注意：ev->edit.length 是选中的长度，可根据需要处理
			Invalidate();   // 刷新显示
			return TRUE;
		}
		else if (ev->type == SDL_EVENT_TEXT_INPUT)
		{
			// 确认输入时，先清除组合状态
			if (!m_sComposition.IsEmpty())
			{
				m_sComposition.Empty();
				m_compositionCursor = 0;
			}
			CDuiString s = CDuiStringUtf8(ev->text.text);
			InsertText(s);
			Invalidate();
			return TRUE;
		}
		else if (ev->type == SDL_EVENT_WINDOW_MOUSE_ENTER)
		{
			GetManager()->SetCursor(DUI_IBEAM);
			return TRUE;
		}
		else if (ev->type == SDL_EVENT_WINDOW_MOUSE_LEAVE)
		{
			GetManager()->SetCursor(DUI_ARROW);
			return TRUE;
		}
		else if (ev->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			return FALSE;
		}
		return FALSE;
	}

	void CEditWndSDL::OnPaint()
	{
		CDuiRect rcClient;
		GetClientRect(&rcClient);
		if (rcClient.IsEmpty()) return;

		UIRender* pRender = GetManager()->Render();
		pRender->BeginPaint();
		pRender->Resize(rcClient);

		// 绘制背景色
		pRender->DrawColor(rcClient, CDuiSize(0, 0), GetNativeEditBkColor());

		// 绘制选择高亮区域
		if (m_selStart != m_selEnd)
		{
			int start = MIN(m_selStart, m_selEnd);
			int end = MAX(m_selStart, m_selEnd);
			int xStart = GetCharXPos(start);
			int xEnd = GetCharXPos(end);
			CDuiRect rcSel;
			rcSel.left = xStart;
			rcSel.right = xEnd;
			CDuiRect rcCaret = GetCaretPos(); //用光标的高度来绘制高亮区域
			rcSel.top = rcCaret.top;
			rcSel.bottom = rcCaret.bottom;
			DWORD selColor = UIARGB(255, 0, 120, 215); //0xFF3399FF; // 选择高亮色
			pRender->DrawColor(rcSel, CDuiSize(0, 0), selColor);
		}

		// 准备显示文本（密码模式处理）
		CDuiString displayText = m_sText;
		if (IsPasswordMode() && !displayText.IsEmpty())
		{
			CDuiString masked;
			for (int i = 0; i < displayText.GetLength(); ++i)
				masked += GetPasswordChar();
			displayText = masked;
		}

		// 文本颜色
		DWORD textColor = GetNativeEditTextColor();
		if (textColor == 0) textColor = m_pOwner->GetTextColor();
		if (textColor == 0) textColor = GetManager()->GetDefaultFontColor();

		// 文本反白颜色
		DWORD selectedColor = 0xFFFFFFFF; // 反白显示为白色

		// 绘制文本
		CDuiRect rcText = rcClient;
		rcText.SetPadding(m_pOwner->GetTextPadding());
		UINT uStyle = DT_LEFT | DT_VCENTER;
		if (IsMultiLine()) uStyle = DT_LEFT | DT_TOP | DT_WORDBREAK;
		else uStyle |= DT_SINGLELINE;

// 		pRender->DrawText(rcText, CDuiRect(0, 0, 0, 0), displayText,
// 			textColor, m_pOwner->GetFont(), uStyle);

		int curX = rcText.left;
		int curY = rcText.top;
		// 垂直居中（单行模式）
		if (!IsMultiLine())
		{
			int lineHeight = GetManager()->GetFontHeight(m_pOwner->GetFont());
			if (lineHeight < rcText.GetHeight())
				curY = rcText.top + (rcText.GetHeight() - lineHeight) / 2;
		}

		// 逐字符绘制
		for (int i = 0; i < displayText.GetLength(); ++i)
		{
			CDuiString ch = displayText.Mid(i, 1);
			CDuiStringUtf8 utf8(ch);
			CDuiSize sz = pRender->GetTextSize(ch.toString(), GetOwner()->GetFont(), DT_LEFT | DT_VCENTER);
			if (sz.cy == 0) continue;

			// 判断当前字符是否在选中范围内
			bool selected = false;
			if (m_selStart != m_selEnd)
			{
				int selStart = MIN(m_selStart, m_selEnd);
				int selEnd = MAX(m_selStart, m_selEnd);
				if (i >= selStart && i < selEnd)
					selected = true;
			}

			DWORD color = selected ? selectedColor : textColor;
			CDuiRect rcChar = { curX, curY, curX + sz.cx, curY + sz.cy };
			pRender->DrawText(rcChar, CDuiRect(0, 0, 0, 0), ch, color, m_pOwner->GetFont(), DT_LEFT | DT_TOP);

			curX += sz.cx;
			if (curX > rcText.right) break; // 超出区域则停止（多行模式需要换行，暂简化）
		}

		// 绘制组合文本（IME 预编辑）
		if (!m_sComposition.IsEmpty())
		{
			// 获取光标前的 X 坐标（用于放置组合文本）
			int caretX = GetCharXPos(m_cursorPos);
			CDuiRect rcText = rcClient;
			rcText.SetPadding(m_pOwner->GetTextPadding());
			int curY = rcText.top;
			if (!IsMultiLine())
			{
				int lineHeight = GetManager()->GetFontHeight(m_pOwner->GetFont());
				if (lineHeight < rcText.GetHeight())
					curY = rcText.top + (rcText.GetHeight() - lineHeight) / 2;
			}
			// 绘制组合文本
			CDuiRect rcCompose = { caretX, rcText.top, caretX + 200, rcText.bottom }; // 临时矩形，实际应测量宽度
			// 测量组合文本宽度
			SIZE szCompose = pRender->GetTextSize(m_sComposition, m_pOwner->GetFont(), DT_LEFT | DT_VCENTER);
			rcCompose.right = caretX + szCompose.cx;
			pRender->DrawText(rcCompose, CDuiRect(0, 0, 0, 0), m_sComposition,
				textColor, m_pOwner->GetFont(), DT_LEFT | DT_VCENTER);

			// 绘制下划线（简单实现：在组合文本下方画一条线）
			// 获取字体高度，用于下划线位置
			int lineHeight = GetManager()->GetFontHeight(m_pOwner->GetFont());
			int underlineY = rcCompose.bottom - 2; // 底部往上 2 像素
			//pRender->DrawLine(rcCompose.left, underlineY, rcCompose.right, underlineY,
			//	1, textColor);
			// 绘制虚线下划线
			int dashLen = 2;   // 实线长度
			int gapLen = 2;    // 间隙长度
			for (int x = rcCompose.left; x < rcCompose.right; x += dashLen + gapLen)
			{
				int endX = x + dashLen;
				if (endX > rcCompose.right) endX = rcCompose.right;
				pRender->DrawLine(x, underlineY, endX, underlineY, 1, textColor);
			}
		}

		//绘制光标（仅当有焦点且闪烁标志为真）
		if (m_bDrawCaret)
		{
			// 如果存在组合文本，光标位置应基于组合字符串中的偏移
			int caretX = 0;
			if (!m_sComposition.IsEmpty() && m_compositionCursor >= 0 && m_compositionCursor <= m_sComposition.GetLength())
			{
				// 测量组合文本中光标前的宽度
				CDuiString leftPart = m_sComposition.Left(m_compositionCursor);
				SIZE szLeft = pRender->GetTextSize(leftPart, m_pOwner->GetFont(), DT_LEFT | DT_TOP);
				caretX = GetCharXPos(m_cursorPos) + szLeft.cx;
			}
			else
			{
				caretX = GetCharXPos(m_cursorPos);
			}
			CDuiRect rcCaret = GetCaretPos(); // 调整 GetCaretPos 使用 caretX
			rcCaret.left = caretX;
			rcCaret.right = caretX + 1;
			pRender->DrawRect(rcCaret, 1, 0xFF000000);
		}

		pRender->EndPaint();
	}

	void CEditWndSDL::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		// 如果有组合文本，按下任意编辑键时清除组合
		if (!m_sComposition.IsEmpty())
		{
			SDL_ClearComposition((SDL_Window*)m_hWnd);
			m_sComposition.Empty();
			m_compositionCursor = 0;
			Invalidate();
			return;
		}

		bool ctrl = GetManager()->IsCtrlKeyDown();
		bool shift = GetManager()->IsShiftKeyDown();

		switch (wParam)
		{
			case VK_LEFT:   MoveCursor(-1, shift); break;
			case VK_RIGHT:  MoveCursor(1, shift); break;
			case VK_HOME:   MoveCursorToHome(shift); break;
			case VK_END:    MoveCursorToEnd(shift); break;
			case VK_BACK:   DeleteBackward(); break;
			case VK_DELETE: DeleteForward(); break;
			case VK_NUMPAD4: 
				if (!GetManager()->IsNUmberLockKeyOn()) 
					MoveCursor(-1, shift); 
				break;
			case VK_NUMPAD6: 
				if (!GetManager()->IsNUmberLockKeyOn()) 
					MoveCursor(1, shift); 
				break;
			case VK_NUMPAD7:
				if (!GetManager()->IsNUmberLockKeyOn())
					MoveCursorToHome(shift);
				break;
			case VK_NUMPAD1:
				if (!GetManager()->IsNUmberLockKeyOn())
					MoveCursorToEnd(shift);
				break;
			case VK_RETURN:
				if (IsMultiLine() && IsWantReturn())
					InsertText(_T("\n"));
				else
					GetOwner()->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
				break;
			case 'A':
				if (ctrl) SelectAll();
				break;
			case 'C':
				if (ctrl) CopyToClipboard();
				break;
			case 'X':
				if (ctrl) CutToClipboard();
				break;
			case 'V':
				if (ctrl) PasteFromClipboard();
				break;
			default:
				break;
		}
		Invalidate();
	}

	void CEditWndSDL::OnChar(WPARAM wParam, LPARAM lParam)
	{
// 		TCHAR ch = (TCHAR)wParam;
// 		if (ch >= 0x20 || ch == _T('\t') || (IsMultiLine() && ch == _T('\n')))
// 			InsertText(CDuiString(ch));
// 		Invalidate();
	}

	void CEditWndSDL::OnLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		int newPos = GetCharPosFromPoint(pt);
		if (GetManager()->IsShiftKeyDown())
		{
			// Shift按下：扩展选择
			MoveCursor(newPos - m_cursorPos, true);
		}
		else
		{
			// 普通按下：清除已有选择，设置光标位置
			m_cursorPos = newPos;
			m_selStart = m_selEnd = m_cursorPos;
			// 启动拖动模式
			m_bDragging = true;
			GetManager()->SetCapture();  // 捕获鼠标，确保在子窗口外移动也能接收到消息
		}
		m_bDrawCaret = true;
		Invalidate();
	}

	void CEditWndSDL::OnLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		if (m_bDragging)
		{
			m_bDragging = false;
			GetManager()->ReleaseCapture();
			// 最后更新一次光标位置（可选）
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			int newPos = GetCharPosFromPoint(pt);
			if (m_selStart == m_selEnd)
				m_selStart = m_cursorPos;
			m_selEnd = newPos;
			Invalidate();
		}
	}

	void CEditWndSDL::OnMouseMove(WPARAM wParam, LPARAM lParam)
	{
		if (!m_bDragging) return;

		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		int newPos = GetCharPosFromPoint(pt);
		if (newPos != m_cursorPos)
		{
			// 拖动时，保持光标位置不变，但更新选择终点
			if (m_selStart == m_selEnd)
				m_selStart = m_cursorPos;
			m_selEnd = newPos;
			Invalidate();
		}
	}

	void CEditWndSDL::OnSetFocus(WPARAM wParam, LPARAM lParam)
	{
		// 启动文本输入
		SDL_StartTextInput((SDL_Window*)m_hWnd);

		// 设置文本输入区域，并将光标偏移位置告知系统
		CDuiRect rcCaret = GetCaretPos();
		SDL_Rect rc = { rcCaret.left, rcCaret.right, rcCaret.GetWidth(), rcCaret.GetHeight() };
		SDL_Rect rcArea = CalculateCandidatePosition((SDL_Window*)m_hWnd, &rc, 0);
		SDL_SetTextInputArea((SDL_Window*)m_hWnd, &rcArea, 0);

		// 启动光标闪烁定时器
		GetManager()->SetTimer(m_pOwner, m_caretTimerID, 800);

		GetManager()->SetCursor(DUI_IBEAM);
	}

	void CEditWndSDL::OnKillFocus(WPARAM wParam, LPARAM lParam)
	{
		// 关闭光标闪烁
		GetManager()->KillTimer(m_pOwner, m_caretTimerID);
		// 清除输入
		SDL_ClearComposition((SDL_Window*)m_hWnd);
		// 清除文本输入区域设置
		SDL_SetTextInputArea((SDL_Window*)m_hWnd, NULL, 0);
		// 停止文本输入事件
		SDL_StopTextInput((SDL_Window*)m_hWnd);

		// 清空组合文本
		m_sComposition.Empty();
		m_compositionCursor = 0;

		GetManager()->SetCursor(DUI_ARROW);
		m_pOwner->GetManager()->SetFocus(NULL);
		Close();
	}

	void CEditWndSDL::OnTimer(WPARAM wParam, LPARAM lParam)
	{
		m_bDrawCaret = !m_bDrawCaret;
		Invalidate();
	}

	void CEditWndSDL::InsertText(const CDuiString& text)
	{
		if (IsReadOnly()) return;
		DeleteSelection();

		UINT maxChar = GetMaxChar();
		if (maxChar > 0 && m_sText.GetLength() + text.GetLength() > maxChar)
			return;

		m_sText.Insert(m_cursorPos, text);
		m_cursorPos += text.GetLength();
		m_selStart = m_selEnd = m_cursorPos;
		SetOwnerText(m_sText);
		GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
	}

	void CEditWndSDL::DeleteBackward()
	{
		if (IsReadOnly()) return;
		if (m_selStart != m_selEnd)
		{
			DeleteSelection();
			return;
		}
		if (m_cursorPos > 0)
		{
			m_sText.Delete(m_cursorPos - 1, 1);
			m_cursorPos--;
			m_selStart = m_selEnd = m_cursorPos;
			SetOwnerText(m_sText);
			GetOwner()->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
		}
	}

	void CEditWndSDL::DeleteForward()
	{
		if (IsReadOnly()) return;
		if (m_selStart != m_selEnd)
		{
			DeleteSelection();
			return;
		}
		if (m_cursorPos < m_sText.GetLength())
		{
			m_sText.Delete(m_cursorPos, 1);
			m_selStart = m_selEnd = m_cursorPos;
			SetOwnerText(m_sText);
			GetOwner()->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
		}
	}

	void CEditWndSDL::DeleteSelection()
	{
		if (m_selStart == m_selEnd) return;
		int start = MIN(m_selStart, m_selEnd);
		int end = MAX(m_selStart, m_selEnd);
		m_sText.Delete(start, end - start);
		m_cursorPos = start;
		m_selStart = m_selEnd = m_cursorPos;
		SetOwnerText(m_sText);
		GetOwner()->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
	}

	void CEditWndSDL::MoveCursor(int delta, bool extendSelection)
	{
		int newPos = m_cursorPos + delta;
		newPos = MAX(0, MIN(newPos, m_sText.GetLength()));
		if (extendSelection)
		{
			if (m_selStart == m_selEnd)
				m_selStart = m_cursorPos;
			m_selEnd = newPos;
		}
		else
		{
			m_selStart = m_selEnd = newPos;
		}
		m_cursorPos = newPos;
		Invalidate();
	}

	void CEditWndSDL::MoveCursorToHome(bool extendSelection)
	{
		MoveCursor(-m_cursorPos, extendSelection);
	}

	void CEditWndSDL::MoveCursorToEnd(bool extendSelection)
	{
		MoveCursor(m_sText.GetLength() - m_cursorPos, extendSelection);
	}

	void CEditWndSDL::SelectAll()
	{
		m_selStart = 0;
		m_selEnd = m_sText.GetLength();
		m_cursorPos = m_selEnd;
		Invalidate();
	}

	void CEditWndSDL::CopyToClipboard()
	{
		if (m_selStart != m_selEnd)
		{
			int start = MIN(m_selStart, m_selEnd);
			int end = MAX(m_selStart, m_selEnd);
			CDuiString selected = m_sText.Mid(start, end - start);
			CDuiStringUtf8 utf8(selected);
			SDL_SetClipboardText(utf8.toString());
		}
	}

	void CEditWndSDL::CutToClipboard()
	{
		if (IsReadOnly()) return;
		CopyToClipboard();
		DeleteSelection();
	}

	void CEditWndSDL::PasteFromClipboard()
	{
		if (IsReadOnly()) return;
		char* utf8 = SDL_GetClipboardText();
		if (utf8)
		{
			CDuiString text = CDuiStringUtf8(utf8);
			SDL_free(utf8);
			InsertText(text);
		}
	}

	int CEditWndSDL::GetCharPosFromPoint(const POINT& pt)
	{
		// 计算鼠标点击位置对应的字符索引（精确测量）
		CDuiRect rcClient;
		GetClientRect(&rcClient);
		int x = pt.x - m_pOwner->GetTextPadding().left;
		if (x <= 0) return 0;

		// 获取字体指针
		UIFont* pFont = GetManager()->GetFont(m_pOwner->GetFont());
		if (!pFont) pFont = GetManager()->GetDefaultFontInfo();
		TTF_Font* ttfFont = (TTF_Font*)pFont->GetHandle();
		if (!ttfFont) return 0;

		// 逐字符测量宽度，找到 x 坐标所在的字符位置
		CDuiString displayText = m_sText;
		if (IsPasswordMode())
		{
			CDuiString masked;
			for (int i = 0; i < displayText.GetLength(); ++i)
				masked += GetPasswordChar();
			displayText = masked;
		}

		int curX = 0;
		for (int i = 0; i < displayText.GetLength(); ++i)
		{
			CDuiString ch = displayText.Mid(i, 1);
			CDuiStringUtf8 utf8(ch);
			int w, h;
			TTF_GetStringSize(ttfFont, utf8.toString(), utf8.GetLength(), &w, &h);
			if (x < curX + w / 2) // 中点判断
				return i;
			curX += w;
		}
		return displayText.GetLength();
	}

	int CEditWndSDL::GetCharXPos(int charIndex)
	{
		if (charIndex <= 0) return m_pOwner->GetTextPadding().left;

		CDuiString textBefore = m_sText.Left(charIndex);
		if (IsPasswordMode())
		{
			CDuiString masked;
			for (int i = 0; i < textBefore.GetLength(); ++i)
				masked += GetPasswordChar();
			textBefore = masked;
		}

		UIFont* pFont = GetManager()->GetFont(m_pOwner->GetFont());
		if (!pFont) pFont = GetManager()->GetDefaultFontInfo();
		TTF_Font* ttfFont = (TTF_Font*)pFont->GetHandle();
		if (!ttfFont) return m_pOwner->GetTextPadding().left;

		CDuiStringUtf8 utf8(textBefore);
		int w, h;
		TTF_GetStringSize(ttfFont, utf8.toString(), utf8.GetLength(), &w, &h);
		return m_pOwner->GetTextPadding().left + w;
	}

	CDuiRect CEditWndSDL::GetCaretPos()
	{
		CDuiRect rcCaret;
		int x = GetCharXPos(m_cursorPos);
		if (x < 0) return rcCaret;

		CDuiRect rcClient;
		GetClientRect(&rcClient);
		rcClient.SetPadding(GetOwner()->GetTextPadding());

		UIFont* pFont = GetManager()->GetFont(m_pOwner->GetFont());
		if (!pFont) pFont = GetManager()->GetDefaultFontInfo();
		int caretHeight = pFont->GetHeight(GetManager());
		caretHeight += 6;
		if (caretHeight > rcClient.GetHeight()) caretHeight = rcClient.GetHeight();

		rcCaret.left = x;
		rcCaret.right = x + 1;
		rcCaret.top = rcClient.top + (rcClient.GetHeight() - caretHeight) / 2;
		rcCaret.bottom = rcCaret.top + caretHeight;
		return rcCaret;
	}
}
#endif // DUILIB_SDL

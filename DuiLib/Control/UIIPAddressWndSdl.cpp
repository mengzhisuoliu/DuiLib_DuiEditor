#include "StdAfx.h"
#include "UIIPAddressWndSdl.h"

#ifdef DUILIB_SDL
#include <algorithm>

namespace DuiLib
{

CIPAddressWndSDL::CIPAddressWndSDL()
	: m_pOwner(nullptr)
	, m_nIndex(0)
	, m_bDrawCaret(true)
	, m_caretTimerID(101)
	, m_bDragging(false)
{
	m_dwOldIp = 0;
	m_nFontHeight = 18;
	m_nCaretHeight = 18;
}

CIPAddressWndSDL::~CIPAddressWndSDL()
{
}

DWORD CIPAddressWndSDL::GetLocalIpAddress()
{
	return UIMAKEIPADDRESS(127, 0, 0, 1);
}

void CIPAddressWndSDL::Init(CIPAddressUI* pOwner)
{
	m_pOwner = pOwner;
	m_pOwner->m_nIPUpdateFlag = IP_NONE;

	// 获取当前 IP 值
	m_dwOldIp = m_pOwner->GetIP();
	m_vSegments[0].sValue = HIBYTE(HIWORD(m_dwOldIp));
	m_vSegments[1].sValue = LOBYTE(HIWORD(m_dwOldIp));
	m_vSegments[2].sValue = HIBYTE(LOWORD(m_dwOldIp));
	m_vSegments[3].sValue = LOBYTE(LOWORD(m_dwOldIp));


	m_nIndex = 0;

	if (m_hWnd == NULL)
	{
		CDuiRect rc = CalPos();
		CWindowSDL* pParentWnd = (CWindowSDL*)m_pOwner->GetManager()->GetWindow();
		UIWND pWindow = Create(pParentWnd->GetHWND(), NULL, UI_WNDSTYLE_CHILD, 0,
			rc.left, rc.top, rc.GetWidth(), rc.GetHeight());
		if (!pWindow) return;
	}

	// 显示窗口并获取焦点
	ShowWindow(true, true);
	SDL_RaiseWindow(m_hWnd);
	SDL_StartTextInput(m_hWnd);

	// 启动光标闪烁定时器
	GetManager()->SetTimer(m_pOwner, m_caretTimerID, 800);
}

CDuiRect CIPAddressWndSDL::CalPos()
{
	CDuiRect rcPos = m_pOwner->GetPos();
	rcPos.Deflate(1, 1, 1, 1);
	return rcPos;
}

void CIPAddressWndSDL::OnFinalMessage(UIWND hWnd)
{
	m_pOwner->m_pWindow = NULL;
	// 通知主控件更新 IP 值
	m_pOwner->SetIP(UIMAKEIPADDRESS(m_vSegments[0].sValue.toInt(), m_vSegments[1].sValue.toInt(),
		m_vSegments[2].sValue.toInt(), m_vSegments[3].sValue.toInt()));
	m_pOwner->Invalidate();
	delete this;
}

uiBool CIPAddressWndSDL::OnSdlEvent(const void* pEvent)
{
	SDL_Event* ev = (SDL_Event*)pEvent;

	if (ev->type == SDL_EVENT_TEXT_INPUT)
	{
		// 普通文本输入：只接受数字
		CDuiString s = CDuiStringUtf8(ev->text.text);
		if (s.GetLength() == 1 && s[0] >= _T('0') && s[0] <= _T('9'))
		{
			if (m_bShowCaret)
				DeleteSelection();
			else
				m_vSegments[m_nIndex].sValue.Empty(); //全选状态时，输入前先清空

			int n = m_vSegments[m_nIndex].sValue.toInt();
			n = n * 10 + s.toInt();
			if (n > 255)
			{
				m_vSegments[m_nIndex].sValue = 255;
			}
			else if (n > 99) //3位数，进入下一段
			{
				m_vSegments[m_nIndex].sValue = n;
				SetFocusSegment(m_nIndex + 1, uiFalse);
			}
			else
			{
				m_vSegments[m_nIndex].sValue = n;
				m_vSegments[m_nIndex].curPos++;
				if (!m_bShowCaret) //全选时，输入一个字符，显示光标，去除全选状态
				{
					m_bShowCaret = uiTrue;
				}
			}
			Invalidate();
		}
		else if (ev->text.text[0] == '.')
		{
			SetFocusSegment(m_nIndex + 1, uiFalse);
		}
		return uiTrue;
	}
	else if (ev->type == SDL_EVENT_WINDOW_MOUSE_ENTER)
	{
		GetManager()->SetCursor(DUI_IBEAM);
		return uiTrue;
	}
	else if (ev->type == SDL_EVENT_WINDOW_MOUSE_LEAVE)
	{
		GetManager()->SetCursor(DUI_ARROW);
		return uiTrue;
	}
	else if (ev->type == SDL_EVENT_WINDOW_EXPOSED)
	{
		CDuiRect rcClient;
		GetClientRect(&rcClient);

		rcClient.SetPadding(m_pOwner->GetTextPadding());

		CDuiRect rcPointBase = rcClient;
		GetManager()->Render()->DrawText(rcPointBase, CDuiRect(), 
			_T("."), CDuiColor(), m_pOwner->GetFont(),
			DT_CALCRECT| DT_CENTER | DT_VCENTER | DT_SINGLELINE, 
			CDuiColor());

		m_nFontHeight = GetManager()->GetFontHeight(m_pOwner->GetFont());
		m_nCaretHeight = m_nFontHeight + 6;
		if (m_nCaretHeight > rcClient.GetHeight()) m_nCaretHeight = rcClient.GetHeight();

		int segmentsPerSize = (rcClient.GetWidth() - (rcPointBase.GetWidth() * 3)) / 4;
		int right = rcClient.left;
		for (int i=0; i<4; i++)
		{
			m_vSegments[i].rc.left = right;  right += segmentsPerSize;
			m_vSegments[i].rc.top = rcClient.top;
			m_vSegments[i].rc.right = right;
			m_vSegments[i].rc.bottom = rcClient.bottom;

			if (i < 3)
			{
				m_rcPoints[i].left = right;		right += rcPointBase.GetWidth();
				m_rcPoints[i].top = rcClient.top;
				m_rcPoints[i].right = right;
				m_rcPoints[i].bottom = rcClient.bottom;
			}
		}
		return uiFalse;
	}
	return uiFalse;
}

LRESULT CIPAddressWndSDL::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	m_pm.SetForceUseSharedRes(true);
	m_pm.Init(m_hWnd, NULL, this);
	ShowAndActivateChildWindow();
	return 0;
}

LRESULT CIPAddressWndSDL::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	CDuiRect rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsEmpty()) return 0;

	UIRender* pRender = GetManager()->Render();
	pRender->BeginPaint();
	pRender->Resize(rcClient);

	// 背景
	pRender->DrawColor(rcClient, CDuiSize(0, 0), 0xFFFFFFFF);

	// 字体
	CDuiColor textColor = m_pOwner->GetTextColor();
	if (textColor == 0) textColor = GetManager()->GetDefaultFontColor();

	// 四个段
	for (int i = 0; i < 4; i++)
	{
		if (m_nIndex == i)
		{
			if (m_bShowCaret)
			{
				//绘制字段内的选择区域
				if (m_vSegments[i].selStart != m_vSegments[i].selEnd)
				{
					int start = MIN(m_vSegments[i].selStart, m_vSegments[i].selEnd);
					int end = MAX(m_vSegments[i].selStart, m_vSegments[i].selEnd);
					int xStart = GetCharXPos(start);
					int xEnd = GetCharXPos(end);
					CDuiRect rcSel;
					rcSel.left = xStart;
					rcSel.right = xEnd;
					CDuiRect rcCaret = GetCaretPos(); //用光标的高度来绘制高亮区域
					rcSel.top = rcCaret.top;
					rcSel.bottom = rcCaret.bottom;
					CDuiColor selColor(255, 0, 120, 215); //0xFF3399FF; // 选择高亮色
					pRender->DrawColor(rcSel, CDuiSize(0, 0), selColor);

					//居中显示,先计算出绘制文本的区域
					CDuiSize szTotalString = GetManager()->Render()->GetTextSize(m_vSegments[i].sValue.toString(),
						m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					int curX = m_vSegments[i].rc.left + (m_vSegments[i].rc.GetWidth() - szTotalString.cx) / 2;

					// 逐字符绘制
					CDuiString displayText = m_vSegments[i].sValue;
					for (int i = 0; i < displayText.GetLength(); ++i)
					{
						CDuiString ch = displayText.Mid(i, 1);
						CDuiSize sz = pRender->GetTextSize(ch.toString(), m_pOwner->GetFont(), DT_LEFT | DT_VCENTER);
						if (sz.cy == 0) continue;

						// 判断当前字符是否在选中范围内
						bool selected = false;
						if (i >= start && i < end)
							selected = true;

						CDuiColor color = selected ? CDuiColor::White : textColor;
						CDuiRect rcChar = m_vSegments[i].rc;
						rcChar.left = curX;
						rcChar.right = rcChar.left + sz.cx;
						pRender->DrawText(rcChar, CDuiRect(0, 0, 0, 0), ch, color, m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

						curX += sz.cx;
					}
				}
				else
				{
					pRender->DrawText(m_vSegments[i].rc, CDuiRect(), m_vSegments[i].sValue, textColor, m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
			else
			{
				// 全选状态时, 绘制高亮背景
				CDuiColor selColor(255, 0, 120, 215);
				CDuiRect rcSel = m_vSegments[i].rc;
				rcSel.top = (rcClient.GetHeight() - m_nCaretHeight) / 2;
				rcSel.bottom = rcSel.top + m_nCaretHeight;
				pRender->DrawColor(rcSel, CDuiSize(), selColor);

				CDuiColor selectedColor = 0xFFFFFFFF; // 反白显示为白色
				pRender->DrawText(m_vSegments[i].rc, CDuiRect(), m_vSegments[i].sValue, selectedColor, m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}			
		}
		else
		{
			pRender->DrawText(m_vSegments[i].rc, CDuiRect(), m_vSegments[i].sValue, textColor, m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	//3个点
	for (int i=0; i<3; i++)
	{
		pRender->DrawText(m_rcPoints[i], CDuiRect(), _T("."), textColor, m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	// 绘制光标（在焦点段末尾）
	if (m_bShowCaret && m_bDrawCaret)
	{
		CDuiRect rcCaret = GetCaretPos();
		pRender->DrawRect(rcCaret, 1, 0xFF000000);
	}

	pRender->EndPaint();
	return 0;
}

LRESULT CIPAddressWndSDL::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	switch (wParam)
	{
		case VK_LEFT:
			MoveLeft();
			break;
		case VK_RIGHT:
			MoveRight();
			break;
		case VK_HOME:
			MoveHome();
			break;
		case VK_END:
			MoveEnd();
			break;
		case VK_BACK:
			DeleteBackward();
			break;
		case VK_DELETE:
			DeleteForward();
			break;
		case VK_RETURN:
			m_pOwner->GetManager()->SetFocus(NULL);
			Close();
			break;
		case VK_ESCAPE:
			m_vSegments[0].sValue = HIBYTE(HIWORD(m_dwOldIp));
			m_vSegments[1].sValue = LOBYTE(HIWORD(m_dwOldIp));
			m_vSegments[2].sValue = HIBYTE(LOWORD(m_dwOldIp));
			m_vSegments[3].sValue = LOBYTE(LOWORD(m_dwOldIp));
			m_pOwner->GetManager()->SetFocus(NULL);
			Close();  // 取消编辑，不保存
			break;
// 		case VK_NUMPAD1:
// 			if (!GetManager()->IsNumberLockKeyOn())
// 			{
// 				MoveEnd();
// 			}
// 			break;
// 		case VK_NUMPAD4:
// 			if (!GetManager()->IsNumberLockKeyOn())
// 			{
// 				MoveLeft();
// 			}
// 			break;
// 		case VK_NUMPAD6:
// 			if (!GetManager()->IsNumberLockKeyOn())
// 			{
// 				MoveRight();
// 			}
// 			break;
// 		case VK_NUMPAD7:
// 			if (!GetManager()->IsNumberLockKeyOn())
// 			{
// 				MoveHome();
// 			}
// 			break;
		default:
			break;
	}
	Invalidate();
	return 0;
}

LRESULT CIPAddressWndSDL::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	CDuiPoint pt(lParam);

	for (int i=0; i<4; i++)
	{
		if (m_vSegments[i].rc.PtInRect(pt))
		{
			if (i == m_nIndex)
			{
				SetFocusSegment(i, uiTrue);
			}
			else
			{
				SetFocusSegment(i, uiTrue);
				//return 0;
			}
			break;
		}
	}

	//点击位置是已经获得焦点的字段，取消高亮显示，需要显示光标
	int newPos = GetCharPosFromPoint(pt);
	m_vSegments[m_nIndex].curPos = newPos;

	m_vSegments[m_nIndex].selStart = m_vSegments[m_nIndex].selEnd = m_vSegments[m_nIndex].curPos;
	m_bDragging = true;
	GetManager()->SetCapture();
	return 0;
}

LRESULT CIPAddressWndSDL::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		GetManager()->ReleaseCapture();

		CDuiPoint pt(lParam);
		int newPos = GetCharPosFromPoint(pt);
		if (m_vSegments[m_nIndex].selStart == m_vSegments[m_nIndex].selEnd)
			m_vSegments[m_nIndex].selStart = m_vSegments[m_nIndex].curPos;
		m_vSegments[m_nIndex].selEnd = newPos;
		Invalidate();
	}
	return 0;
}

LRESULT CIPAddressWndSDL::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	if (!m_bDragging) return 0;

	CDuiPoint pt(lParam);
	int newPos = GetCharPosFromPoint(pt);
	if (newPos != m_vSegments[m_nIndex].curPos)
	{
		if (m_vSegments[m_nIndex].selStart == m_vSegments[m_nIndex].selEnd)
			m_vSegments[m_nIndex].selStart = m_vSegments[m_nIndex].curPos;
		m_vSegments[m_nIndex].selEnd = newPos;
		Invalidate();
	}
	return 0;
}

LRESULT CIPAddressWndSDL::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	SDL_StartTextInput(m_hWnd);
	GetManager()->SetCursor(DUI_IBEAM);
	return 0;
}

LRESULT CIPAddressWndSDL::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	GetManager()->KillTimer(m_pOwner, m_caretTimerID);
	SDL_StopTextInput(m_hWnd);
	GetManager()->SetCursor(DUI_ARROW);
	m_pOwner->GetManager()->SetFocus(NULL);
	Close();
	return 0;
}

LRESULT CIPAddressWndSDL::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	m_bDrawCaret = !m_bDrawCaret;
	Invalidate();
	return 0;
}

void CIPAddressWndSDL::SetFocusSegment(int index, uiBool bDrawCaret)
{
	if (index >= 4)
		m_nIndex = 0;
	else
		m_nIndex = index;
	m_bShowCaret = bDrawCaret;
	Invalidate();
}

void CIPAddressWndSDL::DeleteBackward()
{
	if (!m_bShowCaret) //全选状态
	{
		if (m_vSegments[m_nIndex].sValue != _T("0"))
		{
			m_vSegments[m_nIndex].sValue = _T("0");
			Invalidate();
		}
		else
		{
			//当前是0, 退格键回退到上一个字段
			if (m_nIndex != 0)
			{
				SetFocusSegment(m_nIndex - 1, uiFalse);
				Invalidate();
			}
		}
		return;
	}

	//当前是0, 退格键回退到上一个字段
	if (m_vSegments[m_nIndex].sValue == _T("0"))
	{
		if (m_nIndex > 0)
		{
			SetFocusSegment(m_nIndex - 1, uiFalse);
			Invalidate();
		}
		return;
	}

	//光标在最前面
	if (m_vSegments[m_nIndex].curPos <= 0)
		return;

	if (m_vSegments[m_nIndex].selStart != m_vSegments[m_nIndex].selEnd)
	{
		DeleteSelection();
		Invalidate();
		return;
	}

	//正常删除一个字符
	m_vSegments[m_nIndex].sValue.Delete(m_vSegments[m_nIndex].curPos - 1, 1);
	m_vSegments[m_nIndex].curPos--;

	//如果为空,必须填入0
	if (m_vSegments[m_nIndex].sValue.IsEmpty())
	{
		m_vSegments[m_nIndex].sValue = _T("0");
		m_vSegments[m_nIndex].curPos = 1;
	}
	
	Invalidate();
}

void CIPAddressWndSDL::DeleteForward()
{
	if (m_vSegments[m_nIndex].sValue == _T("0"))
		return;

	if (!m_bShowCaret) //全选状态
	{
		if (m_vSegments[m_nIndex].sValue != _T("0"))
		{
			m_vSegments[m_nIndex].sValue = _T("0");
			Invalidate();
		}
		return;
	}

	//光标在最后面
	if (m_vSegments[m_nIndex].curPos >= m_vSegments[m_nIndex].sValue.GetLength())
		return;

	if (m_vSegments[m_nIndex].selStart != m_vSegments[m_nIndex].selEnd)
	{
		DeleteSelection();
		Invalidate();
		return;
	}

	//正常删除一个字符
	m_vSegments[m_nIndex].sValue.Delete(m_vSegments[m_nIndex].curPos, 1);

	//如果为空,必须填入0
	if (m_vSegments[m_nIndex].sValue.IsEmpty())
	{
		m_vSegments[m_nIndex].sValue = _T("0");
	}

	Invalidate();
}

void CIPAddressWndSDL::DeleteSelection()
{
	if (m_vSegments[m_nIndex].selStart == m_vSegments[m_nIndex].selEnd) return;
	int start = MIN(m_vSegments[m_nIndex].selStart, m_vSegments[m_nIndex].selEnd);
	int end = MAX(m_vSegments[m_nIndex].selStart, m_vSegments[m_nIndex].selEnd);
	m_vSegments[m_nIndex].sValue.Delete(start, end - start);
	m_vSegments[m_nIndex].curPos = start;
	m_vSegments[m_nIndex].selStart = m_vSegments[m_nIndex].selEnd = m_vSegments[m_nIndex].curPos;
}

void CIPAddressWndSDL::MoveLeft()
{
	if (!m_bShowCaret) //全选时
	{
		if (m_nIndex > 0)
			SetFocusSegment(m_nIndex - 1, uiFalse);
		return;
	}

	//最前面的字符,移动到前一个字段
	if (m_vSegments[m_nIndex].curPos <= 0)
	{
		if (m_nIndex > 0)
			SetFocusSegment(m_nIndex - 1, uiFalse);
		return;
	}

	m_vSegments[m_nIndex].curPos--;
}

void CIPAddressWndSDL::MoveRight()
{
	if (!m_bShowCaret) //全选时
	{
		if (m_nIndex < 3)
			SetFocusSegment(m_nIndex + 1, uiFalse);
		return;
	}

	//最后面的字符,移动到后一个字段
	if (m_vSegments[m_nIndex].curPos >= m_vSegments[m_nIndex].sValue.GetLength())
	{
		if (m_nIndex < 3)
			SetFocusSegment(m_nIndex + 1, uiFalse);
		return;
	}

	m_vSegments[m_nIndex].curPos++;
}

void CIPAddressWndSDL::MoveHome()
{
	SetFocusSegment(0, uiFalse);
}

void CIPAddressWndSDL::MoveEnd()
{
	SetFocusSegment(3, uiFalse);
}

int CIPAddressWndSDL::GetCharPosFromPoint(const CDuiPoint& pt)
{
	CDuiRect rcClient = m_vSegments[m_nIndex].rc;
	CDuiSize szTotalString = GetManager()->Render()->GetTextSize(m_vSegments[m_nIndex].sValue.toString(), 
		m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	CDuiRect rcTotalText = rcClient;
	rcTotalText.left = m_vSegments[m_nIndex].rc.left + (rcClient.GetWidth() - szTotalString.cx) / 2;
	rcTotalText.right = rcTotalText.left + szTotalString.cx;

	int x = pt.x - rcTotalText.left;

	// 逐字符测量宽度，找到 x 坐标所在的字符位置
	CDuiString displayText = m_vSegments[m_nIndex].sValue;

	int curX = 0;
	for (int i = 0; i < displayText.GetLength(); ++i)
	{
		CDuiString ch = displayText.Mid(i, 1);
		CDuiSize sz = GetManager()->Render()->GetTextSize(ch.toString(), m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		CDuiRect rcText;
		if (x < curX + sz.cx / 2) // 中点判断
			return i;
		curX += sz.cx;
	}
	return displayText.GetLength();
}

int CIPAddressWndSDL::GetCharXPos(int charIndex)
{
	CDuiRect rcClient = m_vSegments[m_nIndex].rc;
	CDuiSize szTotalString = GetManager()->Render()->GetTextSize(m_vSegments[m_nIndex].sValue.toString(),
		m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	CDuiRect rcTotalText = rcClient;
	rcTotalText.left = m_vSegments[m_nIndex].rc.left + (rcClient.GetWidth() - szTotalString.cx) / 2;
	rcTotalText.right = rcTotalText.left + szTotalString.cx;

	if (charIndex <= 0) return rcTotalText.left;

	CDuiString textBefore = m_vSegments[m_nIndex].sValue.Left(charIndex);
	CDuiSize sz = GetManager()->Render()->GetTextSize(textBefore.toString(), m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	return sz.cx + rcTotalText.left;
}

CDuiRect CIPAddressWndSDL::GetCaretPos()
{
	CDuiRect rcCaret;
	int x = GetCharXPos(m_vSegments[m_nIndex].curPos);
	if (x < 0) return rcCaret;

	CDuiRect rcClient = m_vSegments[m_nIndex].rc;

	rcCaret.left = x;
	rcCaret.right = x + 1;
	rcCaret.top = rcClient.top + (rcClient.GetHeight() - m_nCaretHeight) / 2;
	rcCaret.bottom = rcCaret.top + m_nCaretHeight;
	return rcCaret;
}

} // namespace DuiLib
#endif // DUILIB_SDL

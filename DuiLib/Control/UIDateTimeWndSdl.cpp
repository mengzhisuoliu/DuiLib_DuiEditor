#include "StdAfx.h"
#include "UIDateTimeWndSdl.h"
#include "../Utils/UIDialogCalendar.h"

#ifdef DUILIB_SDL

namespace DuiLib
{

void CDateTimeWndSDL::GetDayOfWeek(SYSTEMTIME &st)
{
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	st.wYear -= st.wMonth < 3;
	st.wDayOfWeek = (st.wYear + st.wYear / 4 - st.wYear / 100 + st.wYear / 400 + t[st.wMonth - 1] + st.wDay) % 7;
}

int CDateTimeWndSDL::GetDayOfWeek(int y, int m, int d)
{
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	y -= m < 3;
	return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

CDateTimeWndSDL::CDateTimeWndSDL()
{
	m_pOwner = NULL;
	m_uFormatStyle = DTS_TIMEFORMAT;
	m_nIndex = 0;
	m_sSeparator = _T("-");
	m_pWindowDropDown = NULL;

	memset(&m_TimerUpButton, 0, sizeof(TIMERINFO));
	memset(&m_TimerDownButton, 0, sizeof(TIMERINFO));
}

void CDateTimeWndSDL::Init(CDateTimeUI* pOwner, CDuiRect rcBase, UINT uFormatStyle)
{
	m_pOwner = pOwner;
	m_uFormatStyle = uFormatStyle;
	m_oldDateTime = m_pOwner->GetTime();

	if (m_uFormatStyle == DTS_TIMEFORMAT)
	{
		m_vSegments[0].sValue = m_oldDateTime.wHour;
		m_vSegments[1].sValue = m_oldDateTime.wMinute;
		m_vSegments[2].sValue = m_oldDateTime.wSecond;
		m_sSeparator = _T(":");
	}
	else
	{
		m_vSegments[0].sValue = m_oldDateTime.wYear;
		m_vSegments[1].sValue = m_oldDateTime.wMonth;
		m_vSegments[2].sValue = m_oldDateTime.wDay;
		m_sSeparator = _T("-");
	}

	m_nIndex = 0;

	CDuiRect rc = CalPos(rcBase);
	CWindowSDL* pParentWnd = (CWindowSDL*)m_pOwner->GetManager()->GetWindow();
	UIWND pWindow = Create(pParentWnd->GetHWND(), NULL, UI_WNDSTYLE_CHILD, 0,
		rc.left, rc.top, rc.GetWidth(), rc.GetHeight());
	if (!pWindow) return;

	// 显示窗口并获取焦点
	ShowWindow(true, true);
	SDL_RaiseWindow(m_hWnd);
	SDL_StartTextInput(m_hWnd);
}

void CDateTimeWndSDL::OnFinalMessage(UIWND hWnd)
{
	m_pOwner->m_pWindowDate = NULL;
	m_pOwner->m_pWindowTime = NULL;

	SYSTEMTIME st = m_oldDateTime;
	if (m_uFormatStyle == DTS_TIMEFORMAT)
	{
		st.wHour = m_vSegments[0].sValue.toInt();
		st.wMinute = m_vSegments[1].sValue.toInt();
		st.wSecond = m_vSegments[2].sValue.toInt();
		GetDayOfWeek(st);
	}
	else
	{
		st.wYear = m_vSegments[0].sValue.toInt();
		st.wMonth = m_vSegments[1].sValue.toInt();
		st.wDay = m_vSegments[2].sValue.toInt();
	}

	m_pOwner->SetTime(&st);
	m_pOwner->Invalidate();
	m_pOwner->GetManager()->SetFocus(NULL);
	delete this;
}

CDuiRect CDateTimeWndSDL::CalPos(CDuiRect rcBase)
{
	CDuiRect rcPos = rcBase;//m_pOwner->GetPos();
	rcPos.Deflate(1, 1, 1, 1);

	CControlUI* pParent = m_pOwner;
	CDuiRect rcParent;
	while (pParent = pParent->GetParent())
	{
		if (!pParent->IsVisible())
		{
			rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
			break;
		}
		rcParent = pParent->GetClientPos();
		if (!rcPos.Intersect(rcPos, rcParent))
		{
			rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
			break;
		}
	}

	return rcPos;
}

void CDateTimeWndSDL::SetDate(int year, int month, int day)
{
	if (m_uFormatStyle != DTS_TIMEFORMAT)
	{
		m_vSegments[0].sValue = year;
		m_vSegments[1].sValue = month;
		m_vSegments[2].sValue = day;
	}
}

BOOL CDateTimeWndSDL::OnSdlEvent(const void* pEvent)
{
	SDL_Event* ev = (SDL_Event*)pEvent;

	if (ev->type == SDL_EVENT_TEXT_INPUT)
	{
		// 普通文本输入：只接受数字
		CDuiString s = CDuiStringUtf8(ev->text.text);
		if (s.GetLength() == 1 && s[0] >= _T('0') && s[0] <= _T('9'))
		{
			int n = m_vSegments[m_nIndex].sValue.toInt();
			n = n * 10 + s.toInt();
			if (CheckInput(n))
			{
				m_vSegments[m_nIndex].sValue = n;
			}
			else // 输入不合法, 只使用当前的输入
			{
				m_vSegments[m_nIndex].sValue = s;
			}
			Invalidate();
		}
		else if (ev->text.text[0] == '.' ||
			ev->text.text[0] == ',' ||
			ev->text.text[0] == '/')
		{
			SetFocusSegment(m_nIndex + 1);
		}
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
	else if (ev->type == SDL_EVENT_WINDOW_EXPOSED)
	{
		CDuiRect rcClient;
		GetClientRect(&rcClient);
		rcClient.SetPadding(m_pOwner->GetTextPadding());

		CDuiRect rcPointBase = rcClient;
		GetManager()->Render()->DrawText(rcPointBase, CDuiRect(),
			m_sSeparator, CDuiColor(), m_pOwner->GetFont(),
			DT_CALCRECT | DT_CENTER | DT_VCENTER | DT_SINGLELINE,
			CDuiColor());

		int btnWidth = 20;// rcClient.GetHeight() - 4;
		int btnPadding = 2;

		if (m_uFormatStyle == DTS_TIMEFORMAT)
		{
			int btnHeight = (rcClient.GetHeight() - 4) / 2;

			// 上按钮矩形
			m_rcUpButton.left = rcClient.right - btnWidth - btnPadding;
			m_rcUpButton.top = rcClient.top + btnPadding;
			m_rcUpButton.right = rcClient.right - btnPadding;
			m_rcUpButton.bottom = m_rcUpButton.top + btnHeight;

			// 下按钮矩形
			m_rcDownButton.left = rcClient.right - btnWidth - btnPadding;
			m_rcDownButton.top = m_rcUpButton.bottom;
			m_rcDownButton.right = rcClient.right - btnPadding;
			m_rcDownButton.bottom = m_rcDownButton.top + btnHeight;
		}
		else
		{
			// 日期模式：下拉按钮（一个向下箭头）
			m_rcDropDownButton.left = rcClient.right - btnWidth - btnPadding;
			m_rcDropDownButton.top = rcClient.top + btnPadding;
			m_rcDropDownButton.right = rcClient.right - btnPadding;
			m_rcDropDownButton.bottom = rcClient.bottom - btnPadding;
		}

		//每个字段的宽度
		int segmentsPerSize = (rcClient.GetWidth() - btnWidth - (rcPointBase.GetWidth() * 2)) / 3;

		int right = rcClient.left;
		for (int i = 0; i < 3; i++)
		{
			m_vSegments[i].rc.left = right;  right += segmentsPerSize;
			m_vSegments[i].rc.top = rcClient.top;
			m_vSegments[i].rc.right = right;
			m_vSegments[i].rc.bottom = rcClient.bottom;

			if (i < 2)
			{
				m_rcPoints[i].left = right;		right += rcPointBase.GetWidth();
				m_rcPoints[i].top = rcClient.top;
				m_rcPoints[i].right = right;
				m_rcPoints[i].bottom = rcClient.bottom;
			}
		}
		return FALSE;
	}
	return FALSE;
}

LRESULT CDateTimeWndSDL::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pm.SetForceUseSharedRes(true);
	m_pm.Init(m_hWnd, NULL, this);
	ShowWindow();
	Invalidate();
	return 0;
}

LRESULT CDateTimeWndSDL::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
	for (int i = 0; i < 3; i++)
	{
		if (m_nIndex == i)
		{
			// 全选状态时, 绘制高亮背景
			CDuiColor selColor(255, 0, 120, 215);
			CDuiRect rcText = m_vSegments[i].rc;
			pRender->DrawText(rcText, CDuiRect(), m_vSegments[i].sValue, CDuiColor(), m_pOwner->GetFont(), DT_CALCRECT | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pRender->DrawColor(rcText, CDuiSize(), selColor);

			CDuiColor selectedColor = 0xFFFFFFFF; // 反白显示为白色
			pRender->DrawText(m_vSegments[i].rc, CDuiRect(), m_vSegments[i].sValue, selectedColor, m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		else
		{
			pRender->DrawText(m_vSegments[i].rc, CDuiRect(), m_vSegments[i].sValue, textColor, m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	//2个点
	for (int i = 0; i < 2; i++)
	{
		pRender->DrawText(m_rcPoints[i], CDuiRect(), m_sSeparator, textColor, m_pOwner->GetFont(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	// 绘制按钮
	if (m_uFormatStyle == DTS_TIMEFORMAT)
	{
		// 绘制上按钮
		DrawButton(pRender, m_rcUpButton, m_stateUpButton, true);
		// 绘制下按钮
		DrawButton(pRender, m_rcDownButton, m_stateDownButton, false);
	}
	else
	{
		// 绘制下拉按钮（向下箭头）
		DrawDropDownButton(pRender, m_rcDropDownButton, m_stateDropDown);
	}

	pRender->EndPaint();
	return 0;
}

LRESULT CDateTimeWndSDL::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
			m_vSegments[0].sValue = m_oldDateTime.wYear;
			m_vSegments[1].sValue = m_oldDateTime.wMonth;
			m_vSegments[2].sValue = m_oldDateTime.wDay;
			m_pOwner->GetManager()->SetFocus(NULL);
			Close();  // 取消编辑，不保存
			break;
		default:
			break;
	}
	Invalidate();
	return 0;
}

LRESULT CDateTimeWndSDL::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDuiPoint pt(lParam);

	// 先检查是否点击了按钮
	if (m_uFormatStyle == DTS_TIMEFORMAT)
	{
		if (m_rcUpButton.PtInRect(pt))
		{
			m_stateUpButton.SetPushed(true);
			m_stateUpButton.SetCapture(true);

			GetManager()->SetCapture();
			if (m_TimerUpButton.uWinTimer == 0)
			{
				m_TimerUpButton.hWnd = m_hWnd;
				GetManager()->SetTimer(150, &m_TimerUpButton);
			}

			Invalidate();
			return 0;
		}
		if (m_rcDownButton.PtInRect(pt))
		{
			m_stateDownButton.SetPushed(true);
			m_stateDownButton.SetCapture(true);

			GetManager()->SetCapture();
			if (m_TimerDownButton.uWinTimer == 0)
			{
				m_TimerDownButton.hWnd = m_hWnd;
				GetManager()->SetTimer(150, &m_TimerDownButton);
			}

			Invalidate();
			return 0;
		}
	}
	else
	{
		if (m_rcDropDownButton.PtInRect(pt))
		{
			m_stateDropDown.SetPushed(true);
			m_stateDropDown.SetCapture(true);
			Invalidate();
			return 0;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (m_vSegments[i].rc.PtInRect(pt))
		{
			if (i == m_nIndex)
			{
				SetFocusSegment(i);
			}
			else
			{
				SetFocusSegment(i);
				//return 0;
			}
			break;
		}
	}
	return 0;
}

LRESULT CDateTimeWndSDL::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDuiPoint pt(lParam);

	// 时间模式的上下按钮
	if (m_uFormatStyle == DTS_TIMEFORMAT)
	{
		if (m_stateUpButton.IsPushed())
		{
			m_stateUpButton.SetPushed(false);
			m_stateUpButton.SetCapture(false);
			if (m_rcUpButton.PtInRect(pt))
				OnClickUpButton();

			//设置定时器递增
			GetManager()->ReleaseCapture();
			GetManager()->KillTimer(&m_TimerUpButton);
			m_TimerUpButton.uWinTimer = 0;

			Invalidate();
			return 0;
		}
		if (m_stateDownButton.IsPushed())
		{
			m_stateDownButton.SetPushed(false);
			m_stateDownButton.SetCapture(false);
			if (m_rcDownButton.PtInRect(pt))
				OnClickDownButton();

			//设置定时器递减
			GetManager()->ReleaseCapture();
			GetManager()->KillTimer(&m_TimerDownButton);
			m_TimerDownButton.uWinTimer = 0;
			Invalidate();
			return 0;
		}
	}
	else
	{
		if (m_stateDropDown.IsPushed())
		{
			m_stateDropDown.SetPushed(false);
			m_stateDropDown.SetCapture(false);
			if (m_rcDropDownButton.PtInRect(pt))
				OnClickDropDown();
			Invalidate();
			return 0;
		}
	}
	return 0;
}

LRESULT CDateTimeWndSDL::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDuiPoint pt(lParam);
	bool bHotChanged = false;
	bool bCursorChanged = false;

	if (m_uFormatStyle == DTS_TIMEFORMAT)
	{
		// 上按钮
		if (m_rcUpButton.PtInRect(pt))
		{
			if (!m_stateUpButton.IsHot()) { m_stateUpButton.SetHot(true); bHotChanged = true; }
			GetManager()->SetCursor(DUI_HAND);
			bCursorChanged = true;
		}
		else
		{
			if (m_stateUpButton.IsHot()) { m_stateUpButton.SetHot(false); bHotChanged = true; }
		}

		// 下按钮
		if (m_rcDownButton.PtInRect(pt))
		{
			if (!m_stateDownButton.IsHot()) { m_stateDownButton.SetHot(true); bHotChanged = true; }
			if (!bCursorChanged) { GetManager()->SetCursor(DUI_HAND); bCursorChanged = true; }
		}
		else
		{
			if (m_stateDownButton.IsHot()) { m_stateDownButton.SetHot(false); bHotChanged = true; }
		}
	}
	else
	{
		// 下拉按钮
		if (m_rcDropDownButton.PtInRect(pt))
		{
			if (!m_stateDropDown.IsHot()) { m_stateDropDown.SetHot(true); bHotChanged = true; }
			GetManager()->SetCursor(DUI_HAND);
			bCursorChanged = true;
		}
		else
		{
			if (m_stateDropDown.IsHot()) { m_stateDropDown.SetHot(false); bHotChanged = true; }
		}
	}

	// 如果鼠标不在任何按钮区域，恢复为 IBeam（编辑光标）
	if (!bCursorChanged)
	{
		GetManager()->SetCursor(DUI_IBEAM);
	}

	if (bHotChanged)
		Invalidate();

	return 0;
}

LRESULT CDateTimeWndSDL::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DUITRACE(_T("CDateTimeWndSDL::OnSetFocus wParam=%p"), wParam);
	SDL_StartTextInput(m_hWnd);
	GetManager()->SetCursor(DUI_IBEAM);
	return 0;
}

LRESULT CDateTimeWndSDL::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SDL_StopTextInput(m_hWnd);
	GetManager()->SetCursor(DUI_ARROW);

	if (m_pWindowDropDown && wParam == (WPARAM)m_pWindowDropDown->GetHWND())
		return 0;

	Close();
	return 0;
}

LRESULT CDateTimeWndSDL::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == m_TimerUpButton.uWinTimer)
	{
		OnClickUpButton();
		return 0;
	}
	else if(wParam == m_TimerDownButton.uWinTimer)
	{
		OnClickDownButton();
		return 0;
	}

	bHandled = FALSE;
	return 0;
}

void CDateTimeWndSDL::SetFocusSegment(int index)
{
	if (index >= 3)
		m_nIndex = 0;
	else
		m_nIndex = index;
	Invalidate();
}

void CDateTimeWndSDL::DeleteBackward()
{

}

void CDateTimeWndSDL::DeleteForward()
{

}

void CDateTimeWndSDL::DeleteSelection()
{

}

void CDateTimeWndSDL::MoveLeft()
{
	if (m_nIndex > 0)
		SetFocusSegment(m_nIndex - 1);
}

void CDateTimeWndSDL::MoveRight()
{
	if (m_nIndex < 2)
		SetFocusSegment(m_nIndex + 1);
	return;
}

void CDateTimeWndSDL::MoveHome()
{
	SetFocusSegment(0);
}

void CDateTimeWndSDL::MoveEnd()
{
	SetFocusSegment(2);
}

BOOL CDateTimeWndSDL::CheckInput(int n)
{
	if (m_uFormatStyle == DTS_TIMEFORMAT) // 时间选取框
	{
		if (m_nIndex == 0) // 小时
		{
			return (n >= 0) && (n < 24);
		}
		else if (m_nIndex == 1) // 分钟
		{
			return (n >= 0) && (n < 60);
		}
		else // 秒
		{
			return (n >= 0) && (n < 60);
		}
	}
	else // 日期选取框
	{
		if (m_nIndex == 0) // 年
		{
			return (n >= 1) && (n <= 9999);
		}
		else if (m_nIndex == 1) // 月
		{
			return (n >= 1) && (n <= 12);
		}
		else // 日
		{
			int nYear = m_vSegments[0].sValue.toInt(2000);
			int nMonth = m_vSegments[1].sValue.toInt(1);
			int maxDay = 31;

			switch (nMonth)
			{
				case 1:  // 1 月
				case 3:  // 3 月
				case 5:  // 5 月
				case 7:  // 7 月
				case 8:  // 8 月
				case 10: // 10 月
				case 12: // 12 月
					maxDay = 31;
					break;
				case 4:  // 4 月
				case 6:  // 6 月
				case 9:  // 9 月
				case 11: // 11 月
					maxDay = 30;
					break;
				case 2:  // 2 月（需判断润年）
					{
						// 润年规则：能被4整除且不能被100整除，或者能被400整除
						BOOL bLeap = ((nYear % 4 == 0) && (nYear % 100 != 0)) || (nYear % 400 == 0);
						maxDay = bLeap ? 29 : 28;
						break;
					}
				default:
					return FALSE; // 未知月份
			}
			return (n >= 1) && (n <= maxDay);
		}
	}

	return FALSE;
}

// 绘制上下微调按钮（箭头）
void CDateTimeWndSDL::DrawButton(UIRender* pRender, const CDuiRect& rc, const TButtonState& state, bool bUp)
{
	CDuiColor bkColor = 0xFFEEEEEE;
	CDuiColor borderColor = 0xFFAAAAAA;

	if (state.IsPushed())
	{
		bkColor = 0xFFCCCCCC;
	}
	else if (state.IsHot())
	{
		bkColor = 0xFFDDDDDD;
	}

	// 背景和边框
	pRender->DrawColor(rc, CDuiSize(), bkColor);
	pRender->DrawRect(rc, 1, borderColor);

	// 绘制箭头
	int centerX = (rc.left + rc.right) / 2;
	int centerY = (rc.top + rc.bottom) / 2;
	int arrowSize = min(rc.GetWidth(), rc.GetHeight()) / 5;

	CDuiPoint pts[3];
	if (bUp)
	{
		pts[0].SetPoint(centerX, centerY - arrowSize );
		pts[1].SetPoint(centerX - arrowSize, centerY + arrowSize );
		pts[2].SetPoint(centerX + arrowSize, centerY + arrowSize );
	}
	else
	{
		pts[0].SetPoint(centerX, centerY + arrowSize );
		pts[1].SetPoint(centerX - arrowSize, centerY - arrowSize );
		pts[2].SetPoint(centerX + arrowSize, centerY - arrowSize );
	}

	pRender->DrawLine(pts[0].x, pts[0].y, pts[1].x, pts[1].y, 1, 0xFF000000);
	pRender->DrawLine(pts[1].x, pts[1].y, pts[2].x, pts[2].y, 1, 0xFF000000);
	pRender->DrawLine(pts[2].x, pts[2].y, pts[0].x, pts[0].y, 1, 0xFF000000);
}

void CDateTimeWndSDL::DrawDropDownButton(UIRender* pRender, const CDuiRect& rc, const TButtonState& state)
{
	// 样式同上
	CDuiColor bkColor = 0xFFEEEEEE;
	CDuiColor borderColor = 0xFFAAAAAA;
	if (state.IsPushed())
	{
		bkColor = 0xFFCCCCCC;
	}
	else if (state.IsHot())
	{
		bkColor = 0xFFDDDDDD;
	}
	pRender->DrawColor(rc, CDuiSize(), bkColor);
	pRender->DrawRect(rc, 1, borderColor);

	// 向下箭头
	int centerX = (rc.left + rc.right) / 2;
	int centerY = (rc.top + rc.bottom) / 2;
	int arrowSize = min(rc.GetWidth(), rc.GetHeight()) / 5;

	CDuiPoint pts[3] = {
		{ centerX - arrowSize, centerY - arrowSize },
		{ centerX + arrowSize, centerY - arrowSize },
		{ centerX, centerY + arrowSize }
	};
	for (int i = 0; i < 3; ++i)
		pRender->DrawLine(pts[i].x, pts[i].y, pts[(i + 1) % 3].x, pts[(i + 1) % 3].y, 1, 0xFF000000);
}

void CDateTimeWndSDL::OnClickUpButton()
{
	// 增加当前段的值（小时/分钟/秒）
	int curValue = m_vSegments[m_nIndex].sValue.toInt();
	int newValue = curValue + 1;
	if (CheckInput(newValue))
	{
		m_vSegments[m_nIndex].sValue = newValue;
	}
	else
	{

	}
	Invalidate();
}

void CDateTimeWndSDL::OnClickDownButton()
{
	int curValue = m_vSegments[m_nIndex].sValue.toInt();
	int newValue = curValue - 1;
	if (CheckInput(newValue))
	{
		m_vSegments[m_nIndex].sValue = newValue;
	}
	Invalidate();
}

void CDateTimeWndSDL::OnClickDropDown()
{
// 	CDialogCalendar dlg;
// 	dlg.m_st.wYear = m_vSegments[0].sValue.toInt();
// 	dlg.m_st.wMonth = m_vSegments[1].sValue.toInt();
// 	dlg.m_st.wDay = m_vSegments[2].sValue.toInt();
// 	GetDayOfWeek(dlg.m_st);
// 	dlg.DoModal(m_hWnd);


	CDialogCalendar* dlg = new CDialogCalendar;
	dlg->m_pParentWindow = this;
	dlg->m_st.wYear = m_vSegments[0].sValue.toInt();
	dlg->m_st.wMonth = m_vSegments[1].sValue.toInt();
	dlg->m_st.wDay = m_vSegments[2].sValue.toInt();
	GetDayOfWeek(dlg->m_st);
	dlg->ShowDialog(m_hWnd);

	m_pWindowDropDown = dlg;

	CDuiRect rcPos = m_pOwner->GetPos();
	CDuiRect rcWndParent;
	m_pOwner->GetManager()->GetWindowRect(&rcWndParent);

	CDuiSize szDlg = dlg->GetManager()->GetInitSize();

	CDuiRect rcDlg;
	dlg->GetWindowRect(&rcDlg);
	rcDlg.left = rcWndParent.left + rcPos.left;
	rcDlg.top = rcWndParent.top + rcPos.bottom;
	dlg->SetWindowPos(rcDlg.left, rcDlg.top, szDlg.cx, szDlg.cy, 0);
}

} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

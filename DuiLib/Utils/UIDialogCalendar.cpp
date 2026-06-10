#include "StdAfx.h"
#include "UIDialogCalendar.h"
#include "../Control/UIDateTimeWndSdl.h"

#ifdef DUILIB_SDL
namespace DuiLib{

CDialogCalendar::CDialogCalendar(void)
{
	m_pParentWindow = NULL;

	m_bModal = false;

	m_sWeeks[0] = "Mon";  // 星期一
	m_sWeeks[1] = "Tue";  // 星期二
	m_sWeeks[2] = "Wed";  // 星期三
	m_sWeeks[3] = "Thu";  // 星期四
	m_sWeeks[4] = "Fri";  // 星期五
	m_sWeeks[5] = "Sat";  // 星期六
	m_sWeeks[6] = "Sun";  // 星期日

	m_bOK = FALSE;
}

CDialogCalendar::~CDialogCalendar()
{
	
}

LPCTSTR CDialogCalendar::GetWindowClassName() const
{
	return _T("DialogCalendar");
}

CDuiString CDialogCalendar::GetSkinFile()
{
	return _T("DialogCalendar");
}

void CDialogCalendar::OnFinalMessage( UIWND hWnd )
{
// 	UIWND hParentWnd = DuiLibWindowWnd::GetParentWindow(hWnd);
// 	if(DuiLibWindowWnd::IsWindow(hParentWnd))
// 	{
// 		DuiLibWindowWnd::SetForeground(hParentWnd);
// 	}
	CUIFrameWnd::OnFinalMessage(hWnd);

	// 关闭父编辑窗口（日期时间编辑框）
	if (m_pParentWindow) 
	{
		if(m_bOK)
			m_pParentWindow->SetDate(m_st.wYear, m_st.wMonth, m_st.wDay);
		m_pParentWindow->Close();
	}

	if(!m_bModal)
	{
		delete this;
	}
}

LRESULT CDialogCalendar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 关联UI管理器
	GetManager()->Init(GetHWND(), GetManagerName(), this);
	// 注册PreMessage回调
	GetManager()->AddPreMessageFilter(this);

	CVerticalLayoutUI* pRoot = new CVerticalLayoutUI;
	GetManager()->AttachDialog(pRoot);

	pRoot->SetInset(CDuiRect(2, 2, 2, 2));
	pRoot->SetBkColor(0xFFFFFFFF);
	pRoot->SetBorderSize(1);
	pRoot->SetBorderColor(0xFFc0c0c0);

	CHorizontalLayoutUI* pHoriHead = new CHorizontalLayoutUI;
	pRoot->Add(pHoriHead);
	pHoriHead->SetFixedHeight(30);

	//顶部左边的按钮
	m_btnPrev = new CButtonUI;
	m_btnPrev->SetText(_T("<"));
	m_btnPrev->SetFixedWidth(30);
	m_btnPrev->SetHotTextColor(0xFF000000);
	m_btnPrev->SetTextColor(0xFFa0a0a4);
	m_btnPrev->SetCursor(DUI_HAND);
	pHoriHead->Add(m_btnPrev);

	//顶部中间显示的日期
	CHorizontalLayoutUI* pHori = new CHorizontalLayoutUI;
	pHori->SetChildAlign(DT_CENTER);
	pHori->SetChildVAlign(DT_VCENTER);
	pHoriHead->Add(pHori);
	for (int i=0; i<3; i++)
	{
		m_btnDate[i] = new CButtonUI;
		m_btnDate[i]->SetTextPadding(CDuiRect(2, 0, 2, 0));
		m_btnDate[i]->SetAutoCalcWidth(true);
		m_btnDate[i]->SetCursor(DUI_HAND);
		pHori->Add(m_btnDate[i]);
		if (i < 2)
		{
			CLabelUI* label = new CLabelUI;
			label->SetText(_T("-"));
			label->SetAutoCalcWidth(true);
			pHori->Add(label);
		}
	}

	//顶部右边的按钮
	m_btnNext = new CButtonUI;
	m_btnNext->SetText(_T(">"));
	m_btnNext->SetFixedWidth(30);
	m_btnNext->SetHotTextColor(0xFF000000);
	m_btnNext->SetTextColor(0xFFa0a0a4);
	m_btnNext->SetCursor(DUI_HAND);
	pHoriHead->Add(m_btnNext);

	CHorizontalLayoutUI* pHoriWeek = new CHorizontalLayoutUI;
	pHoriWeek->SetFixedHeight(30);
	pRoot->Add(pHoriWeek);
	for (int i=0; i<7; i++)
	{
		CButtonUI* btnWeek = new CButtonUI;
		pHoriWeek->Add(btnWeek);
		btnWeek->SetText(m_sWeeks[i]);
		m_btnWeeks[i] = btnWeek;
	}

	int nIndex = 0;
	for (int i=0; i<6; i++)
	{
		CHorizontalLayoutUI* pHori1 = new CHorizontalLayoutUI;
		pHori1->SetFixedHeight(30);
		pRoot->Add(pHori1);
		for (int j=0; j<7; j++)
		{
			CBtnCalendarDayUI* btn = new CBtnCalendarDayUI;
			btn->SetText(_T("xx"));
			btn->SetHotBorderColor(0xFFc0c0c0);
			btn->SetPushedBorderColor(0xFFa0a0a4);
			btn->SetSelectedBorderColor(0xFFa0a0a4);
			btn->SetSelectedBkColor(0xFFf0f0f0);
			btn->SetBorderSize(1);
			btn->SetCursor(DUI_HAND);
			btn->SetGroup(_T("group_calendar_days"));
			pHori1->Add(btn);
			m_btnDays[nIndex++] = btn;
		}
	}

	GetManager()->SetInitSize(210, 240);

	// 添加Notify事件接口
	GetManager()->AddNotifier(this);
	// 窗口初始化完毕
	__InitWindow();
	return 0;
}

LRESULT CDialogCalendar::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close(IDCANCEL);
	return 0;
}

UINT CDialogCalendar::DoModal(UIWND hWndParent)
{
	Create(hWndParent, GetWindowClassName(), UI_WNDSTYLE_DIALOG|WS_SIZEBOX, WS_EX_WINDOWEDGE);
	CenterWindow();
	m_bModal = true;
	return ShowModal();
}

UINT CDialogCalendar::DoModal(CUIFrmBase *pParentWnd)
{
	if(pParentWnd)
		Create(pParentWnd->GetManager()->GetPaintWindow(), GetWindowClassName(), UI_WNDSTYLE_DIALOG|WS_SIZEBOX, WS_EX_WINDOWEDGE);
	else
		Create(NULL, GetWindowClassName(), UI_WNDSTYLE_DIALOG|WS_SIZEBOX, WS_EX_WINDOWEDGE);
	CenterWindow();
	m_bModal = true;
	return ShowModal();
}

void CDialogCalendar::ShowDialog(UIWND hWndParent)
{
	Create(hWndParent, GetWindowClassName(), WS_POPUP|UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	CenterWindow();
	m_bModal = false;
	return ShowWindow();
}

void CDialogCalendar::ShowDialog(CUIFrmBase *pParentWnd)
{
	if(pParentWnd)
		Create(pParentWnd->GetManager()->GetPaintWindow(), GetWindowClassName(), WS_POPUP|UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	else
		Create(NULL, GetWindowClassName(), WS_POPUP|UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	CenterWindow();
	m_bModal = false;
	return ShowWindow();
}

void CDialogCalendar::InitWindow()
{
	UpdateCalendar();
}

void CDialogCalendar::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		// 上月按钮
		if (msg.pSender == m_btnPrev)
		{
			// 月份减一，年份可能改变
			if (m_st.wMonth == 1)
			{
				m_st.wMonth = 12;
				m_st.wYear--;
			}
			else
			{
				m_st.wMonth--;
			}
			// 确保日期不超过新月份的天数
			int maxDay = GetDaysInMonth(m_st.wYear, m_st.wMonth);
			if (m_st.wDay > maxDay) m_st.wDay = maxDay;
			UpdateCalendar();
			return;
		}
		// 下月按钮
		else if (msg.pSender == m_btnNext)
		{
			if (m_st.wMonth == 12)
			{
				m_st.wMonth = 1;
				m_st.wYear++;
			}
			else
			{
				m_st.wMonth++;
			}
			int maxDay = GetDaysInMonth(m_st.wYear, m_st.wMonth);
			if (m_st.wDay > maxDay) m_st.wDay = maxDay;
			UpdateCalendar();
			return;
		}
		// 年份按钮（点击后可弹出年份选择器，这里简单处理：增加一年）
		else if (msg.pSender == m_btnDate[0])
		{
			m_st.wYear++;
			UpdateCalendar();
			return;
		}
		// 月份按钮（增加一个月）
		else if (msg.pSender == m_btnDate[1])
		{
			if (m_st.wMonth == 12)
			{
				m_st.wMonth = 1;
				m_st.wYear++;
			}
			else
			{
				m_st.wMonth++;
			}
			int maxDay = GetDaysInMonth(m_st.wYear, m_st.wMonth);
			if (m_st.wDay > maxDay) m_st.wDay = maxDay;
			UpdateCalendar();
			return;
		}
		// 日期按钮（点击日期）
		else
		{
			// 遍历42个日期按钮，查找被点击的按钮
			for (int i = 0; i < 42; ++i)
			{
				if (msg.pSender == m_btnDays[i])
				{
					m_st.wYear = m_btnDays[i]->m_st.wYear;
					m_st.wMonth = m_btnDays[i]->m_st.wMonth;
					m_st.wDay = m_btnDays[i]->m_st.wDay;
					CDateTimeWndSDL::GetDayOfWeek(m_st);
					m_bOK = TRUE;
					Close(IDOK);
					return;
				}
			}
		}
	}
	CUIFrameWnd::Notify(msg);
}
void CDialogCalendar::UpdateCalendar()
{
	int year = m_st.wYear;
	int month = m_st.wMonth;
	int selectedDay = m_st.wDay;

	// 更新顶部年月日显示
	m_btnDate[0]->SetTextV(_T("%04d"), year);
	m_btnDate[1]->SetTextV(_T("%02d"), month);
	m_btnDate[2]->SetTextV(_T("%02d"), selectedDay);

	// 获取当月第一天是星期几（返回值：0=周一, 1=周二, ..., 6=周日）
	int firstDayOfWeek = GetFirstDayOfWeek(year, month);
	int daysInMonth = GetDaysInMonth(year, month);

	// 计算上个月的信息
	int preYear = (month == 1) ? year - 1 : year;
	int preMonth = (month == 1) ? 12 : month - 1;
	int daysInPrevMonth = GetDaysInMonth(preYear, preMonth);
	// 上月需要显示的开始日期（从 daysInPrevMonth - firstDayOfWeek + 1 开始）
	int prevMonthStart = daysInPrevMonth - firstDayOfWeek + 1;

	int nextYear	= (month == 12) ? year + 1 : year;
	int nextMonth	= (month == 12) ? 1 : month + 1;

	int preDay = prevMonthStart; //上月计数
	int curDay = 1; // 当月计数
	int nextDay = 1; //下月计数

	for (int i = 0; i < 42; ++i)
	{
		if (preDay <= daysInPrevMonth) //上月
		{
			m_btnDays[i]->SetTextV(_T("%d"), preDay);
			m_btnDays[i]->SetTextColor(0xFF999999);
			m_btnDays[i]->m_st.wYear	= preYear;
			m_btnDays[i]->m_st.wMonth	= preMonth;
			m_btnDays[i]->m_st.wDay		= preDay;
			preDay++;
		}
		else if(curDay <= daysInMonth) //当月
		{
			m_btnDays[i]->SetTextV(_T("%d"), curDay);
			if (curDay == selectedDay) m_btnDays[i]->Selected(true);
			m_btnDays[i]->SetTextColor(0xFF000000);
			m_btnDays[i]->m_st.wYear = year;
			m_btnDays[i]->m_st.wMonth = month;
			m_btnDays[i]->m_st.wDay = curDay;
			curDay++;
		}
		else //下月
		{
			m_btnDays[i]->SetTextV(_T("%d"), nextDay);
			m_btnDays[i]->SetTextColor(0xFF999999);
			m_btnDays[i]->m_st.wYear = nextYear;
			m_btnDays[i]->m_st.wMonth = nextMonth;
			m_btnDays[i]->m_st.wDay = nextDay;
			nextDay++;
		}
	}
}

// 判断闰年
BOOL CDialogCalendar::IsLeapYear(int year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 获取某月的天数
int CDialogCalendar::GetDaysInMonth(int year, int month)
{
	static const int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (month == 2 && IsLeapYear(year))
		return 29;
	return daysInMonth[month - 1];
}

// 计算某年某月1号是星期几（返回 0=周一, 1=周二, ..., 6=周日）
// 使用基姆拉尔森计算公式 (Kim Larsen) 或蔡勒公式，这里使用一个可靠的手动计算方式
int CDialogCalendar::GetFirstDayOfWeek(int year, int month)
{
	// 使用 Tomohiko Sakamoto 算法，输入年份和月份，返回星期几（0=周日, 6=周六）
	// 我们需要的返回值是 0=周一, 1=周二, ..., 6=周日，所以需要转换。
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	int y = year - (month < 3);
	int w = (y + y / 4 - y / 100 + y / 400 + t[month - 1] + 1) % 7; // w: 0=周日, 6=周六
	// 转换: 周一=0, 周二=1, ..., 周日=6
	return (w + 6) % 7;
}

} // namespace DuiLib{
#endif //#ifdef DUILIB_SDL

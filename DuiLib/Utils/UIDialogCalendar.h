#pragma once
#include "../Control/UIDateTimeWndSdl.h"

#ifdef DUILIB_SDL
namespace DuiLib {

class UILIB_API CDialogCalendar : public CUIFrameWnd
{
public:
	class UILIB_API CBtnCalendarDayUI : public COptionUI
	{
	public:
		SYSTEMTIME m_st;
	};
public:
	CDialogCalendar(void);
	virtual ~CDialogCalendar();
	SYSTEMTIME m_st; // 初始时间
	CDateTimeWndSDL* m_pParentWindow;

	virtual LPCTSTR GetWindowClassName() const override;
	virtual CDuiString GetSkinFile() override;

	virtual void OnFinalMessage( UIWND hWnd ) override;

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;

	virtual UINT DoModal(UIWND hWndParent);
	virtual UINT DoModal(CUIFrmBase *pParentWnd = NULL);

	virtual void ShowDialog(UIWND hWndParent);
	virtual void ShowDialog(CUIFrmBase *pParentWnd = NULL);

	virtual void Notify(TNotifyUI& msg) override;

	virtual void InitWindow() override;

private:
	uiBool IsLeapYear(int year);					// 是否闰年
	void UpdateCalendar();						// 刷新日历网格
	int GetDaysInMonth(int year, int month);	// 获取某月的天数
	int GetFirstDayOfWeek(int year, int month); // 返回0=周一 ... 6=周日
private:
	bool m_bModal;
	CButtonUI* m_btnPrev;		// 上月按钮
	CButtonUI* m_btnNext;		// 下月按钮
	CButtonUI* m_btnDate[3];	// 顶部按钮 小时 - 分钟 - 秒
	CDuiString m_sWeeks[7];		// 星期的文字信息，如 SUN MON ...
	CButtonUI* m_btnWeeks[7];	// 星期的按钮
	CBtnCalendarDayUI* m_btnDays[42];	// 每天的按钮
	int m_firstDayOfWeek; //// 获取本月第一天是星期几（周一为0）

	uiBool m_bOK;	//是否更新到父窗口
};

} //namespace DuiLib {
#endif //#ifdef DUILIB_SDL
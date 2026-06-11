#ifndef __UIDATETIME_WND_SDL_H__
#define __UIDATETIME_WND_SDL_H__

#pragma once

#ifdef DUILIB_SDL
namespace DuiLib
{

class UILIB_API CDateTimeWndSDL : public CWindowSDL
{
public:
	struct tagDTSegment
	{
		tagDTSegment() { curPos = 0; selStart = 0; selEnd = 0; }
		CDuiRect rc;
		CDuiString sValue;
		int curPos;      // 光标位置
		int selStart;    // 选择起始
		int selEnd;      // 选择结束
	};

	// 0=Sunday, 1=Monday.....6=Saturday
	static void GetDayOfWeek(SYSTEMTIME& st);
	static int GetDayOfWeek(int y, int m, int d);

	CDateTimeWndSDL();

	virtual void Init(CDateTimeUI* pOwner, CDuiRect rcBase, UINT uFormatStyle);
	virtual void OnFinalMessage(UIWND hWnd) override;
	CDuiRect CalPos(CDuiRect rcBase);
	void CloseCalendar();

	void SetDate(int year, int month, int day);
protected:
	virtual BOOL OnSdlEvent(const void* pEvent) override;
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

private:
	void SetFocusSegment(int index);
	void DeleteBackward();
	void DeleteForward();
	void DeleteSelection();
	void MoveLeft();
	void MoveRight();
	void MoveHome();
	void MoveEnd();
	BOOL CheckInput(int n);

	void DrawButton(UIRender* pRender, const CDuiRect& rc, const TButtonState& state, bool bUp);
	void DrawDropDownButton(UIRender* pRender, const CDuiRect& rc, const TButtonState& state);
	void OnClickUpButton();
	void OnClickDownButton();
	void OnClickDropDown();
protected:
	CDateTimeUI* m_pOwner;
	UINT m_uFormatStyle;
	SYSTEMTIME m_oldDateTime;		// 原始日期时间，用于 ESC 恢复
	int m_nIndex;					// 当前获得焦点的段索引 0..2
	tagDTSegment m_vSegments[3];	// 日期或时间3个段
	CDuiRect m_rcPoints[3];			// 3个点的区域
	CDuiString m_sSeparator;		// 分割符的字符

	CWindowSDL* m_pWindowDropDown;
	TButtonState m_stateDropDown;	// 日期选取控件的下拉按钮状态
	CDuiRect m_rcDropDownButton;	// 日期选取控件的下拉按钮区域

	TButtonState m_stateUpButton;	// 时间选取控件的上升按钮状态
	CDuiRect m_rcUpButton;			// 时间选取控件的上升按钮区域

	TButtonState m_stateDownButton;	// 时间选取控件的下降按钮状态
	CDuiRect m_rcDownButton;		// 时间选取控件的下降按钮区域

	TIMERINFO m_TimerUpButton;	//上升按钮按住时递增数值的定时器
	TIMERINFO m_TimerDownButton;	//下降按钮按住时递减数值的定时器
};

} // namespace DuiLib
#endif //#ifdef DUILIB_SDL
#endif // __UIDATETIME_H__
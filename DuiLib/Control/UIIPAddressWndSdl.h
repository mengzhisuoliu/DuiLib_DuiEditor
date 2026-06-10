#ifndef __UIIPADDRESS_WND_SDL_H__
#define __UIIPADDRESS_WND_SDL_H__

#pragma once

#ifdef DUILIB_SDL

namespace DuiLib
{
#define IP_NONE   0
#define IP_UPDATE 1
#define IP_DELETE 2
#define IP_KEEP   3

class UILIB_API CIPAddressWndSDL : public CWindowSDL
{
public:
	struct tagIPSegment
	{
		tagIPSegment() { curPos = 0; selStart = 0; selEnd = 0; }
		CDuiRect rc;
		CDuiString sValue;
		int curPos;
		int selStart;
		int selEnd;
	};
public:
	CIPAddressWndSDL();
	virtual ~CIPAddressWndSDL();

	// 获取本机 IPv4 地址（网络字节序 DWORD）
	static DWORD GetLocalIpAddress();

	// 初始化 IP 控件窗口
	void Init(CIPAddressUI* pOwner);
	CDuiRect CalPos();

	virtual void OnFinalMessage(UIWND hWnd) override;
	virtual BOOL OnSdlEvent(const void* pEvent) override;

protected:
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
	void SetFocusSegment(int index, BOOL bDrawCaret);
	void DeleteBackward();
	void DeleteForward();
	void DeleteSelection();
	void MoveLeft();
	void MoveRight();
	void MoveHome();
	void MoveEnd();

	int GetCharPosFromPoint(const CDuiPoint& pt);
	int GetCharXPos(int charIndex);
	CDuiRect GetCaretPos();

	CIPAddressUI* m_pOwner;
	DWORD m_dwOldIp;

	int m_nIndex;				// 当前获得焦点的段索引 0..3
	BOOL m_bShowCaret;			// 是否显示光标. 不显示光标时, 表示在全选状态.
	bool m_bDrawCaret;          // 光标闪烁标志, 定时器负责切换
	UINT m_caretTimerID;
	bool m_bDragging;

	int m_nFontHeight;
	int m_nCaretHeight;
	tagIPSegment m_vSegments[4];	// IP的4个段
	CDuiRect m_rcPoints[3];			//3个点的区域
};

} // namespace DuiLib

#endif // DUILIB_SDL
#endif // __UIIPADDRESS_WND_SDL_H__
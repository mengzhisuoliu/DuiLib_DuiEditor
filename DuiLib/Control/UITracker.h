#pragma once

//提交CCanvasContainerUI，深圳—逸云(QQ：1112880)
//修改为CTrackerUI, liqs99
//2023-09-16
namespace DuiLib
{

class UILIB_API CTrackerUI: public CContainerUI
{
	DECLARE_DUICONTROL(CTrackerUI)
public:
	CTrackerUI();
	virtual LPCTSTR GetClass() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;
	virtual UINT GetControlFlags() const override;

	// 0    1    2
	// 3    4    5
	// 6    7    8
	CDuiRect GetSizerRect(int iIndex);
	int GetSizerCursor(CDuiPoint& pt, int& iCursor);
	virtual void PaintBorder(UIRender *pRender) override;
	virtual void DoEvent(TEventUI& event) override;
	void MoveRect(int cx, int cy);
	void SizeRect(int cx, int cy);
	int GetCorIndex(int fx, int fy, int sx, int sy);
	//void DoPostPaint(HDC hDC, const RECT& rcPaint);
	CDuiRect GetRect();
protected:
	int m_iCursor;
	CDuiRect m_rcNewPos;
	bool m_bMouseDown;
};

};


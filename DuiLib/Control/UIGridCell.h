#pragma once
#include "IGridUI.h"

namespace DuiLib
{

//单元格内部的CheckBox
class UILIB_API CGridCellInnerCheckBoxUI : public CCheckBoxUI
{
public:
	CGridCellInnerCheckBoxUI();

	void SetOwner(CControlUI *pCellUI) { m_pOwner = pCellUI; }
	CControlUI *GetOwner() const { return m_pOwner; }

	virtual bool Activate() override;
	virtual bool IsSelected() const override;
	virtual void Selected(bool bSelected, bool bTriggerEvent=true) override;

	virtual bool IsSelectedState() const override;

	CControlUI *m_pOwner;
};

class UILIB_API CGridCellUI : public COptionLayoutUI
{
	DECLARE_DUICONTROL(CGridCellUI)
public:
	CGridCellUI();
	virtual ~CGridCellUI(void);	
	
	void SetOwner(IGridUI *pGrid) { m_pOwner = pGrid; }
	IGridUI *GetOwner() const { return m_pOwner; }

	virtual LPCTSTR GetClass() const override;
	virtual UINT GetControlFlags() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;

	virtual CDuiString GetText() const override;
	virtual void SetText(LPCTSTR pstrText) override;

	virtual CDuiColor GetTextColor() const override;
	virtual void SetTextColor(CDuiColor dwColor) override;

	bool IsMergedWithOthers();

	void SetCellIndex(int row, int col);
	void InitCell();

	int GetRow() const { return m_row; }
	int GetCol() const { return m_col; }

	uiBool IsFixedRow() const;
	uiBool IsFixedCol() const;

	virtual void SetFixedWidth(int cx) override;
	virtual bool IsSelected() const override;
	virtual void Selected(bool bSelected, bool bTriggerEvent=true) override;
	virtual bool IsFocused() const override;
	virtual bool IsHotState() const override;

	CDuiRect GetCellPos();

	void CreateInnerControl();
	void DeleteInnerControl();
	CControlUI *GetInnerControl() const;
	bool OnEventInnerControl(void* param);
	bool OnNotifyInnerControl(void* param);
	void OnCellSetFocus();
	void OnCellKillFocus();

	virtual void DoInit() override;
	virtual void SetPos(CDuiRect rc, bool bNeedInvalidate = true) override;
	virtual CDuiSize EstimateSize(CDuiSize szAvailable) override;

	virtual void DoEvent(TEventUI& event) override;
	virtual bool DoPaint(UIRender *pRender, const CDuiRect& rcPaint, CControlUI* pStopControl) override;
	virtual void PaintBkColor(UIRender *pRender) override;
	virtual void PaintBkImage(UIRender *pRender) override;
	virtual void PaintStatusImage(UIRender *pRender) override;
	virtual void PaintForeColor(UIRender *pRender) override;
	virtual void PaintForeImage(UIRender *pRender) override;
	virtual void PaintText(UIRender *pRender) override;
	virtual void PaintBorder(UIRender *pRender) override;

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
protected:
	IGridUI *m_pOwner;
	int m_row, m_col;
	CControlUI *m_pInnerControl;
	int		m_nSeparatorType; //1=drag width, 2=drag height
	CDuiRect m_rcPaint2;
protected:
	CDuiRect m_rcMerge;
	bool m_bMergeWithOther;
	GridCellType m_celltype;
};


}
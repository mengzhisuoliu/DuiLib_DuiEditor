#pragma once
#include "UIGridHeader.h"
#include "UIGridBody.h"
#include "UIGridRowUI.h"
#include "UIGridCell.h"

namespace DuiLib
{

#ifndef WIN32
	typedef int (CALLBACK* PFNLVCOMPARE)(LPARAM, LPARAM, LPARAM);
#endif

class UILIB_API CGridUI : public CVerticalLayoutUI, public IGridUI
{
	DECLARE_DUICONTROL(CGridUI)

	struct TGridNotify
	{
		CDuiString sNotifyName;
		WPARAM wparam;
		LPARAM lparam;
	};
public:
	CGridUI(void); 
	~CGridUI(void);

	virtual LPCTSTR GetClass() const override;
	virtual UINT GetControlFlags() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;

	virtual void SendGridNotify(LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false) override;
	virtual void Refresh(bool bNeedUpdate = false) override;

	virtual bool Add(CControlUI* pControl) override;
	virtual bool AddAt(CControlUI* pControl, int iIndex) override;

	int InsertRow(int nIndex = -1);
	bool DeleteRow(int nIndex);

	int InsertColumn(int nIndex = -1);
	void DeleteColumn(int nIndex);

	void ResetGrid();
	void ResetGridBody();

	void EnsureVisible(int row, int col); //滚动使单元格可见。

	virtual uiBool SetRowCount(int rows) override;
	virtual int GetRowCount() override;
	virtual uiBool SetColumnCount(int cols) override;
	virtual int GetColumnCount() override;

	virtual uiBool SetFixedRowCount(int rows) override;
	virtual int GetFixedRowCount() override;

	virtual uiBool SetRowHeight(int row, int height) override;
	virtual int  GetRowHeight(int row, uiBool bScaleByDPI=uiFalse) override;

	virtual uiBool SetColumnWidth(int col, int width) override;
	virtual int  GetColumnWidth(int col, uiBool bScaleByDPI=uiFalse) override;

	//设置整列单元格类型
	void SetCellType(int col, GridCellType cellType);

	//获取整列单元格类型
	GridCellType GetCellType(int col);

	//设置单元格类型
	void SetCellType(int row, int col, GridCellType cellType);

	//获取单元格类型
	GridCellType GetCellType(int row, int col);

	virtual void ClearSelectedRows() override;
	virtual void SelectRow(int row, uiBool bSelected=uiTrue, uiBool bTriggerEvent= uiFalse) override;

	virtual void SetVirtualGrid(uiBool bVirtual) override;
	virtual void SetVirtualRowCount(int nRows);

	void SetRowTag(int row, UINT_PTR tag);
	UINT_PTR GetRowTag(int row);
	int FindRowFromRowTag(UINT_PTR tag);

	uiBool IsMergedCell(int row, int col);
	TGridMergeRange GetCellMergeRange(int row, int col);
	TGridMergeRange GetCellMergeRangeEx(int row, int col);
	void SetMergeCellsNeedPaint(int row0, int col0, bool bPaint);
	void MergeCells(int nStartRow, int nStartCol, int nEndRow, int nEndCol);

	CGridCellUI *GetCellUI(int row, int col);
	CGridCellUI *GetCellUI(const TCellID &cellID);
	CGridCellUI *GetCellUI(TCellData *pCellData);
	CGridCellUI *GetCellUIFromPoint(const CDuiPoint &pt);

	CGridRowUI *GetRowUI(int row);

	CGridCellUI *GetHotCell() { return m_pHotCell; }
	CGridRowUI *GetHotRow() { return m_pHotRow; }

	uiBool IsFixedRow(int row);
	uiBool IsFixedCol(int col);

	virtual void SetFocusCell(int row, int col) override;
	virtual void SetFocusCell(const TCellID &cellID) override;
	virtual const TCellID &GetFocusCell() const override;

	//设置col列是否允许点击表头排序
	void SetColumnSort(int col, uiBool bSort);

	//获取col列是否允许点击表头排序
	uiBool IsColumnSort(int col) const;

	//设置自定义排序算法回调函数，默认按单元格字符串比较，_tcscmp()
	void SetSortCallbackFun(PFNLVCOMPARE pfnCompare);
	//获取排序算法回调函数
	PFNLVCOMPARE GetSortCallbackFun() const;

	//执行按col列进行排序
	void SortItems(int col);
	void SortItems(int col, uiBool bSortAscending); //指定升序还是降序， TRUE=升序，FALSE=降序
	virtual void OnSortItem(int col, uiBool bAscending);

	//获取当前排序的列
	int GetSortColumn() const		{ return m_nSortCol; }

	//TRUE=升序排列，FALSE=降序排列
	uiBool GetSortAscending() const	{ return m_bSortAscending; }
protected:
	uiBool SortItems(PFNLVCOMPARE pfnCompare, int col, uiBool bAscending, LPARAM data, int low, int high);
	static int CALLBACK pfnCellTextCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	bool OnGridNotify(void *param);
public:
	virtual void DoInit() override;
	virtual void DoEvent(TEventUI& event) override;
	bool OnSizeColumnOrRow(TEventUI& event);
	virtual void SetPos(CDuiRect rc, bool bNeedInvalidate = true) override;
	virtual void BuildRows(CDuiRect rc, bool bNeedInvalidate = true);
	virtual void OnDrawItem(int nBeginRow, int nEndRow);
	virtual void SetScrollPos(CDuiSize szPos, bool bMsg) override;
	virtual void ProcessScrollBar(CDuiRect rc, int cxRequired, int cyRequired) override;
	virtual bool DoPaint(UIRender *pRender, const CDuiRect& rcPaint, CControlUI* pStopControl) override;
	virtual void PaintBorder(UIRender *pRender) override;

public:
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

protected:
	CGridHeaderUI *m_pHeader;
	CGridBodyUI *m_pBody;
	int m_nRowCount;
	int m_nColCount;
	int m_mapColumnWidth[MAX_GRID_COLUMN_COUNT];
	int m_mapColWidthFixed[MAX_GRID_COLUMN_COUNT];
	GridCellType m_mapColumnCellType[MAX_GRID_COLUMN_COUNT];
	uiBool m_mapColumnSort[MAX_GRID_COLUMN_COUNT];

	CDuiRect m_rcTracker; //拖动鼠标进行选择的框框

	int m_nSortCol;
	uiBool m_bSortAscending;
	PFNLVCOMPARE m_pfnCompare;

private:
	CGridCellUI *m_pCellLButtonDown;
	CGridRowUI *m_pHotRow;
	CGridCellUI *m_pHotCell;
	int		m_nSeparatorType; //1=drag width, 2=drag height
};


}

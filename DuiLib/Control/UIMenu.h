#ifndef __UIMENU_H__
#define __UIMENU_H__

#pragma once

#include "../Utils/observer_impl_base.h"

namespace DuiLib {

struct ContextMenuParam
{
	// 1: remove all
	// 2: remove the sub menu
	WPARAM wParam;
	UIWND hWnd;
};

struct MenuItemInfo
{
	TCHAR szName[256];
	bool bChecked;
};


enum MenuAlignment
{
	eMenuAlignment_Left = 1 << 1,
	eMenuAlignment_Top = 1 << 2,
	eMenuAlignment_Right = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};


enum MenuItemDefaultInfo
{
	ITEM_DEFAULT_HEIGHT = 30,		//每一个item的默认高度（只在竖状排列时自定义）
	ITEM_DEFAULT_WIDTH = 150,		//窗口的默认宽度

	ITEM_DEFAULT_ICON_WIDTH = 26,	//默认图标所占宽度
	ITEM_DEFAULT_ICON_SIZE = 16,	//默认图标的大小

	ITEM_DEFAULT_EXPLAND_ICON_WIDTH = 20,	//默认下级菜单扩展图标所占宽度
	ITEM_DEFAULT_EXPLAND_ICON_SIZE = 9,		//默认下级菜单扩展图标的大小

	DEFAULT_LINE_LEFT_INSET = ITEM_DEFAULT_ICON_WIDTH + 3,	//默认分隔线的左边距
	DEFAULT_LINE_RIGHT_INSET = 7,	//默认分隔线的右边距
	DEFAULT_LINE_HEIGHT = 6,		//默认分隔线所占高度
	DEFAULT_LINE_COLOR = 0xFFBCBFC4	//默认分隔线颜色

};



///////////////////////////////////////////////
class MenuMenuReceiverImplBase;
class MenuMenuObserverImplBase
{
public:
	virtual void AddReceiver(MenuMenuReceiverImplBase* receiver) = 0;
	virtual void RemoveReceiver(MenuMenuReceiverImplBase* receiver) = 0;
	virtual uiBool RBroadcast(ContextMenuParam param) = 0;
};
/////////////////////////////////////////////////
class MenuMenuReceiverImplBase
{
public:
	virtual void AddObserver(MenuMenuObserverImplBase* observer) = 0;
	virtual void RemoveObserver() = 0;
	virtual uiBool Receive(ContextMenuParam param) = 0;
};
/////////////////////////////////////////////////

class MenuReceiverImpl;
class UILIB_API MenuObserverImpl : public MenuMenuObserverImplBase
{
	friend class Iterator;
public:
	MenuObserverImpl():
	m_pMainWndPaintManager(NULL),
	m_pMenuCheckInfo(NULL)
	{
		pReceivers_ = new ReceiversVector;
	}

	~MenuObserverImpl()
	{
		if (pReceivers_ != NULL)
		{
			delete pReceivers_;
			pReceivers_ = NULL;
		}
		
	}

	virtual void AddReceiver(MenuMenuReceiverImplBase* receiver) override
	{
		if (receiver == NULL)
			return;

		pReceivers_->push_back(receiver);
		receiver->AddObserver(this);
	}

	virtual void RemoveReceiver(MenuMenuReceiverImplBase* receiver) override
	{
		if (receiver == NULL)
			return;

		ReceiversVector::iterator it = pReceivers_->begin();
		for (; it != pReceivers_->end(); ++it)
		{
			if (*it == receiver)
			{
				pReceivers_->erase(it);
				break;
			}
		}
	}

	virtual uiBool RBroadcast(ContextMenuParam param) override
	{
		ReceiversVector::reverse_iterator it = pReceivers_->rbegin();
		for (; it != pReceivers_->rend(); ++it)
		{
			(*it)->Receive(param);
		}

		return uiBool();
	}


	class Iterator
	{
		MenuObserverImpl & _tbl;
		DWORD index;
		MenuMenuReceiverImplBase* ptr;
	public:
		Iterator( MenuObserverImpl & table )
			: _tbl( table ), index(0), ptr(NULL)
		{}

		Iterator( const Iterator & v )
			: _tbl( v._tbl ), index(v.index), ptr(v.ptr)
		{}

		MenuMenuReceiverImplBase* next()
		{
			if ( index >= _tbl.pReceivers_->size() )
				return NULL;

			for ( ; index < _tbl.pReceivers_->size(); )
			{
				ptr = (*(_tbl.pReceivers_))[ index++ ];
				if ( ptr )
					return ptr;
			}
			return NULL;
		}
	};

	virtual void SetManger(CPaintManagerUI* pManager)
	{
		if (pManager != NULL)
			m_pMainWndPaintManager = pManager;
	}

	virtual CPaintManagerUI* GetManager() const
	{
		return m_pMainWndPaintManager;
	}

	virtual void SetMenuCheckInfo(CStdStringPtrMap* pInfo)
	{
		if (pInfo != NULL)
			m_pMenuCheckInfo = pInfo;
		else
			m_pMenuCheckInfo = NULL;
	}

	virtual CStdStringPtrMap* GetMenuCheckInfo() const
	{
		return m_pMenuCheckInfo;
	}

protected:
	typedef std::vector<MenuMenuReceiverImplBase*> ReceiversVector;
	ReceiversVector *pReceivers_;
	CPaintManagerUI* m_pMainWndPaintManager;
	CStdStringPtrMap* m_pMenuCheckInfo;
};

////////////////////////////////////////////////////
class UILIB_API MenuReceiverImpl : public MenuMenuReceiverImplBase
{
public:
	MenuReceiverImpl()
	{
		pObservers_ = new ObserversVector;
	}

	~MenuReceiverImpl()
	{
		if (pObservers_ != NULL)
		{
			delete pObservers_;
			pObservers_ = NULL;
		}
	}

	virtual void AddObserver(MenuMenuObserverImplBase* observer) override
	{
		pObservers_->push_back(observer);
	}

	virtual void RemoveObserver() override
	{
		ObserversVector::iterator it = pObservers_->begin();
		for (; it != pObservers_->end(); ++it)
		{
			(*it)->RemoveReceiver(this);
		}
	}

	virtual uiBool Receive(ContextMenuParam param) override
	{
		return uiBool();
	}

protected:
	typedef std::vector<MenuMenuObserverImplBase*> ObserversVector;
	ObserversVector* pObservers_;
};

/////////////////////////////////////////////////////////////////////////////////////
//

//class CListUI;
class CMenuWnd;
class UILIB_API CMenuUI : public CListUI
{
	DECLARE_DUICONTROL(CMenuUI)
public:
	CMenuUI();
	virtual ~CMenuUI();
	CMenuWnd*	m_pWindow;

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
	virtual UINT GetListType() override;

	virtual void DoEvent(TEventUI& event) override;

    virtual bool Add(CControlUI* pControl) override;
    virtual bool AddAt(CControlUI* pControl, int iIndex) override;

    virtual int GetItemIndex(CControlUI* pControl) const override;
    virtual bool SetItemIndex(CControlUI* pControl, int iIndex) override;
    virtual bool Remove(CControlUI* pControl, bool bDoNotDestroy=false) override;

	virtual CDuiSize EstimateSize(CDuiSize szAvailable) override;

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

	void SetExpandIcon(LPCTSTR strIcon);
	CDuiString GetExpandIcon();
	void SetCheckIcon(LPCTSTR strIcon);
	CDuiString GetCheckIcon();

	void SetIconWidth(int nWidth);
	int GetIconWidth();
	void SetIconBkColor(CDuiColor dwColor);
	CDuiColor GetIconBkColor();
private:
	CDuiString m_strExpandIcon;
	CDuiString m_strCheckIcon;
	int m_nIconWidth;
	CDuiColor m_dwIconBkColor;
};

/////////////////////////////////////////////////////////////////////////////////////
//
//class CListContainerElementUI;
class UILIB_API CMenuElementUI : public CListContainerElementUI
{
	DECLARE_DUICONTROL(CMenuElementUI)
	friend CMenuWnd;
public:
    CMenuElementUI();
	~CMenuElementUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual bool DoPaint(UIRender *pRender, const CDuiRect& rcPaint, CControlUI* pStopControl) override;
	virtual void DrawItemText(UIRender *pRender, const CDuiRect& rcItem) override;
	void DrawItemBorder(UIRender *pRender, const CDuiRect& rcItem);
	virtual CDuiSize EstimateSize(CDuiSize szAvailable) override;

	virtual void DoEvent(TEventUI& event) override;

	CMenuWnd* GetMenuWnd();
	CMenuUI* GetMenuUI();
	void CreateMenuWnd();
	
	void SetLineType();
	void SetLineColor(CDuiColor color);
	CDuiColor GetLineColor() const;
	void SetLinePadding(CDuiRect rcInset);
	CDuiRect GetLinePadding() const;
	void SetIcon(LPCTSTR strIcon);
	void SetUnCheckIcon(LPCTSTR strIcon);
	void SetIconSize(LONG cx, LONG cy);
	void DrawItemIcon(UIRender *pRender, const CDuiRect& rcItem);
	void SetChecked(bool bCheck = true);
	bool GetChecked();
	void SetCheckItem(bool bCheckItem = false);
	bool GetCheckItem() const;

	void SetShowExplandIcon(bool bShow);
	void DrawItemExpland(UIRender *pRender, const CDuiRect& rcItem);

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

	MenuItemInfo* GetItemInfo(LPCTSTR pstrName);
	MenuItemInfo* SetItemInfo(LPCTSTR pstrName, bool bChecked);

	void OnUpdateCommandUI();

	void DrawDisableItemIcon(UIRender *pRender, LPCTSTR pStrImage, LPCTSTR pStrModify);
protected:
	bool _DrawImageMenuDisableIcon(UIRender *pRender, CPaintManagerUI* pManager, const CDuiRect& rc, const CDuiRect& rcPaint, const CDuiString& sImageName, \
		const CDuiString& sImageResType, CDuiRect rcItem, CDuiRect rcBmpPart, CDuiRect rcCorner, CDuiColor dwMask, BYTE bFade, \
		bool bHole, bool bTiledX, bool bTiledY, int width, int height, CDuiColor fillcolor, HINSTANCE instance = NULL);
protected:
	CMenuWnd*	m_pWindow;

	bool		m_bDrawLine;	//画分隔线
	CDuiColor		m_dwLineColor;  //分隔线颜色
	CDuiRect		m_rcLinePadding;	//分割线的左右边距

	CDuiSize		m_szIconSize; 		//画图标
	CDuiString	m_strIcon;
	CDuiString  m_strUnCheckIcon;	//未选中时的图标
	bool		m_bCheckItem;	//菜单是否拥有复选框
	bool		m_bCheck;		//复选框是否选中

	bool		m_bShowExplandIcon;
};

class CMenuCmdUI
{
	friend class CMenuElementUI;
public:
	CMenuCmdUI(CMenuElementUI *p);
	virtual void Enable(uiBool bEnable = uiTrue);
	virtual uiBool IsEnable();

	virtual void SetCheck(uiBool bCheck = uiTrue);
	virtual uiBool IsCheck();

	virtual void SetText(LPCTSTR lpszText);
	virtual CDuiString GetText();

	virtual CDuiString GetName();

	virtual CMenuElementUI *GetMenuElementUI() const;
private:
	CMenuElementUI *pMenuElement;
};

} // namespace DuiLib

#endif // __UIMENU_H__



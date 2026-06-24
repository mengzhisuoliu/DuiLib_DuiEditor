#include "StdAfx.h"
#include "UITreeView.h"

#pragma warning( disable: 4251 )
namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CTreeNodeUI)

		//************************************
		// 函数名称: CTreeNodeUI
		// 返回类型: 
		// 参数信息: CTreeNodeUI * _ParentNode
		// 函数说明: 
		//************************************
		CTreeNodeUI::CTreeNodeUI( CTreeNodeUI* _ParentNode /*= NULL*/ )
	{
		m_dwItemTextColor = 0x00000000;
		m_dwItemHotTextColor = 0;
		m_dwSelItemTextColor = 0;
		m_dwSelItemHotTextColor	= 0;

		pTreeView = NULL;
		m_iTreeLavel = 0;
		m_bIsVisable = uiTrue;
		m_bIsCheckBox = uiFalse;
		pParentTreeNode	= NULL;

		pHoriz = new CHorizontalLayoutUI();
		pFolderButton = new CCheckBoxUI();
		pDottedLine = new CLabelUI();
		pCheckBox = new CCheckBoxUI();
		pItemButton = new COptionUI();

		this->SetFixedHeight(18);
		this->SetFixedWidth(250);
		pFolderButton->SetFixedWidth(GetFixedHeight());
		pDottedLine->SetFixedWidth(2);
		pCheckBox->SetFixedWidth(GetFixedHeight());
		pItemButton->SetAttribute(_T("align"),_T("left"));
		pDottedLine->SetVisible(uiFalse);
		pCheckBox->SetVisible(uiFalse);
		pItemButton->SetMouseEnabled(uiFalse);

		if(_ParentNode) {
			if (_tcsicmp(_ParentNode->GetClass(), _T("TreeNodeUI")) != 0) return;
			//pDottedLine->SetVisible(_ParentNode->IsVisible());
			//pDottedLine->SetFixedWidth(_ParentNode->GetDottedLine()->GetFixedWidth()+16);
			pDottedLine->SetVisible(false);
			this->SetParentNode(_ParentNode);
		}
		pHoriz->SetChildVAlign(DT_VCENTER);
		pHoriz->Add(pDottedLine);
		pHoriz->Add(pFolderButton);
		pHoriz->Add(pCheckBox);
		pHoriz->Add(pItemButton);
		CListContainerElementUI::Add(pHoriz);
	}

	//************************************
	// 函数名称: ~CTreeNodeUI
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	CTreeNodeUI::~CTreeNodeUI( void )
	{

	}

	//************************************
	// 函数名称: GetClass
	// 返回类型: LPCTSTR
	// 函数说明: 
	//************************************
	LPCTSTR CTreeNodeUI::GetClass() const
	{
		return _T("TreeNodeUI");
	}

	//************************************
	// 函数名称: GetInterface
	// 返回类型: LPVOID
	// 参数信息: LPCTSTR pstrName
	// 函数说明: 
	//************************************
	LPVOID CTreeNodeUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcsicmp(pstrName, DUI_CTR_TREENODE) == 0 )
			return static_cast<CTreeNodeUI*>(this);
		return CListContainerElementUI::GetInterface(pstrName);
	}

	void CTreeNodeUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CListContainerElementUI::SetManager(pManager, pParent, bInit);
	}

	//************************************
	// 函数名称: DoEvent
	// 返回类型: void
	// 参数信息: TEventUI & event
	// 函数说明:
	//************************************
	void CTreeNodeUI::DoEvent( TEventUI& event )
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}
		CListContainerElementUI::DoEvent(event);
		if( event.Type == UIEVENT_DBLCLICK ) {
			if( IsEnabled() ) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_TREEITEMDBCLICK);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER ) {
			if( IsEnabled()) {
				if(m_bSelected && GetSelItemHotTextColor())
					pItemButton->SetTextColor(GetSelItemHotTextColor());
				else
					pItemButton->SetTextColor(GetItemHotTextColor());
			}
			else 
				pItemButton->SetTextColor(pItemButton->GetDisabledTextColor());

			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE ) {
			if( IsEnabled()) {
				if(m_bSelected && GetSelItemTextColor())
					pItemButton->SetTextColor(GetSelItemTextColor());
				else if(!m_bSelected)
					pItemButton->SetTextColor(GetItemTextColor());
			}
			else 
				pItemButton->SetTextColor(pItemButton->GetDisabledTextColor());

			return;
		}
		if(event.Type == UIEVENT_RBUTTONUP)
		{
			if( IsEnabled() ) 
			{
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMRCLICK);
			}
			return;
		}
	}

	//************************************
	// 函数名称: Invalidate
	// 返回类型: void
	// 函数说明: 
	//************************************
	void CTreeNodeUI::Invalidate()
	{
		if( !IsVisible() )
			return;

		if( GetParent() ) {
			CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) {
				CDuiRect rc = pParentContainer->GetPos();
				CDuiRect rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				CDuiRect invalidateRc = m_rcItem;
				//if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
				if( !invalidateRc.Intersect(m_rcItem, rc) )
					return;

				CControlUI* pParent = GetParent();
				CDuiRect rcTemp;
				CDuiRect rcParent;
				while( pParent = pParent->GetParent() ) {
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					//if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
					if( !invalidateRc.Intersect(rcTemp, rcParent) ) 
						return;
				}

				if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
			}
			else {
				CContainerUI::Invalidate();
			}
		}
		else {
			CContainerUI::Invalidate();
		}
	}

	//************************************
	// 函数名称: Select
	// 返回类型: bool
	// 参数信息: bool bSelect
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::Select( bool bSelect /*= true*/ )
	{
		bool nRet = CListContainerElementUI::Select(bSelect);
		if(m_bSelected)
			pItemButton->SetTextColor(GetSelItemTextColor());
		else 
			pItemButton->SetTextColor(GetItemTextColor());

		return nRet;
	}

	bool CTreeNodeUI::SelectMulti(bool bSelect)
	{
		bool nRet = CListContainerElementUI::SelectMulti(bSelect);
		if(m_bSelected)
			pItemButton->SetTextColor(GetSelItemTextColor());
		else 
			pItemButton->SetTextColor(GetItemTextColor());

		return nRet;
	}
	//************************************
	// 函数名称: Add
	// 返回类型: bool
	// 参数信息: CControlUI * _pTreeNodeUI
	// 函数说明: 通过节点对象添加节点
	//************************************
	bool CTreeNodeUI::Add( CControlUI* _pTreeNodeUI )
	{
		if (NULL != static_cast<CTreeNodeUI*>(_pTreeNodeUI->GetInterface(_T("TreeNode"))))
			return AddChildNode((CTreeNodeUI*)_pTreeNodeUI);

		return GetTreeNodeHoriznotal()->Add(_pTreeNodeUI);
		//return CListContainerElementUI::Add(_pTreeNodeUI);
	}

	//************************************
	// 函数名称: AddAt
	// 返回类型: bool
	// 参数信息: CControlUI * pControl
	// 参数信息: int iIndex				该参数仅针对当前节点下的兄弟索引，并非列表视图索引
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::AddAt( CControlUI* pControl, int iIndex )
	{
		if(NULL == static_cast<CTreeNodeUI*>(pControl->GetInterface(_T("TreeNode"))))
			return uiFalse;

		CTreeNodeUI* pIndexNode = static_cast<CTreeNodeUI*>(mTreeNodes.GetAt(iIndex));
		if(!pIndexNode){
			if(!mTreeNodes.Add(pControl))
				return uiFalse;
		}
		else if(pIndexNode && !mTreeNodes.InsertAt(iIndex,pControl))
			return uiFalse;

		if(!pIndexNode && pTreeView && pTreeView->GetItemAt(GetTreeIndex()+1))
			pIndexNode = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(GetTreeIndex()+1)->GetInterface(_T("TreeNode")));

		pControl = CalLocation((CTreeNodeUI*)pControl);

		if(pTreeView)
		{
			if(pIndexNode)
				return pTreeView->AddAt((CTreeNodeUI*)pControl,pIndexNode);
			else 
				return pTreeView->Add((CTreeNodeUI*)pControl);
		}

		return uiTrue;
	}

	//************************************
	// 函数名称: Remove
	// 返回类型: bool
	// 参数信息: CControlUI * pControl
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::Remove( CControlUI* pControl, bool bDoNotDestroy  )
	{
		return RemoveNode((CTreeNodeUI*)pControl);
	}

	//************************************
	// 函数名称: SetVisibleTag
	// 返回类型: void
	// 参数信息: bool _IsVisible
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetVisibleTag( bool _IsVisible )
	{
		m_bIsVisable = _IsVisible;
	}

	//************************************
	// 函数名称: GetVisibleTag
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::GetVisibleTag()
	{
		return m_bIsVisable;
	}

	//************************************
	// 函数名称: SetItemText
	// 返回类型: void
	// 参数信息: LPCTSTR pstrValue
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetItemText( LPCTSTR pstrValue )
	{
		pItemButton->SetText(pstrValue);
	}

	//************************************
	// 函数名称: GetItemText
	// 返回类型: DuiLib::CDuiString
	// 函数说明: 
	//************************************
	CDuiString CTreeNodeUI::GetItemText()
	{
		return pItemButton->GetText();
	}

	//************************************
	// 函数名称: CheckBoxSelected
	// 返回类型: void
	// 参数信息: bool _Selected
	// 函数说明: 
	//************************************
	void CTreeNodeUI::CheckBoxSelected( bool _Selected )
	{
		pCheckBox->Selected(_Selected);
	}

	//************************************
	// 函数名称: IsCheckBoxSelected
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::IsCheckBoxSelected() const
	{
		return pCheckBox->IsSelected();
	}

	//************************************
	// 函数名称: IsHasChild
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::IsHasChild() const
	{
		return !mTreeNodes.IsEmpty();
	}

	long CTreeNodeUI::GetTreeLevel()
	{
		long level = 0;
		CTreeNodeUI* pParentNode = GetParentNode();
		while(pParentNode != NULL) {
			level++;
			pParentNode = pParentNode->GetParentNode();
		}
		return level;
	}

	//************************************
	// 函数名称: AddChildNode
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * _pTreeNodeUI
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::AddChildNode( CTreeNodeUI* _pTreeNodeUI )
	{
		if (!_pTreeNodeUI)
			return uiFalse;

		if (NULL == static_cast<CTreeNodeUI*>(_pTreeNodeUI->GetInterface(_T("TreeNode"))))
			return uiFalse;

		_pTreeNodeUI = CalLocation(_pTreeNodeUI);

		bool nRet = uiTrue;

		if(pTreeView){
			CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(mTreeNodes.GetAt(mTreeNodes.GetSize()-1));
			if(!pNode || !pNode->GetLastNode())
				nRet = pTreeView->AddAt(_pTreeNodeUI,GetTreeIndex()+1);
			else nRet = pTreeView->AddAt(_pTreeNodeUI,pNode->GetLastNode()->GetTreeIndex()+1);
		}

		if(nRet)
			mTreeNodes.Add(_pTreeNodeUI);

		return nRet;
	}

	//************************************
	// 函数名称: RemoveAt
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * _pTreeNodeUI
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::RemoveNode( CTreeNodeUI* _pTreeNodeUI )
	{
		int nIndex = mTreeNodes.Find(_pTreeNodeUI);
		CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(mTreeNodes.GetAt(nIndex));
		if(pNode && pNode == _pTreeNodeUI)
		{
			while(pNode->IsHasChild())
				pNode->RemoveNode(static_cast<CTreeNodeUI*>(pNode->mTreeNodes.GetAt(0)));

			mTreeNodes.Remove(nIndex);

			if(pTreeView)
				pTreeView->Remove(_pTreeNodeUI);

			return uiTrue;
		}
		return uiFalse;
	}

	//************************************
	// 函数名称: SetParentNode
	// 返回类型: void
	// 参数信息: CTreeNodeUI * _pParentTreeNode
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetParentNode( CTreeNodeUI* _pParentTreeNode )
	{
		pParentTreeNode = _pParentTreeNode;
	}

	//************************************
	// 函数名称: GetParentNode
	// 返回类型: CTreeNodeUI*
	// 函数说明: 
	//************************************
	CTreeNodeUI* CTreeNodeUI::GetParentNode()
	{
		return pParentTreeNode;
	}

	//************************************
	// 函数名称: GetCountChild
	// 返回类型: long
	// 函数说明: 
	//************************************
	long CTreeNodeUI::GetCountChild()
	{
		return mTreeNodes.GetSize();
	}

	//************************************
	// 函数名称: SetTreeView
	// 返回类型: void
	// 参数信息: CTreeViewUI * _CTreeViewUI
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetTreeView( CTreeViewUI* _CTreeViewUI )
	{
		pTreeView = _CTreeViewUI;
	}

	//************************************
	// 函数名称: GetTreeView
	// 返回类型: CTreeViewUI*
	// 函数说明: 
	//************************************
	CTreeViewUI* CTreeNodeUI::GetTreeView()
	{
		return pTreeView;
	}

	//************************************
	// 函数名称: SetAttribute
	// 返回类型: void
	// 参数信息: LPCTSTR pstrName
	// 参数信息: LPCTSTR pstrValue
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if(_tcsicmp(pstrName, _T("text")) == 0 )				pItemButton->SetText(pstrValue);
		else if(_tcsicmp(pstrName, _T("horizattr")) == 0 )		pHoriz->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("dotlineattr")) == 0 )	pDottedLine->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("folderattr")) == 0 )		pFolderButton->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("checkboxattr")) == 0 )	pCheckBox->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("itemattr")) == 0 )		pItemButton->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("itemtextcolor")) == 0 )		SetItemTextColor(pstrValue);
		else if(_tcsicmp(pstrName, _T("itemhottextcolor")) == 0 )	SetItemHotTextColor(pstrValue);
		else if(_tcsicmp(pstrName, _T("selitemtextcolor")) == 0 )	SetSelItemTextColor(pstrValue);
		else if(_tcsicmp(pstrName, _T("selitemhottextcolor")) == 0 ) SetSelItemHotTextColor(pstrValue);
		else CListContainerElementUI::SetAttribute(pstrName,pstrValue);
	}

	//************************************
	// 函数名称: GetTreeNodes
	// 返回类型: DuiLib::CStdPtrArray
	// 函数说明: 
	//************************************
	CStdPtrArray CTreeNodeUI::GetTreeNodes()
	{
		return mTreeNodes;
	}

	//************************************
	// 函数名称: GetChildNode
	// 返回类型: CTreeNodeUI*
	// 参数信息: int _nIndex
	// 函数说明: 
	//************************************
	CTreeNodeUI* CTreeNodeUI::GetChildNode( int _nIndex )
	{
		return static_cast<CTreeNodeUI*>(mTreeNodes.GetAt(_nIndex));
	}

	//************************************
	// 函数名称: SetVisibleFolderBtn
	// 返回类型: void
	// 参数信息: bool _IsVisibled
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetVisibleFolderBtn( bool _IsVisibled )
	{
		pFolderButton->SetVisible(_IsVisibled);
	}

	//************************************
	// 函数名称: GetVisibleFolderBtn
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::GetVisibleFolderBtn()
	{
		return pFolderButton->IsVisible();
	}

	//************************************
	// 函数名称: SetVisibleCheckBtn
	// 返回类型: void
	// 参数信息: bool _IsVisibled
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetVisibleCheckBtn( bool _IsVisibled )
	{
		pCheckBox->SetVisible(_IsVisibled);
	}

	//************************************
	// 函数名称: GetVisibleCheckBtn
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CTreeNodeUI::GetVisibleCheckBtn()
	{
		return pCheckBox->IsVisible();
	}

	//************************************
	// 函数名称: GetNodeIndex
	// 返回类型: int
	// 函数说明: 取得全局树视图的索引
	//************************************
	int CTreeNodeUI::GetTreeIndex()
	{
		if(!pTreeView)
			return -1;

		for(int nIndex = 0;nIndex < pTreeView->GetCount();nIndex++){
			if(this == pTreeView->GetItemAt(nIndex))
				return nIndex;
		}

		return -1;
	}

	//************************************
	// 函数名称: GetNodeIndex
	// 返回类型: int
	// 函数说明: 取得相对于兄弟节点的当前索引
	//************************************
	int CTreeNodeUI::GetNodeIndex()
	{
		if(!GetParentNode() && !pTreeView)
			return -1;

		if(!GetParentNode() && pTreeView)
			return GetTreeIndex();

		return GetParentNode()->GetTreeNodes().Find(this);
	}

	//************************************
	// 函数名称: GetLastNode
	// 返回类型: CTreeNodeUI*
	// 函数说明:
	//************************************
	CTreeNodeUI* CTreeNodeUI::GetLastNode( )
	{
		if(!IsHasChild()) return this;

		CTreeNodeUI* nRetNode = NULL;
		for(int nIndex = 0;nIndex < GetTreeNodes().GetSize();nIndex++){
			CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(GetTreeNodes().GetAt(nIndex));
			if(!pNode) continue;
			if(pNode->IsHasChild())
				nRetNode = pNode->GetLastNode();
			else 
				nRetNode = pNode;
		}

		return nRetNode;
	}

	//************************************
	// 函数名称: CalLocation
	// 返回类型: CTreeNodeUI*
	// 参数信息: CTreeNodeUI * _pTreeNodeUI
	// 函数说明: 缩进计算
	//************************************
	CTreeNodeUI* CTreeNodeUI::CalLocation( CTreeNodeUI* _pTreeNodeUI )
	{
		//_pTreeNodeUI->GetDottedLine()->SetVisible(TRUE);
		//_pTreeNodeUI->GetDottedLine()->SetFixedWidth(pDottedLine->GetFixedWidth()+16);
		_pTreeNodeUI->GetDottedLine()->SetVisible(false);
		_pTreeNodeUI->SetParentNode(this);
		_pTreeNodeUI->GetItemButton()->SetGroup(pItemButton->GetGroup());
		_pTreeNodeUI->SetTreeView(pTreeView);

		return _pTreeNodeUI;
	}

	//************************************
	// 函数名称: SetTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwTextColor
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetItemTextColor(CDuiColor _dwItemTextColor )
	{
		m_dwItemTextColor	= _dwItemTextColor;
		pItemButton->SetTextColor(m_dwItemTextColor);
	}

	//************************************
	// 函数名称: GetTextColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	CDuiColor CTreeNodeUI::GetItemTextColor() const
	{
		return m_dwItemTextColor;
	}

	//************************************
	// 函数名称: SetTextHotColor
	// 返回类型: void
	// 参数信息: DWORD _dwTextHotColor
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetItemHotTextColor(CDuiColor _dwItemHotTextColor )
	{
		m_dwItemHotTextColor = _dwItemHotTextColor;
		Invalidate();
	}

	//************************************
	// 函数名称: GetTextHotColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	CDuiColor CTreeNodeUI::GetItemHotTextColor() const
	{
		return m_dwItemHotTextColor;
	}

	//************************************
	// 函数名称: SetSelItemTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwSelItemTextColor
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetSelItemTextColor(CDuiColor _dwSelItemTextColor )
	{
		m_dwSelItemTextColor = _dwSelItemTextColor;
		Invalidate();
	}

	//************************************
	// 函数名称: GetSelItemTextColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	CDuiColor CTreeNodeUI::GetSelItemTextColor() const
	{
		return m_dwSelItemTextColor;
	}

	//************************************
	// 函数名称: SetSelHotItemTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwSelHotItemTextColor
	// 函数说明: 
	//************************************
	void CTreeNodeUI::SetSelItemHotTextColor(CDuiColor _dwSelHotItemTextColor )
	{
		m_dwSelItemHotTextColor = _dwSelHotItemTextColor;
		Invalidate();
	}

	//************************************
	// 函数名称: GetSelHotItemTextColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	CDuiColor CTreeNodeUI::GetSelItemHotTextColor() const
	{
		return m_dwSelItemHotTextColor;
	}

	/*****************************************************************************/
	/*****************************************************************************/
	/*****************************************************************************/
	IMPLEMENT_DUICONTROL(CTreeViewUI)

		//************************************
		// 函数名称: CTreeViewUI
		// 返回类型: 
		// 参数信息: void
		// 函数说明: 
		//************************************
		CTreeViewUI::CTreeViewUI( void ) : m_bVisibleFolderBtn(uiTrue),m_bVisibleCheckBtn(uiFalse),m_uItemMinWidth(0)
	{
		this->GetHeader()->SetVisible(uiFalse);
	}

	//************************************
	// 函数名称: ~CTreeViewUI
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	CTreeViewUI::~CTreeViewUI( void )
	{

	}

	//************************************
	// 函数名称: GetClass
	// 返回类型: LPCTSTR
	// 函数说明: 
	//************************************
	LPCTSTR CTreeViewUI::GetClass() const
	{
		return _T("TreeViewUI");
	}


	UINT CTreeViewUI::GetListType()
	{
		return LT_TREE;
	}

	//************************************
	// 函数名称: GetInterface
	// 返回类型: LPVOID
	// 参数信息: LPCTSTR pstrName
	// 函数说明: 
	//************************************
	LPVOID CTreeViewUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcsicmp(pstrName, DUI_CTR_TREEVIEW) == 0 ) return static_cast<CTreeViewUI*>(this);
		return CListUI::GetInterface(pstrName);
	}

	//************************************
	// 函数名称: Add
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * pControl
	// 函数说明: 
	//************************************
	bool CTreeViewUI::Add( CControlUI* pControl1 )
	{
		if (!pControl1) return false;
		CTreeNodeUI *pControl = static_cast<CTreeNodeUI*>(pControl1->GetInterface(DUI_CTR_TREENODE));
		if (!pControl) return false;

		pControl->OnNotify += MakeDelegate(this,&CTreeViewUI::OnDBClickItem);
		pControl->GetFolderButton()->OnNotify += MakeDelegate(this,&CTreeViewUI::OnFolderChanged);
		pControl->GetCheckBox()->OnNotify += MakeDelegate(this,&CTreeViewUI::OnCheckBoxChanged);

		pControl->SetVisibleFolderBtn(m_bVisibleFolderBtn);
		pControl->SetVisibleCheckBtn(m_bVisibleCheckBtn);
		if(m_uItemMinWidth > 0)
			pControl->SetMinWidth(m_uItemMinWidth);

		CListUI::Add(pControl);

		int nLevel = pControl->GetTreeLevel();
		int nFolderWidth = pControl->GetFolderButton()->GetFixedWidth();
		if(nFolderWidth <= 0) nFolderWidth = 16;
		if(!pControl->GetFolderButton()->IsVisible()) nFolderWidth = 0;
		pControl->GetFolderButton()->SetPadding(CDuiRect(nLevel * nFolderWidth, 0, 0, 0));

		if(pControl->GetCountChild() > 0) {
			int nCount = pControl->GetCountChild();
			for(int nIndex = 0;nIndex < nCount;nIndex++) {
				CTreeNodeUI* pNode = pControl->GetChildNode(nIndex);
				if(pNode) Add(pNode);
			}
		}

		pControl->SetTreeView(this);
		return true;
	}

	//************************************
	// 函数名称: AddAt
	// 返回类型: long
	// 参数信息: CTreeNodeUI * pControl
	// 参数信息: int iIndex
	// 函数说明: 该方法不会将待插入的节点进行缩位处理，若打算插入的节点为非根节点，请使用AddAt(CTreeNodeUI* pControl,CTreeNodeUI* _IndexNode) 方法
	//************************************
	bool CTreeViewUI::AddAt( CControlUI* pControl1, int iIndex )
	{
		if (!pControl1) return false;
		CTreeNodeUI *pControl = static_cast<CTreeNodeUI*>(pControl1->GetInterface(DUI_CTR_TREENODE));
		if (!pControl) return false;

		pControl->OnNotify += MakeDelegate(this,&CTreeViewUI::OnDBClickItem);
		pControl->GetFolderButton()->OnNotify += MakeDelegate(this,&CTreeViewUI::OnFolderChanged);
		pControl->GetCheckBox()->OnNotify += MakeDelegate(this,&CTreeViewUI::OnCheckBoxChanged);
		pControl->SetVisibleFolderBtn(m_bVisibleFolderBtn);
		pControl->SetVisibleCheckBtn(m_bVisibleCheckBtn);

		if(m_uItemMinWidth > 0) {
			pControl->SetMinWidth(m_uItemMinWidth);
		}

		if(!CListUI::AddAt(pControl, iIndex))
			return false;

		int nLevel = pControl->GetTreeLevel();
		int nFolderWidth = pControl->GetFolderButton()->GetFixedWidth();
		if(nFolderWidth <= 0) nFolderWidth = 16;
		if(!pControl->GetFolderButton()->IsVisible()) nFolderWidth = 0;
		pControl->GetFolderButton()->SetPadding(CDuiRect(nLevel * nFolderWidth, 0, 0, 0));

		if(pControl->GetCountChild() > 0) {
			int nCount = pControl->GetCountChild();
			for(int nIndex = 0; nIndex < nCount; nIndex++) {
				CTreeNodeUI* pNode = pControl->GetChildNode(nIndex);
				if(pNode)
					return AddAt(pNode,iIndex+1);
			}
		}
		else {
			return true;
		}

		return false;
	}

	//************************************
	// 函数名称: AddAt
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * pControl
	// 参数信息: CTreeNodeUI * _IndexNode
	// 函数说明:
	//************************************
	bool CTreeViewUI::AddAt( CControlUI* pControl1, CControlUI* _IndexNode )
	{
		if (!pControl1) return false;
		CTreeNodeUI *pControl = static_cast<CTreeNodeUI*>(pControl1->GetInterface(DUI_CTR_TREENODE));
		if (!pControl) return false;

		if(!_IndexNode && !pControl)
			return false;

		int nItemIndex = -1;
		for(int nIndex = 0;nIndex < GetCount();nIndex++) {
			if(_IndexNode == GetItemAt(nIndex)) {
				nItemIndex = nIndex;
				break;
			}
		}

		if(nItemIndex == -1)
			return false;

		bool bRet = AddAt(pControl,nItemIndex);
		if(bRet) {
			int nLevel = pControl->GetTreeLevel();
			int nFolderWidth = pControl->GetFolderButton()->GetFixedWidth();
			if(nFolderWidth <= 0) nFolderWidth = 16;
			if(!pControl->GetFolderButton()->IsVisible()) nFolderWidth = 0;
			pControl->GetFolderButton()->SetPadding(CDuiRect(nLevel * nFolderWidth, 0, 0, 0));
		}

		return bRet;
	}
	
	//************************************
	// 函数名称: Remove
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * pControl
	// 函数说明: pControl 对象以及下的所有节点将被一并移除
	//************************************
	bool CTreeViewUI::Remove( CControlUI* pControl1, bool bDoNotDestroy  )
	{
		if (!pControl1) return false;
		CTreeNodeUI *pControl = static_cast<CTreeNodeUI*>(pControl1->GetInterface(DUI_CTR_TREENODE));
		if (!pControl) return false;

		if(pControl->GetCountChild() > 0) {
			int nCount = pControl->GetCountChild();
			for(int nIndex = nCount - 1; nIndex >= 0; nIndex--) {
				CTreeNodeUI* pNode = pControl->GetChildNode(nIndex);
				if(pNode){
					pControl->Remove(pNode);
				}
			}
		}
		CListUI::Remove(pControl);
		return uiTrue;
	}

	//************************************
	// 函数名称: RemoveAt
	// 返回类型: bool
	// 参数信息: int iIndex
	// 函数说明: iIndex 索引以及下的所有节点将被一并移除
	//************************************
	bool CTreeViewUI::RemoveAt( int iIndex, bool bDoNotDestroy )
	{
		CTreeNodeUI* pItem = (CTreeNodeUI*)GetItemAt(iIndex);
		if(pItem->GetCountChild())
			Remove(pItem);
		return uiTrue;
	}

	void CTreeViewUI::RemoveAll()
	{
		CListUI::RemoveAll();
	}

	//************************************
	// 函数名称: Notify
	// 返回类型: void
	// 参数信息: TNotifyUI & msg
	// 函数说明: 
	//************************************
	void CTreeViewUI::Notify( TNotifyUI& msg )
	{

	}

	//************************************
	// 函数名称: OnCheckBoxChanged
	// 返回类型: bool
	// 参数信息: void * param
	// 函数说明: 
	//************************************
	bool CTreeViewUI::OnCheckBoxChanged( void* param )
	{
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if(pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			CCheckBoxUI* pCheckBox = (CCheckBoxUI*)pMsg->pSender;
			CTreeNodeUI* pItem = (CTreeNodeUI*)pCheckBox->GetParent()->GetParent();
			SetItemCheckBox(pCheckBox->GetCheck(),pItem);
			return uiTrue;
		}
		return uiTrue;
	}

	//************************************
	// 函数名称: OnFolderChanged
	// 返回类型: bool
	// 参数信息: void * param
	// 函数说明: 
	//************************************
	bool CTreeViewUI::OnFolderChanged( void* param )
	{
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if(pMsg->sType == DUI_MSGTYPE_SELECTCHANGED) {
			CCheckBoxUI* pFolder = (CCheckBoxUI*)pMsg->pSender;
			CTreeNodeUI* pItem = (CTreeNodeUI*)pFolder->GetParent()->GetParent();
			pItem->SetVisibleTag(!pFolder->GetCheck());
			SetItemExpand(!pFolder->GetCheck(),pItem);
			return uiTrue;
		}
		return uiTrue;
	}

	//************************************
	// 函数名称: OnDBClickItem
	// 返回类型: bool
	// 参数信息: void * param
	// 函数说明:
	//************************************
	bool CTreeViewUI::OnDBClickItem( void* param )
	{
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if(_tcsicmp(pMsg->sType, DUI_MSGTYPE_TREEITEMDBCLICK) == 0) {
			CTreeNodeUI* pItem		= static_cast<CTreeNodeUI*>(pMsg->pSender);
			CCheckBoxUI* pFolder	= pItem->GetFolderButton();
			pFolder->Selected(!pFolder->IsSelected());
			pItem->SetVisibleTag(!pFolder->GetCheck());
			SetItemExpand(!pFolder->GetCheck(),pItem);
			return uiTrue;
		}
		return uiFalse;
	}

	//************************************
	// 函数名称: SetItemCheckBox
	// 返回类型: bool
	// 参数信息: bool _Selected
	// 参数信息: CTreeNodeUI * _TreeNode
	// 函数说明: 
	//************************************
	bool CTreeViewUI::SetItemCheckBox( bool _Selected,CTreeNodeUI* _TreeNode /*= NULL*/ )
	{
		if(_TreeNode) {
			if(_TreeNode->GetCountChild() > 0) {
				int nCount = _TreeNode->GetCountChild();
				for(int nIndex = 0;nIndex < nCount;nIndex++) {
					CTreeNodeUI* pItem = _TreeNode->GetChildNode(nIndex);
					pItem->GetCheckBox()->Selected(_Selected);
					if(pItem->GetCountChild())
						SetItemCheckBox(_Selected,pItem);
				}
			}
			return uiTrue;
		}
		else {
			int nIndex = 0;
			int nCount = GetCount();
			while(nIndex < nCount) {
				CTreeNodeUI* pItem = (CTreeNodeUI*)GetItemAt(nIndex);
				pItem->GetCheckBox()->Selected(_Selected);
				if(pItem->GetCountChild())
					SetItemCheckBox(_Selected,pItem);

				nIndex++;
			}
			return uiTrue;
		}
		return uiFalse;
	}

	//************************************
	// 函数名称: SetItemExpand
	// 返回类型: void
	// 参数信息: bool _Expanded
	// 参数信息: CTreeNodeUI * _TreeNode
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetItemExpand( bool _Expanded,CTreeNodeUI* _TreeNode /*= NULL*/ )
	{
		if(_TreeNode) {
			if(_TreeNode->GetCountChild() > 0) {
				int nCount = _TreeNode->GetCountChild();
				for(int nIndex = 0;nIndex < nCount;nIndex++) {
					CTreeNodeUI* pItem = _TreeNode->GetChildNode(nIndex);
					pItem->SetVisible(_Expanded);
					if(pItem->GetCountChild() && !pItem->GetFolderButton()->IsSelected()) {
						SetItemExpand(_Expanded,pItem);
					}
				}
			}
		}
		else {
			int nIndex = 0;
			int nCount = GetCount();
			while(nIndex < nCount) {
				CTreeNodeUI* pItem = (CTreeNodeUI*)GetItemAt(nIndex);
				pItem->SetVisible(_Expanded);
				if(pItem->GetCountChild() && !pItem->GetFolderButton()->IsSelected()) {
					SetItemExpand(_Expanded,pItem);
				}
				nIndex++;
			}
		}
	}

	//************************************
	// 函数名称: SetVisibleFolderBtn
	// 返回类型: void
	// 参数信息: bool _IsVisibled
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetVisibleFolderBtn( bool _IsVisibled )
	{
		m_bVisibleFolderBtn = _IsVisibled;
		int nCount = this->GetCount();
		for(int nIndex = 0; nIndex < nCount; nIndex++) {
			CTreeNodeUI* pItem = static_cast<CTreeNodeUI*>(this->GetItemAt(nIndex));
			pItem->GetFolderButton()->SetVisible(m_bVisibleFolderBtn);
		}
	}

	//************************************
	// 函数名称: GetVisibleFolderBtn
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CTreeViewUI::GetVisibleFolderBtn()
	{
		return m_bVisibleFolderBtn;
	}

	//************************************
	// 函数名称: SetVisibleCheckBtn
	// 返回类型: void
	// 参数信息: bool _IsVisibled
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetVisibleCheckBtn( bool _IsVisibled )
	{
		m_bVisibleCheckBtn = _IsVisibled;
		int nCount = this->GetCount();
		for(int nIndex = 0; nIndex < nCount; nIndex++) {
			CTreeNodeUI* pItem = static_cast<CTreeNodeUI*>(this->GetItemAt(nIndex));
			pItem->GetCheckBox()->SetVisible(m_bVisibleCheckBtn);
		}
	}

	//************************************
	// 函数名称: GetVisibleCheckBtn
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CTreeViewUI::GetVisibleCheckBtn()
	{
		return m_bVisibleCheckBtn;
	}

	//************************************
	// 函数名称: SetItemMinWidth
	// 返回类型: void
	// 参数信息: UINT _ItemMinWidth
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetItemMinWidth( UINT _ItemMinWidth )
	{
		m_uItemMinWidth = _ItemMinWidth;

		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetMinWidth(GetItemMinWidth());
			}
		}
		Invalidate();
	}

	//************************************
	// 函数名称: GetItemMinWidth
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT CTreeViewUI::GetItemMinWidth()
	{
		return m_uItemMinWidth;
	}

	//************************************
	// 函数名称: SetItemTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwItemTextColor
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetItemTextColor(CDuiColor _dwItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetItemTextColor(_dwItemTextColor);
			}
		}
	}

	//************************************
	// 函数名称: SetItemHotTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwItemHotTextColor
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetItemHotTextColor(CDuiColor _dwItemHotTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetItemHotTextColor(_dwItemHotTextColor);
			}
		}
	}

	//************************************
	// 函数名称: SetSelItemTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwSelItemTextColor
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetSelItemTextColor(CDuiColor _dwSelItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetSelItemTextColor(_dwSelItemTextColor);
			}
		}
	}

	//************************************
	// 函数名称: SetSelItemHotTextColor
	// 返回类型: void
	// 参数信息: DWORD _dwSelHotItemTextColor
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetSelItemHotTextColor(CDuiColor _dwSelHotItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode) {
				pTreeNode->SetSelItemHotTextColor(_dwSelHotItemTextColor);
			}
		}
	}

	//************************************
	// 函数名称: SetAttribute
	// 返回类型: void
	// 参数信息: LPCTSTR pstrName
	// 参数信息: LPCTSTR pstrValue
	// 函数说明: 
	//************************************
	void CTreeViewUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if(_tcsicmp(pstrName,_T("visiblefolderbtn")) == 0)			SetVisibleFolderBtn(_tcsicmp(pstrValue,_T("true")) == 0);
		else if(_tcsicmp(pstrName,_T("visiblecheckbtn")) == 0)		SetVisibleCheckBtn(_tcsicmp(pstrValue,_T("true")) == 0);
		else if(_tcsicmp(pstrName,_T("itemminwidth")) == 0)			SetItemMinWidth(_ttoi(pstrValue));
		else if(_tcsicmp(pstrName, _T("itemtextcolor")) == 0 )		SetItemTextColor(pstrValue);
		else if(_tcsicmp(pstrName, _T("itemhottextcolor")) == 0 )	SetItemHotTextColor(pstrValue);
		else if(_tcsicmp(pstrName, _T("selitemtextcolor")) == 0 )	SetSelItemTextColor(pstrValue);
		else if(_tcsicmp(pstrName, _T("selitemhottextcolor")) == 0 ) SetSelItemHotTextColor(pstrValue);
		else CListUI::SetAttribute(pstrName,pstrValue);
	}

}

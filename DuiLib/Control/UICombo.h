#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {

#ifndef WIN32
#define CBS_DROPDOWN          0x0002L	//带编辑框的下拉框
#define CBS_DROPDOWNLIST      0x0003L	//下拉框
#endif
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CComboWnd;

	class UILIB_API CComboUI : public CContainerUI, public IListOwnerUI
	{
		DECLARE_DUICONTROL(CComboUI)
		friend class CComboWnd;
		friend class CEditWnd;
	public:
		CComboUI();
		virtual ~CComboUI();

		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;

		virtual void DoInit() override;
		virtual UINT GetControlFlags() const override;
	
		bool IsShowShadow();
		void SetShowShadow(bool bShow = true);

		CDuiString GetDropBoxAttributeList();
		void SetDropBoxAttributeList(LPCTSTR pstrList);
		CDuiSize GetDropBoxSize() const;
		void SetDropBoxSize(CDuiSize szDropBox);

		virtual UINT GetListType() override;
		virtual TListInfoUI* GetListInfo() override;
		virtual int GetCurSel() const override;  
		bool SelectItem(LPCTSTR pstrText, bool bTriggerEvent=true);
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent=true) override;
		virtual bool SelectMultiItem(int iIndex, bool bTakeFocus = false) override;
		virtual bool UnSelectItem(int iIndex, bool bOthers = false) override;
		virtual bool SetItemIndex(CControlUI* pControl, int iIndex) override;

		virtual bool Add(CControlUI* pControl) override;
		virtual bool AddAt(CControlUI* pControl, int iIndex) override;
		virtual bool Remove(CControlUI* pControl, bool bDoNotDestroy=false) override;
		virtual bool RemoveAt(int iIndex, bool bDoNotDestroy=false) override;
		virtual void RemoveAll() override;

		virtual bool Activate() override;

		virtual CDuiSize EstimateSize(CDuiSize szAvailable) override;
		virtual void SetPos(CDuiRect rc, bool bNeedInvalidate = true) override;
		virtual void Move(CDuiSize szOffset, bool bNeedInvalidate = true) override;
		virtual void DoEvent(TEventUI& event) override;
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

		virtual bool DoPaint(UIRender *pRender, const CDuiRect& rcPaint, CControlUI* pStopControl) override;
		virtual bool DrawDropButtonImage(UIRender *pRender, LPCTSTR pStrImage, LPCTSTR pStrModify=NULL);
		virtual void PaintText(UIRender *pRender) override;

		virtual void SetText(LPCTSTR pstrText) override;

		// 客户区域（除去scrollbar、inset、下拉按钮）
		virtual CDuiRect GetClientPos() override; 

		CControlUI *AddString(LPCTSTR pstrText, UINT_PTR pItemData=0);
		bool DeleteString(LPCTSTR pstrText); 
		bool DeleteString_byItemData(UINT_PTR pItemData);

		bool SelectString(LPCTSTR pstrText);
		bool SetCurSel(int iIndex, bool bTakeFocus = false);
		bool SetCurSelFromItemData(UINT_PTR ptrItemData);
		bool SetItemData(CControlUI *pControl, UINT_PTR ptrItemData);
		UINT_PTR GetCurSelItemData();

		LPCTSTR GetdbNormalImage() const;
		void SetdbNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetdbHotImage() const;
		void SetdbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetdbPushedImage() const;
		void SetdbPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetdbFocusedImage() const;
		void SetdbFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetdbDisabledImage() const;
		void SetdbDisabledImage(LPCTSTR pStrImage);

		//CBS_DROPDOWN, CBS_DROPDOWNLIST
		int GetDropType() const;
		//CBS_DROPDOWN, CBS_DROPDOWNLIST
		void SetDropType(int type);

		void SetTipValue(LPCTSTR pStrTipValue);
		CDuiString GetTipValue();
		void SetTipValueColor(CDuiColor dwColor);
		CDuiColor GetTipValueColor();

		CDuiRect GetDropButtonRect();
	protected:
		CComboWnd* m_pWindow;

		int m_iCurSel;
		
		bool	m_bShowShadow;
		CDuiString m_sDropBoxAttributes;
		CDuiSize m_szDropBox;

		TListInfoUI m_ListInfo;

		CEditWnd *m_pEditWindow;
		int m_type; //CBS_DROPDOWN CBS_DROPDOWNLIST
		CDuiSize m_szDropButtonSize;
		CDuiString m_dbNormalImage;
		CDuiString m_dbHotImage;
		CDuiString m_dbPushedImage;
		CDuiString m_dbFocusedImage;
		CDuiString m_dbDisabledImage;
		CDuiString m_sTipValue;
		CDuiColor m_dwTipValueColor;
	};

} // namespace DuiLib
#endif // __UICOMBO_H__


#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

namespace DuiLib
{
	class CEditWnd;
	class UILIB_API CEditUI : public CLabelUI
	{
		DECLARE_DUICONTROL(CEditUI)
		friend class CEditWnd;
	public:
		CEditUI();
		virtual ~CEditUI();

		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;
		virtual UINT GetControlFlags() const override;

		void SetMaxChar(UINT uMax)					{ m_uMaxChar = uMax; }
		UINT GetMaxChar()							{ return m_uMaxChar; }
		void SetPasswordMode(bool bPasswordMode)	{ m_bPasswordMode = bPasswordMode; }
		bool IsPasswordMode() const					{ return m_bPasswordMode; }
		void SetPasswordChar(TCHAR cPasswordChar)	{ m_cPasswordChar = cPasswordChar; }
		TCHAR GetPasswordChar() const				{ return m_cPasswordChar; }
		void SetNumberOnly(bool bNumberOnly)		{ m_bNumberOnly = bNumberOnly; }
		bool IsNumberOnly() const					{ return m_bNumberOnly; }
		void SetLowerCase(bool bLowerCase)			{ m_bLowerCase = bLowerCase; }
		bool IsLowerCase() const					{ return m_bLowerCase; }
		void SetUpperCase(bool bUpperCase)			{ m_bUpperCase = bUpperCase; }
		bool IsUpperCase() const					{ return m_bUpperCase; }
		void SetMultiLine(bool bMultiLine)			{ m_bMultiLine = bMultiLine; }
		bool IsMultiLine() const					{ return m_bMultiLine; }
		void SetWantReturn(bool bWantReturn = true) { m_bWantReturn = bWantReturn; }
		bool IsWantReturn()							{ return m_bWantReturn; }

		void SetNativeEditBkColor(CDuiColor dwColor)	{ m_dwEditbkColor = dwColor; }
		CDuiColor GetNativeEditBkColor() const			{ return m_dwEditbkColor; }
		void SetNativeEditTextColor(CDuiColor dwColor ) { m_dwEditTextColor = dwColor; }
		CDuiColor GetNativeEditTextColor() const		{ return m_dwEditTextColor; }

		bool IsAutoSelAll()							{ return m_bAutoSelAll; }
		void SetAutoSelAll(bool bAutoSelAll)		{ m_bAutoSelAll = bAutoSelAll; }

		void SetTipValue(LPCTSTR pStrTipValue)		{ m_sTipValue = pStrTipValue; }
		CDuiString GetTipValue()					{ return m_sTipValue; }
		void SetTipValueColor(CDuiColor dwColor)		{ m_dwTipValueColor = dwColor; }
		CDuiColor GetTipValueColor()					{ return m_dwTipValueColor; }

		virtual CDuiString GetText() const override;
		void SetText(LPCTSTR pstrText) override;

		void SetSel(long nStartChar, long nEndChar);
		void SetSelAll();
		void SetReplaceSel(LPCTSTR lpszReplace);

		void SetPos(CDuiRect rc, bool bNeedInvalidate = true) override;
		void Move(CDuiSize szOffset, bool bNeedInvalidate = true) override;
		void SetVisible(bool bVisible = true) override;
		void SetInternVisible(bool bVisible = true) override;
		CDuiSize EstimateSize(CDuiSize szAvailable) override;
		void DoEvent(TEventUI& event) override;
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

		void PaintText(UIRender *pRender) override;

		virtual bool OnEnableResponseDefaultKeyEvent(WPARAM wParam) override;
	protected:
		CEditWnd* m_pWindow;

		UINT m_uMaxChar;
		bool m_bPasswordMode;
		bool m_bAutoSelAll;
		TCHAR m_cPasswordChar;
		bool m_bNumberOnly;
		bool m_bLowerCase;
		bool m_bUpperCase;
		bool m_bMultiLine;
		bool m_bWantReturn;

		CDuiString m_sTipValue;
		CDuiColor m_dwTipValueColor;
		CDuiColor m_dwEditbkColor;
		CDuiColor m_dwEditTextColor;
	};

	//////////////////////////////////////////////////////////////////////////
	class UILIB_API CEditWnd
	{
	public:
		CEditWnd(CControlUI *pOwner);
		virtual ~CEditWnd();

		CControlUI *GetOwner() const	{ return m_pOwner; }
		virtual void Init() = 0;
		virtual CDuiRect CalPos() = 0;
		virtual void SetEditText(LPCTSTR sText) = 0;
		virtual void SetOwnerText(LPCTSTR sText);

		UINT GetMaxChar();
		bool IsReadOnly() const;
		bool IsPasswordMode() const;
		TCHAR GetPasswordChar() const;
		bool IsNumberOnly() const;
		bool IsLowerCase() const;
		bool IsUpperCase() const;
		bool IsMultiLine() const;
		bool IsWantReturn();
		CDuiColor GetNativeEditBkColor() const;
		CDuiColor GetNativeEditTextColor() const;
		bool IsAutoSelAll();
	protected:
		CControlUI* m_pOwner;
	};

}
#endif // __UIEDIT_H__


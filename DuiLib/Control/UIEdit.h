#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

namespace DuiLib
{
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

		void SetNativeEditBkColor(DWORD dwColor)	{ m_dwEditbkColor = dwColor; }
		DWORD GetNativeEditBkColor() const			{ return m_dwEditbkColor; }
		void SetNativeEditTextColor( DWORD dwColor ) { m_dwEditTextColor = dwColor; }
		DWORD GetNativeEditTextColor() const		{ return m_dwEditTextColor; }

		bool IsAutoSelAll()							{ return m_bAutoSelAll; }
		void SetAutoSelAll(bool bAutoSelAll)		{ m_bAutoSelAll = bAutoSelAll; }

		void SetTipValue(LPCTSTR pStrTipValue)		{ m_sTipValue = pStrTipValue; }
		CDuiString GetTipValue()					{ return m_sTipValue; }
		void SetTipValueColor(DWORD dwColor)		{ m_dwTipValueColor = dwColor; }
		DWORD GetTipValueColor()					{ return m_dwTipValueColor; }

		virtual CDuiString GetText() const;
		void SetText(LPCTSTR pstrText);

		void SetSel(long nStartChar, long nEndChar);
		void SetSelAll();
		void SetReplaceSel(LPCTSTR lpszReplace);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(UIRender *pRender);

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
		DWORD m_dwTipValueColor;
		DWORD m_dwEditbkColor;
		DWORD m_dwEditTextColor;
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
		DWORD GetNativeEditBkColor() const;
		DWORD GetNativeEditTextColor() const;
		bool IsAutoSelAll();
	protected:
		CControlUI* m_pOwner;
	};

}
#endif // __UIEDIT_H__
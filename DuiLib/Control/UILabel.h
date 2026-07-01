#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib
{
	template< typename T = CControlUI>
	class TemplateLabelUI : public T
	//class TemplateLabelUI : public CContainerUI
	{
	public:
		TemplateLabelUI() {}
		virtual ~TemplateLabelUI() {}

		virtual UINT GetControlFlags() const override
		{
			return this->IsEnabled() ? UIFLAG_SETCURSOR : 0;
		}

		virtual CDuiSize EstimateSize(CDuiSize szAvailable) override
		{
			if (this->IsAutoCalcWidth() || this->IsAutoCalcHeight())
			{			
				CDuiSize szFact = szAvailable;

				if(this->IsAutoCalcWidth())
				{
					CDuiRect rcText = szAvailable;
					CDuiRect rcTextPadding = this->GetTextPadding();
					this->GetManager()->Render()->DrawText(rcText, rcTextPadding, this->GetText(), this->GetTextColor(), this->GetFont(), DT_CALCRECT | this->GetTextStyle());

					szFact.cx = rcText.right - rcText.left;
				}

				if(this->IsAutoCalcHeight())
				{
					int h = this->GetManager()->GetFontHeight(this->GetFont());
					szFact.cy = this->m_rcTextPadding.top + this->m_rcTextPadding.bottom + h + 4;
				}

				return this->GetManager()->GetDPIObj()->ScaleSize(szFact);
			}

			return CControlUI::EstimateSize(szAvailable);
		}

		virtual void DoEvent(TEventUI& event) override
		{
			if( event.Type == UIEVENT_SETFOCUS ) 
			{
				this->SetFocusState(true);
				return;
			}
			if( event.Type == UIEVENT_KILLFOCUS ) 
			{
				this->SetFocusState(false);
				return;
			}
			CControlUI::DoEvent(event);
		}

		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override
		{
			T::SetAttribute(pstrName, pstrValue);
		}

		virtual void PaintText(UIRender *pRender) override
		{
			CDuiString sText = this->GetText();
			if(sText.IsEmpty()) return;

			CDuiRect rcText = this->GetPos();
			CDuiColor dwColor = 0;
			int iFont = -1;

			//////////////////////////////////////////////////////////////////////////
			if( !this->IsEnabled() )
				iFont = this->GetDisabledFont();

			else if(this->IsSelectedState() )
				iFont = this->GetSelectedFont();

			else if(this->IsPushedState() )
				iFont = this->GetPushedFont();

			else if(this->IsHotState() )
				iFont = this->GetHotFont();

			else if(this->IsFocused() )
				iFont = this->GetFocusedFont();

			if(iFont == -1)
				iFont = this->GetFont();

			//////////////////////////////////////////////////////////////////////////
			if( !this->IsEnabled() )
				dwColor = this->GetDisabledTextColor();

			else if(this->IsSelectedState() )
				dwColor = this->GetSelectedTextColor();

			else if(this->IsPushedState() )
				dwColor = this->GetPushedTextColor();

			else if(this->IsHotState() )
				dwColor = this->GetHotTextColor();

			else if(this->IsFocused() )
				dwColor = this->GetFocusedTextColor();

			if(dwColor == 0)
				dwColor = this->GetTextColor();

			if(dwColor == 0 && this->GetManager())
				dwColor = this->GetManager()->GetDefaultFontColor();

			pRender->DrawText(rcText, this->GetTextPadding(), sText, dwColor, iFont, this->GetTextStyle());
			return;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	class UILIB_API CLabelHLayoutUI : public TemplateLabelUI<CHorizontalLayoutUI>
	{
	public:
		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;

		DECLARE_DUICONTROL(CLabelHLayoutUI)
	};

	//////////////////////////////////////////////////////////////////////////
	class UILIB_API CLabelVLayoutUI : public TemplateLabelUI<CVerticalLayoutUI>
	{
	public:
		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;

		DECLARE_DUICONTROL(CLabelVLayoutUI)
	};

	//////////////////////////////////////////////////////////////////////////
	class UILIB_API CLabelLayoutUI : public TemplateLabelUI<CDynamicLayoutUI>
	{
	public:
		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;

		DECLARE_DUICONTROL(CLabelLayoutUI)
	};

	//////////////////////////////////////////////////////////////////////////
	class UILIB_API CLabelUI : public CControlUI
	{
		DECLARE_DUICONTROL(CLabelUI)
	public:
		CLabelUI();
		~CLabelUI();

		LPCTSTR GetClass() const override;
		LPVOID GetInterface(LPCTSTR pstrName) override;
		UINT GetControlFlags() const override;

		CDuiSize EstimateSize(CDuiSize szAvailable) override;
		void DoEvent(TEventUI& event) override;
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

		virtual void PaintText(UIRender *pRender) override;
	};
}

#endif // __UILABEL_H__

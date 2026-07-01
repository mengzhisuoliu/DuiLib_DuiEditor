#ifndef __UIDYNAMICLAYOUT_H__
#define __UIDYNAMICLAYOUT_H__

#pragma once

namespace DuiLib
{
	enum emLayoutType
	{
		Layout_HorizontalLayout = 0,
		Layout_VerticalLayout,
		Layout_Tile
	};

	//允许动态设置布局方式的类
	class UILIB_API CDynamicLayoutUI : public CContainerUI
	{
		DECLARE_DUICONTROL(CDynamicLayoutUI)
	public:
		enum emSepAction
		{
			eSepNull = 0,
			eSepWidth,		//拖动宽度
			eSepHeight,		//拖动高度
			eSepWidthHeight	//拖动边角
		};
		CDynamicLayoutUI();

		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;
		virtual UINT GetControlFlags() const override;

		void SetLayout(emLayoutType eMode);
		emLayoutType GetLayout() const;

		void SetSepWidth(int iWidth);
		int GetSepWidth() const;
		void SetSepHeight(int iHeight);
		int GetSepHeight() const;
		void SetSepImmMode(bool bImmediately);
		bool IsSepImmMode() const;
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

		virtual void DoEvent(TEventUI& event) override;

		virtual CDuiSize EstimateSize(CDuiSize szAvailable) override;
		virtual void SetPos(CDuiRect rc, bool bNeedInvalidate = true) override;
		virtual void DoPostPaint(UIRender *pRender, const CDuiRect& rcPaint) override;

		virtual CDuiRect GetThumbRect(bool bUseNew = false) const;

	protected:
		int m_iSepWidth;
		int m_iSepHeight;
		CDuiRect m_rcNewPos;
		bool m_bImmMode;

		emLayoutType m_eLayout; 
		emSepAction m_eSepAction;
	};
}
#endif // __UIHORIZONTALLAYOUT_H__


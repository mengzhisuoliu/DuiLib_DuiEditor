#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace DuiLib
{
	class CDateTimeWnd;

	/// 时间选择控件
	class UILIB_API CDateTimeUI : public CLabelUI
	{
		DECLARE_DUICONTROL(CDateTimeUI)
		friend class CDateTimeWnd;
	public:
		CDateTimeUI();
		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;

		virtual void DoInit() override;

		SYSTEMTIME& GetTime();
		void SetTime(SYSTEMTIME* pst);

		void SetFormatStyle(UINT uStyle);
		UINT GetFormatStyle();

		virtual void SetText(LPCTSTR pstrText) override;
		virtual CDuiString GetText() const override;

		void SetPos(CDuiRect rc, bool bNeedInvalidate = true) override;
		void Move(CDuiSize szOffset, bool bNeedInvalidate = true) override;
		void SetVisible(bool bVisible = true) override;
		void SetInternVisible(bool bVisible = true) override;
		void DoEvent(TEventUI& event) override;
		virtual void PaintText(UIRender *pRender) override;

		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

	protected:
		SYSTEMTIME m_sysTime;
		int        m_uFormatStyle; //0=date, 1=time, 2=datetime  add by liqs99

		CDateTimeWnd* m_pWindowDate;
		CDateTimeWnd* m_pWindowTime;
	};
}
#endif // __UIDATETIME_H__
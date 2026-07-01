#ifndef __UIDateTimeEx_H__
#define __UIDateTimeEx_H__

#pragma once

#define UIDTS_DATE		0
#define UIDTS_TIME		1
#define UIDTS_DATETIME	2


namespace DuiLib
{
	//class CDateTimeExWnd;

	/// 时间选择控件，原有的功能合并到CDateTimeUI，此为了兼容保留
	class UILIB_API CDateTimeExUI : public CDateTimeUI
	{
		DECLARE_DUICONTROL(CDateTimeExUI)
	public:
		CDateTimeExUI();
		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;
	};
}
#endif // __UIDateTimeEx_H__


#pragma once


namespace DuiLib
{

class UILIB_API CComboExUI : public CComboUI
{
	DECLARE_DUICONTROL(CComboExUI)
public:
	CComboExUI(void);
	virtual ~CComboExUI(void);
	virtual LPCTSTR GetClass() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;
};


}
#include "StdAfx.h"
#include "UIComboEx.h"

namespace DuiLib
{

IMPLEMENT_DUICONTROL(CComboExUI)
CComboExUI::CComboExUI(void)
{
}

CComboExUI::~CComboExUI(void)
{
}

LPCTSTR CComboExUI::GetClass() const
{
	return _T("ComboExUI");
}

LPVOID CComboExUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_COMBOEX) == 0 ) return static_cast<CComboExUI*>(this);
	return CComboUI::GetInterface(pstrName);
}

}

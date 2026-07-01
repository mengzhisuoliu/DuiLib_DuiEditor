#include "StdAfx.h"
#include "UIDateTimeEx.h"
//#include "atlconv.h"

namespace DuiLib
{
	//////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_DUICONTROL(CDateTimeExUI)

	CDateTimeExUI::CDateTimeExUI() 
	{
	
	}
 
	LPCTSTR CDateTimeExUI::GetClass() const
	{
		return _T("DateTimeExUI");
	}

	LPVOID CDateTimeExUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_DATETIMEEX) == 0 ) return static_cast<CDateTimeExUI*>(this);
		return CDateTimeUI::GetInterface(pstrName);
	}
}



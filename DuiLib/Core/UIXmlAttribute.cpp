#include "StdAfx.h"

namespace DuiLib {
///////////////////////////////////////////////////////////////////////////////////////
//
//
//
#ifdef _UNICODE
	//
	#define XMLSTRA(x)	CDuiString(x)
	//
	#define XMLSTRW(x)	x
#else
	//
	#define XMLSTRA(x)	x
	//
	#define XMLSTRW(x)	CDuiString(x)
#endif

inline ui_pugi::xml_attribute impxmlattr(PVOID attr) { return ui_pugi::xml_attribute((ui_pugi::xml_attribute_struct *)attr); }

CXmlAttributeUI::CXmlAttributeUI() : _attr(0)
{
}

CXmlAttributeUI::CXmlAttributeUI(PVOID attr_struct) : _attr(attr_struct)
{
}

static void unspecified_bool_xml_attribute(CXmlAttributeUI***)
{
}

CXmlAttributeUI::operator CXmlAttributeUI::unspecified_bool_type() const
{
	return _attr ? unspecified_bool_xml_attribute : 0;
}

bool CXmlAttributeUI::operator!() const
{
	return !_attr;
}

bool CXmlAttributeUI::empty() const
{
	return impxmlattr(_attr).empty();
}

LPCTSTR CXmlAttributeUI::name() const
{
	return impxmlattr(_attr).name();
}

LPCTSTR CXmlAttributeUI::value() const
{
	return impxmlattr(_attr).value();
}

LPCTSTR CXmlAttributeUI::as_string(LPCTSTR def) const
{
	return impxmlattr(_attr).as_string(def);
}

int CXmlAttributeUI::as_int(int def) const
{
	return impxmlattr(_attr).as_int(def);
}

unsigned int CXmlAttributeUI::as_uint(unsigned int def) const
{
	return impxmlattr(_attr).as_uint(def);
}

double CXmlAttributeUI::as_double(double def) const
{
	return impxmlattr(_attr).as_double(def);
}

float CXmlAttributeUI::as_float(float def) const
{
	return impxmlattr(_attr).as_float(def);
}

bool CXmlAttributeUI::as_bool(bool def) const
{
	return impxmlattr(_attr).as_bool(def);
}

CDuiPoint CXmlAttributeUI::as_point(int def_x, int def_y) const
{
	CDuiPoint pt;
	if(!pt.FromString(as_string()))
	{
		pt.x = def_x;
		pt.y = def_y;
	}
	return pt;
}

CDuiSize CXmlAttributeUI::as_size(int def_cx, int def_cy) const
{
	CDuiSize sz;
	if(!sz.FromString(as_string()))
	{
		sz.cx = def_cx;
		sz.cy = def_cy;
	}
	return sz;
}

CDuiRect CXmlAttributeUI::as_rect(int def_left, int def_top, int def_right, int def_bottom) const
{
	CDuiRect rc;
	if(!rc.FromString(as_string()))
	{
		rc.left = def_left;
		rc.top = def_top;
		rc.right = def_right;
		rc.bottom = def_bottom;
	}
	return rc;
}

CDuiColor CXmlAttributeUI::as_uicolor(CDuiColor def_color) const
{
	CDuiColor clr;
	if (!clr.FromString(as_string()))
		clr = def_color;
	return clr;
}

bool CXmlAttributeUI::set_name(LPCSTR rhs)
{
	return impxmlattr(_attr).set_name(XMLSTRA(rhs));
}

bool CXmlAttributeUI::set_name(LPCWSTR rhs)
{
	return impxmlattr(_attr).set_name(XMLSTRW(rhs));
}

bool CXmlAttributeUI::set_value(LPCTSTR rhs)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(int rhs)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(unsigned int rhs)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(long rhs)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(unsigned long rhs)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(double rhs)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(double rhs, int precision)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(float rhs)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(float rhs, int precision)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(bool rhs)
{
	return impxmlattr(_attr).set_value(rhs);
}

bool CXmlAttributeUI::set_value(CDuiPoint rhs)
{
	CDuiPoint pt(rhs);
	return set_value(pt.ToString().GetData());
}

bool CXmlAttributeUI::set_value(CDuiSize rhs)
{
	CDuiSize sz(rhs);
	return set_value(sz.ToString().GetData());
}

bool CXmlAttributeUI::set_value(CDuiRect rhs)
{
	CDuiRect rc(rhs);
	return set_value(rc.ToString().GetData());
}

bool CXmlAttributeUI::set_uicolor(CDuiColor rhs)
{
	CDuiString s;
	s.Format(_T("0x%08X"), rhs);
	return set_value(s.GetData());
}

CXmlAttributeUI CXmlAttributeUI::next_attribute() const
{
	return CXmlAttributeUI(impxmlattr(_attr).next_attribute().internal_object());
}

CXmlAttributeUI CXmlAttributeUI::previous_attribute() const
{
	return CXmlAttributeUI(impxmlattr(_attr).previous_attribute().internal_object());
}

PVOID CXmlAttributeUI::internal_object() const
{
	return _attr;
}

} // namespace DuiLib



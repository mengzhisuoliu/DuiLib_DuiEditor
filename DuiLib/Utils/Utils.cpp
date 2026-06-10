#include "StdAfx.h"

namespace DuiLib
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiPoint::CDuiPoint()
	{
		x = y = 0;
	}

	CDuiPoint::CDuiPoint(const POINT& src)
	{
		x = src.x;
		y = src.y;
	}

	CDuiPoint::CDuiPoint(const SIZE& src)
	{
		x = src.cx;
		y = src.cy;
	}

	CDuiPoint::CDuiPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	CDuiPoint::CDuiPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}

	CDuiPoint::CDuiPoint(LPCTSTR pstrValue)
	{
		FromString(pstrValue);
	}

	bool CDuiPoint::FromString(LPCTSTR pstrValue) //从"x,y"构造POINT
	{
		x = y = 0;
		if (pstrValue == NULL || *pstrValue == _T('\0')) return false;
		LPTSTR pstr = NULL;
		x = _tcstol(pstrValue, &pstr, 10); if(!pstr) return false;
		y = _tcstol(pstr + 1, &pstr, 10);  if(!pstr) return false;
		return true;
	}

	CDuiString CDuiPoint::ToString()				//输出字符串"x,y"
	{
		CDuiString sPoint;
		sPoint.SmallFormat(_T("%ld,%ld"), x, y);
		return sPoint;
	}

	CDuiPoint::operator LPSIZE() const
	{
		return (LPSIZE)this;   // POINT 和 SIZE 内存布局相同（两个 int）
	}

	CDuiPoint::operator SIZE() const
	{
		SIZE sz = { x, y };
		return sz;
	}

	CDuiPoint::operator PPOINT() throw()
	{
		return this;
	}

	CDuiPoint& CDuiPoint::operator=(const POINT& src)
	{
		x = src.x;
		y = src.y;
		return *this;
	}

	CDuiPoint& CDuiPoint::operator=(const SIZE& src)
	{
		x = src.cx;
		y = src.cy;
		return *this;
	}

	bool CDuiPoint::operator==(const POINT& src) const
	{
		return x == src.x && y == src.y;
	}

	bool CDuiPoint::operator!=(const POINT& src) const
	{
		return !(*this == src);
	}

	bool CDuiPoint::operator==(const SIZE& src) const
	{
		return x == src.cx && y == src.cy;
	}

	bool CDuiPoint::operator!=(const SIZE& src) const
	{
		return !(*this == src);
	}

	CDuiPoint CDuiPoint::operator+(const POINT& src) const
	{
		return CDuiPoint(x + src.x, y + src.y);
	}

	CDuiPoint CDuiPoint::operator-(const POINT& src) const
	{
		return CDuiPoint(x - src.x, y - src.y);
	}

	CDuiPoint CDuiPoint::operator+(const SIZE& src) const
	{
		return CDuiPoint(x + src.cx, y + src.cy);
	}

	CDuiPoint CDuiPoint::operator-(const SIZE& src) const
	{
		return CDuiPoint(x - src.cx, y - src.cy);
	}

	CDuiPoint& CDuiPoint::operator+=(const POINT& src)
	{
		x += src.x;
		y += src.y;
		return *this;
	}

	CDuiPoint& CDuiPoint::operator-=(const POINT& src)
	{
		x -= src.x;
		y -= src.y;
		return *this;
	}

	CDuiPoint& CDuiPoint::operator+=(const SIZE& src)
	{
		x += src.cx;
		y += src.cy;
		return *this;
	}

	CDuiPoint& CDuiPoint::operator-=(const SIZE& src)
	{
		x -= src.cx;
		y -= src.cy;
		return *this;
	}

	void CDuiPoint::SetPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	void CDuiPoint::Offset(int dx, int dy)
	{
		x += dx;
		y += dy;
	}

	void CDuiPoint::Offset(const POINT& delta)
	{
		Offset(delta.x, delta.y);
	}

	void CDuiPoint::Offset(const SIZE& delta)
	{
		Offset(delta.cx, delta.cy);
	}

	bool CDuiPoint::IsEmpty() const
	{
		return x == 0 && y == 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiSize::CDuiSize()
	{
		cx = cy = 0;
	}

	CDuiSize::CDuiSize(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CDuiSize::CDuiSize(const RECT& rc)
	{
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
	}

	CDuiSize::CDuiSize(const POINT& pt)
	{
		cx = pt.x;
		cy = pt.y;
	}

	CDuiSize::CDuiSize(int _cx, int _cy)
	{
		cx = _cx;
		cy = _cy;
	}

	CDuiSize::CDuiSize(LPCTSTR pstrValue)
	{
		FromString(pstrValue);
	}

	bool CDuiSize::FromString(LPCTSTR pstrValue) //从"cx,cy"构造SIZE
	{
		cx = cy = 0;
		if (pstrValue == NULL || *pstrValue == _T('\0')) return false;
		LPTSTR pstr = NULL;
		cx = _tcstol(pstrValue, &pstr, 10);		if(!pstr) return false;
		cy = _tcstol(pstr + 1, &pstr, 10);      if(!pstr) return false;
		return true;
	}

	CDuiString CDuiSize::ToString()				//输出字符串"cx,cy"
	{
		CDuiString sSize;
		sSize.SmallFormat(_T("%ld,%ld"), cx, cy);
		return sSize;
	}

	void CDuiSize::SetSize(int cx, int cy)
	{
		CDuiSize::cx = cx;
		CDuiSize::cy = cy;
	}

	void CDuiSize::SetSize(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CDuiSize::operator LPSIZE() throw()
	{
		return this;
	}

	CDuiSize::operator POINT() const
	{
		POINT pt = { cx, cy };
		return pt;
	}

	CDuiSize::operator RECT() const
	{
		RECT rc = { 0, 0, cx, cy };
		return rc;
	}

	CDuiSize& CDuiSize::operator=(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
		return *this;
	}

	bool CDuiSize::operator==(const SIZE& src) const
	{
		return cx == src.cx && cy == src.cy;
	}

	bool CDuiSize::operator!=(const SIZE& src) const
	{
		return !(*this == src);
	}

	bool CDuiSize::operator==(const POINT& src) const
	{
		return cx == src.x && cy == src.y;
	}

	bool CDuiSize::operator!=(const POINT& src) const
	{
		return !(*this == src);
	}

	bool CDuiSize::operator==(const RECT& src) const
	{
		return (cx == src.right - src.left) && (cy == src.bottom - src.top);
	}

	bool CDuiSize::operator!=(const RECT& src) const
	{
		return !(*this == src);
	}

	CDuiSize CDuiSize::operator+(const SIZE& src) const
	{
		return CDuiSize(cx + src.cx, cy + src.cy);
	}

	CDuiSize CDuiSize::operator-(const SIZE& src) const
	{
		return CDuiSize(cx - src.cx, cy - src.cy);
	}

	CDuiSize CDuiSize::operator+(const POINT& src) const
	{
		return CDuiSize(cx + src.x, cy + src.y);
	}

	CDuiSize CDuiSize::operator-(const POINT& src) const
	{
		return CDuiSize(cx - src.x, cy - src.y);
	}

	CDuiSize CDuiSize::operator+(const RECT& src) const
	{
		return CDuiSize(cx + (src.right - src.left), cy + (src.bottom-src.top));
	}

	CDuiSize CDuiSize::operator-(const RECT& src) const
	{
		return CDuiSize(cx - (src.right - src.left), cy - (src.bottom - src.top));
	}

	CDuiSize& CDuiSize::operator+=(const SIZE& src)
	{
		cx += src.cx;
		cy += src.cy;
		return *this;
	}

	CDuiSize& CDuiSize::operator-=(const SIZE& src)
	{
		cx -= src.cx;
		cy -= src.cy;
		return *this;
	}

	CDuiSize& CDuiSize::operator+=(const POINT& src)
	{
		cx += src.x;
		cy += src.y;
		return *this;
	}

	CDuiSize& CDuiSize::operator-=(const POINT& src)
	{
		cx -= src.x;
		cy -= src.y;
		return *this;
	}

	CDuiSize& CDuiSize::operator+=(const RECT& src)
	{
		cx += src.right - src.left;
		cy += src.bottom - src.top;
		return *this;
	}

	CDuiSize& CDuiSize::operator-=(const RECT& src)
	{
		cx -= src.right - src.left;
		cy -= src.bottom - src.top;
		return *this;
	}

	//放大
	void CDuiSize::Inflate(int x, int y)
	{
		cx += x;
		cy += y;
	}

	//缩小
	void CDuiSize::Deflate(int x, int y)
	{
		cx -= x;
		cy -= y;
	}


	#ifdef DUILIB_SDL
	CDuiSize::CDuiSize(const SDL_Rect& rc)
	{
		cx = rc.w;
		cy = rc.h;
	}

	CDuiSize& CDuiSize::operator=(const SDL_Rect& rc)
	{
		cx = rc.w;
		cy = rc.h;
		return *this;
	}
	#endif
/////////////////////////////////////////////////////////////////////////////////////
//
//
#ifdef DUILIB_WIN32
#define WIN32_RECT_API
#endif
	CDuiRect::CDuiRect()
	{
		left = top = right = bottom = 0;
	}

	CDuiRect::CDuiRect(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
	}

	CDuiRect::CDuiRect(LPCRECT src)
	{
		left = src->left;
		top = src->top;
		right = src->right;
		bottom = src->bottom;
	}

	CDuiRect::CDuiRect(int iLeft, int iTop, int iRight, int iBottom)
	{
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}

	CDuiRect::CDuiRect(const POINT &ptLeftTop, const SIZE &szWidthHeight)
	{
		SetRect(ptLeftTop, szWidthHeight);
	}

	CDuiRect::CDuiRect(const POINT &ptLeftTop, const POINT &ptRightBottom)
	{
		SetRect(ptLeftTop, ptRightBottom);
	}

	CDuiRect::CDuiRect(const SIZE& src)
	{
		left = 0;
		top = 0;
		right = src.cx;
		bottom = src.cy;
	}

	CDuiRect::CDuiRect(LPCTSTR pstrValue)
	{
		FromString(pstrValue);
	}

	bool CDuiRect::FromString(LPCTSTR pstrValue) //从"left,top,right,bottom"构造RECT
	{
		left = top = right = bottom = 0;
		if (pstrValue == NULL || *pstrValue == _T('\0')) return false;
		LPTSTR pstr = NULL;
		left	= _tcstol(pstrValue, &pstr, 10);	if(!pstr) return false;
		top		= _tcstol(pstr + 1, &pstr, 10);		if(!pstr) return false;
		right	= _tcstol(pstr + 1, &pstr, 10);		if(!pstr) return false;
		bottom	= _tcstol(pstr + 1, &pstr, 10);		if(!pstr) return false;
		return true;
	}

	CDuiString CDuiRect::ToString()				//输出字符串"left,top,right,bottom"
	{
		CDuiString sRect;
		sRect.SmallFormat(_T("%ld,%ld,%ld,%ld"), left, top, right, bottom);
		return sRect;
	}

	CDuiRect::operator LPRECT() throw()
	{
		return this;
	}

	CDuiRect::operator LPCRECT() const throw()
	{
		return this;
	}

	CDuiRect::operator SIZE() const throw()
	{
		SIZE sz = {GetWidth(), GetHeight()};
		return sz;
	}

	CDuiRect& CDuiRect::operator=(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
		return *this;
	}

	CDuiRect& CDuiRect::operator=(const SIZE& src)
	{
		left = 0;
		top = 0;
		right = src.cx;
		bottom = src.cy;
		return *this;
	}

	CDuiRect& CDuiRect::operator=(LPCTSTR pstrValue)
	{
		FromString(pstrValue);
		return *this;
	}

	CDuiPoint CDuiRect::LeftTop()
	{
		return CDuiPoint(left,top);
	}

	CDuiPoint CDuiRect::RightBottom()
	{
		return CDuiPoint(right,bottom);
	}

	void CDuiRect::SetRect(int left, int top, int right, int bottom)
	{
		CDuiRect::left = left;
		CDuiRect::top = top;
		CDuiRect::right = right;
		CDuiRect::bottom = bottom;
	}

	void CDuiRect::SetRect(const POINT &ptLeftTop, const SIZE &szWidthHeight)
	{
		CDuiRect::left = ptLeftTop.x;
		CDuiRect::top = ptLeftTop.y;
		CDuiRect::right = ptLeftTop.x + szWidthHeight.cx;
		CDuiRect::bottom = ptLeftTop.y + szWidthHeight.cy;
	}

	void CDuiRect::SetRect(const POINT &ptLeftTop, const POINT &ptRightBottom)
	{
		left = ptLeftTop.x;
		top = ptLeftTop.y;
		right = ptRightBottom.x;
		bottom = ptRightBottom.y;
	}

	int CDuiRect::GetWidth() const
	{
		return right - left;
	}

	int CDuiRect::GetHeight() const
	{
		return bottom - top;
	}

	void CDuiRect::Empty()
	{
		left = top = right = bottom = 0;
	}

	bool CDuiRect::IsNull() const
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0);
	}

	bool CDuiRect::IsEmpty() const
	{
#ifdef WIN32_RECT_API
		return ::IsRectEmpty(this) == TRUE;
#else
		return (right <= left) || (bottom <= top);
#endif
	}

	void CDuiRect::Join(const RECT& rc)
	{
		if( rc.left < left ) left = rc.left;
		if( rc.top < top ) top = rc.top;
		if( rc.right > right ) right = rc.right;
		if( rc.bottom > bottom ) bottom = rc.bottom;
	}

	void CDuiRect::ResetOffset()
	{
#ifdef WIN32_RECT_API
		::OffsetRect(this, -left, -top);
#else
		right -= left;
		bottom -= top;
		left = 0;
		top = 0;
#endif
	}

	void CDuiRect::Normalize()
	{
		if( left > right ) { int iTemp = left; left = right; right = iTemp; }
		if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
	}

	void CDuiRect::Offset(int cx, int cy)
	{
#ifdef WIN32_RECT_API
		::OffsetRect(this, cx, cy);
#else
		left += cx;
		top += cy;
		right += cx;
		bottom += cy;
#endif
	}

	void CDuiRect::Inflate(int cx, int cy)
	{
#ifdef WIN32_RECT_API
		::InflateRect(this, cx, cy);
#else
		left -= cx;
		top -= cy;
		right += cx;
		bottom += cy;
#endif
	}

	void CDuiRect::Inflate(int left, int top, int right, int bottom)
	{
		CDuiRect::left -= left;
		CDuiRect::top -= top;
		CDuiRect::right += right;
		CDuiRect::bottom += bottom;
	}

	void CDuiRect::Inflate(const RECT &rc)
	{
		left -= rc.left;
		top -= rc.top;
		right += rc.right;
		bottom += rc.bottom;
	}

	void CDuiRect::Deflate(int cx, int cy)
	{
		Inflate(-cx, -cy);
	}

	void CDuiRect::Deflate(int left, int top, int right, int bottom)
	{
		CDuiRect::left += left;
		CDuiRect::top += top;
		CDuiRect::right -= right;
		CDuiRect::bottom -= bottom;
	}

	void CDuiRect::Deflate(const RECT &rc)
	{
		left += rc.left;
		top += rc.top;
		right -= rc.right;
		bottom -= rc.bottom;
	}

	void CDuiRect::Union(const RECT& rc1, const RECT& rc2)
	{
#ifdef WIN32_RECT_API
		::UnionRect(this, &rc1, &rc2);
#else
		left	= MIN(rc1.left,		rc2.left);
		top		= MIN(rc1.top,		rc2.top);
		right	= MAX(rc1.right,	rc2.right);
		bottom	= MAX(rc1.bottom,	rc2.bottom);
#endif
	}

	BOOL CDuiRect::Intersect(const RECT &rect1, const RECT &rect2)
	{
#ifdef WIN32_RECT_API
		return ::IntersectRect(this, (LPRECT)&rect1, (LPRECT)&rect2);
#else
		CDuiRect rcUnion = rect1;
		rcUnion.Join(rect2);
		left = (rect1.left == rcUnion.left) ? rect2.left : rect1.left;
		top = (rect1.top == rcUnion.top) ? rect2.top : rect1.top;
		right = (rect1.right == rcUnion.right) ? rect2.right : rect1.right;
		bottom = (rect1.bottom == rcUnion.bottom) ? rect2.bottom : rect1.bottom;
		return !IsEmpty();
#endif
	}

	POINT CDuiRect::CenterPoint() const
	{
		POINT pt = {(left+right)/2, (top+bottom)/2};
		return pt;
	}

	void CDuiRect::AlignRect(const RECT &rc, UINT uAlign)
	{
		POINT ptCenter = {(rc.left+rc.right)/2, (rc.top+rc.bottom)/2};
		int width = right - left;
		int height = bottom - top;
		if((uAlign & DT_CENTER) == DT_CENTER)
		{
			left = ptCenter.x - width/2;
			right = ptCenter.x + width/2;
		}
		else if((uAlign & DT_LEFT) == DT_LEFT)
		{
			left = rc.left;
			right = rc.left + width;
		}
		else if((uAlign & DT_RIGHT) == DT_RIGHT)
		{
			left = rc.right - width;
			right = rc.right;
		}

		if((uAlign & DT_VCENTER) == DT_VCENTER)
		{
			top = ptCenter.y - height/2;
			bottom = ptCenter.y + height/2;
		}
		else if((uAlign & DT_TOP) == DT_TOP)
		{
			top = rc.top;
			bottom = rc.top + height;
		}
		else if((uAlign & DT_BOTTOM) == DT_BOTTOM)
		{
			top = rc.bottom - height;
			bottom = rc.bottom;
		}
	}

	BOOL CDuiRect::EqualRect(const CDuiRect& rc) const
	{
		return left == rc.left && right == rc.right && top == rc.top && bottom == rc.bottom;
	}

	BOOL CDuiRect::PtInRect(POINT pt) const
	{
#ifdef WIN32_RECT_API
		return ::PtInRect(this, pt);
#else
		return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom;
#endif
	}

	void CDuiRect::SetPadding(const RECT& rc)
	{
		left += rc.left;
		right -= rc.right;
		top += rc.top;
		bottom -= rc.bottom;
	}

	void CDuiRect::SetInset(const RECT& rc)
	{
		left += rc.left;
		right -= rc.right;
		top += rc.top;
		bottom -= rc.bottom;
	}

#ifdef DUILIB_SDL
	CDuiRect::CDuiRect(const SDL_Rect& rc)
	{
		left = rc.x;
		top = rc.y;
		right = rc.x + rc.w;
		bottom = rc.y + rc.h;
	}

	SDL_Rect CDuiRect::ToSDL_Rect() const
	{
		SDL_Rect rc;
		rc.x = left;
		rc.y = top;
		rc.w = GetWidth();
		rc.h = GetHeight();
		return rc;
	}

	CDuiRect::operator SDL_Rect() const
	{
		return ToSDL_Rect();
	}

	CDuiRect& CDuiRect::operator=(const SDL_Rect& rc)
	{
		left = rc.x;
		top = rc.y;
		right = rc.x + rc.w;
		bottom = rc.y + rc.h;
		return *this;
	}

	CDuiRect::CDuiRect(const SDL_FRect& rc)
	{
		left = (int)rc.x;
		top = (int)rc.y;
		right = (int)(rc.x + rc.w);
		bottom = (int)(rc.y + rc.h);
	}

	SDL_FRect CDuiRect::ToSDL_FRect() const
	{
		SDL_FRect rc;
		rc.x = (float)left;
		rc.y = (float)top;
		rc.w = (float)GetWidth();
		rc.h = (float)GetHeight();
		return rc;
	}

	CDuiRect::operator SDL_FRect() const
	{
		return ToSDL_FRect();
	}

	CDuiRect& CDuiRect::operator=(const SDL_FRect& rc)
	{
		left = (int)rc.x;
		top = (int)rc.y;
		right = (int)(rc.x + rc.w);
		bottom = (int)(rc.y + rc.h);
		return *this;
	}
#endif // DUILIB_SDL

	//////////////////////////////////////////////////////////////////////////
	// 
	// 
	CDuiColor::CDuiColor() : m_dwColor(0x00000000) {}

	CDuiColor::CDuiColor(DWORD dwColor) : m_dwColor(dwColor) {}

	CDuiColor::CDuiColor(BYTE r, BYTE g, BYTE b)
		: m_dwColor(0x00000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | b) {}

	CDuiColor::CDuiColor(BYTE a, BYTE r, BYTE g, BYTE b)
		: m_dwColor(((DWORD)a << 24) | ((DWORD)r << 16) | ((DWORD)g << 8) | b) {}

	CDuiColor::CDuiColor(LPCTSTR pstrValue)
	{
		if (!FromString(pstrValue)) 
		{
			m_dwColor = 0x00000000;
		}
	}

	CDuiColor::CDuiColor(int n)
	{
		m_dwColor = (DWORD)n;
	}

	CDuiColor::CDuiColor(UINT n)
	{
		m_dwColor = (DWORD)n;
	}

	bool CDuiColor::FromString(LPCTSTR pstrValue)
	{
		if (!pstrValue || !*pstrValue) return false;

		// 跳过前导空格
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t')) ++pstrValue;

		// 格式1: "0xAARRGGBB"
		if (pstrValue[0] == _T('0') && (pstrValue[1] == _T('x') || pstrValue[1] == _T('X')))
		{
			DWORD dw = 0;
			if (_stscanf(pstrValue, _T("%lx"), &dw) == 1)
			{
				m_dwColor = dw;
				return true;
			}
			return false;
		}

		// 格式2: "#AARRGGBB"
		if (pstrValue[0] == _T('#'))
		{
			int len = _tcslen(pstrValue + 1);
			if (len == 8)  // #AARRGGBB
			{
				TCHAR buf[3] = { 0 };
				buf[0] = pstrValue[1]; buf[1] = pstrValue[2];
				BYTE a = (BYTE)_tcstol(buf, NULL, 16);
				buf[0] = pstrValue[3]; buf[1] = pstrValue[4];
				BYTE r = (BYTE)_tcstol(buf, NULL, 16);
				buf[0] = pstrValue[5]; buf[1] = pstrValue[6];
				BYTE g = (BYTE)_tcstol(buf, NULL, 16);
				buf[0] = pstrValue[7]; buf[1] = pstrValue[8];
				BYTE b = (BYTE)_tcstol(buf, NULL, 16);
				m_dwColor = ((DWORD)a << 24) | ((DWORD)r << 16) | ((DWORD)g << 8) | b;
				return true;
			}
			else if (len == 6)  // #RRGGBB (alpha=255)
			{
				TCHAR buf[3] = { 0 };
				buf[0] = pstrValue[1]; buf[1] = pstrValue[2];
				BYTE r = (BYTE)_tcstol(buf, NULL, 16);
				buf[0] = pstrValue[3]; buf[1] = pstrValue[4];
				BYTE g = (BYTE)_tcstol(buf, NULL, 16);
				buf[0] = pstrValue[5]; buf[1] = pstrValue[6];
				BYTE b = (BYTE)_tcstol(buf, NULL, 16);
				m_dwColor = 0xFF000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | b;
				return true;
			}
			return false;
		}

		// 格式3: "a,r,g,b" 或 "r,g,b"
		int a = 255, r = 0, g = 0, b = 0;
		int matched = _stscanf(pstrValue, _T("%d,%d,%d,%d"), &a, &r, &g, &b);
		if (matched == 4)
		{
			m_dwColor = ((DWORD)(BYTE)a << 24) | ((DWORD)(BYTE)r << 16) | ((DWORD)(BYTE)g << 8) | (BYTE)b;
			return true;
		}
		matched = _stscanf(pstrValue, _T("%d,%d,%d"), &r, &g, &b);
		if (matched == 3)
		{
			m_dwColor = 0xFF000000 | ((DWORD)(BYTE)r << 16) | ((DWORD)(BYTE)g << 8) | (BYTE)b;
			return true;
		}

		return false;
	}

	CDuiString CDuiColor::ToString()
	{
		CDuiString s;
		s.Format(_T("0x%02X%02X%02X%02X"), GetA(), GetR(), GetG(), GetB());
		return s;
	}

	void CDuiColor::FromCOLORREF(COLORREF cr)
	{
		BYTE r = GetRValue(cr);
		BYTE g = GetGValue(cr);
		BYTE b = GetBValue(cr);
		m_dwColor = 0xFF000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | b;
	}

	COLORREF CDuiColor::ToCOLORREF()
	{
		return RGB(GetR(), GetG(), GetB());
	}

	BYTE CDuiColor::GetA() const { return (BYTE)(m_dwColor >> 24); }
	BYTE CDuiColor::GetR() const { return (BYTE)(m_dwColor >> 16); }
	BYTE CDuiColor::GetG() const { return (BYTE)(m_dwColor >> 8); }
	BYTE CDuiColor::GetB() const { return (BYTE)(m_dwColor); }
	void CDuiColor::SetA(BYTE a) { m_dwColor = (m_dwColor & 0x00FFFFFF) | ((DWORD)a << 24); }
	void CDuiColor::SetR(BYTE r) { m_dwColor = (m_dwColor & 0xFF00FFFF) | ((DWORD)r << 16); }
	void CDuiColor::SetG(BYTE g) { m_dwColor = (m_dwColor & 0xFFFF00FF) | ((DWORD)g << 8); }
	void CDuiColor::SetB(BYTE b) { m_dwColor = (m_dwColor & 0xFFFFFF00) | (DWORD)b; }
	DWORD CDuiColor::GetColor() const { return m_dwColor; }
	void CDuiColor::SetColor(DWORD dwColor) { m_dwColor = dwColor; }


	CDuiColor::operator DWORD() const { return m_dwColor; }

	bool CDuiColor::operator==(const CDuiColor& other) const { return m_dwColor == other.m_dwColor; }
	bool CDuiColor::operator!=(const CDuiColor& other) const { return m_dwColor != other.m_dwColor; }
	bool CDuiColor::operator==(int other) const { return m_dwColor == (DWORD)other; }
	bool CDuiColor::operator!=(int other) const { return m_dwColor != (DWORD)other; }
	bool CDuiColor::operator==(UINT other) const { return m_dwColor == (DWORD)other; }
	bool CDuiColor::operator!=(UINT other) const { return m_dwColor != (DWORD)other; }

	CDuiColor& CDuiColor::operator=(DWORD dwColor)
	{
		m_dwColor = dwColor;
		return *this;
	}

	CDuiColor& CDuiColor::operator=(LPCTSTR pstrValue)
	{
		if (!FromString(pstrValue)) 
		{
			m_dwColor = 0x00000000;
		}
		return *this;
	}

	CDuiColor& CDuiColor::operator=(int n)
	{
		m_dwColor = (DWORD)n;
		return *this;
	}

	CDuiColor& CDuiColor::operator=(unsigned int n)
	{
		m_dwColor = n;
		return *this;
	}

	#ifdef DUILIB_SDL
	CDuiColor::CDuiColor(const SDL_Color& color)
	{
		m_dwColor = ((DWORD)color.a << 24) | ((DWORD)color.r << 16) | ((DWORD)color.g << 8) | color.b;
	}
	SDL_Color CDuiColor::ToSDL_Color()
	{
		return { GetR(), GetG(), GetB(), GetA() };
	}
	CDuiColor::operator SDL_Color() const { return SDL_Color{ GetR(), GetG(), GetB(), GetA() }; }
	CDuiColor& CDuiColor::operator=(const SDL_Color& color)
	{
		m_dwColor = ((DWORD)color.a << 24) | ((DWORD)color.r << 16) | ((DWORD)color.g << 8) | color.b;
		return *this;
	}
	#endif

	// 静态常量初始化
	CDuiColor CDuiColor::Black(0xFF000000);
	CDuiColor CDuiColor::White(0xFFFFFFFF);
	CDuiColor CDuiColor::Red(0xFFFF0000);
	CDuiColor CDuiColor::Green(0xFF00FF00);
	CDuiColor CDuiColor::Blue(0xFF0000FF);

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CStdPtrArray::CStdPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize), m_bSaveIndexMap(false)
	{
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
	}

	CStdPtrArray::CStdPtrArray(const CStdPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0), m_bSaveIndexMap(false)
	{
		for(int i=0; i<src.GetSize(); i++)
			Add(src.GetAt(i));
	}

	CStdPtrArray::~CStdPtrArray()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
	}

	void CStdPtrArray::Empty()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
		m_ppVoid = NULL;
		m_nCount = m_nAllocated = 0;
		m_mapIndex.clear();
	}

	void CStdPtrArray::Resize(int iSize)
	{
		Empty();
		m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
		memset(m_ppVoid, 0, iSize * sizeof(LPVOID));
		m_nAllocated = iSize;
		m_nCount = iSize;
	}

	bool CStdPtrArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CStdPtrArray::Add(LPVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		m_ppVoid[m_nCount - 1] = pData;
		if(m_bSaveIndexMap){
			m_mapIndex[pData] = m_nCount - 1;
		}
		return true;
	}

	bool CStdPtrArray::InsertAt(int iIndex, LPVOID pData)
	{
		//if( iIndex < 0 || iIndex > m_nCount ) return false;
		if( iIndex < 0) return false;
		if(iIndex > m_nCount) iIndex = m_nCount; //插入位置超出原有长度时，总是插入到最后。
		if( iIndex == m_nCount ) return Add(pData);
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
		m_ppVoid[iIndex] = pData;

		if(m_bSaveIndexMap)
		{
			for( int i = iIndex; i < m_nCount; i++ ) m_mapIndex[m_ppVoid[i]] = i;
		}
		return true;
	}

	bool CStdPtrArray::SetAt(int iIndex, LPVOID pData)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;

		if(m_bSaveIndexMap)
		{
			if(m_ppVoid[iIndex] != NULL)
			{
				std::map<LPVOID, int>::iterator it = m_mapIndex.find(m_ppVoid[iIndex]);
				if(it != m_mapIndex.end()) m_mapIndex.erase(it);
			}
			m_mapIndex[pData] = iIndex;
		}

		m_ppVoid[iIndex] = pData;
		return true;
	}

	bool CStdPtrArray::Remove(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;

		if(m_bSaveIndexMap)
		{
			if(m_ppVoid[iIndex] != NULL)
			{
				std::map<LPVOID, int>::iterator it = m_mapIndex.find(m_ppVoid[iIndex]);
				if(it != m_mapIndex.end()) m_mapIndex.erase(it);
			}
		}

		if( iIndex < --m_nCount ) ::memcpy(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));

		if(m_bSaveIndexMap)
		{
			for( int i = iIndex; i < m_nCount; i++ ) m_mapIndex[m_ppVoid[i]] = i;
		}
		return true;
	}

	bool CStdPtrArray::Remove(LPVOID pData)
	{
		int nIndex = Find(pData);
		if(nIndex >= 0)
			return Remove(nIndex);
		return false;
	}

	int CStdPtrArray::Find(LPVOID pData) const
	{
		if(m_bSaveIndexMap)
		{
			CStdPtrArray *pArray = const_cast<CStdPtrArray *>(this);
			return pArray->FindInMap(pData);
		}
		for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
		return -1;
	}

	int CStdPtrArray::FindInMap(LPVOID pData)
	{
		std::map<LPVOID, int>::iterator it = m_mapIndex.find(pData);
		if(it == m_mapIndex.end()) return -1;
		return it->second;
	}

	int CStdPtrArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID* CStdPtrArray::GetData()
	{
		return m_ppVoid;
	}

	LPVOID CStdPtrArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_ppVoid[iIndex];
	}

	LPVOID CStdPtrArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_ppVoid[iIndex];
	}

	void CStdPtrArray::SetSaveIndexMap(bool bIndexMap)
	{
		m_bSaveIndexMap = bIndexMap;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CStdValArray::CStdValArray(int iElementSize, int iPreallocSize /*= 0*/) :
	m_pVoid(NULL),
		m_iElementSize(iElementSize),
		m_nCount(0),
		m_nAllocated(iPreallocSize)
	{
		ASSERT(iElementSize>0);
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
	}

	CStdValArray::~CStdValArray()
	{
		if( m_pVoid != NULL ) free(m_pVoid);
	}

	void CStdValArray::Empty()
	{
		m_nCount = 0;  // NOTE: We keep the memory in place
	}

	bool CStdValArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CStdValArray::Add(LPCVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
			if( pVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_pVoid = pVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		::memcpy(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
		return true;
	}

	bool CStdValArray::Remove(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		if( iIndex < --m_nCount ) ::memcpy(m_pVoid + (iIndex * m_iElementSize), m_pVoid + ((iIndex + 1) * m_iElementSize), (m_nCount - iIndex) * m_iElementSize);
		return true;
	}

	int CStdValArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID CStdValArray::GetData()
	{
		return static_cast<LPVOID>(m_pVoid);
	}

	LPVOID CStdValArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_pVoid + (iIndex * m_iElementSize);
	}

	LPVOID CStdValArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_pVoid + (iIndex * m_iElementSize);
	}


	/////////////////////////////////////////////////////////////////////////////
	//
	//

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		size_t len = _tcslen(Key);
		while( len-- > 0 ) i = (i << 5) + i + Key[len];
		return i;
	}
	/*
	static UINT HashKey(const CDuiString& Key)
	{
		return HashKey((LPCTSTR)Key);
	}
	*/
	CStdStringPtrMap::CStdStringPtrMap(int nSize) : m_nCount(0)
	{
		if( nSize < 16 ) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	}

	CStdStringPtrMap::~CStdStringPtrMap()
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}
	}

	void CStdStringPtrMap::RemoveAll()
	{
		this->Resize(m_nBuckets);
	}

	void CStdStringPtrMap::Resize(int nSize)
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}

		if( nSize < 0 ) nSize = 0;
		if( nSize > 0 ) {
			m_aT = new TITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(TITEM*));
		}
		m_nBuckets = nSize;
		m_nCount = 0;
	}

	LPVOID CStdStringPtrMap::Find(LPCTSTR key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
			if( pItem->Key == key ) {
				if (optimize && pItem != m_aT[slot]) {
					if (pItem->pNext) {
						pItem->pNext->pPrev = pItem->pPrev;
					}
					pItem->pPrev->pNext = pItem->pNext;
					pItem->pPrev = NULL;
					pItem->pNext = m_aT[slot];
					pItem->pNext->pPrev = pItem;
					//将item移动至链条头部
					m_aT[slot] = pItem;
				}
				return pItem->Data;
			}
		}

		return NULL;
	}

	bool CStdStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return false;
		if( Find(key) ) return false;

		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		TITEM* pItem = new TITEM;
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	LPVOID CStdStringPtrMap::Set(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return pData;

		if (GetSize()>0) {
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
				if( pItem->Key == key ) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	}

	bool CStdStringPtrMap::Remove(LPCTSTR key)
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		UINT slot = HashKey(key) % m_nBuckets;
		TITEM** ppItem = &m_aT[slot];
		while( *ppItem ) {
			if( (*ppItem)->Key == key ) {
				TITEM* pKill = *ppItem;
				*ppItem = (*ppItem)->pNext;
				if (*ppItem)
					(*ppItem)->pPrev = pKill->pPrev;
				delete pKill;
				m_nCount--;
				return true;
			}
			ppItem = &((*ppItem)->pNext);
		}

		return false;
	}

	int CStdStringPtrMap::GetSize() const
	{
#if 0//def _DEBUG
		int nCount = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) nCount++;
		}
		ASSERT(m_nCount==nCount);
#endif
		return m_nCount;
	}

	LPCTSTR CStdStringPtrMap::GetAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					return pItem->Key.GetData();
				}
			}
		}

		return NULL;
	}

	LPCTSTR CStdStringPtrMap::operator[] (int nIndex) const
	{
		return GetAt(nIndex);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiWaitCursor::CDuiWaitCursor()
	{
	    #ifdef DUILIB_WIN32
		m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
		#endif
	}

	CDuiWaitCursor::~CDuiWaitCursor()
	{
	    #ifdef DUILIB_WIN32
		::SetCursor(m_hOrigCursor);
		#endif
	}

	//////////////////////////////////////////////////////////////////////////
	CDuiLock::CDuiLock() 
	{
	#ifdef DUILIB_WIN32
		InitializeCriticalSectionAndSpinCount(&m_lock, 5000);
	#elif defined DUILIB_SDL
		m_lock = (UINT_PTR)SDL_CreateMutex();
	#else
	#endif
	}
	CDuiLock::~CDuiLock()
	{
	#ifdef DUILIB_WIN32
		DeleteCriticalSection(&m_lock);
	#elif defined DUILIB_SDL
		SDL_DestroyMutex((SDL_Mutex *)m_lock);
	#else
	#endif
	}

	void CDuiLock::Lock()
	{
	#ifdef DUILIB_WIN32
		EnterCriticalSection(&m_lock);
	#elif defined DUILIB_SDL
		SDL_LockMutex((SDL_Mutex*)m_lock);
	#else
	#endif
	}

	void CDuiLock::Unlock()
	{
	#ifdef DUILIB_WIN32
		LeaveCriticalSection(&m_lock);
	#elif defined DUILIB_SDL
		SDL_UnlockMutex((SDL_Mutex*)m_lock);
	#else
	#endif
	}

} // namespace DuiLib


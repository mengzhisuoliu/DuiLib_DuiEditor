#ifndef __UTILS_H__
#define __UTILS_H__
#pragma once

#ifdef DUILIB_WIN32
#include "OAIdl.h"
#endif

#include <vector>
#include <map>

#ifndef DUILIB_WIN32
#include <mutex>
#include <atomic>
#endif


#include "DuiString.h"
namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//
#ifndef MAKEINTRESOURCE
	#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
	#define MAKEINTRESOURCEW(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
	#ifdef UNICODE
		#define MAKEINTRESOURCE  MAKEINTRESOURCEW
	#else
		#define MAKEINTRESOURCE  MAKEINTRESOURCEA
	#endif
#endif

	class UILIB_API STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
#ifndef WIN32
	typedef struct tagPOINT
	{
		int  x;
		int  y;
	} POINT, * PPOINT, * LPPOINT;

	typedef struct tagSIZE
	{
		int        cx;
		int        cy;
	} SIZE, * PSIZE, * LPSIZE;

	typedef struct tagRECT
	{
		int    left;
		int    top;
		int    right;
		int    bottom;
	} RECT, * PRECT, * LPRECT;
	typedef const RECT * LPCRECT;
	
	typedef struct _SYSTEMTIME {
		WORD wYear;
		WORD wMonth;
		WORD wDayOfWeek;
		WORD wDay;
		WORD wHour;
		WORD wMinute;
		WORD wSecond;
		WORD wMilliseconds;
	} SYSTEMTIME, * PSYSTEMTIME, * LPSYSTEMTIME;
#endif

	class UILIB_API CDuiPoint : public tagPOINT
	{
	public:
		CDuiPoint()						{ x = y = 0;											}
		CDuiPoint(const POINT& src)		{ x = src.x; y = src.y;									}
		CDuiPoint(const SIZE& src)		{ x = src.cx; y = src.cy;								}
		CDuiPoint(int _x, int _y)		{ x = _x; y = _y;										}
		CDuiPoint(LPARAM lParam)		{ x = GET_X_LPARAM(lParam); y = GET_Y_LPARAM(lParam);	}
		CDuiPoint(LPCTSTR pstrValue)	{ FromString(pstrValue);								}

		bool FromString(LPCTSTR pstrValue); //从"x,y"构造POINT
		CDuiString ToString() const;				//输出字符串"x,y"

		// 类型转换
		operator LPSIZE() const { return (LPSIZE)this;				}
		operator SIZE() const	{ SIZE sz = { x, y }; return sz;	}
		operator PPOINT()		{ return this;						}

		// 赋值运算符
		CDuiPoint& operator=(const POINT& src)	{ x = src.x; y = src.y; return *this;	}
		CDuiPoint& operator=(const SIZE& src)	{ x = src.cx; y = src.cy; return *this; }

		// 比较运算符
		bool operator==(const POINT& src) const { return x == src.x && y == src.y;		}
		bool operator!=(const POINT& src) const { return !(*this == src);				}
		bool operator==(const SIZE& src) const	{ return x == src.cx && y == src.cy;	}
		bool operator!=(const SIZE& src) const	{ return !(*this == src);				}

		// 算术运算符（与 POINT/SIZE）
		CDuiPoint operator+(const POINT& src) const { return CDuiPoint(x + src.x, y + src.y);	}
		CDuiPoint operator-(const POINT& src) const { return CDuiPoint(x - src.x, y - src.y);	}
		CDuiPoint operator+(const SIZE& src) const	{ return CDuiPoint(x + src.cx, y + src.cy); }
		CDuiPoint operator-(const SIZE& src) const	{ return CDuiPoint(x - src.cx, y - src.cy); }

		// 复合赋值运算符
		CDuiPoint& operator+=(const POINT& src) { x += src.x; y += src.y; return *this; }
		CDuiPoint& operator-=(const POINT& src) { x -= src.x; y -= src.y; return *this; }
		CDuiPoint& operator+=(const SIZE& src) { x += src.cx; y += src.cy; return *this; }
		CDuiPoint& operator-=(const SIZE& src) { x -= src.cx; y -= src.cy; return *this; }

		// 坐标操作
		void SetPoint(int _x, int _y)		{ x = _x; y = _y;				}
		void Offset(int dx, int dy)			{ x += dx; y += dy;				}
		void Offset(const POINT& delta)		{ Offset(delta.x, delta.y);		}
		void Offset(const SIZE& delta)		{ Offset(delta.cx, delta.cy);	}
		bool IsEmpty() const				{ return x == 0 && y == 0;		}
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiSize : public tagSIZE
	{
	public:
		CDuiSize()					{ cx = cy = 0;										}
		CDuiSize(const SIZE& src)	{ cx = src.cx; cy = src.cy;							}
		CDuiSize(const RECT& rc)	{ cx = rc.right - rc.left; cy = rc.bottom - rc.top; }
		CDuiSize(const POINT& pt)	{ cx = pt.x; cy = pt.y;								}
		CDuiSize(int _cx, int _cy)	{ cx = _cx; cy = _cy;								}
		CDuiSize(LPCTSTR pstrValue) { FromString(pstrValue);							}

		bool FromString(LPCTSTR pstrValue); //从"cx,cy"构造SIZE
		CDuiString ToString() const;				//输出字符串"cx,cy"

		void SetSize(int _cx, int _cy) { cx = _cx; cy = _cy; }
		void SetSize(const SIZE& src) { cx = src.cx; cy = src.cy; }

		// 类型转换
		operator LPSIZE() throw() { return this; }
		operator POINT() const { POINT pt = { cx, cy }; return pt; }
		operator RECT() const { RECT rc = { 0, 0, cx, cy }; return rc; }

		// 赋值运算符
		CDuiSize& operator=(const SIZE& src) { cx = src.cx; cy = src.cy; return *this; }

		// 比较运算符
		bool operator==(const SIZE& src) const { return cx == src.cx && cy == src.cy; }
		bool operator!=(const SIZE& src) const { return !(*this == src); }
		bool operator==(const POINT& src) const { return cx == src.x && cy == src.y; }
		bool operator!=(const POINT& src) const { return !(*this == src); }
		bool operator==(const RECT& src) const { return (cx == src.right - src.left) && (cy == src.bottom - src.top); }
		bool operator!=(const RECT& src) const { return !(*this == src); }

		// 算术运算符
		CDuiSize operator+(const SIZE& src) const { return CDuiSize(cx + src.cx, cy + src.cy); }
		CDuiSize operator-(const SIZE& src) const { return CDuiSize(cx - src.cx, cy - src.cy); }
		CDuiSize operator+(const POINT& src) const { return CDuiSize(cx + src.x, cy + src.y); }
		CDuiSize operator-(const POINT& src) const { return CDuiSize(cx - src.x, cy - src.y); }
		CDuiSize operator+(const RECT& src) const { return CDuiSize(cx + (src.right - src.left), cy + (src.bottom - src.top)); }
		CDuiSize operator-(const RECT& src) const { return CDuiSize(cx - (src.right - src.left), cy - (src.bottom - src.top)); }

		// 复合赋值运算符
		CDuiSize& operator+=(const SIZE& src) { cx += src.cx; cy += src.cy; return *this; }
		CDuiSize& operator-=(const SIZE& src) { cx -= src.cx; cy -= src.cy; return *this; }
		CDuiSize& operator+=(const POINT& src) { cx += src.x; cy += src.y; return *this; }
		CDuiSize& operator-=(const POINT& src) { cx -= src.x; cy -= src.y; return *this; }
		CDuiSize& operator+=(const RECT& src) { cx += src.right - src.left; cy += src.bottom - src.top; return *this; }
		CDuiSize& operator-=(const RECT& src) { cx -= src.right - src.left; cy -= src.bottom - src.top; return *this; }

		// 放大
		void Inflate(int x, int y) { cx += x; cy += y; }
		//缩小
		void Deflate(int x, int y) { cx -= x; cy -= y; }

	#ifdef DUILIB_SDL
		CDuiSize(const SDL_Rect& rc);               // 从 SDL_Rect 构造
		CDuiSize& operator=(const SDL_Rect& rc);   // 赋值
	#endif
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect()
		{
			left = top = right = bottom = 0;
		}

		CDuiRect(const RECT& src)
		{
			left = src.left; top = src.top; right = src.right; bottom = src.bottom;
		}

		CDuiRect(LPCRECT src)
		{
			left = src->left; top = src->top; right = src->right; bottom = src->bottom;
		}

		CDuiRect(int iLeft, int iTop, int iRight, int iBottom)
		{
			left = iLeft;
			top = iTop;
			right = iRight;
			bottom = iBottom;
		}

		CDuiRect(const POINT& ptLeftTop, const SIZE& szWidthHeight)
		{
			SetRect(ptLeftTop, szWidthHeight);
		}

		CDuiRect(const POINT& ptLeftTop, const POINT& ptRightBottom)
		{
			SetRect(ptLeftTop, ptRightBottom);
		}

		CDuiRect(const SIZE& src)
		{
			left = 0;
			top = 0;
			right = src.cx;
			bottom = src.cy;
		}
	
		CDuiRect(LPCTSTR pstrValue);		 //从"left,top,right,bottom"构造RECT	
		bool FromString(LPCTSTR pstrValue);  //从"left,top,right,bottom"构造RECT	
		CDuiString ToString() const;		 //输出字符串"left,top,right,bottom"		
		
		// 类型转换
		operator LPRECT()			{ return this; }
		operator LPCRECT() const	{ return this; }
		operator SIZE() const		{ SIZE sz = { GetWidth(), GetHeight() }; return sz; }

		// 赋值运算符
		CDuiRect& operator=(const RECT& src)
		{
			left = src.left; top = src.top; right = src.right; bottom = src.bottom;
			return *this;
		}
		CDuiRect& operator=(const SIZE& src)
		{
			left = 0; top = 0; right = src.cx; bottom = src.cy;
			return *this;
		}
		CDuiRect& operator=(LPCTSTR pstrValue)
		{
			FromString(pstrValue);
			return *this;
		}

		CDuiPoint LeftTop()		const { return CDuiPoint(left, top);		}
		CDuiPoint RightBottom() const { return CDuiPoint(right, bottom);	}

		void SetRect(int ileft, int itop, int iright, int ibottom)
		{
			left	= ileft; 
			top		= itop;
			right	= iright; 
			bottom	= ibottom;
		}
		void SetRect(const POINT& ptLeftTop, const SIZE& szWidthHeight)
		{
			left	= ptLeftTop.x; 
			top		= ptLeftTop.y;
			right	= ptLeftTop.x + szWidthHeight.cx;
			bottom	= ptLeftTop.y + szWidthHeight.cy;
		}
		void SetRect(const POINT& ptLeftTop, const POINT& ptRightBottom)
		{
			left	= ptLeftTop.x; 
			top		= ptLeftTop.y;
			right	= ptRightBottom.x; 
			bottom	= ptRightBottom.y;
		}

		int GetWidth() const	{ return right - left; }
		int GetHeight() const	{ return bottom - top; }

		void Empty() { left = top = right = bottom = 0; }

		bool IsNull() const		{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
		bool IsEmpty() const	{ return (right <= left) || (bottom <= top); }

		//把rc加入当前区域中，并集
		void Join(const RECT& rc)
		{
			#ifdef DUILIB_WIN32
			RECT temp;
			::UnionRect(&temp, this, &rc);
			*this = temp;
			#else
			if (rc.left < left) left = rc.left;
			if (rc.top < top) top = rc.top;
			if (rc.right > right) right = rc.right;
			if (rc.bottom > bottom) bottom = rc.bottom;
			#endif
		}

		//重置偏移位置，结果为：left = 0, top = 0, left = {width}, right = {height}
		void ResetOffset()
		{
			#ifdef DUILIB_WIN32
			::OffsetRect(this, -left, -top);
			#else
			right -= left;
			bottom -= top;
			left = 0;
			top = 0;
			#endif
		}

		//使规范化，结果为: left <= right, top <= bottom
		void Normalize()
		{
			if (left > right) { int iTemp = left; left = right; right = iTemp; }
			if (top > bottom) { int iTemp = top; top = bottom; bottom = iTemp; }
		}

		//偏移，移动区域位置
		void Offset(int cx, int cy)
		{
			#ifdef DUILIB_WIN32
			::OffsetRect(this, cx, cy);
			#else
			left += cx;
			top += cy;
			right += cx;
			bottom += cy;
			#endif
		}

		//放大
		void Inflate(int cx, int cy)
		{
			#ifdef DUILIB_WIN32
			::InflateRect(this, cx, cy);
			#else
			left -= cx;
			top -= cy;
			right += cx;
			bottom += cy;
			#endif
		}

		//放大
		void Inflate(int ileft, int itop, int iright, int ibottom)
		{
			left	-= ileft;
			top		-= itop;
			right	+= iright;
			bottom	+= ibottom;
		}

		//放大
		void Inflate(const RECT& rc)
		{
			left -= rc.left;
			top -= rc.top;
			right += rc.right;
			bottom += rc.bottom;
		}

		//缩小
		void Deflate(int cx, int cy)
		{
			Inflate(-cx, -cy);
		}

		//缩小
		void Deflate(int ileft, int itop, int iright, int ibottom)
		{
			left	+= ileft;
			top		+= itop;
			right	-= iright;
			bottom	-= ibottom;
		}

		//缩小
		void Deflate(const RECT& rc)
		{
			left += rc.left;
			top += rc.top;
			right -= rc.right;
			bottom -= rc.bottom;
		}

		//并集，合并矩形
		void Union(const RECT& rc1, const RECT& rc2)
		{
			#ifdef DUILIB_WIN32
			::UnionRect(this, &rc1, &rc2);
			#else
			left	= rc1.left		< rc2.left		? rc1.left		: rc2.left;
			top		= rc1.top		< rc2.top		? rc1.top		: rc2.top;
			right	= rc1.right		> rc2.right		? rc1.right		: rc2.right;
			bottom	= rc1.bottom	> rc2.bottom	? rc1.bottom	: rc2.bottom;
			#endif
		}

		//交集，两个区域的交叉部分
		uiBool Intersect(const RECT& rect1, const RECT& rect2)
		{
			#ifdef DUILIB_WIN32
			return ::IntersectRect(this, (LPRECT)&rect1, (LPRECT)&rect2);
			#else
			left = rect1.left > rect2.left ? rect1.left : rect2.left;
			top = rect1.top > rect2.top ? rect1.top : rect2.top;
			right = rect1.right < rect2.right ? rect1.right : rect2.right;
			bottom = rect1.bottom < rect2.bottom ? rect1.bottom: rect2.bottom;
			return !IsEmpty();
			#endif
		}

		//中心点
		CDuiPoint CenterPoint() const
		{
			return CDuiPoint((left + right) / 2, (top + bottom) / 2);
		}

		//使区域相对于rc的位置对齐，不改变当前的大小
		void AlignRect(const RECT &rc, UINT uAlign = DT_CENTER|DT_VCENTER);

		//区域是否相等
		uiBool EqualRect(const CDuiRect& rc) const
		{
			return left == rc.left && right == rc.right && top == rc.top && bottom == rc.bottom;
		}

		//pt是否在区域中
		uiBool PtInRect(POINT pt) const
		{
			#ifdef DUILIB_WIN32
			return ::PtInRect(this, pt);
			#else
			return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom;
			#endif
		}

		//设置边距，获得除去边距后的区域
		void SetInset(const RECT& rc)
		{
			left += rc.left;
			right -= rc.right;
			top += rc.top;
			bottom -= rc.bottom;
		}

		//设置边距，获得除去边距后的区域
		void SetPadding(const RECT& rc)
		{
			SetInset(rc);
		}

	#ifdef DUILIB_SDL //前向声明，不可内联。
		CDuiRect(const SDL_Rect& rc);               // 从 SDL_Rect 构造
		SDL_Rect ToSDL_Rect() const;               // 转换为 SDL_Rect
		operator SDL_Rect() const;                 // 隐式转换
		CDuiRect& operator=(const SDL_Rect& rc);   // 赋值

		CDuiRect(const SDL_FRect& rc);             // 从 SDL_FRect 构造
		SDL_FRect ToSDL_FRect() const;             // 转换为 SDL_FRect
		operator SDL_FRect() const;                // 隐式转换
		CDuiRect& operator=(const SDL_FRect& rc);  // 赋值
	#endif
	};

	// 兼容保留，不再使用
	#define UIARGB(a,r,g,b)  ((COLORREF)((((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16))  |(((DWORD)(BYTE)(a))<<24))  )
	#define UIARGB_GetAValue(argb)      (LOBYTE((argb)>>24))
	#define UIARGB_GetRValue(argb)      (LOBYTE((argb)>>16))
	#define UIARGB_GetGValue(argb)      (LOBYTE(((WORD)(argb)) >> 8))
	#define UIARGB_GetBValue(argb)      (LOBYTE(argb))
	#define UIRGB(r,g,b)	((COLORREF)((((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16))  |(((DWORD)(BYTE)(255))<<24))  )
	#define UIARGB_2_RGB(argb)	(RGB(UIARGB_GetRValue(argb), UIARGB_GetGValue(argb), UIARGB_GetBValue(argb)))
	#define RGB_2_UIRGB(rgb)	(UIRGB(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)))
	//////////////////////////////////////////////////////////////////////////
	// 
	class UILIB_API CDuiColor
	{
	public:
		CDuiColor()									: m_dwColor(0x00000000) {}
		CDuiColor(BYTE r, BYTE g, BYTE b)			: m_dwColor(0xFF000000		 | ((DWORD)r << 16) | ((DWORD)g << 8) | b) {}
		CDuiColor(BYTE a, BYTE r, BYTE g, BYTE b)	: m_dwColor(((DWORD)a << 24) | ((DWORD)r << 16) | ((DWORD)g << 8) | b) {}
		CDuiColor(DWORD dwColor)					: m_dwColor(dwColor)  {}
		CDuiColor(int n)							: m_dwColor((DWORD)n) {}
		CDuiColor(UINT n)							: m_dwColor((DWORD)n) {}
		CDuiColor(LPCTSTR pstrValue);  // 从字符串解析

		// 从字符串解析，支持格式：
		// "0xAARRGGBB"  (例如 "0xFFFF0000" 表示红色)
		// "#AARRGGBB"   (例如 "#FFFF0000")
		// "a,r,g,b"     (例如 "255,255,0,0")
		// "r,g,b"       (例如 "255,0,0"  此时 alpha 自动设为 255)
		bool FromString(LPCTSTR pstrValue);

		//输出字符串, 如："0xFF000000"
		CDuiString ToString() const;				

		// 从 COLORREF 构造（Alpha = 255）
		void FromCOLORREF(COLORREF cr)                                      
		{
			// COLORREF 格式: 0x00BBGGRR (低8位=R, 中8位=G, 高8位=B)
			// 转换为内部 ARGB: 0xFFRRGGBB
			m_dwColor = 0xFF000000 | ((cr & 0x000000FF) << 16) | (cr & 0x0000FF00) | ((cr & 0x00FF0000) >> 16);
		}

		// 转换为 COLORREF（0xFFBBGGRR）
		COLORREF ToCOLORREF() const                                         
		{
			// 内部 ARGB: 0xAARRGGBB -> 转换为 0xFFBBGGRR
			return 0xFF000000 | ((m_dwColor & 0x000000FF) << 16) | (m_dwColor & 0x0000FF00) | ((m_dwColor & 0x00FF0000) >> 16);
		}

		// 获取各通道
		BYTE GetA() const { return (BYTE)(m_dwColor >> 24); }
		BYTE GetR() const { return (BYTE)(m_dwColor >> 16); }
		BYTE GetG() const { return (BYTE)(m_dwColor >> 8); }
		BYTE GetB() const { return (BYTE)(m_dwColor); }

		// 设置各通道
		void SetA(BYTE a) { m_dwColor = (m_dwColor & 0x00FFFFFF) | ((DWORD)a << 24); }
		void SetR(BYTE r) { m_dwColor = (m_dwColor & 0xFF00FFFF) | ((DWORD)r << 16); }
		void SetG(BYTE g) { m_dwColor = (m_dwColor & 0xFFFF00FF) | ((DWORD)g << 8); }
		void SetB(BYTE b) { m_dwColor = (m_dwColor & 0xFFFFFF00) | (DWORD)b; }

		// 直接获取/设置
		DWORD GetColor() const			{ return m_dwColor; }
		void SetColor(DWORD dwColor)	{ m_dwColor = dwColor; }

		// 类型转换
		operator DWORD() const { return m_dwColor; }

		// 运算符重载
		bool operator==(const CDuiColor& other) const	{ return m_dwColor == other.m_dwColor; }
		bool operator!=(const CDuiColor& other) const	{ return m_dwColor != other.m_dwColor; }
		bool operator==(int other) const				{ return m_dwColor == (DWORD)other; }
		bool operator!=(int other) const				{ return m_dwColor != (DWORD)other; }
		bool operator==(UINT other) const				{ return m_dwColor == (DWORD)other; }
		bool operator!=(UINT other) const				{ return m_dwColor != (DWORD)other; }

		// 赋值运算符重载
		CDuiColor& operator=(DWORD dwColor)				{ m_dwColor = dwColor;	return *this; }
		CDuiColor& operator=(int n)						{ m_dwColor = (DWORD)n; return *this; }
		CDuiColor& operator=(unsigned int n)			{ m_dwColor = (DWORD)n;	return *this; }
		CDuiColor& operator=(LPCTSTR pstrValue);

	#ifdef DUILIB_SDL //SDL_Color 前向声明，不可内联。
		CDuiColor(const SDL_Color& color); // 从 SDL_Color 构造
		SDL_Color ToSDL_Color() const;
		operator SDL_Color() const;
		CDuiColor& operator=(const SDL_Color& color);

		CDuiColor(const SDL_FColor& color); // 从 SDL_Color 构造
		SDL_FColor ToSDL_FColor() const;
		operator SDL_FColor() const;
		CDuiColor& operator=(const SDL_FColor& color);
	#endif

		// 静态预定义颜色
		static CDuiColor Black;   // 0xFF000000
		static CDuiColor White;   // 0xFFFFFFFF
		static CDuiColor Red;     // 0xFFFF0000
		static CDuiColor Green;   // 0xFF00FF00
		static CDuiColor Blue;    // 0xFF0000FF
	private:
		DWORD m_dwColor;
	};
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		int FindInMap(LPVOID iIndex);
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		bool Remove(LPVOID pData);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

		void SetSaveIndexMap(bool bIndexMap);
	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;

		bool m_bSaveIndexMap;
		std::map<LPVOID, int> m_mapIndex;	//存储数组序号，提高查找指针的效率
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//
#ifdef WIN32
	typedef volatile LONG REF_NUMBER;
	#define UIATOMIC_INC(ptr) InterlockedIncrement((LONG*)ptr)
	#define UIATOMIC_DEC(ptr) InterlockedDecrement((LONG*)ptr)
	#define UIATOMIC_GET(ptr) InterlockedCompareExchange((LONG*)ptr, 0, 0)
	#define UIATOMIC_SET(ptr, val) InterlockedExchange((LONG*)ptr, (LONG)val)
#else
	typedef volatile int REF_NUMBER; // 使用 GCC/Clang 内置原子操作
	#define UIATOMIC_INC(ptr) __sync_add_and_fetch(ptr, 1)
	#define UIATOMIC_DEC(ptr) __sync_sub_and_fetch(ptr, 1)
	#define UIATOMIC_GET(ptr) __sync_fetch_and_add(ptr, 0)   // 原子读
	#define UIATOMIC_SET(ptr, val) __sync_lock_test_and_set(ptr, val)
#endif

	struct IObjRef
	{
		virtual REF_NUMBER AddRef() = 0;
		virtual REF_NUMBER Release() = 0;
		virtual void OnFinalRelease() = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//
	template<class T>
	class TObjRefImpl :  public T
	{
	public:
		TObjRefImpl():m_cRef(1)
		{
		}
		virtual ~TObjRefImpl() {}

		//添加引用
		virtual REF_NUMBER AddRef() override
		{
			return UIATOMIC_INC(&m_cRef);
		}

		//!释放引用
		virtual REF_NUMBER Release() override
		{
			REF_NUMBER lRet = UIATOMIC_DEC(&m_cRef);
			if (lRet == 0)
			{
				OnFinalRelease();
			}
			return lRet;
		}

		//!释放对象
		virtual void OnFinalRelease() override
		{
			delete this;
		}
	protected:
		REF_NUMBER m_cRef;
	};

	template <class T>
	class MakeRefPtr
	{
	public:
		MakeRefPtr(T *lp) { p = lp; }
		T *p;
	};

	//////////////////////////////////////////////////////////////////////////
	/*  注意：直接赋值指针会导致引用增加，如果真的是需要直接赋值，应该使用MakeRefPtr，类似于std::make_shared。
	  UIFont *pFont = UIGlobal::CreateFont();					//引用 = 1
	  1, CStdRefPtr<UIFont> font = pFont;						//引用 = 2;
	  2, CStdRefPtr<UIFont> font = MakeRefPtr<UIFont>(pFont)	//引用 = 1;
	  3, *((UIFont**)&font) = pFont;							//引用 = 1;  获取CStdRefPtr内部指针直接赋值。
	  4, 为了防止手误，禁止delete pFont; 只能pFont->Release() 或 pFont = NULL;
	  5, pFont->Relase() 和 pFont = NULL 的区别, 
	     两者都会引用-1, 不可重复调用.
		 Relase之后不能再给pFont赋值, 否则导致原来的pFont引用-1. 
		 赋值NULL之后,可以继续给pFont赋值新的.
	*/
	template <class T>
	class CStdRefPtr
	{
	public:
		CStdRefPtr() throw()
		{
			p = NULL;
		}
		CStdRefPtr(int nNull) throw()
		{
			(void)nNull;
			p = NULL;
		}
		CStdRefPtr(T* lp) throw()
		{
			p = lp;
			if (p != NULL)
			{
				p->AddRef();
			}
		}

		CStdRefPtr(const CStdRefPtr & src) throw()
		{
			p=src.p;
			if(p)
			{
				p->AddRef();
			}
		}

		CStdRefPtr(const MakeRefPtr<T> & src) throw()
		{
			p = src.p;
			//不要AddRef();
		}

		~CStdRefPtr() throw()
		{
			if (p)
			{
				p->Release();
			}
		}

		T* operator->() const throw()
		{
			return p;
		}

		operator T*() const throw()
		{
			return p;
		}

		T& operator*() const
		{
			return *p;
		}

		bool operator!() const throw()
		{
			return (p == NULL);
		}

		bool operator<(T* pT) const throw()
		{
			return p < pT;
		}

		bool operator!=(T* pT) const
		{
			return !operator==(pT);
		}

		bool operator==(T* pT) const throw()
		{
			return p == pT;
		}

		T* operator=(T* lp) throw()
		{
			if(*this!=lp)
			{
				if(p)
				{
					p->Release();
				}
				p=lp;
				if(p)
				{
					p->AddRef();
				}
			}
			return *this;
		}

		T* operator=(const MakeRefPtr<T> &mk) throw()
		{
			if(*this != mk.p)
			{
				if(p)
				{
					p->Release();
				}
				p=mk.p;
				//不要AddRef();
			}
			return *this;
		}

		T* operator=(const CStdRefPtr<T>& lp) throw()
		{
			if(*this!=lp)
			{
				if(p)
				{
					p->Release();
				}
				p=lp;
				if(p)
				{
					p->AddRef();
				}
			}
			return *this;
		}

		// Release the interface and set to NULL
		void Release() throw()
		{
			T* pTemp = p;
			if (pTemp)
			{
				p = NULL;
				pTemp->Release();
			}
		}

		// Attach to an existing interface (does not AddRef)
		void Attach(T* p2) throw()
		{
			if (p)
			{
				p->Release();
			}
			p = p2;
		}
		// Detach the interface (does not Release)
		T* Detach() throw()
		{
			T* pt = p;
			p = NULL;
			return pt;
		}
		uiBool CopyTo(T** ppT) throw()
		{
			if (ppT == NULL)
				return uiFalse;
			*ppT = p;
			if (p)
			{
				p->AddRef();
			}
			return uiTrue;
		}

	protected:
		T* p;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM
	{
		CDuiString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

	class UILIB_API CStdStringPtrMap
	{
	public:
		CStdStringPtrMap(int nSize = 83);
		~CStdStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiWaitCursor
	{
	public:
		CDuiWaitCursor();
		~CDuiWaitCursor();

	protected:
#ifdef DUILIB_WIN32
		HCURSOR m_hOrigCursor;
#endif
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
#ifdef WIN32
	class CDuiVariant : public VARIANT
	{
	public:
		CDuiVariant()
		{
			VariantInit(this);
		}
		CDuiVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CDuiVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CDuiVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CDuiVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CDuiVariant()
		{
			VariantClear(this);
		}
	};
#endif //#ifdef DUILIB_WIN32
	//////////////////////////////////////////////////////////////////////////
	////
	//lock类
	class CDuiLock
	{
	public:
		CDuiLock();
		~CDuiLock();
		void Lock();
		void Unlock();

	private:
#ifdef DUILIB_WIN32
		CRITICAL_SECTION m_lock;
#elif defined DUILIB_SDL
		SDL_Mutex *m_lock;
#else
		std::mutex m_lock;
#endif
	};

	//////////////////////////////////////////////////////////////////////////
	//lock类
	class CDuiInnerLock
	{
	public:
		CDuiInnerLock(CDuiLock *p) : ptr(p) { ptr->Lock(); }
		~CDuiInnerLock(){ ptr->Unlock(); }
	private:
		CDuiLock *ptr;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//双向链表类接口
	struct UILIB_API ILinkedList
	{
		ILinkedList()
		{
			pLinkedPrev = NULL;
			pLinkedNext = NULL;
		}

		ILinkedList *prev() { return pLinkedPrev; }
		ILinkedList *next() { return pLinkedNext; }

		ILinkedList *pLinkedPrev;
		ILinkedList *pLinkedNext;
	};

	//双向链表类
	template <class T>
	class CStdLinkList
	{
	public:
		CStdLinkList()
		{
			m_pHeader = NULL;
			m_pTail = NULL;
			m_count = 0;
		}

		void push_front(T *pItem)
		{
			if(m_pHeader == NULL)
			{
				pItem->pLinkedPrev = NULL;
				pItem->pLinkedNext = NULL;
				m_pHeader = pItem;
				m_pTail = pItem;
			}
			else
			{
				m_pHeader->pLinkedPrev = pItem;
				pItem->pLinkedPrev = NULL;
				pItem->pLinkedNext = m_pHeader;
				m_pHeader = pItem;
			}
			m_count++;
		}

		void push_back(T *pItem)
		{
			if(m_pHeader == NULL)
			{
				pItem->pLinkedPrev = NULL;
				pItem->pLinkedNext = NULL;
				m_pHeader = pItem;
				m_pTail = pItem;
			}
			else
			{
				m_pTail->pLinkedNext = pItem;
				pItem->pLinkedPrev = m_pTail;
				pItem->pLinkedNext = NULL;
				m_pTail = pItem;
			}
			m_count++;
		}

		void insert(T *pItemPosition, T *pItem)
		{
			if(pItemPosition == m_pHeader)
			{
				push_front(pItem);
				m_count++;
			}
			else if(pItemPosition == m_pTail)
			{
				push_back(pItem);
				m_count++;
			}
			else
			{
				pItem->pLinkedNext = pItemPosition;
				pItem->pLinkedPrev = pItemPosition->pLinkedPrev;
				pItem->pLinkedPrev->pLinkedNext = pItem;
				pItemPosition->pLinkedPrev = pItem;
				m_count++;
			}
		}

		void erase(T *pItem)
		{
			if(pItem == m_pHeader && m_pHeader == m_pTail)
			{
				m_pHeader = m_pTail = NULL;
				m_count--;
			}
			else if(pItem == m_pHeader)
			{
				m_pHeader = (T *)m_pHeader->pLinkedNext;
				m_pHeader->pLinkedPrev = NULL;
				m_count--;
			}
			else if(pItem == m_pTail)
			{
				m_pTail = (T *)m_pTail->pLinkedPrev;
				m_pTail->pLinkedNext = NULL;
				m_count--;
			}
			else
			{
				pItem->pLinkedPrev->pLinkedNext = pItem->pLinkedNext;
				pItem->pLinkedNext->pLinkedPrev = pItem->pLinkedPrev;
				m_count--;
			}
		}

		T* pop_front()
		{
			if(m_pHeader == NULL)
				return NULL;

			T *pTemp = NULL;

			if(m_pHeader == m_pTail)
			{
				pTemp = m_pHeader;
				m_pHeader = NULL;
				m_pTail = NULL;
			}
			else
			{
				pTemp = m_pHeader;
				m_pHeader = (T *)m_pHeader->pLinkedNext;
				m_pHeader->pLinkedPrev = NULL;
			}

			m_count--;
			return pTemp;
		}

		T* pop_back()
		{
			if(m_pHeader == NULL)
				return NULL;

			T *pTemp = NULL;

			if(m_pHeader == m_pTail)
			{
				pTemp = m_pHeader;
				m_pHeader = NULL;
				m_pTail = NULL;
			}
			else
			{
				pTemp = m_pTail;
				m_pTail = (T *)m_pTail->pLinkedPrev;
				m_pTail->pLinkedNext = NULL;
			}

			m_count--;
			return pTemp;
		}

		bool empty()
		{
			return m_pHeader == NULL;
		}

		T* header()
		{
			return m_pHeader;
		}

		T* tail()
		{
			return m_pTail;
		}

		int size()
		{
			return m_count;
		}
	private:
		T *m_pHeader;
		T *m_pTail;
		int m_count;
	};

	//////////////////////////////////////////////////////////////////////////
	//控件内存
	template <class T>
	class CStdControlPool
	{
	public:
		CStdControlPool()
		{
			_blockcountnext = 32;
			_nMaxMemoryPageSize = 1024 * 5;
#ifdef _DEBUG
//			OutputDebugStringA("construct pool\r\n");
#endif
		}

		~CStdControlPool()
		{
			ClearMemory();
#ifdef _DEBUG
//			OutputDebugStringA("destroy pool\r\n");
#endif
		}

		//设定每次申请内存时，控件个数的上限。 多了浪费内存，少了影响效率，根据使用场景调整。
		void SetMaxMemoryPageSize(int nSize)
		{
			_nMaxMemoryPageSize = nSize;
		}

		T* Alloc()
		{
			if(m_listControl.empty())
			{
				MakePool();
			}

			T *tt = (T *)m_listControl.pop_back();
			new (tt)T; //使用tt这个内存地址new T()
			return tt;
		}

		void Free(T *t)
		{
			t->~T();
			//memset(t, 0, sizeof(T));
			PBYTE pMem = reinterpret_cast<PBYTE>(t);
			memset(pMem, 0, sizeof(T));
			m_listControl.push_back(t);
		}

		//清理内存，如果调用这个函数之前，有内存没有归还，也会清理，对象变成野指针。
		//要小心使用，可能导致无法判断运行时内存泄漏。
		void ClearMemory()
		{
			//DWORD tk = GetTickCount();

			for (int i=0; i<m_pListMemBlock.GetSize(); i++)
			{
				free( (BYTE*)m_pListMemBlock[i] );
			}

			// 			CDuiString s;
			// 			s.Format(_T("%d"), GetTickCount() - tk);
			// 			MessageBox(NULL, s, _T("释放时间"), MB_OK);
		}

	protected:
		void MakePool()
		{
			//分配一个连续空间，因为如果每个object都new一次，new和delete都会耗费大量时间。
			int tagSize = sizeof(T);
			BYTE *pBlock = (BYTE *)malloc(tagSize * _blockcountnext);
			memset(pBlock, 0, tagSize);
			for (int i=0; i<_blockcountnext; i++)
			{
				T *p = (T *)(pBlock + i*tagSize);
				m_listControl.push_back(p);
			}
			m_pListMemBlock.Add(pBlock);

			//当下次分配内存时，不要简单粗暴的乘以2，设定一个上限
			if(_blockcountnext < _nMaxMemoryPageSize)
				_blockcountnext *= 2;
			if(_blockcountnext > _nMaxMemoryPageSize)
				_blockcountnext = _nMaxMemoryPageSize;
		}

	private:
		CStdLinkList<T> m_listControl;
		int _blockcountnext;
		int _nMaxMemoryPageSize;
		CStdPtrArray m_pListMemBlock;
	};

}// namespace DuiLib

#endif // __UTILS_H__

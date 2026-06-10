#ifndef __UTILS_H__
#define __UTILS_H__
#pragma once

#ifdef DUILIB_WIN32
#include "OAIdl.h"
#endif

#include <vector>
#include <map>

#ifndef WIN32
#include <mutex>
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
		CDuiPoint();
		CDuiPoint(const POINT& src);
		CDuiPoint(const SIZE& src);
		CDuiPoint(int x, int y);
		CDuiPoint(LPARAM lParam);
		CDuiPoint(LPCTSTR pstrValue);

		bool FromString(LPCTSTR pstrValue); //从"x,y"构造POINT
		CDuiString ToString();				//输出字符串"x,y"

		// 类型转换
		operator LPSIZE() const;                  // 转换为 SIZE*（实际指向自身，但注意 SIZE 布局与 POINT 相同）
		operator SIZE() const;                    // 转换为 SIZE（返回 {x,y}）
		operator PPOINT() throw();

		// 赋值运算符
		CDuiPoint& operator=(const POINT& src);
		CDuiPoint& operator=(const SIZE& src);

		// 比较运算符
		bool operator==(const POINT& src) const;
		bool operator!=(const POINT& src) const;
		bool operator==(const SIZE& src) const;
		bool operator!=(const SIZE& src) const;

		// 算术运算符（与 POINT/SIZE）
		CDuiPoint operator+(const POINT& src) const;
		CDuiPoint operator-(const POINT& src) const;
		CDuiPoint operator+(const SIZE& src) const;
		CDuiPoint operator-(const SIZE& src) const;

		// 复合赋值运算符
		CDuiPoint& operator+=(const POINT& src);
		CDuiPoint& operator-=(const POINT& src);
		CDuiPoint& operator+=(const SIZE& src);
		CDuiPoint& operator-=(const SIZE& src);

		// 坐标操作
		void SetPoint(int x, int y);
		void Offset(int dx, int dy);
		void Offset(const POINT& delta);
		void Offset(const SIZE& delta);
		bool IsEmpty() const;                     // x==0 && y==0
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiSize : public tagSIZE
	{
	public:
		CDuiSize();
		CDuiSize(const SIZE& src);
		CDuiSize(const RECT& rc);
		CDuiSize(const POINT& pt);
		CDuiSize(int cx, int cy);
		CDuiSize(LPCTSTR pstrValue);

		bool FromString(LPCTSTR pstrValue); //从"cx,cy"构造SIZE
		CDuiString ToString();				//输出字符串"cx,cy"

		void SetSize(int cx, int cy);
		void SetSize(const SIZE& src);

		// 类型转换
		operator LPSIZE() throw();
		operator POINT() const;
		operator RECT() const;

		// 赋值运算符
		CDuiSize& operator=(const SIZE& src);

		// 比较运算符
		bool operator==(const SIZE& src) const;
		bool operator!=(const SIZE& src) const;

		bool operator==(const POINT& src) const;
		bool operator!=(const POINT& src) const;

		bool operator==(const RECT& src) const;
		bool operator!=(const RECT& src) const;

		// 算术运算符（二元）
		CDuiSize operator+(const SIZE& src) const;
		CDuiSize operator-(const SIZE& src) const;

		CDuiSize operator+(const POINT& src) const;
		CDuiSize operator-(const POINT& src) const;

		CDuiSize operator+(const RECT& src) const;
		CDuiSize operator-(const RECT& src) const;

		// 复合赋值运算符
		CDuiSize& operator+=(const SIZE& src);
		CDuiSize& operator-=(const SIZE& src);

		CDuiSize& operator+=(const POINT& src);
		CDuiSize& operator-=(const POINT& src);

		CDuiSize& operator+=(const RECT& src);
		CDuiSize& operator-=(const RECT& src);

		//放大
		void Inflate(int x, int y);

		//缩小
		void Deflate(int x, int y);

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
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(LPCRECT src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);
		CDuiRect(const POINT &ptLeftTop, const SIZE &szWidthHeight);
		CDuiRect(const POINT &ptLeftTop, const POINT &ptRightBottom);
		CDuiRect(const SIZE& src);
		CDuiRect(LPCTSTR pstrValue);

		bool FromString(LPCTSTR pstrValue); //从"left,top,right,bottom"构造RECT
		CDuiString ToString();				//输出字符串"left,top,right,bottom"

		operator LPRECT() throw();
		operator LPCRECT() const throw();
		operator SIZE() const throw();

		CDuiRect& operator=(const RECT& src);
		CDuiRect& operator=(const SIZE& src);
		CDuiRect& operator=(LPCTSTR pstrValue);

		CDuiPoint LeftTop();
		CDuiPoint RightBottom();

		void SetRect(int left, int top, int right, int bottom);
		void SetRect(const POINT &ptLeftTop, const SIZE &szWidthHeight);
		void SetRect(const POINT &ptLeftTop, const POINT &ptRightBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		bool IsEmpty() const;

		//把rc加入当前区域中，并集
		void Join(const RECT& rc);

		//重置偏移位置，结果为：left = 0, top = 0, left = {width}, right = {height}
		void ResetOffset();

		//使规范化，结果为: left <= right, top <= bottom
		void Normalize();

		//偏移，移动区域位置
		void Offset(int cx, int cy);

		//放大
		void Inflate(int cx, int cy);
		void Inflate(int left, int top, int right, int bottom);
		void Inflate(const RECT &rc);

		//缩小
		void Deflate(int cx, int cy);
		void Deflate(int left, int top, int right, int bottom);
		void Deflate(const RECT &rc);

		//并集，合并矩形
		void Union(const RECT& rc1, const RECT& rc2);

		//交集，两个区域的交叉部分
		BOOL Intersect(const RECT &rect1, const RECT &rect2);

		//中心点
		POINT CenterPoint() const;

		//使区域相对于rc的位置对齐，不改变当前的大小
		void AlignRect(const RECT &rc, UINT uAlign = DT_CENTER|DT_VCENTER);

		//区域是否相等
		BOOL EqualRect(const CDuiRect& rc) const;

		//pt是否在区域中
		BOOL PtInRect(POINT pt) const;
		//bool operator == (LPCRECT lpRect) const;

		//设置边距，获得除去边距后的区域
		void SetPadding(const RECT& rc);
		void SetInset(const RECT& rc);

	#ifdef DUILIB_SDL
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
		CDuiColor();                                          // 默认透明（0x00000000）
		CDuiColor(DWORD dwColor);                             // 直接使用 ARGB 值
		CDuiColor(BYTE r, BYTE g, BYTE b);                    // 不透明 RGB，A = 255
		CDuiColor(BYTE a, BYTE r, BYTE g, BYTE b);            // 完整 ARGB
		CDuiColor(LPCTSTR pstrValue);                         // 从字符串解析
		CDuiColor(int n);										// 直接使用 ARGB 值
		CDuiColor(UINT n);										// 直接使用 ARGB 值

		// 从字符串解析，支持格式：
		// "0xAARRGGBB"  (例如 "0xFFFF0000" 表示红色)
		// "#AARRGGBB"   (例如 "#FFFF0000")
		// "a,r,g,b"     (例如 "255,255,0,0")
		// "r,g,b"       (例如 "255,0,0"  此时 alpha 自动设为 255)
		bool FromString(LPCTSTR pstrValue);

		//输出字符串, 如："0xFF000000"
		CDuiString ToString();				

		void FromCOLORREF(COLORREF clr);
		COLORREF ToCOLORREF();

		// 获取各通道
		BYTE GetA() const;
		BYTE GetR() const;
		BYTE GetG() const;
		BYTE GetB() const;

		// 设置各通道
		void SetA(BYTE a);
		void SetR(BYTE r);
		void SetG(BYTE g);
		void SetB(BYTE b);

		// 直接获取/设置
		DWORD GetColor() const;
		void SetColor(DWORD dwColor);

		// 类型转换
		operator DWORD() const;

		// 运算符重载
		bool operator==(const CDuiColor& other) const;
		bool operator!=(const CDuiColor& other) const;
		bool operator==(int other) const;
		bool operator!=(int other) const;
		bool operator==(UINT other) const;
		bool operator!=(UINT other) const;

		// 赋值运算符重载
		CDuiColor& operator=(DWORD dwColor);
		CDuiColor& operator=(LPCTSTR pstrValue);
		CDuiColor& operator=(int n); 
		CDuiColor& operator=(unsigned int n);

	#ifdef DUILIB_SDL
		CDuiColor(const SDL_Color& color);                    // 从 SDL_Color 构造
		SDL_Color ToSDL_Color();
		operator SDL_Color() const;
		CDuiColor& operator=(const SDL_Color& color);
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
#define _REF_NUMBER	LONG
#else
#define _REF_NUMBER	int
#endif
	struct IObjRef
	{
		virtual _REF_NUMBER AddRef() = 0;

		virtual _REF_NUMBER Release() = 0;

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
		virtual _REF_NUMBER AddRef() override
		{
#ifdef WIN32
			return ::InterlockedIncrement(&m_cRef);
#else
			return ++m_cRef;
#endif
		}

		//!释放引用
		virtual _REF_NUMBER Release() override
		{
#ifdef WIN32
			long lRet = ::InterlockedDecrement(&m_cRef);
#else
			int lRet = --m_cRef;
#endif
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
		volatile _REF_NUMBER m_cRef;
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
		BOOL CopyTo(T** ppT) throw()
		{
			if (ppT == NULL)
				return FALSE
			*ppT = p;
			if (p)
			{
				p->AddRef();
			}
			return TRUE;
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
		UINT_PTR m_lock;
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

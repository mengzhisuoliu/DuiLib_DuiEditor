#include "StdAfx.h"

namespace DuiLib
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	bool CDuiPoint::FromString(LPCTSTR pstrValue) //从"x,y"构造POINT
	{
		x = y = 0;
		if (!pstrValue || !*pstrValue)
			return false;

		while (*pstrValue == _T(' ') || *pstrValue == _T('\t')) ++pstrValue;

		TCHAR* pEnd = nullptr;
		x = _tcstol(pstrValue, &pEnd, 10);
		if (pEnd == pstrValue)
			return false;

		pstrValue = pEnd;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t')) ++pstrValue;
		if (*pstrValue != _T(','))
			return false;
		++pstrValue;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t')) ++pstrValue;

		y = _tcstol(pstrValue, &pEnd, 10);
		if (pEnd == pstrValue)
			return false;
		return true;
	}

	CDuiString CDuiPoint::ToString() const				//输出字符串"x,y"
	{
		CDuiString sPoint;
		sPoint.SmallFormat(_T("%ld,%ld"), x, y);
		return sPoint;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	bool CDuiSize::FromString(LPCTSTR pstrValue) //从"cx,cy"构造SIZE
	{
		cx = cy = 0;
		if (!pstrValue || !*pstrValue)
			return false;

		while (*pstrValue == _T(' ') || *pstrValue == _T('\t')) ++pstrValue;

		TCHAR* pEnd = nullptr;
		cx = _tcstol(pstrValue, &pEnd, 10);
		if (pEnd == pstrValue)
			return false;

		pstrValue = pEnd;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t')) ++pstrValue;
		if (*pstrValue != _T(','))
			return false;
		++pstrValue;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t')) ++pstrValue;

		cy = _tcstol(pstrValue, &pEnd, 10);
		if (pEnd == pstrValue)
			return false;
		return true;
	}

	CDuiString CDuiSize::ToString() const				//输出字符串"cx,cy"
	{
		CDuiString sSize;
		sSize.SmallFormat(_T("%ld,%ld"), cx, cy);
		return sSize;
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
	CDuiRect::CDuiRect(LPCTSTR pstrValue)
	{
		FromString(pstrValue);
	}

	bool CDuiRect::FromString(LPCTSTR pstrValue) //从"left,top,right,bottom"构造RECT
	{
		// 初始化所有边为0
		left = top = right = bottom = 0;

		// 空字符串或空指针直接失败
		if (!pstrValue || !*pstrValue)
			return false;

		// 跳过字符串开头的空白字符
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t'))
			++pstrValue;

		TCHAR* pEnd = nullptr;

		// 解析 left
		left = _tcstol(pstrValue, &pEnd, 10);
		if (pEnd == pstrValue)  // 没有转换出任何数字
			return false;

		// 跳过数字后的空白，并期望逗号
		pstrValue = pEnd;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t'))
			++pstrValue;
		if (*pstrValue != _T(','))
			return false;
		++pstrValue;  // 跳过逗号

		// 跳过逗号后的空白
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t'))
			++pstrValue;

		// 解析 top
		top = _tcstol(pstrValue, &pEnd, 10);
		if (pEnd == pstrValue)
			return false;
		pstrValue = pEnd;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t'))
			++pstrValue;
		if (*pstrValue != _T(','))
			return false;
		++pstrValue;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t'))
			++pstrValue;

		// 解析 right
		right = _tcstol(pstrValue, &pEnd, 10);
		if (pEnd == pstrValue)
			return false;
		pstrValue = pEnd;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t'))
			++pstrValue;
		if (*pstrValue != _T(','))
			return false;
		++pstrValue;
		while (*pstrValue == _T(' ') || *pstrValue == _T('\t'))
			++pstrValue;

		// 解析 bottom
		bottom = _tcstol(pstrValue, &pEnd, 10);
		if (pEnd == pstrValue)
			return false;

		// 可选：允许末尾多余空白，但不再检查
		return true;
	}

	CDuiString CDuiRect::ToString() const				//输出字符串"left,top,right,bottom"
	{
		CDuiString sRect;
		sRect.SmallFormat(_T("%ld,%ld,%ld,%ld"), left, top, right, bottom);
		return sRect;
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
	CDuiColor::CDuiColor(LPCTSTR pstrValue)
	{
		if (!FromString(pstrValue)) 
		{
			m_dwColor = 0x00000000;
		}
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

	CDuiString CDuiColor::ToString() const 
	{
		CDuiString s;
		s.Format(_T("0x%02X%02X%02X%02X"), GetA(), GetR(), GetG(), GetB());
		return s;
	}

	CDuiColor& CDuiColor::operator=(LPCTSTR pstrValue)
	{
		if (!FromString(pstrValue)) 
		{
			m_dwColor = 0x00000000;
		}
		return *this;
	}

	#ifdef DUILIB_SDL
	CDuiColor::CDuiColor(const SDL_Color& color)
	{
		m_dwColor = ((DWORD)color.a << 24) | ((DWORD)color.r << 16) | ((DWORD)color.g << 8) | color.b;
	}

	SDL_Color CDuiColor::ToSDL_Color() const
	{
		return { GetR(), GetG(), GetB(), GetA() };
	}

	CDuiColor::operator SDL_Color() const 
	{ 
		return SDL_Color{ GetR(), GetG(), GetB(), GetA() }; 
	}

	CDuiColor& CDuiColor::operator=(const SDL_Color& color)
	{
		m_dwColor = ((DWORD)color.a << 24) | ((DWORD)color.r << 16) | ((DWORD)color.g << 8) | color.b;
		return *this;
	}
	//////////////////////////////////////////////////////////////////////////

	CDuiColor::CDuiColor(const SDL_FColor& color)
	{
		m_dwColor = (((DWORD)color.a) << 24) | (((DWORD)color.r) << 16) | (((DWORD)color.g) << 8) | (DWORD)color.b;
	}

	SDL_FColor CDuiColor::ToSDL_FColor() const
	{
		SDL_FColor c1;
		c1.r = GetR() / 255.0f;
		c1.g = GetG() / 255.0f;
		c1.b = GetB() / 255.0f;
		c1.a = GetA() / 255.0f;
		return c1;
	}

	CDuiColor::operator SDL_FColor() const
	{
		SDL_FColor c1;
		c1.r = GetR() / 255.0f;
		c1.g = GetG() / 255.0f;
		c1.b = GetB() / 255.0f;
		c1.a = GetA() / 255.0f;
		return c1;
	}

	CDuiColor& CDuiColor::operator=(const SDL_FColor& color)
	{
		BYTE r = color.r * 255.0f;
		BYTE g = color.g * 255.0f;
		BYTE b = color.b * 255.0f;
		BYTE a = color.a * 255.0f;
		m_dwColor = ((DWORD)a << 24) | ((DWORD)r << 16) | ((DWORD)g << 8) | b;
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
		m_lock = SDL_CreateMutex();
	#else
	#endif
	}
	CDuiLock::~CDuiLock()
	{
	#ifdef DUILIB_WIN32
		DeleteCriticalSection(&m_lock);
	#elif defined DUILIB_SDL
		SDL_DestroyMutex(m_lock);
	#else
	#endif
	}

	void CDuiLock::Lock()
	{
	#ifdef DUILIB_WIN32
		EnterCriticalSection(&m_lock);
	#elif defined DUILIB_SDL
		SDL_LockMutex(m_lock);
	#else
		m_lock.lock();
	#endif
	}

	void CDuiLock::Unlock()
	{
	#ifdef DUILIB_WIN32
		LeaveCriticalSection(&m_lock);
	#elif defined DUILIB_SDL
		SDL_UnlockMutex(m_lock);
	#else
		m_lock.unlock();
	#endif
	}

} // namespace DuiLib



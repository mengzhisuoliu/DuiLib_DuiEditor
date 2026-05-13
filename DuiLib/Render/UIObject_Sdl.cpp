#include "StdAfx.h"

#include "../Render/UIObject_Sdl.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

///////////////////////////////////////////////////////////////////////////////////////
namespace DuiLib {
	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIFont_SDL::UIFont_SDL()
	{
		m_pTTF = NULL;
	}

	UIFont_SDL::~UIFont_SDL()
	{
		
	}

	void UIFont_SDL::DeleteObject()
	{
		
	}

	BOOL UIFont_SDL::CreateDefaultFont()
	{
		if (CPaintManagerUI::m_aFonts.GetSize() > 0)
		{
			tagFontFileTTF* pTtfFile = static_cast<tagFontFileTTF*>(CPaintManagerUI::m_aFonts.GetAt(0));
			m_pTTF = pTtfFile->pData;
			return TRUE;
		}
		return FALSE;
	}

	UINT_PTR UIFont_SDL::GetHandle()
	{
		return (UINT_PTR)m_pTTF;
	}

	UIFont* UIFont_SDL::Clone(CPaintManagerUI *pManager)
	{
		UIFont_SDL* pNewFont = new UIFont_SDL;
		pNewFont->id = id;
		pNewFont->sFontName = sFontName;
		pNewFont->iSize = iSize;
		pNewFont->bBold = bBold;
		pNewFont->bUnderline = bUnderline;
		pNewFont->bItalic = bItalic;
		pNewFont->bDefault = bDefault;
		pNewFont->bShared = bShared;
		if (!pNewFont->_buildFont(pManager))
		{
			delete pNewFont; pNewFont = NULL;
		}
		ASSERT(pNewFont);
		return pNewFont;
	}

	int UIFont_SDL::GetHeight(CPaintManagerUI *pManager)
	{
		return iSize;
	}

	BOOL UIFont_SDL::_buildFont(CPaintManagerUI *pManager)
	{
		for (int i = 0; i < CPaintManagerUI::m_aFonts.GetSize(); ++i)
		{
			tagFontFileTTF* pTtfFile = static_cast<tagFontFileTTF*>(CPaintManagerUI::m_aFonts.GetAt(i));
			if (pTtfFile->sName == sFontName && pTtfFile->size == iSize)
			{
				m_pTTF = pTtfFile->pData;
			}
		}
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIPen_SDL::UIPen_SDL()
	{
	}

	UIPen_SDL::~UIPen_SDL()
	{
		
	}

	void UIPen_SDL::DeleteObject()
	{
		
	}

	BOOL UIPen_SDL::CreatePen(int nStyle, int nWidth, DWORD dwColor)
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIBrush_SDL::UIBrush_SDL()
	{

	}

	UIBrush_SDL::~UIBrush_SDL()
	{
		
	}

	void UIBrush_SDL::DeleteObject()
	{
		
	}

	BOOL UIBrush_SDL::CreateSolidBrush(DWORD clr)
	{
        return FALSE;
	}

	BOOL UIBrush_SDL::CreateBitmapBrush(UIBitmap *bitmap)
	{
        return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIPath_SDL::UIPath_SDL()
	{
		
	}

	UIPath_SDL::~UIPath_SDL()
	{

	}

	void UIPath_SDL::DeleteObject()
	{

	}

	BOOL UIPath_SDL::Beginpath()
	{
		return TRUE;
	}

	BOOL UIPath_SDL::EndPath()
	{
		return TRUE;
	}

	BOOL UIPath_SDL::AbortPath()
	{
		return TRUE;
	}

	BOOL UIPath_SDL::AddLine(int x1, int y1, int x2, int y2)
	{
		return TRUE;
	}

	BOOL UIPath_SDL::AddLines(CDuiPoint *points, int count)
	{
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIBitmap_SDL::UIBitmap_SDL()
	{
		m_surface = NULL;
		m_pDataBits = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
	}

	UIBitmap_SDL::~UIBitmap_SDL()
	{
		if (m_surface) { SDL_DestroySurface(m_surface); m_surface = NULL; }
		if (m_pDataBits) { free(m_pDataBits); m_pDataBits = NULL; }
		m_nWidth = 0;
		m_nHeight = 0;
	}

	void UIBitmap_SDL::DeleteObject()
	{
		if (m_surface) { SDL_DestroySurface(m_surface); m_surface = NULL; }
		if (m_pDataBits) { free(m_pDataBits); m_pDataBits = NULL; }
		m_nWidth = 0;
		m_nHeight = 0;
	}


	BOOL UIBitmap_SDL::CreateFromData(LPBYTE pImage, int width, int height, DWORD mask)
	{
		DeleteObject();

		m_pDataBits = (BYTE*)malloc(width * height * 4);
		memcpy(m_pDataBits, pImage, width * height * 4);

		m_nWidth = width;
		m_nHeight = height;

		m_surface = SDL_CreateSurfaceFrom(width, height,
			SDL_PIXELFORMAT_ABGR8888,
			m_pDataBits,
			width * 4);

		return m_surface != NULL;
	}

	UINT_PTR UIBitmap_SDL::GetHandle()
	{
		return (UINT_PTR)m_surface;
	}

	BYTE* UIBitmap_SDL::GetBits()
	{
		return m_pDataBits;
	}

	int	UIBitmap_SDL::GetWidth()
	{
		return m_nWidth;
	}

	int UIBitmap_SDL::GetHeight()
	{
		return m_nHeight;
	}

	BOOL UIBitmap_SDL::IsAlpha()
	{
		return 0;
	}

	UIBitmap *UIBitmap_SDL::Clone()
	{
		UIBitmap_SDL* bmp = NULL;
		return bmp;
	}

	void UIBitmap_SDL::Clear()
	{
		
	}

	void UIBitmap_SDL::ClearAlpha(const RECT &rc, int alpha)
	{
		
	}

	BOOL UIBitmap_SDL::SaveFile(LPCTSTR pstrFileName)
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIImage_SDL::UIImage_SDL()
	{
		bitmap = UIGlobal::CreateBitmap();
		pBits		= nullptr;
		pSrcBits	= nullptr;;
		nWidth			= 0;
		nHeight			= 0;
		bAlpha		= false;
		bUseHSL		= false;
		dwMask		= 0;
		delay		= 0;
	}

	UIImage_SDL::~UIImage_SDL()
	{
		//pBits是从::CreateDIBSection搞过来的，不能delete
		//pSrcBits 是HSL色彩转换时分配的，需要delete
		if (pSrcBits) { delete[] pSrcBits; pSrcBits = NULL; }	
		if(bitmap) { bitmap->Release(); };
	}

	void UIImage_SDL::DeleteObject()
	{
		bitmap->DeleteObject();
		if (pSrcBits) { delete[] pSrcBits; pSrcBits = NULL; }
	}

} // namespace DuiLib


#include "StdAfx.h"

#include "../Render/UIObject_Sdl.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

///////////////////////////////////////////////////////////////////////////////////////
#ifdef DUILIB_SDL
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
		if (m_pTTF) { TTF_CloseFont(m_pTTF); m_pTTF = NULL; }
	}

	void UIFont_SDL::DeleteObject()
	{
		if (m_pTTF) { TTF_CloseFont(m_pTTF); m_pTTF = NULL; }
	}

	BOOL UIFont_SDL::CreateDefaultFont()
	{
		if (CPaintManagerUI::m_aFontFiles.GetSize() > 0)
		{
			tagFontFile* ttfFile = static_cast<tagFontFile*>(CPaintManagerUI::m_aFontFiles.GetAt(0));
			CDuiStringUtf8 utf8 = ttfFile->sPathName;
			m_pTTF = TTF_OpenFont(utf8.toString(), GetSize());
			return m_pTTF != NULL;
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

	int UIFont_SDL::GetHeight()
	{
		if (m_pTTF) {
			return TTF_GetFontHeight(m_pTTF);
		}
		return iSize;
	}

	BOOL UIFont_SDL::_buildFont(CPaintManagerUI *pManager)
	{
		for (int i = 0; i < CPaintManagerUI::m_aFontFiles.GetSize(); ++i)
		{
			tagFontFile* ttfFile = static_cast<tagFontFile*>(CPaintManagerUI::m_aFontFiles.GetAt(i));
			if (ttfFile->sName == sFontName)
			{
				int dpi = pManager->GetDPIObj()->GetDPI();
				int lfHeight = pManager ? pManager->GetDPIObj()->ScaleInt(iSize) : iSize;
				float ptsize = (lfHeight * 72.0f) / dpi; // 像素转点, 为了视觉大小与WIN32兼容。

				CDuiStringUtf8 utf8 = ttfFile->sPathName;
				m_pTTF = TTF_OpenFont(utf8.toString(), ptsize);
				TTF_SetFontSizeDPI(m_pTTF, ptsize, dpi, dpi);
				return m_pTTF != NULL;
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
		DeleteAllTextures();

		if (m_surface) { SDL_DestroySurface(m_surface); m_surface = NULL; }
		if (m_pDataBits) { free(m_pDataBits); m_pDataBits = NULL; }
		m_nWidth = 0;
		m_nHeight = 0;
	}

	void UIBitmap_SDL::DeleteObject()
	{
		DeleteAllTextures();

		if (m_surface) { SDL_DestroySurface(m_surface); m_surface = NULL; }
		if (m_pDataBits) { free(m_pDataBits); m_pDataBits = NULL; }
		m_nWidth = 0;
		m_nHeight = 0;
	}


	BOOL UIBitmap_SDL::CreateFromData(LPBYTE pImage, int width, int height, DWORD mask)
	{
		DeleteObject();

		m_pDataBits = (BYTE*)malloc(width * height * 4);
		if (pImage)
		{
			memcpy(m_pDataBits, pImage, width * height * 4);
		}
		else
		{
			memset(m_pDataBits, 0, width * height * 4);
		}

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
		DeleteAllTextures();
	}

	void UIBitmap_SDL::ClearAlpha(const RECT &rc, int alpha)
	{
		DeleteAllTextures();
	}

	BOOL UIBitmap_SDL::SaveFile(LPCTSTR pstrFileName)
	{
		return FALSE;
	}

	SDL_Texture* UIBitmap_SDL::GetTexture(SDL_Renderer* pRenderer)
	{
		if (!pRenderer || !m_surface) return nullptr;

		// 查找是否已为该渲染器创建过纹理
		auto it = m_textureMap.find(pRenderer);
		if (it != m_textureMap.end()) {
			return it->second;
		}

		// 创建新纹理
		SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, m_surface);
		if (!texture) return nullptr;

		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST); // SDL_SCALEMODE_LINEAR);

		// 存入 map
		m_textureMap[pRenderer] = texture;
		return texture;
	}

	void UIBitmap_SDL::DeleteTexture(SDL_Renderer* pRenderer)
	{
		if (!pRenderer) return;
		auto it = m_textureMap.find(pRenderer);
		if (it != m_textureMap.end()) {
			if (it->second) SDL_DestroyTexture(it->second);
			m_textureMap.erase(it);
		}
	}

	void UIBitmap_SDL::DeleteAllTextures()
	{
		for (auto& pair : m_textureMap) {
			if (pair.second) SDL_DestroyTexture(pair.second);
		}
		m_textureMap.clear();
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
#endif //#ifdef DUILIB_SDL


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
		for (auto pFont : m_vSpareFonts) TTF_CloseFont(pFont);
		m_vSpareFonts.clear();
	}

	void UIFont_SDL::DeleteObject()
	{
		if (m_pTTF) { TTF_CloseFont(m_pTTF); m_pTTF = NULL; }
		for (auto pFont : m_vSpareFonts) TTF_CloseFont(pFont);
		m_vSpareFonts.clear();
	}

	uiBool UIFont_SDL::CreateDefaultFont()
	{
		DeleteObject();
		return _buildFont(NULL);
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

	uiBool UIFont_SDL::_buildFont(CPaintManagerUI *pManager)
	{
		int nFontCount = CPaintManagerUI::m_aFontFiles.GetSize();
		if (nFontCount == 0)
			return uiFalse;

		// DPI 相关计算
		int dpi = 96;
		float ptsize = iSize;
		if (pManager)
		{
			dpi = pManager->GetDPIObj()->GetDPI();
			int lfHeight = pManager->GetDPIObj()->ScaleInt(iSize);
			ptsize = (lfHeight * 72.0f) / dpi;   // 像素转点，视觉大小与 Win32 兼容
		}
		else
		{
			//直接获取主显示器的DPI
			SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
			if (primaryDisplay != 0) 
			{
				float scale = SDL_GetDisplayContentScale(primaryDisplay);
				if (scale > 0.0f) 
				{
					dpi = static_cast<int>(96.0f * scale);
					float lfHeight = iSize * scale;
					ptsize = lfHeight * 72.0f / dpi;
				}
			}
		}

		// 查找与字体名称匹配的主字体索引
		int mainFontIndex = -1;
		for (int i = 0; i < nFontCount; ++i) 
		{
			tagFontFile* ttfFile = static_cast<tagFontFile*>(CPaintManagerUI::m_aFontFiles.GetAt(i));
			if (ttfFile && ttfFile->sName == sFontName) 
			{
				mainFontIndex = i;
				break;
			}
		}

		// 如果未匹配到，使用默认逻辑：第一个字体作为主字体（索引0）
		if (mainFontIndex == -1) 
		{
			mainFontIndex = 0;
		}

		// 创建主字体
		tagFontFile* mainTtf = static_cast<tagFontFile*>(CPaintManagerUI::m_aFontFiles.GetAt(mainFontIndex));
		if (!mainTtf)
			return uiFalse;
		CDuiStringUtf8 utf8Path(mainTtf->sPathName);
		m_pTTF = TTF_OpenFont(utf8Path.toString(), ptsize);
		if (!m_pTTF) return uiFalse;   // 主字体创建失败则整体失败
		TTF_SetFontSizeDPI(m_pTTF, ptsize, dpi, dpi);

		// 创建备用字体（遍历所有字体，跳过主字体使用的那个文件）
		for (int i = 0; i < nFontCount; ++i) {
			if (i == mainFontIndex) continue;
			tagFontFile* spareTtf = static_cast<tagFontFile*>(CPaintManagerUI::m_aFontFiles.GetAt(i));
			if (!spareTtf) continue;

			CDuiStringUtf8 utf8SparePath(spareTtf->sPathName);
			TTF_Font* pSpareFont = TTF_OpenFont(utf8SparePath.toString(), ptsize);
			if (pSpareFont) 
			{
				if (TTF_AddFallbackFont(m_pTTF, pSpareFont)) 
				{
					m_vSpareFonts.push_back(pSpareFont); // 存储，以备未来释放
				}
				else 
				{
					TTF_CloseFont(pSpareFont); // 失败立即释放
				}
			}
		}
		return uiTrue;
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

	uiBool UIPen_SDL::CreatePen(int nStyle, int nWidth, CDuiColor dwColor)
	{
		return uiFalse;
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

	uiBool UIBrush_SDL::CreateSolidBrush(CDuiColor clr)
	{
        return uiFalse;
	}

	uiBool UIBrush_SDL::CreateBitmapBrush(UIBitmap *bitmap)
	{
        return uiFalse;
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

	uiBool UIPath_SDL::Beginpath()
	{
		return uiTrue;
	}

	uiBool UIPath_SDL::EndPath()
	{
		return uiTrue;
	}

	uiBool UIPath_SDL::AbortPath()
	{
		return uiTrue;
	}

	uiBool UIPath_SDL::AddLine(int x1, int y1, int x2, int y2)
	{
		return uiTrue;
	}

	uiBool UIPath_SDL::AddLines(CDuiPoint *points, int count)
	{
		return uiTrue;
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


	uiBool UIBitmap_SDL::CreateFromData(LPBYTE pImage, int width, int height, CDuiColor mask)
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

	uiBool UIBitmap_SDL::IsAlpha()
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

	void UIBitmap_SDL::ClearAlpha(const CDuiRect &rc, int alpha)
	{
		DeleteAllTextures();
	}

	uiBool UIBitmap_SDL::SaveFile(LPCTSTR pstrFileName)
	{
		return uiFalse;
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



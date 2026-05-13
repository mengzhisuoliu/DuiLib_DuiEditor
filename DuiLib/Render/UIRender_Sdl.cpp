#include "StdAfx.h"

#include "../Render/UIRender_Sdl.h"

#ifdef DUILIB_SDL
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	static SDL_Color GetSDLColor(DWORD color)
	{
		SDL_Color c;
		c.b = GetRValue(color);
		c.g = GetGValue(color);
		c.r = GetBValue(color);
		c.a = (color >> 24) & 0xFF;
		return c;
	}

	UIRender_Sdl::UIRender_Sdl()
	{
		m_pManager	= NULL;
		m_pRenderer = NULL;
		m_pTexture = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
	}

	UIRender_Sdl::~UIRender_Sdl()
	{
		RestoreDefaultObject();
		if (m_pTexture) SDL_DestroyTexture(m_pTexture);
		if (m_pRenderer != NULL) { SDL_DestroyRenderer(m_pRenderer); m_pRenderer = NULL; }
	}

	void UIRender_Sdl::Init(CPaintManagerUI* pManager, PVOID pParam)
	{
		if (m_pRenderer != NULL) { SDL_DestroyRenderer(m_pRenderer); m_pRenderer = NULL; }
		SDL_Window* pWindow = (SDL_Window*)pParam;
		m_pManager = pManager;
		//m_pRenderer = SDL_CreateRenderer(pWindow, "opengl");
		//m_pRenderer = SDL_CreateRenderer(pWindow, "direct3d11");
		m_pRenderer = SDL_CreateRenderer(pWindow, NULL); //windows默认使用direct3d11

		CDuiRect rc;
		pManager->GetClientRect(rc);
		Resize(rc);
	}

	void UIRender_Sdl::BeginPaint()
	{
		if(m_pRenderer && m_pTexture)
			SDL_SetRenderTarget(m_pRenderer, m_pTexture);
	}

	void UIRender_Sdl::EndPaint()
	{
		if (m_pRenderer)
		{
			SDL_SetRenderTarget(m_pRenderer, NULL);
			if (m_pTexture)
				SDL_RenderTexture(m_pRenderer, m_pTexture, NULL, NULL);
			SDL_RenderPresent(m_pRenderer);
		}
	}

	bool UIRender_Sdl::CloneFrom(UIRender *pSrcRender)
	{
		return false;
	}

	bool UIRender_Sdl::Resize(int width, int height)
	{
		if (m_nWidth == width && m_nHeight == height) return false;
		m_nWidth = width;
		m_nHeight = height;
		if (m_pTexture) SDL_DestroyTexture(m_pTexture);
		m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
		SDL_SetRenderTarget(m_pRenderer, m_pTexture);
		return true;
	}

	bool UIRender_Sdl::Resize(const RECT &rc)
	{
		return Resize(rc.right - rc.left, rc.bottom - rc.top);
	}

	UIBitmap *UIRender_Sdl::GetBitmap()
	{
		return NULL;
	}

	int UIRender_Sdl::GetWidth() const
	{ 
		return m_nWidth;
	}

	int UIRender_Sdl::GetHeight() const
	{
		return m_nHeight;
	}

	void UIRender_Sdl::Clear()
	{
		SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(m_pRenderer);
	}

	void UIRender_Sdl::ClearAlpha(const RECT &rc, int alpha)
	{
		// 设置混合模式
		SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, alpha);
		SDL_FRect rect = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };
		SDL_RenderFillRect(m_pRenderer, &rect);
	}

	void UIRender_Sdl::SaveDC()
	{
		
	}

	void UIRender_Sdl::RestoreDC()
	{
		
	}

	CStdRefPtr<UIObject> UIRender_Sdl::SelectObject(UIObject *pObject)
	{	
		emUIOBJTYPE eType = pObject->ObjectType();
		if(eType == OT_FONT)
		{
			
		}
		else if(eType == OT_PEN)
		{
			
		}
		else if(eType == OT_BRUSH)
		{
			
		}
		else if(eType == OT_BITMAP)
		{
			
		}

		return CStdRefPtr<UIObject>();
	}

	void UIRender_Sdl::RestoreObject(UIObject *pObject)
	{
		
	}

	void UIRender_Sdl::RestoreDefaultObject()
	{
		
	}

	DWORD UIRender_Sdl::SetPixel(int x, int y, DWORD dwColor)
	{
		// 简单实现
		Uint8 r = GetRValue(dwColor), g = GetGValue(dwColor), b = GetBValue(dwColor), a = (dwColor >> 24) & 0xFF;
		SDL_SetRenderDrawColor(m_pRenderer, r, g, b, a);
		SDL_RenderPoint(m_pRenderer, x, y);
		return 0;
	}

	BOOL UIRender_Sdl::BitBlt(int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, DWORD dwRop)
	{
		return AlphaBlend(x, y, nWidth, nHeight, pSrcRender, xSrc, ySrc, nWidth, nHeight, 255);
	}

	BOOL UIRender_Sdl::StretchBlt(int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, DWORD dwRop)
	{
		return AlphaBlend(x, y, nWidth, nHeight, pSrcRender, xSrc, ySrc, nWidth, nHeight, 255);
	}

	BOOL UIRender_Sdl::AlphaBlend(int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha )
	{
		return TRUE;
	}

	void UIRender_Sdl::DrawBitmapAlpha(int x, int y, int nWidth, int nHeight, UIBitmap *pUiBitmap, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha)
	{
		SDL_Texture* texture = SDL_CreateTextureFromSurface(m_pRenderer, (SDL_Surface *)pUiBitmap->GetHandle());
		if (!texture) return;

		//设置纹理混合模式为 Alpha 混合
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

		// 设置整体透明度调制（alpha 参数，0=完全透明，255=完全不透明）
		SDL_SetTextureAlphaMod(texture, (Uint8)alpha);

		SDL_FRect dstRect = { x, y, nWidth, nHeight };
		SDL_FRect srcRect = { xSrc, ySrc, nWidthSrc, nHeightSrc };
		SDL_RenderTexture(m_pRenderer, texture, &srcRect, &dstRect);

		SDL_DestroyTexture(texture);
	}

	void UIRender_Sdl::DrawColor(const RECT& rc, const SIZE &round, DWORD dwColor)
	{
		if (dwColor <= 0x00FFFFFF) return;

		bool bDraw = false;
		Uint8 r = GetRValue(dwColor), g = GetGValue(dwColor), b = GetBValue(dwColor), a = (dwColor >> 24) & 0xFF;
		SDL_SetRenderDrawColor(m_pRenderer, b, g, r, a);
		SDL_FRect rect = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };
		if (round.cx > 0 || round.cy > 0)
		{
			// 圆角矩形需要更复杂处理，这里简化为填充
			bDraw = SDL_RenderFillRect(m_pRenderer, &rect);
		}
		else
		{
			bDraw = SDL_RenderFillRect(m_pRenderer, &rect);
		}
		const char *pErr = SDL_GetError();
	}

	void UIRender_Sdl::DrawGradient(const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps)
	{
		
	}

	void UIRender_Sdl::DrawLine(int x1, int y1, int x2, int y2, int nSize, DWORD dwPenColor,int nStyle)
	{
		SDL_Color color = GetSDLColor(dwPenColor);
		SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
		thickLineRGBA(m_pRenderer, x1, y1, x2, y2, nSize, color.r, color.g, color.b, color.a);
	}

	void UIRender_Sdl::DrawRect(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/)
	{
		if (nSize <= 0) return;
		SDL_Color color = GetSDLColor(dwPenColor);
		int x = rc.left, y = rc.top, w = rc.right - rc.left, h = rc.bottom - rc.top;
		// 使用 rectangleRGBA 绘制矩形边框，线宽通过循环 offset 实现
		for (int i = 0; i < nSize; ++i) {
			rectangleRGBA(m_pRenderer, x + i, y + i, x + w - i, y + h - i, color.r, color.g, color.b, color.a);
		}
	}

	void UIRender_Sdl::DrawRoundRect(const RECT& rc, int nSize, const SIZE& round, DWORD dwPenColor, int nStyle /*= PS_SOLID*/)
	{
		if (nSize <= 0) return;
		SDL_Color color = GetSDLColor(dwPenColor);
		int x = rc.left, y = rc.top, w = rc.right - rc.left, h = rc.bottom - rc.top;
		int rx = round.cx, ry = round.cy;
		for (int i = 0; i < nSize; ++i) {
			roundedRectangleRGBA(m_pRenderer, x + i, y + i, x + w - i, y + h - i, rx - i, color.r, color.g, color.b, color.a);
		}
	}

	void UIRender_Sdl::DrawEllipse(const RECT& rc, int nSize, DWORD dwPenColor, int nStyle)
	{
		if (nSize <= 0) return;
		SDL_Color color = GetSDLColor(dwPenColor);
		int cx = (rc.left + rc.right) / 2;
		int cy = (rc.top + rc.bottom) / 2;
		int rx = (rc.right - rc.left) / 2;
		int ry = (rc.bottom - rc.top) / 2;
		for (int i = 0; i < nSize; ++i) 
		{
			ellipseRGBA(m_pRenderer, cx, cy, rx - i, ry - i, color.r, color.g, color.b, color.a);
		}
	}

	void UIRender_Sdl::FillEllipse(const RECT& rc, DWORD dwColor)
	{
		SDL_Color color = GetSDLColor(dwColor);
		int cx = (rc.left + rc.right) / 2;
		int cy = (rc.top + rc.bottom) / 2;
		int rx = (rc.right - rc.left) / 2;
		int ry = (rc.bottom - rc.top) / 2;
		filledEllipseRGBA(m_pRenderer, cx, cy, rx, ry, color.r, color.g, color.b, color.a);
	}

	void UIRender_Sdl::DrawText(RECT& rc, LPCTSTR pstrText, DWORD dwColor, int iFont, UINT uStyle)
	{
		CDuiStringUtf8 utf8Text(pstrText);

		CPaintManagerUI* pMgr = GetManager();
		UIFont* pFont = pMgr->GetFont(iFont);
		if (!pFont) return;
		TTF_Font* ttfFont = (TTF_Font*)pFont->GetHandle();
		if (!ttfFont) return;
		SDL_Color color = GetSDLColor(dwColor);
		SDL_Surface* surface = TTF_RenderText_Blended(ttfFont, utf8Text.toString(), utf8Text.GetLength(), color);
		if (surface)
		{
			SDL_Texture* texture = SDL_CreateTextureFromSurface(m_pRenderer, surface);
			SDL_FRect dstRect = { rc.left, rc.top, surface->w, surface->h };
			if (uStyle & DT_CENTER)
				dstRect.x = rc.left + (rc.right - rc.left - surface->w) / 2;
			if (uStyle & DT_VCENTER)
				dstRect.y = rc.top + (rc.bottom - rc.top - surface->h) / 2;
			SDL_RenderTexture(m_pRenderer, texture, NULL, &dstRect);
			SDL_DestroyTexture(texture);
			SDL_DestroySurface(surface);
		}
	}

	UIPath* UIRender_Sdl::CreatePath()
	{
		return new UIPath_SDL();
	}

	BOOL UIRender_Sdl::DrawPath(const UIPath* path, int nSize, DWORD dwColor)
	{
		return FALSE;
	}

	BOOL UIRender_Sdl::FillPath(const UIPath* path, const DWORD dwColor)
	{
		return FALSE;
	}

	SIZE UIRender_Sdl::GetTextSize(LPCTSTR pstrText, int iFont, UINT uStyle )
	{	
		SIZE sz = { 0 };
		if (!m_pManager) return sz;
		TTF_Font* ttfFont = (TTF_Font*)GetManager()->GetFont(iFont)->GetHandle();
		if (!ttfFont) return sz;
		CDuiStringUtf8 sUtf8 = CDuiString(pstrText);
		int w = 0, h = 0;
		TTF_GetStringSize(ttfFont, sUtf8.toString(), sUtf8.GetLength(), &w, &h);
		sz.cx = w; sz.cy = h;
		return sz;
	}

} // namespace DuiLib
#endif //#ifdef DUILIB_USE_RENDER_Sdl

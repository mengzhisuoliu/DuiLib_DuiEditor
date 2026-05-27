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
		m_bWindowRender = true;
		m_pRenderer = NULL;
		m_pTexture = NULL;
		m_ttfEngine = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
	}

	UIRender_Sdl::~UIRender_Sdl()
	{
		if (m_pTexture) SDL_DestroyTexture(m_pTexture);
		DestroyRender();
	}

	void UIRender_Sdl::DestroyRender()
	{
		//注意清理图片纹理缓存
		if (m_pManager)
		{
			CPaintManagerSDLUI *pManager = dynamic_cast<CPaintManagerSDLUI*>(GetManager());
			if (pManager)
			{
				pManager->ClearImageTexture(this);
			}
		}
		if (m_ttfEngine) { TTF_DestroyRendererTextEngine(m_ttfEngine); m_ttfEngine = NULL; }
		if (m_pRenderer != NULL) { SDL_DestroyRenderer(m_pRenderer); m_pRenderer = NULL; }
	}

	void UIRender_Sdl::Init(CPaintManagerUI* pManager, PVOID pParam)
	{
		if (m_pTexture) SDL_DestroyTexture(m_pTexture);
		DestroyRender();

		m_pManager = pManager;

		SDL_Window* pWindow = (SDL_Window*)pParam;
		if (pWindow)
		{
			m_bWindowRender = true;
			//m_pRenderer = SDL_CreateRenderer(pWindow, "opengl");
			//m_pRenderer = SDL_CreateRenderer(pWindow, "direct3d11");
			m_pRenderer = SDL_CreateRenderer(pWindow, NULL); //windows默认使用direct3d11
		}
		else
		{
			m_bWindowRender = false;

			m_curBmp = MakeRefPtr<UIBitmap>(UIGlobal::CreateBitmap());
			m_curBmp->CreateFromData(NULL, 1, 1, 0);

			//无窗口渲染
			SDL_PropertiesID props = SDL_CreateProperties();
			if (props == 0) return;
			SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_SURFACE_POINTER, (SDL_Surface *)m_curBmp->GetHandle());
			SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, "software");
			m_pRenderer = SDL_CreateRendererWithProperties(props);
			SDL_DestroyProperties(props);
		}

		SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);// 设置混合模式	

		SDL_PropertiesID props2 = SDL_CreateProperties();
		SDL_SetPointerProperty(props2, TTF_PROP_RENDERER_TEXT_ENGINE_RENDERER_POINTER, m_pRenderer);
		// 设置图集纹理大小为2048，默认是1024。更大的尺寸可以减少纹理切换，但会增加显存占用。
		SDL_SetNumberProperty(props2, TTF_PROP_RENDERER_TEXT_ENGINE_ATLAS_TEXTURE_SIZE_NUMBER, 2048);
		//m_ttfEngine = TTF_CreateRendererTextEngine(m_pRenderer);
		m_ttfEngine = TTF_CreateRendererTextEngineWithProperties(props2);
		SDL_DestroyProperties(props2);

		if (pManager)
		{
			CDuiRect rc;
			pManager->GetClientRect(rc);
			if(!rc.IsEmpty()) Resize(rc);
		}
	}

	HANDLE UIRender_Sdl::GetHandle()
	{
		return m_pRenderer;
	}

	void UIRender_Sdl::BeginPaint()
	{
		DUITRACE(_T("BeginPaint: %s"), m_rcInvalidate.ToString());

		if(m_bWindowRender)
			SDL_SetRenderTarget(m_pRenderer, m_pTexture);

		if (!m_rcInvalidate.IsEmpty())
		{
			//ClearAlpha(m_rcInvalidate);
			SDL_Rect clipRect = { m_rcInvalidate.left, m_rcInvalidate.top,
								m_rcInvalidate.right, m_rcInvalidate.bottom };
			SDL_SetRenderClipRect(m_pRenderer, &clipRect);
		}
		else
		{
			SDL_SetRenderClipRect(m_pRenderer, NULL);
		}
	}

	void UIRender_Sdl::EndPaint()
	{
		DUITRACE(_T("EndPaint"));
		if (!m_pRenderer) return;

		if (m_bWindowRender)
		{
			SDL_SetRenderTarget(m_pRenderer, NULL);
			if (m_pTexture)
				SDL_RenderTexture(m_pRenderer, m_pTexture, NULL, NULL);
			SDL_RenderPresent(m_pRenderer);
		}

		SDL_SetRenderClipRect(m_pRenderer, NULL);
		m_rcInvalidate.Empty();
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
		if (m_bWindowRender)
		{
			if (m_pTexture) SDL_DestroyTexture(m_pTexture);
			m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
			SDL_SetRenderTarget(m_pRenderer, m_pTexture);
		}
		else
		{
			//无窗口模式，需要重新创建render
			DestroyRender();
			m_curBmp = MakeRefPtr<UIBitmap>(UIGlobal::CreateBitmap());
			m_curBmp->CreateFromData(NULL, width, height, 0);
			SDL_PropertiesID props = SDL_CreateProperties();
			if (props == 0) return false;
			SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_SURFACE_POINTER, (SDL_Surface*)m_curBmp->GetHandle());
			SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, "software");
			m_pRenderer = SDL_CreateRendererWithProperties(props);
			SDL_DestroyProperties(props);
		}
		return true;
	}

	bool UIRender_Sdl::Resize(const RECT &rc)
	{
		return Resize(rc.right - rc.left, rc.bottom - rc.top);
	}

	UIBitmap *UIRender_Sdl::GetBitmap()
	{
		if (m_bWindowRender)
		{
			return NULL;
		}
		return *(UIBitmap**)&m_curBmp;
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
		SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, alpha);
		SDL_FRect rect = { (float)rc.left, (float)rc.top, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top) };
		SDL_RenderFillRect(m_pRenderer, &rect);
	}

	void UIRender_Sdl::InvalidRect(const RECT* lpRect)
	{
		if (!lpRect)
		{
			m_rcInvalidate.Empty();
			return;
		}
		if (m_rcInvalidate.IsEmpty())
			m_rcInvalidate = *lpRect;
		else
			m_rcInvalidate.Join(*lpRect);
		DUITRACE(_T("InvalidRect: %s"), m_rcInvalidate.ToString());
	}

	CDuiRect UIRender_Sdl::GetInvalidRect()
	{
		return m_rcInvalidate;
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
		return StretchBlt(x, y, nWidth, nHeight, pSrcRender, xSrc, ySrc, nWidth, nHeight);
	}

	BOOL UIRender_Sdl::StretchBlt(int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, DWORD dwRop)
	{
		UIRender_Sdl* pSrc = dynamic_cast<UIRender_Sdl*>(pSrcRender);
		if (!pSrc) return FALSE;

		if (!m_pRenderer || !pSrc->m_pRenderer)
			return FALSE;

		SDL_Rect rc = { xSrc, ySrc, nWidthSrc, nHeightSrc };
		SDL_Surface* pSrcSurface = SDL_RenderReadPixels(pSrc->m_pRenderer, &rc);
		if (!pSrcSurface) return FALSE;
		if (nWidthSrc > pSrcSurface->w) rc.w = pSrcSurface->w;
		if (nHeightSrc > pSrcSurface->h) rc.h = pSrcSurface->h;

		//源区域矩形
		SDL_FRect srcRect = { (float)rc.x, (float)rc.y, (float)rc.w, (float)rc.h };
		// 目标区域矩形
		SDL_FRect dstRect = { (float)x, (float)y, (float)nWidth, (float)nHeight };

		SDL_Texture* texture = SDL_CreateTextureFromSurface(m_pRenderer, pSrcSurface);
		if (texture)
		{
			SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST); //纹理缩放模式为最近邻（像素风格）
			SDL_RenderTexture(m_pRenderer, texture, &srcRect, &dstRect);
			SDL_DestroyTexture(texture);
		}

		SDL_DestroySurface(pSrcSurface);
		return TRUE;
	}

	BOOL UIRender_Sdl::AlphaBlend(int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha )
	{
		UIRender_Sdl* pSrc = dynamic_cast<UIRender_Sdl*>(pSrcRender);
		if (!pSrc || !m_pRenderer || !pSrc->m_pRenderer) return FALSE;

		// 源矩形（注意边界修正）
		SDL_Rect srcRect = { xSrc, ySrc, nWidthSrc, nHeightSrc };
		// 从源渲染器读取像素（强制 ARGB8888 格式，保留 Alpha 通道）
		SDL_Surface* srcSurface = SDL_RenderReadPixels(pSrc->m_pRenderer, &srcRect);
		if (!srcSurface) return FALSE;

		// 若实际读取的尺寸与请求不同，调整源区域宽度/高度（理论上不会发生，但确保安全）
		if (nWidthSrc > srcSurface->w) srcRect.w = srcSurface->w;
		if (nHeightSrc > srcSurface->h) srcRect.h = srcSurface->h;

		// 源纹理矩形（浮点）
		SDL_FRect srcTexRect = { (float)srcRect.x, (float)srcRect.y,
								 (float)srcRect.w, (float)srcRect.h };
		// 目标矩形（浮点，支持缩放）
		SDL_FRect dstRect = { (float)x, (float)y, (float)nWidth, (float)nHeight };

		// 创建纹理
		SDL_Texture* texture = SDL_CreateTextureFromSurface(m_pRenderer, srcSurface);
		BOOL result = FALSE;
		if (texture)
		{
			// 设置混合模式（启用 Alpha 混合）
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			// 设置全局透明度调制
			SDL_SetTextureAlphaMod(texture, (Uint8)alpha);
			// 绘制（自动缩放）
			result = SDL_RenderTexture(m_pRenderer, texture, &srcTexRect, &dstRect);
			SDL_DestroyTexture(texture);
		}

		SDL_DestroySurface(srcSurface);
		return result;
	}

	void UIRender_Sdl::DrawBitmapAlpha(int x, int y, int nWidth, int nHeight, UIBitmap *pUiBitmap, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha)
	{
		UIBitmap_SDL* pBitmap = dynamic_cast<UIBitmap_SDL*>(pUiBitmap);
		if (!pBitmap) return;

		SDL_Texture* texture = pBitmap->GetTexture(m_pRenderer);
		if (!texture) return;

		// 设置整体透明度调制（alpha 参数，0=完全透明，255=完全不透明）
		SDL_SetTextureAlphaMod(texture, (Uint8)alpha);

		SDL_FRect dstRect = { (float)x, (float)y, (float)nWidth, (float)nHeight };
		SDL_FRect srcRect = { (float)xSrc, (float)ySrc, (float)nWidthSrc, (float)nHeightSrc };
		SDL_RenderTexture(m_pRenderer, texture, &srcRect, &dstRect);
	}

	void UIRender_Sdl::DrawColor(const RECT& rc, const SIZE &round, DWORD dwColor)
	{
		if (dwColor <= 0x00FFFFFF) return;

		bool bDraw = false;
		Uint8 r = GetRValue(dwColor), g = GetGValue(dwColor), b = GetBValue(dwColor), a = (dwColor >> 24) & 0xFF;
		SDL_SetRenderDrawColor(m_pRenderer, b, g, r, a);
		SDL_FRect rect = { (float)rc.left, (float)rc.top, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top) };
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
		// 将 ARGB 颜色转换为 SDL_FColor（浮点 0.0-1.0 范围）
		SDL_FColor c1;
		c1.r = GetBValue(dwFirst) / 255.0f;
		c1.g = GetGValue(dwFirst) / 255.0f;
		c1.b = GetRValue(dwFirst) / 255.0f;
		c1.a = ((dwFirst >> 24) & 0xFF) / 255.0f;

		SDL_FColor c2;
		c2.r = GetBValue(dwSecond) / 255.0f;
		c2.g = GetGValue(dwSecond) / 255.0f;
		c2.b = GetRValue(dwSecond) / 255.0f;
		c2.a = ((dwSecond >> 24) & 0xFF) / 255.0f;

		// 定义四个顶点
		SDL_Vertex vertices[4];
		// 位置
		vertices[0].position.x = (float)rc.left;
		vertices[0].position.y = (float)rc.top;
		vertices[1].position.x = (float)rc.right;
		vertices[1].position.y = (float)rc.top;
		vertices[2].position.x = (float)rc.right;
		vertices[2].position.y = (float)rc.bottom;
		vertices[3].position.x = (float)rc.left;
		vertices[3].position.y = (float)rc.bottom;

		// 颜色（根据渐变方向设置）
		if (bVertical) {
			vertices[0].color = c1;
			vertices[1].color = c1;
			vertices[2].color = c2;
			vertices[3].color = c2;
		}
		else {
			vertices[0].color = c1;
			vertices[1].color = c2;
			vertices[2].color = c2;
			vertices[3].color = c1;
		}

		// 索引（两个三角形构成矩形）
		int indices[6] = { 0, 1, 2, 0, 2, 3 };
		SDL_RenderGeometry(m_pRenderer, NULL, vertices, 4, indices, 6);
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

		UIFont* pFont = GetManager()->GetFont(iFont);
		if (!pFont) return;
		TTF_Font* ttfFont = (TTF_Font*)pFont->GetHandle();
		if (!ttfFont) return;

		if (uStyle & DT_CALCRECT)
		{
			int w = 0, h = 0;
			TTF_GetStringSize(ttfFont, utf8Text.toString(), utf8Text.GetLength(), &w, &h);
			rc.right = rc.left + w;
			rc.bottom = rc.top + h;
			return;
		}

		// 创建临时 TTF_Text 对象
		TTF_Text* ttfText = TTF_CreateText(m_ttfEngine, ttfFont, utf8Text.toString(), utf8Text.GetLength());

		// 设置颜色
		SDL_Color color = GetSDLColor(dwColor);
		TTF_SetTextColor(ttfText, color.r, color.g, color.b, color.a);

		// 对齐方式
		int x = rc.left;
		int y = rc.top;
		int texW = 0, texH = 0;
		TTF_GetTextSize(ttfText, &texW, &texH);
		if (uStyle & DT_CENTER) 
		{
			x = rc.left + (rc.right - rc.left - texW) / 2;
		}
		else if (uStyle & DT_RIGHT) 
		{
			x = rc.right - texW;
		}
		if (uStyle & DT_VCENTER) 
		{
			y = rc.top + (rc.bottom - rc.top - texH) / 2;
		}
		else if (uStyle & DT_BOTTOM) 
		{
			y = rc.bottom - texH;
		}

		// 设置位置, 额外的偏移，不要设置
		//TTF_SetTextPosition(ttfText, x, y);

		// 绘制
		TTF_DrawRendererText(ttfText, (float)x, (float)y);

		// 销毁临时对象（引擎内部缓存字形和图集，不会丢失）
		TTF_DestroyText(ttfText);
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

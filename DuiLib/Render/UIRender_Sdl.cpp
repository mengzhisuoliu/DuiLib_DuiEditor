#include "StdAfx.h"

#include "../Render/UIRender_Sdl.h"

#ifdef DUILIB_SDL
namespace DuiLib {

#ifndef M_PI
#define M_PI	3.1415926535897932384626433832795
#endif

// 可调精度参数：每段弧的最大弦长（像素），越小越平滑，默认 0.8 可兼顾性能和效果
static const float ARC_PRECISION = 0.8f;

// 辅助函数：添加弧段（基于最大弦长自适应分段）
static void AddArcPointsAdaptive(std::vector<SDL_FPoint>& pts, float cx, float cy, float r,
	float startAng, float endAng) {
	float arcAngle = endAng - startAng;
	if (arcAngle < 0.0f) arcAngle += 2.0f * (float)M_PI;
	if (r < 0.001f) {
		// 半径极小，直接添加起点终点
		pts.push_back({ cx + r * cosf(startAng), cy + r * sinf(startAng) });
		pts.push_back({ cx + r * cosf(endAng), cy + r * sinf(endAng) });
		return;
	}
	// 弧长
	float arcLength = r * arcAngle;
	// 分段数 = ceil(弧长 / 最大步长)
	int steps = (int)ceilf(arcLength / ARC_PRECISION);
	if (steps < 1) steps = 1;
	// 限制最大分段数以防性能问题（例如 200）
	if (steps > 200) steps = 200;

	for (int i = 0; i <= steps; ++i) {
		float t = (float)i / steps;
		float ang = startAng + arcAngle * t;
		pts.push_back({ cx + r * cosf(ang), cy + r * sinf(ang) });
	}
}

// 辅助函数：生成圆角矩形轮廓点（顺时针，起点左上角弧结束处）
static std::vector<SDL_FPoint> GetRoundRectPoints(const CDuiRect& rc, int radius) 
{
	std::vector<SDL_FPoint> pts;
	float left = (float)rc.left, top = (float)rc.top;
	float right = (float)rc.right, bottom = (float)rc.bottom;
	float r = (float)radius;

	// 按顺时针顺序构建
	// 左上角弧：π -> 3π/2
	AddArcPointsAdaptive(pts, left + r, top + r, r, (float)M_PI, (float)(M_PI * 1.5));
	// 顶部直线终点
	pts.push_back({ right - r, top });
	// 右上角弧：3π/2 -> 2π
	AddArcPointsAdaptive(pts, right - r, top + r, r, (float)(M_PI * 1.5), (float)(2 * M_PI));
	// 右侧直线终点
	pts.push_back({ right, bottom - r });
	// 右下角弧：0 -> π/2
	AddArcPointsAdaptive(pts, right - r, bottom - r, r, 0.0f, (float)(M_PI / 2));
	// 底部直线终点
	pts.push_back({ left + r, bottom });
	// 左下角弧：π/2 -> π
	AddArcPointsAdaptive(pts, left + r, bottom - r, r, (float)(M_PI / 2), (float)M_PI);
	// 左侧直线终点（回到起点）
	pts.push_back({ left, top + r });

	return pts;
}

// 辅助函数：生成椭圆点集
static std::vector<SDL_FPoint> GetEllipsePoints(float cx, float cy, float rx, float ry, int segments = 32) 
{
	// 椭圆周长近似：Ramanujan 近似，然后分段
	float a = (rx + ry) / 2.0f;
	float h = ((rx - ry) * (rx - ry)) / ((rx + ry) * (rx + ry));
	float perimeter = (float)M_PI * (rx + ry) * (1.0f + 3.0f * h / (10.0f + sqrtf(4.0f - 3.0f * h)));
	int steps = (int)ceilf(perimeter / ARC_PRECISION);
	if (steps < 16) steps = 16;
	if (steps > 200) steps = 200;

	std::vector<SDL_FPoint> pts(steps + 1);
	for (int i = 0; i <= steps; ++i) {
		float theta = (float)i / steps * 2.0f * (float)M_PI;
		pts[i] = { cx + rx * cosf(theta), cy + ry * sinf(theta) };
	}
	return pts;
}

// 辅助函数：用三角扇绘制填充多边形（中心点+轮廓）
static void DrawFilledPolygon(SDL_Renderer* renderer, const std::vector<SDL_FPoint>& outline, CDuiColor color) {
	if (outline.size() < 3) return;
	// 中心点取所有顶点的平均（或使用第一个点，但最好用中心）
	float cx = 0, cy = 0;
	for (auto& p : outline) { cx += p.x; cy += p.y; }
	cx /= outline.size(); cy /= outline.size();

	std::vector<SDL_Vertex> vertices;
	vertices.reserve(outline.size() + 1);
	SDL_Vertex center;
	center.position = { cx, cy };
	center.color = color;
	center.tex_coord = { 0, 0 };
	vertices.push_back(center);
	for (auto& p : outline) {
		SDL_Vertex v;
		v.position = p;
		v.color = color;
		v.tex_coord = { 0, 0 };
		vertices.push_back(v);
	}

	std::vector<int> indices;
	indices.reserve((outline.size()) * 3);
	for (int i = 1; i < (int)vertices.size() - 1; ++i) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);
	}
	// 闭合
	if (vertices.size() > 2) {
		indices.push_back(0);
		indices.push_back((int)vertices.size() - 1);
		indices.push_back(1);
	}
	SDL_RenderGeometry(renderer, nullptr, vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size());
}

// 线段对齐方式
enum ThickLineAlign {
	ALIGN_CENTER,          // 线宽居中（默认）
	ALIGN_POSITIVE_NORMAL, // 线宽全部在法线正方向（左侧/上侧）
	ALIGN_NEGATIVE_NORMAL  // 线宽全部在法线负方向（右侧/下侧）
};

// 辅助函数：绘制粗线段（可控制扩展方向）
static void DrawThickLine(SDL_Renderer* renderer, float x1, float y1, float x2, float y2,
	float thickness, CDuiColor color, ThickLineAlign align = ALIGN_CENTER) 
{
	if (thickness <= 0) return;
	float dx = x2 - x1, dy = y2 - y1;
	float len = sqrtf(dx * dx + dy * dy);
	if (len < 0.001f) return;
	float nx = -dy / len; // 法线单位向量（指向左侧/上侧）
	float ny = dx / len;

	// 计算两侧偏移量
	float half = thickness / 2.0f;
	float offsetPos, offsetNeg; // 正法线方向偏移量，负法线方向偏移量
	switch (align) {
		case ALIGN_CENTER:
			offsetPos = half;
			offsetNeg = half;
			break;
		case ALIGN_POSITIVE_NORMAL:
			offsetPos = thickness;
			offsetNeg = 0.0f;
			break;
		case ALIGN_NEGATIVE_NORMAL:
			offsetPos = 0.0f;
			offsetNeg = thickness;
			break;
		default:
			offsetPos = half;
			offsetNeg = half;
			break;
	}

	SDL_Vertex verts[4];
	// 四个顶点：起点和终点分别沿法线正负方向偏移
	verts[0].position = { x1 + nx * offsetPos, y1 + ny * offsetPos };
	verts[1].position = { x1 - nx * offsetNeg, y1 - ny * offsetNeg };
	verts[2].position = { x2 - nx * offsetNeg, y2 - ny * offsetNeg };
	verts[3].position = { x2 + nx * offsetPos, y2 + ny * offsetPos };
	for (int i = 0; i < 4; ++i) {
		verts[i].color = color;
		verts[i].tex_coord = { 0, 0 };
	}
	int indices[6] = { 0, 1, 2, 0, 2, 3 };
	SDL_RenderGeometry(renderer, nullptr, verts, 4, indices, 6);
}

// 辅助函数：绘制虚线（支持不同样式）
static void DrawDashedLine(SDL_Renderer* renderer,
	float x1, float y1, float x2, float y2,
	float thickness, CDuiColor color, int style)
{
	static const int dash[] = { 8, 4 };
	static const int dot[] = { 2, 2 };
	static const int dashdot[] = { 8, 4, 2, 4 };
	static const int dashdotdot[] = { 8, 4, 2, 4, 2, 4 };

	// 定义虚线模式（实心长度, 空白长度）交替，单位像素
	const int* pattern = nullptr;
	int patternLen = 0;
	switch (style & PS_STYLE_MASK) {
		case PS_DASH:
			pattern = dash; patternLen = 2;
			break;
		case PS_DOT:
			pattern = dot; patternLen = 2;
			break;
		case PS_DASHDOT:
			pattern = dashdot; patternLen = 4;
			break;
		case PS_DASHDOTDOT:
			pattern = dashdotdot; patternLen = 6;
			break;
		default:
			return; // 未知样式，不绘制
	}

	float dx = x2 - x1, dy = y2 - y1;
	float len = sqrtf(dx * dx + dy * dy);
	if (len < 0.001f) return;
	float ux = dx / len, uy = dy / len; // 单位方向向量

	float totalLen = 0.0f;
	int patternIndex = 0;
	bool isSolid = true; // 第一段为实心

	while (totalLen < len) {
		// 当前段的长度（实心或空白）
		float segLen = (float)pattern[patternIndex];
		// 如果剩余长度不足，截断
		if (totalLen + segLen > len)
			segLen = len - totalLen;

		if (isSolid && segLen > 0.001f) {
			// 计算该实心段的起点和终点
			float sx = x1 + ux * totalLen;
			float sy = y1 + uy * totalLen;
			float ex = sx + ux * segLen;
			float ey = sy + uy * segLen;
			// 绘制该段
			if (thickness <= 1.0f) {
				// 细线直接用 SDL_RenderLine
				SDL_RenderLine(renderer, sx, sy, ex, ey);
			}
			else {
				// 粗线用 DrawThickLine
				DrawThickLine(renderer, sx, sy, ex, ey, thickness, color, ALIGN_CENTER);
			}
		}

		// 前进并切换状态
		totalLen += segLen;
		patternIndex = (patternIndex + 1) % patternLen;
		isSolid = !isSolid; // 实心/空白交替
	}
}
/////////////////////////////////////////////////////////////////////////////////////
//
//
UIRender_Sdl::UIRender_Sdl()
{
	m_pManager = NULL;
	m_bWindowRender = true;
	m_pRenderer = NULL;
	m_pTexture = NULL;
	m_ttfEngine = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
}

UIRender_Sdl::~UIRender_Sdl()
{
	DestroyRender();
}

void UIRender_Sdl::DestroyRender()
{
	//注意清理图片纹理缓存
	if (m_pManager)
	{
		CPaintManagerSDLUI* pManager = dynamic_cast<CPaintManagerSDLUI*>(GetManager());
		if (pManager)
		{
			pManager->ClearImageTexture(this);
		}
	}
	if (m_pTexture) { SDL_DestroyTexture(m_pTexture); m_pTexture = NULL; }
	if (m_ttfEngine) { TTF_DestroyRendererTextEngine(m_ttfEngine); m_ttfEngine = NULL; }
	if (m_pRenderer != NULL) { SDL_DestroyRenderer(m_pRenderer); m_pRenderer = NULL; }
}

void UIRender_Sdl::Init(CPaintManagerUI* pManager, PVOID pParam)
{
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
		m_curBmp->CreateFromData(NULL, 1, 1, CDuiColor());

		//无窗口渲染
		SDL_PropertiesID props = SDL_CreateProperties();
		if (props == 0) return;
		SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_SURFACE_POINTER, (SDL_Surface*)m_curBmp->GetHandle());
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
		if (!rc.IsEmpty()) Resize(rc);
	}
}

HANDLE UIRender_Sdl::GetHandle()
{
	return m_pRenderer;
}

void UIRender_Sdl::BeginPaint()
{
	//DUITRACE(_T("BeginPaint: %s"), m_rcInvalidate.ToString());

	if (m_bWindowRender)
		SDL_SetRenderTarget(m_pRenderer, m_pTexture);

	if (!m_rcInvalidate.IsEmpty())
	{
		//ClearAlpha(m_rcInvalidate);
		SDL_Rect clipRect = m_rcInvalidate.ToSDL_Rect();
		SDL_SetRenderClipRect(m_pRenderer, &clipRect);
	}
	else
	{
		SDL_SetRenderClipRect(m_pRenderer, NULL);
	}
}

void UIRender_Sdl::EndPaint()
{
	//DUITRACE(_T("EndPaint"));
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

bool UIRender_Sdl::CloneFrom(UIRender* pSrcRender)
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
		m_curBmp->CreateFromData(NULL, width, height, CDuiColor());
		SDL_PropertiesID props = SDL_CreateProperties();
		if (props == 0) return false;
		SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_SURFACE_POINTER, (SDL_Surface*)m_curBmp->GetHandle());
		SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, "software");
		m_pRenderer = SDL_CreateRendererWithProperties(props);
		SDL_DestroyProperties(props);
	}
	return true;
}

bool UIRender_Sdl::Resize(const CDuiRect& rc)
{
	return Resize(rc.right - rc.left, rc.bottom - rc.top);
}

UIBitmap* UIRender_Sdl::GetBitmap()
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
	SetDrawColor(CDuiColor(0, 0, 0, 255));
	SDL_RenderClear(m_pRenderer);
}

void UIRender_Sdl::ClearAlpha(const CDuiRect& rc, int alpha)
{
	SetDrawColor(CDuiColor(0, 0, 0, alpha));
	SDL_FRect rect = { (float)rc.left, (float)rc.top, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top) };
	SDL_RenderFillRect(m_pRenderer, &rect);
}

void UIRender_Sdl::InvalidRect(const CDuiRect* lpRect)
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
	//DUITRACE(_T("InvalidRect: %s"), m_rcInvalidate.ToString());
}

CDuiRect UIRender_Sdl::GetInvalidRect()
{
	return m_rcInvalidate;
}

CDuiColor UIRender_Sdl::SetPixel(int x, int y, CDuiColor clr)
{
	SetDrawColor(clr);
	SDL_RenderPoint(m_pRenderer, (float)x, (float)y);
	return clr;
}

uiBool UIRender_Sdl::BitBlt(int x, int y, int nWidth, int nHeight, UIRender* pSrcRender, int xSrc, int ySrc, DWORD dwRop)
{
	return StretchBlt(x, y, nWidth, nHeight, pSrcRender, xSrc, ySrc, nWidth, nHeight);
}

uiBool UIRender_Sdl::StretchBlt(int x, int y, int nWidth, int nHeight, UIRender* pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, DWORD dwRop)
{
	UIRender_Sdl* pSrc = dynamic_cast<UIRender_Sdl*>(pSrcRender);
	if (!pSrc) return uiFalse;

	if (!m_pRenderer || !pSrc->m_pRenderer)
		return uiFalse;

	SDL_Rect rc = { xSrc, ySrc, nWidthSrc, nHeightSrc };
	SDL_Surface* pSrcSurface = SDL_RenderReadPixels(pSrc->m_pRenderer, &rc);
	if (!pSrcSurface) return uiFalse;
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
	return uiTrue;
}

uiBool UIRender_Sdl::AlphaBlend(int x, int y, int nWidth, int nHeight, UIRender* pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha)
{
	UIRender_Sdl* pSrc = dynamic_cast<UIRender_Sdl*>(pSrcRender);
	if (!pSrc || !m_pRenderer || !pSrc->m_pRenderer) return uiFalse;

	// 源矩形（注意边界修正）
	SDL_Rect srcRect = { xSrc, ySrc, nWidthSrc, nHeightSrc };
	// 从源渲染器读取像素（强制 ARGB8888 格式，保留 Alpha 通道）
	SDL_Surface* srcSurface = SDL_RenderReadPixels(pSrc->m_pRenderer, &srcRect);
	if (!srcSurface) return uiFalse;

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
	uiBool result = uiFalse;
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

void UIRender_Sdl::DrawBitmapAlpha(int x, int y, int nWidth, int nHeight, UIBitmap* pUiBitmap, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha)
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

void UIRender_Sdl::DrawColor(const CDuiRect& rc, const CDuiSize& round, CDuiColor clr)
{
	if (clr.GetColor() <= 0x00FFFFFF) return;

	SetDrawColor(clr);

	if (round.cx == 0 && round.cy == 0) {
		// 普通矩形
		SDL_FRect rect = rc.ToSDL_FRect();
		SDL_RenderFillRect(m_pRenderer, &rect);
		return;
	}

	int radius = (round.cx + round.cy) / 2;
	if (radius <= 0) { // 安全回退
		SDL_FRect rect = rc.ToSDL_FRect();
		SDL_RenderFillRect(m_pRenderer, &rect);
		return;
	}

	int maxR = MIN(rc.GetWidth() / 2, rc.GetHeight() / 2);
	if (radius > maxR) radius = maxR;
	if (radius <= 0) {
		SDL_FRect rect = rc.ToSDL_FRect();
		SDL_RenderFillRect(m_pRenderer, &rect);
		return;
	}

	// 生成圆角矩形轮廓
	auto pts = GetRoundRectPoints(rc, radius);
	DrawFilledPolygon(m_pRenderer, pts, clr);
}

void UIRender_Sdl::DrawGradient(const CDuiRect& rc, CDuiColor dwFirst, CDuiColor dwSecond, bool bVertical, int nSteps)
{
	// 将 ARGB 颜色转换为 SDL_FColor（浮点 0.0-1.0 范围）
	SDL_FColor c1 = dwFirst.ToSDL_FColor();
	SDL_FColor c2 = dwSecond.ToSDL_FColor();

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

void UIRender_Sdl::DrawLine(int x1, int y1, int x2, int y2, int nSize, CDuiColor clr, int nStyle)
{
	if (nSize <= 0) return;

	SetDrawColor(clr);

	if (nStyle == PS_SOLID)
	{
		if (nSize == 1)
			SDL_RenderLine(m_pRenderer, (float)x1, (float)y1, (float)x2, (float)y2);
		else
			DrawThickLine(m_pRenderer, (float)x1, (float)y1, (float)x2, (float)y2, (float)nSize, clr);
	}
	else
	{
		DrawDashedLine(m_pRenderer, (float)x1, (float)y1, (float)x2, (float)y2, (float)nSize, clr, nStyle);
	}
}

void UIRender_Sdl::DrawRect(const CDuiRect& rc, int nSize, CDuiColor clr, int nStyle /*= PS_SOLID*/)
{
	if (nSize <= 0) return;
	SetDrawColor(clr);
	for (int i = 0; i < nSize; ++i) 
	{
		SDL_FRect rect = { (float)(rc.left + i), (float)(rc.top + i),
						   (float)(rc.GetWidth() - 2 * i), (float)(rc.GetHeight() - 2 * i) };
		if (rect.w <= 0 || rect.h <= 0) break;
		SDL_RenderRect(m_pRenderer, &rect);
	}
}

void UIRender_Sdl::DrawRoundRect(const CDuiRect& rc, int nSize, const CDuiSize& round, CDuiColor clr, int nStyle /*= PS_SOLID*/)
{
	if (nSize <= 0) return;
	int radius = (round.cx + round.cy) / 2;
	if (radius <= 0) {
		DrawRect(rc, nSize, clr, nStyle);
		return;
	}
	int maxR = MIN(rc.GetWidth() / 2, rc.GetHeight() / 2);
	if (radius > maxR) radius = maxR;
	if (radius <= 0) {
		DrawRect(rc, nSize, clr, nStyle);
		return;
	}

	SetDrawColor(clr);

	for (int offset = 0; offset < nSize; ++offset) {
		int r = radius - offset;
		if (r < 1) r = 1;
		CDuiRect rc2 = rc;
		rc2.Deflate(offset, offset);
		if (rc2.GetWidth() <= 0 || rc2.GetHeight() <= 0) break;
		auto pts = GetRoundRectPoints(rc2, r);
		if (pts.size() > 1) {
			// 用 SDL_RenderLines 绘制1px宽轮廓
			SDL_RenderLines(m_pRenderer, pts.data(), (int)pts.size());
		}
	}
}

void UIRender_Sdl::DrawEllipse(const CDuiRect& rc, int nSize, CDuiColor clr, int nStyle)
{
	if (nSize <= 0) return;
	float cx = (rc.left + rc.right) / 2.0f;
	float cy = (rc.top + rc.bottom) / 2.0f;
	float rx = (rc.right - rc.left) / 2.0f;
	float ry = (rc.bottom - rc.top) / 2.0f;
	if (rx <= 0 || ry <= 0) return;

	SetDrawColor(clr);

	// 生成基础椭圆点（单位圆）
	auto basePts = GetEllipsePoints(0, 0, 1, 1, 32);
	for (int offset = 0; offset < nSize; ++offset) {
		float scaleX = (rx + offset) / rx; // 实际应该是(半径+偏移)/半径？更好的方式是偏移量叠加，但这里简单做：向外扩展
		float scaleY = (ry + offset) / ry;
		// 更精确：新半径 = 原半径 + offset
		float newRx = rx + offset;
		float newRy = ry + offset;
		std::vector<SDL_FPoint> pts;
		pts.reserve(basePts.size());
		for (auto& p : basePts) {
			pts.push_back({ cx + p.x * newRx, cy + p.y * newRy });
		}
		SDL_RenderLines(m_pRenderer, pts.data(), (int)pts.size());
	}
}

void UIRender_Sdl::FillEllipse(const CDuiRect& rc, CDuiColor clr)
{
	float cx = (rc.left + rc.right) / 2.0f;
	float cy = (rc.top + rc.bottom) / 2.0f;
	float rx = (rc.right - rc.left) / 2.0f;
	float ry = (rc.bottom - rc.top) / 2.0f;
	if (rx <= 0 || ry <= 0) return;
	SDL_Color color = { clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA() };
	auto pts = GetEllipsePoints(cx, cy, rx, ry, 32);
	DrawFilledPolygon(m_pRenderer, pts, color);
}

void UIRender_Sdl::DrawText(CDuiRect& rc, LPCTSTR pstrText, CDuiColor clr, int iFont, UINT uStyle)
{
	CDuiStringUtf8 utf8Text(pstrText);

	UIFont* pFont = GetManager()->GetFont(iFont);
	if (!pFont) return;
	TTF_Font* ttfFont = (TTF_Font*)pFont->GetHandle();
	if (!ttfFont) return;

	if (uStyle & DT_CALCRECT)
	{
		CDuiRect rcTemp = rc;

		int w = 0, h = 0;
		TTF_GetStringSize(ttfFont, utf8Text.toString(), utf8Text.GetLength(), &w, &h);
		rc.right = rc.left + w;
		rc.bottom = rc.top + h;

		if (uStyle & DT_CENTER)
		{
			rc.left = rcTemp.left + (rcTemp.GetWidth() - w) / 2;
			rc.right = rc.left + w;
		}
		else if (uStyle & DT_RIGHT)
		{
			rc.left = rcTemp.right - (rcTemp.GetWidth() + w);
			rc.right = rcTemp.right;
		}
		if (uStyle & DT_VCENTER)
		{
			rc.top = rcTemp.top + (rcTemp.GetHeight() - h) / 2;
			rc.bottom = rc.top + h;
		}
		else if (uStyle & DT_BOTTOM)
		{
			rc.top = rcTemp.bottom - (rcTemp.GetWidth() + h);
			rc.bottom = rcTemp.bottom;
		}
		return;
	}

	// 创建 TTF_Text 对象 (SDL3内部会有缓存）
	TTF_Text* ttfText = TTF_CreateText(m_ttfEngine, ttfFont, utf8Text.toString(), utf8Text.GetLength());

	// 设置颜色
	TTF_SetTextColor(ttfText, clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA());

	// 对齐方式
	int x = rc.left;
	int y = rc.top;
	int texW = 0, texH = 0;
	TTF_GetTextSize(ttfText, &texW, &texH);
	if (uStyle & DT_CENTER)
	{
		x = rc.left + (rc.GetWidth() - texW) / 2;
	}
	else if (uStyle & DT_RIGHT)
	{
		x = rc.right - texW;
	}
	if (uStyle & DT_VCENTER)
	{
		y = rc.top + (rc.GetHeight() - texH) / 2;
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

uiBool UIRender_Sdl::DrawPath(const UIPath* path, int nSize, CDuiColor dwColor)
{
	return uiFalse;
}

uiBool UIRender_Sdl::FillPath(const UIPath* path, const CDuiColor dwColor)
{
	return uiFalse;
}

CDuiSize UIRender_Sdl::GetTextSize(LPCTSTR pstrText, int iFont, UINT uStyle)
{
	CDuiSize sz;
	if (!m_pManager) return sz;
	TTF_Font* ttfFont = (TTF_Font*)GetManager()->GetFont(iFont)->GetHandle();
	if (!ttfFont) return sz;
	CDuiStringUtf8 sUtf8 = CDuiString(pstrText);
	int w = 0, h = 0;
	TTF_GetStringSize(ttfFont, sUtf8.toString(), sUtf8.GetLength(), &w, &h);
	sz.cx = w; sz.cy = h;
	return sz;
}

void UIRender_Sdl::SetDrawColor(CDuiColor clr)
{
	if (m_lastDrawColor == clr)
		return;
	SDL_SetRenderDrawColor(m_pRenderer, clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA());
}

} // namespace DuiLib
#endif //#ifdef DUILIB_USE_RENDER_Sdl

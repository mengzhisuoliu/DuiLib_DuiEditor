#ifndef __UIRENDER_Sdl_H__
#define __UIRENDER_Sdl_H__
#pragma once

#include "UIObject_Sdl.h"

#ifdef DUILIB_SDL
namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//
//
class UILIB_API UIRender_Sdl : public TObjRefImpl<UIRender>
{
public:
	UIRender_Sdl();
	virtual ~UIRender_Sdl();

	void DestroyRender();

	virtual void Init(CPaintManagerUI* pManager, PVOID pParam) override;
	virtual HANDLE GetHandle() override;

	virtual void BeginPaint() override;
	virtual void EndPaint() override;

	virtual bool CloneFrom(UIRender* pSrcRender) override;

	virtual bool Resize(int width, int height) override;
	virtual bool Resize(const CDuiRect& rc) override;
	virtual UIBitmap* GetBitmap() override;
	virtual int GetWidth() const override;
	virtual int GetHeight() const override;
	virtual void Clear() override;
	virtual void ClearAlpha(const CDuiRect& rc, int alpha = 0) override;

	void InvalidRect(const CDuiRect* lpRect);
	CDuiRect GetInvalidRect();

	virtual CDuiColor SetPixel(int x, int y, CDuiColor dwColor) override;

	virtual uiBool BitBlt(int x, int y, int nWidth, int nHeight, UIRender* pSrcRender, int xSrc, int ySrc, DWORD dwRop = SRCCOPY) override;
	virtual uiBool StretchBlt(int x, int y, int nWidth, int nHeight, UIRender* pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, DWORD dwRop = SRCCOPY) override;
	virtual uiBool AlphaBlend(int x, int y, int nWidth, int nHeight, UIRender* pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha) override;
	virtual void DrawBitmapAlpha(int x, int y, int nWidth, int nHeight, UIBitmap* pUiBitmap, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha) override;

	virtual void DrawColor(const CDuiRect& rc, const CDuiSize& round, CDuiColor color) override;
	virtual void DrawGradient(const CDuiRect& rc, CDuiColor dwFirst, CDuiColor dwSecond, bool bVertical, int nSteps) override;

	virtual void DrawLine(int x1, int y1, int x2, int y2, int nSize, CDuiColor dwPenColor, int nStyle = PS_SOLID) override;
	virtual void DrawRect(const CDuiRect& rc, int nSize, CDuiColor dwPenColor, int nStyle = PS_SOLID) override;
	virtual void DrawRoundRect(const CDuiRect& rc, int nSize, const CDuiSize& round, CDuiColor dwPenColor, int nStyle = PS_SOLID) override;
	virtual void DrawEllipse(const CDuiRect& rc, int nSize, CDuiColor dwPenColor, int nStyle = PS_SOLID) override;
	virtual void FillEllipse(const CDuiRect& rc, CDuiColor dwColor) override;
	virtual void DrawText(CDuiRect& rc, LPCTSTR pstrText, CDuiColor dwColor, int iFont, UINT uStyle) override;

	virtual UIPath* CreatePath() override;
	virtual uiBool DrawPath(const UIPath* path, int nSize, CDuiColor dwColor) override;
	virtual uiBool FillPath(const UIPath* path, const CDuiColor dwColor) override;

	virtual CDuiSize GetTextSize(LPCTSTR pstrText, int iFont, UINT uStyle) override;

protected:
	void SetDrawColor(CDuiColor clr);
protected:
	bool m_bWindowRender;			// 是否为窗口渲染模式
	SDL_Renderer* m_pRenderer;		// 渲染器
	SDL_Texture* m_pTexture;		// 离屏渲染纹理
	TTF_TextEngine* m_ttfEngine;	// 文本引擎
	CDuiColor m_lastDrawColor;		// 缓存最后一次使用的颜色
	CStdRefPtr<UIBitmap> m_curBmp;
	int m_nWidth;
	int m_nHeight;
	CDuiRect m_rcInvalidate;
};

} // namespace DuiLib
#endif //#ifdef DUILIB_USE_RENDER_Sdl

#endif // __UIRENDER_H__


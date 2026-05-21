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

		virtual void Init(CPaintManagerUI* pManager, PVOID pParam) override;
		virtual HANDLE GetHandle() override;

		virtual void BeginPaint() override;
		virtual void EndPaint() override;

		virtual bool CloneFrom(UIRender *pSrcRender) override;

		virtual bool Resize(int width, int height) override;
		virtual bool Resize(const RECT &rc) override;
		virtual UIBitmap *GetBitmap() override;
		virtual int GetWidth() const override;
		virtual int GetHeight() const override;
		virtual void Clear() override;
		virtual void ClearAlpha(const RECT &rc, int alpha = 0) override;

		void InvalidRect(const RECT* lpRect);

		virtual DWORD SetPixel(int x, int y, DWORD dwColor) override;

		virtual BOOL BitBlt(int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, DWORD dwRop = SRCCOPY ) override;
		virtual BOOL StretchBlt(int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, DWORD dwRop = SRCCOPY) override;
		virtual BOOL AlphaBlend(int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha ) override;
		virtual void DrawBitmapAlpha(int x, int y, int nWidth, int nHeight, UIBitmap *pUiBitmap, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, int alpha) override;

		virtual void DrawColor(const RECT& rc, const SIZE &round, DWORD color) override;
		virtual void DrawGradient(const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps) override;

		virtual void DrawLine(int x1, int y1, int x2, int y2, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID) override;
		virtual void DrawRect(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID) override;
		virtual void DrawRoundRect(const RECT& rc, int nSize, const SIZE &round, DWORD dwPenColor,int nStyle = PS_SOLID) override;
		virtual void DrawEllipse(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID) override;
		virtual void FillEllipse(const RECT& rc, DWORD dwColor) override;
		virtual void DrawText(RECT& rc, LPCTSTR pstrText, DWORD dwColor, int iFont, UINT uStyle) override;

		virtual UIPath* CreatePath() override;	
		virtual BOOL DrawPath(const UIPath* path, int nSize, DWORD dwColor) override;
		virtual BOOL FillPath(const UIPath* path, const DWORD dwColor) override;

		virtual SIZE GetTextSize(LPCTSTR pstrText, int iFont, UINT uStyle) override;

	protected:
		bool m_bWindowRender;   // 是否为窗口渲染模式
		SDL_Renderer* m_pRenderer;
		SDL_Texture* m_pTexture;
		CStdRefPtr<UIBitmap> m_curBmp;
		int m_nWidth;
		int m_nHeight;
		CDuiRect m_rcInvalidate;
	};

} // namespace DuiLib
#endif //#ifdef DUILIB_USE_RENDER_Sdl

#endif // __UIRENDER_H__

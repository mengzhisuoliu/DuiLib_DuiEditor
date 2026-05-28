#ifndef __UIOBJECT_SDL_H__
#define __UIOBJECT_SDL_H__
#pragma once

#include "IRender.h"

#ifdef DUILIB_SDL
namespace DuiLib { 
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIFont_SDL : public UIFont
	{
	public:
		UIFont_SDL();

		virtual BOOL CreateDefaultFont() override;

		virtual void DeleteObject() override;

		virtual UINT_PTR  GetHandle() override;

		virtual UIFont* Clone(CPaintManagerUI *pManager) override;

		virtual int GetHeight() override;

	protected:
		virtual ~UIFont_SDL();

		virtual BOOL _buildFont(CPaintManagerUI *pManager=NULL) override;
	protected:
		TTF_Font *m_pTTF; //主字体
		std::vector<TTF_Font*> m_vSpareFonts; //备用字体
	};

	//////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIPen_SDL : public UIPen
	{
	public:
		UIPen_SDL();
		virtual void DeleteObject() override;
		virtual BOOL CreatePen(int nStyle, int nWidth, DWORD dwColor) override;
	protected:
		virtual ~UIPen_SDL();
	private:
	};

	//////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIBitmap_SDL : public UIBitmap
	{
	public:
		UIBitmap_SDL();
		const emUIOBJTYPE ObjectType() const override {  return OT_BITMAP_SDL;  }
		virtual void DeleteObject() override;
		virtual BOOL CreateFromData(LPBYTE pImage, int width, int height, DWORD mask) override;

		virtual UINT_PTR  GetHandle()	override;
		virtual BYTE* GetBits() override;
		virtual int	GetWidth() override;
		virtual int GetHeight() override;
		virtual BOOL IsAlpha() override;

		virtual UIBitmap *Clone() override;

		virtual void Clear() override;
		virtual void ClearAlpha(const RECT &rc, int alpha = 0) override;

		virtual BOOL SaveFile(LPCTSTR pstrFileName) override;

		//获取纹理，纹理持久化。不存在则创建新的，记得找个机会删除。
		SDL_Texture* GetTexture(SDL_Renderer* pRenderer);

		// 删除指定渲染器对应的纹理
		void DeleteTexture(SDL_Renderer* pRenderer);

		// 删除所有缓存纹理
		void DeleteAllTextures();

	protected:
		virtual ~UIBitmap_SDL();
	protected:
		SDL_Surface* m_surface;
		BYTE* m_pDataBits;
		int m_nWidth;
		int m_nHeight;

		std::map<SDL_Renderer*, SDL_Texture*> m_textureMap;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIImage_SDL : public UIImage
	{
	public:
		UIImage_SDL();

		virtual void DeleteObject() override;
	protected:
		virtual ~UIImage_SDL();
	};

	//////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIBrush_SDL : public UIBrush
	{
	public:
		UIBrush_SDL();

		virtual void DeleteObject() override;
		virtual BOOL CreateSolidBrush(DWORD clr) override;
		virtual BOOL CreateBitmapBrush(UIBitmap *bitmap) override;

	protected:
		virtual ~UIBrush_SDL();

	private:
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//
	class UILIB_API UIPath_SDL  : public UIPath
	{
	public:
		UIPath_SDL();

		virtual void DeleteObject() override;

		virtual BOOL Beginpath() override;
		virtual BOOL EndPath() override;
		virtual BOOL AbortPath() override;

		virtual BOOL AddLine(int x1, int y1, int x2, int y2) override;
		virtual BOOL AddLines(CDuiPoint *points, int count) override;

// 		virtual BOOL AddArc(int x, int y, int width, int height, double angel1, double angel2) override;
// 
// 		virtual BOOL AddRect(const RECT& rc, const SIZE &round) override;
	protected:
		virtual ~UIPath_SDL();

	protected:
	};

} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

#endif // __UIOBJECT_CAIRO_H__

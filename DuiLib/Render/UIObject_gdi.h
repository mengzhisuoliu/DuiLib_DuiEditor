#ifndef __UIFONT_GDI_H__
#define __UIFONT_GDI_H__

#pragma once
#include "IRender.h"

#ifdef DUILIB_WIN32
namespace DuiLib { 
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIFont_gdi : public UIFont
	{
	public:
		UIFont_gdi();

		virtual void DeleteObject() override;

		virtual uiBool CreateDefaultFont() override;

		virtual UINT_PTR  GetHandle() override;
		virtual HFONT GetHFONT(CPaintManagerUI *pManager=NULL) override;

		virtual UIFont* Clone(CPaintManagerUI *pManager) override;

		virtual int GetHeight() override;

	protected:
		virtual ~UIFont_gdi();

		virtual uiBool _buildFont(CPaintManagerUI *pManager=NULL) override;
	protected:
		HFONT m_hFont;
		TEXTMETRIC tm;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIPen_gdi : public UIPen
	{
	public:
		UIPen_gdi();

		virtual void DeleteObject() override;

		virtual HPEN GetHPEN() const override;

		virtual uiBool CreatePen(int nStyle, int nWidth, CDuiColor dwColor) override;

	protected:
		virtual ~UIPen_gdi();
	private:
		HPEN  m_hPen;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIBitmap_gdi : public UIBitmap
	{
	public:
		UIBitmap_gdi();

		virtual void DeleteObject() override;

		virtual uiBool CreateFromHBitmap(HBITMAP hBitmap) override;

		virtual uiBool CreateARGB32Bitmap(HDC hDC, int width, int height, uiBool bFlip) override;

		virtual uiBool CreateCompatibleBitmap(HDC hDC, int width, int height) override;

		virtual uiBool CreateFromData(LPBYTE pImage, int width, int height, CDuiColor mask) override;

		virtual UINT_PTR  GetHandle()	override;
		virtual HBITMAP GetHBITMAP() override;
		virtual BYTE* GetBits() override;
		virtual int	GetWidth() override;
		virtual int GetHeight() override;
		virtual uiBool IsAlpha() override;

		virtual UIBitmap *Clone() override;

		virtual void Clear() override;
		virtual void ClearAlpha(const CDuiRect &rc, int alpha = 0) override;

		virtual uiBool SaveFile(LPCTSTR pstrFileName) override;
	protected:
		virtual ~UIBitmap_gdi();
	protected:
		HBITMAP	m_hBitmap;
		BYTE	*m_pBits;
		int		m_nWidth;
		int		m_nHeight;
		uiBool	m_bAlphaChannel;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIImage_gdi : public UIImage
	{
	public:
		UIImage_gdi();

		virtual void DeleteObject() override;

		virtual uiBool CreateImage(HBITMAP hBitmap, bool bAlpha) override;
	protected:
		virtual ~UIImage_gdi();
	};

	//////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API UIBrush_gdi : public UIBrush
	{
	public:
		UIBrush_gdi();

		virtual void DeleteObject() override;

		virtual HBRUSH GetHBrush() const override;

		virtual uiBool CreateFromHBrush(HBRUSH hBrush) override;
		virtual uiBool CreateSolidBrush(CDuiColor clr) override;
		virtual uiBool CreateBitmapBrush(UIBitmap *bitmap) override;

	protected:
		virtual ~UIBrush_gdi();

	private:
		HBRUSH m_hBrush;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//
	class UILIB_API UIPath_gdi  : public UIPath
	{
	public:
		UIPath_gdi();
		UIPath_gdi(HDC hDC);

		virtual void DeleteObject() override;

		virtual uiBool Beginpath() override;
		virtual uiBool EndPath() override;
		virtual uiBool AbortPath() override;

		virtual uiBool AddLine(int x1, int y1, int x2, int y2) override;
		virtual uiBool AddLines(CDuiPoint *points, int count) override;

	protected:
		virtual ~UIPath_gdi();

	protected:
		HDC m_hDC;
		CDuiPoint m_curPoint;
	};

} // namespace DuiLib

#endif //#ifdef DUILIB_WIN32
#endif // __UIRENDER_H__

#include "StdAfx.h"

#include "../Render/UIObject_Cairo.h"

#ifdef DUILIB_GTK

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

///////////////////////////////////////////////////////////////////////////////////////
namespace DuiLib {
	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIFont_pango::UIFont_pango()
	{
		m_pPangoFontDesc = NULL; 
	}

	UIFont_pango::~UIFont_pango()
	{
		if (m_pPangoFontDesc)
		{
			//cairo_font_face_destroy(m_hFont);
			pango_font_description_free(m_pPangoFontDesc);
			m_pPangoFontDesc = NULL;
		}
	}

	void UIFont_pango::DeleteObject()
	{
		if (m_pPangoFontDesc)
		{
			//cairo_font_face_destroy(m_hFont);
			pango_font_description_free(m_pPangoFontDesc);
			m_pPangoFontDesc = NULL;
		}
	}

	BOOL UIFont_pango::CreateDefaultFont()
	{
		sFontName = _T("Sans");
		iSize = 14;
		bBold = FALSE;
		bUnderline = FALSE;
		bItalic = FALSE;
		return _buildFont(NULL);
	}

	UINT_PTR UIFont_pango::GetHandle()
	{
		return (UINT_PTR)m_pPangoFontDesc;
	}

	UIFont* UIFont_pango::Clone(CPaintManagerUI *pManager)
	{
		UIFont_pango *pNewFont	= new UIFont_pango;
		pNewFont->id			= id;
		pNewFont->sFontName		= sFontName;
		pNewFont->iSize			= iSize;
		pNewFont->bBold			= bBold;
		pNewFont->bUnderline	= bUnderline;
		pNewFont->bItalic		= bItalic;
		pNewFont->bDefault		= bDefault;
		pNewFont->bShared		= bShared;
		//pNewFont->m_hFont		= cairo_font_face_reference(m_hFont);
		pNewFont->m_pPangoFontDesc		= pango_font_description_copy(m_pPangoFontDesc);
		ASSERT(pNewFont);
		return pNewFont;
	}

	int UIFont_pango::GetHeight()
	{
		if (m_pPangoFontDesc)
		{
			return pango_font_description_get_size(m_pPangoFontDesc) / PANGO_SCALE;
		}
		return 0;
	}

	BOOL UIFont_pango::_buildFont(CPaintManagerUI *pManager)
	{
		if(sFontName.IsEmpty())
			return FALSE;

 		UISTRING_CONVERSION;
// 		m_hFont = cairo_toy_font_face_create ((LPCSTR)UIA2UTF8(sFontName), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

		PangoFontDescription *desc = pango_font_description_new();

		if (!sFontName.IsEmpty())
		{
			pango_font_description_set_family(desc, (LPCSTR)UIT2UTF8(sFontName));
		}

		//pango_font_description_set_size(desc, iSize*PANGO_SCALE);
		pango_font_description_set_absolute_size(desc, iSize*PANGO_SCALE);

		if (bItalic)
		{
			pango_font_description_set_style(desc, PANGO_STYLE_ITALIC);
		}
		else
		{
			pango_font_description_set_style(desc, PANGO_STYLE_NORMAL);
		}

		if (bBold)
		{
			pango_font_description_set_weight(desc, PANGO_WEIGHT_BOLD);
		}
		else
		{
			pango_font_description_set_weight(desc, PANGO_WEIGHT_NORMAL);
		}

		m_pPangoFontDesc = desc;
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIPen_Cairo::UIPen_Cairo()
	{

	}

	UIPen_Cairo::~UIPen_Cairo() 
	{
		
	}

	void UIPen_Cairo::DeleteObject()
	{
		
	}

	BOOL UIPen_Cairo::CreatePen(int nStyle, int nWidth, DWORD dwColor)
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIBrush_Cairo::UIBrush_Cairo() 
	{
	}

	UIBrush_Cairo::~UIBrush_Cairo() 
	{
		
	}

	void UIBrush_Cairo::DeleteObject()
	{
		
	}

	BOOL UIBrush_Cairo::CreateSolidBrush(DWORD clr)
	{
        return FALSE;
	}

	BOOL UIBrush_Cairo::CreateBitmapBrush(UIBitmap *bitmap)
	{
        return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIPath_Cairo::UIPath_Cairo()
	{
		m_cr = NULL;
		m_path = NULL;
	}

	UIPath_Cairo::UIPath_Cairo(cairo_t *cr)
	{
		m_cr = cr;
		m_path = NULL;
	}

	UIPath_Cairo::~UIPath_Cairo()
	{
		if(m_path)
		{
			cairo_path_destroy(m_path); m_path = NULL;
		}
	}

	void UIPath_Cairo::DeleteObject()
	{
		if(m_path)
		{
			cairo_path_destroy(m_path); m_path = NULL;
		}
	}

	BOOL UIPath_Cairo::Beginpath()
	{
		cairo_new_path(m_cr);
		return TRUE;
	}

	BOOL UIPath_Cairo::EndPath()
	{
		cairo_close_path(m_cr);
		return TRUE;
	}

	BOOL UIPath_Cairo::AbortPath()
	{
		return TRUE;
	}

	BOOL UIPath_Cairo::AddLine(int x1, int y1, int x2, int y2)
	{
		if(m_curPoint.x != x1 && m_curPoint.y != y1)
		{
			cairo_move_to(m_cr, x1, y1);
		}
		cairo_line_to(m_cr, x2, y2);
		m_curPoint.x = x2;
		m_curPoint.y = y2;
		return TRUE;
	}

	BOOL UIPath_Cairo::AddLines(CDuiPoint *points, int count)
	{
		cairo_move_to(m_cr, points[0].x, points[0].y);
		for (int i=1; i<count; i++)
		{
			cairo_line_to(m_cr, points[i].x, points[i].y);
		}
		cairo_line_to(m_cr, points[0].x, points[0].y);
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIBitmap_Cairo::UIBitmap_Cairo()
	{
		m_surface = NULL;
		m_pDataBits = NULL;
		m_bAlphaChannel = FALSE;
	}

	UIBitmap_Cairo::~UIBitmap_Cairo()
	{
		if(m_surface) { cairo_surface_destroy(m_surface); m_surface = NULL; }
		if(m_pDataBits) { free(m_pDataBits); m_pDataBits = NULL; }
	}

	void UIBitmap_Cairo::DeleteObject()
	{
		if(m_surface) { cairo_surface_destroy(m_surface); m_surface = NULL; }
		if(m_pDataBits) { free(m_pDataBits); m_pDataBits = NULL; }
	}

	BOOL UIBitmap_Cairo::CreateFromData(LPBYTE pImage, int width, int height, DWORD mask)
	{
		DeleteObject();

		//每行图像有几个字节
		int tride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,width);

		//Creates an image surface for the provided pixel data. The output buffer must be kept around until the #cairo_surface_t is destroyed or cairo_surface_finish() is called on the surface. The initial contents of data will be used as the initial image contents; you must explicitly clear the buffer, using, for example, cairo_rectangle() and cairo_fill() if you want it cleared.
		//cairo不会备份pImage，所以自己存起来。
		m_pDataBits = (BYTE *)malloc(tride*height);
		memcpy(m_pDataBits, pImage, tride*height);
		m_surface = cairo_image_surface_create_for_data(m_pDataBits, CAIRO_FORMAT_ARGB32, width, height, tride);
		if(m_surface == NULL) return FALSE;
		if(cairo_surface_status(m_surface) != CAIRO_STATUS_SUCCESS)
			return FALSE;

		//stbi的图像
		//pImage[0], R
		//pImage[1], G
		//pImage[2], B
		//pImage[3], alpha

		//cairo的图像 B  G  R
		//pDest[0], B
		//pDest[1], G
		//pDest[2], R
		//pDest[3], alpha

		//RGB 转 BGR
		BYTE *pDest = GetBits();
		m_bAlphaChannel = FALSE;
		for( int i = 0; i < width * height; i++ ) 
		{
			if( pDest[i*4 + 3] < 255 )
			{
				BYTE tp = pDest[i*4];
				pDest[i*4]		= pDest[i*4 + 2] * pDest[i*4 + 3] / 255;
				pDest[i*4 + 1] = (BYTE)(DWORD(pDest[i*4 + 1])*pDest[i*4 + 3]/255);
				pDest[i*4 + 2]	= tp * pDest[i*4 + 3] / 255;
				m_bAlphaChannel = TRUE;
			}
			else
			{ 
				BYTE tp = pDest[i*4];
				pDest[i*4]		= pDest[i*4 + 2];
				pDest[i*4 + 2]	= tp;
			}

			if( *(DWORD*)(&pDest[i*4]) == mask ) {
				pDest[i*4] = (BYTE)0;
				pDest[i*4 + 1] = (BYTE)0;
				pDest[i*4 + 2] = (BYTE)0; 
				pDest[i*4 + 3] = (BYTE)0;
				m_bAlphaChannel = TRUE;
			}
		}
		return TRUE;
	}

	UINT_PTR UIBitmap_Cairo::GetHandle()
	{
		return (UINT_PTR)m_surface;
	}

	BYTE* UIBitmap_Cairo::GetBits()
	{
		return cairo_image_surface_get_data(m_surface);
	}

	int	UIBitmap_Cairo::GetWidth()
	{
		if(!m_surface) return 0;
		return cairo_image_surface_get_width(m_surface);
	}

	int UIBitmap_Cairo::GetHeight()
	{
		if(!m_surface) return 0;
		return cairo_image_surface_get_height(m_surface);
	}

	BOOL UIBitmap_Cairo::IsAlpha()
	{
		return m_bAlphaChannel;
	}

	UIBitmap *UIBitmap_Cairo::Clone()
	{
		UIBitmap_Cairo *bmp = (UIBitmap_Cairo *)UIGlobal::CreateBitmap();
		bmp->m_surface = cairo_surface_reference(m_surface);
		return bmp;
	}

	void UIBitmap_Cairo::Clear()
	{
		
	}

	void UIBitmap_Cairo::ClearAlpha(const RECT &rc, int alpha)
	{
		
	}

	BOOL UIBitmap_Cairo::SaveFile(LPCTSTR pstrFileName)
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	UIBitmap_Pixbuf::UIBitmap_Pixbuf()
	{
		m_pixbuf = NULL;
		m_pDataBits = NULL;
		m_bAlphaChannel = FALSE;
	}

	UIBitmap_Pixbuf::~UIBitmap_Pixbuf()
	{
		if(m_pixbuf) { g_object_unref(m_pixbuf); m_pixbuf = NULL; }
		if(m_pDataBits) { free(m_pDataBits); m_pDataBits = NULL; }
	}

	void UIBitmap_Pixbuf::DeleteObject()
	{
		if(m_pixbuf) { g_object_unref(m_pixbuf); m_pixbuf = NULL; }
		if(m_pDataBits) { free(m_pDataBits); m_pDataBits = NULL; }
	}

	BOOL UIBitmap_Pixbuf::CreateFromData(LPBYTE pImage, int width, int height, DWORD mask)
	{
		DeleteObject();

		//每行图像有几个字节
		int tride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,width);

		m_pDataBits = (BYTE *)malloc(tride*height);
		memcpy(m_pDataBits, pImage, tride*height);

		m_pixbuf = gdk_pixbuf_new_from_data(m_pDataBits, GDK_COLORSPACE_RGB, true, 8, width, height, tride, NULL, NULL);

		return m_pixbuf != NULL;
	}

	UINT_PTR UIBitmap_Pixbuf::GetHandle()
	{
		return (UINT_PTR)m_pixbuf;
	}

	BYTE* UIBitmap_Pixbuf::GetBits()
	{
		return m_pixbuf ? gdk_pixbuf_get_pixels(m_pixbuf) : NULL;
	}

	int	UIBitmap_Pixbuf::GetWidth()
	{
		return m_pixbuf ? gdk_pixbuf_get_width(m_pixbuf) : 0;
	}

	int UIBitmap_Pixbuf::GetHeight()
	{
		return m_pixbuf ? gdk_pixbuf_get_height(m_pixbuf) : 0;
	}

	BOOL UIBitmap_Pixbuf::IsAlpha()
	{
		return m_bAlphaChannel;
	}

	UIBitmap *UIBitmap_Pixbuf::Clone()
	{
		//????
		return NULL;
	}

	void UIBitmap_Pixbuf::Clear()
	{

	}

	void UIBitmap_Pixbuf::ClearAlpha(const RECT &rc, int alpha)
	{

	}

	BOOL UIBitmap_Pixbuf::SaveFile(LPCTSTR pstrFileName)
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIImage_Cairo::UIImage_Cairo()
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

	UIImage_Cairo::~UIImage_Cairo()
	{
		//pBits是从::CreateDIBSection搞过来的，不能delete
		//pSrcBits 是HSL色彩转换时分配的，需要delete
		if (pSrcBits) { delete[] pSrcBits; pSrcBits = NULL; }	
		if(bitmap) { bitmap->Release(); };
	}

	void UIImage_Cairo::DeleteObject()
	{
		bitmap->DeleteObject();
		if (pSrcBits) { delete[] pSrcBits; pSrcBits = NULL; }
	}

} // namespace DuiLib
#endif //#ifdef DUILIB_GTK


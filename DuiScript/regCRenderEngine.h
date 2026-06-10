#pragma once

namespace DuiLib
{
	static BOOL UIFile_LoadFile(LPCTSTR pStrImage, CUIFile &dest)
	{
		return dest.LoadFile(pStrImage, 0, NULL);
	}

class regUIFile
{
	DECL_FACTORY(CUIFile);
public:
	static void Register(asIScriptEngine *engine)
	{
		CDuiStringA classname = "CUIFile";

		int r = 0;
		r = engine->RegisterObjectType(classname, 0, asOBJ_REF|asOBJ_NOCOUNT); 
		REG_FACTORY(CUIFile);

		REG_METHOD_FUNPR(CUIFile, LPBYTE,	GetData,	() const	);
		REG_METHOD_FUNPR(CUIFile, DWORD,	GetSize,	() const	);
		REG_METHOD_FUNPR(CUIFile, void,		Empty,		()			);

		//REG_METHOD_FUNPR(CUIFile, BOOL, LoadFile, (const STRINGorID &bitmap, LPCTSTR type, HINSTANCE instance)	);
		//REG_METHOD_FUNPR(CUIFile, BOOL, LoadFile, (LPCTSTR pStrImage, LPCTSTR type, HINSTANCE instance)			);
		//REG_METHOD_FUNPR(CUIFile, BOOL, LoadFile, (UINT nID, LPCTSTR type, HINSTANCE instance)					);

		r = engine->RegisterObjectMethod("CUIFile", "BOOL LoadFile(string)", asFUNCTIONPR(UIFile_LoadFile, (LPCTSTR, CUIFile &), BOOL), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	}	
};

class regUIGlobal
{
public:
	static void Register(asIScriptEngine *engine)
	{
		int r = 0;

// 		REG_GLOBAL_FUNPR(UIRender *,	UIGlobal::CreateRenderTarget,	()	);
// 		REG_GLOBAL_FUNPR(UIFont *,		UIGlobal::CreateFont,			()	);
// 		REG_GLOBAL_FUNPR(UIPen *,		UIGlobal::CreatePen,			()	);
// 		REG_GLOBAL_FUNPR(UIBitmap *,	UIGlobal::CreateBitmap,			()	);
// 		REG_GLOBAL_FUNPR(UIBrush *,		UIGlobal::CreateBrush,			()	);
// 		REG_GLOBAL_FUNPR(UIImage *,		UIGlobal::CreateImage,			()	);
	}
};

class regUIRender
{
	static UIRender *UIRender_Ref_Factory() { return UIGlobal::CreateRenderTarget(); }
public:
	static void Register(asIScriptEngine *engine)
	{
		CDuiStringA classname = "UIRender";

		int r = 0;
		r = engine->RegisterObjectType(classname, 0, asOBJ_REF|asOBJ_NOCOUNT); 
		REG_FACTORY(UIRender);

		REG_METHOD_FUNPR(UIRender, void, Init, (CPaintManagerUI *pManager, HDC hDC) );
		REG_METHOD_FUNPR(UIRender, void, AttachDC, (CPaintManagerUI *pManager, HDC hDC) );

		REG_METHOD_FUNPR(UIRender, CPaintManagerUI *, GetManager, () );
		REG_METHOD_FUNPR(UIRender, HDC,  GetDC, () );
		REG_METHOD_FUNPR(UIRender, UIBitmap *, GetBitmap, () );

		REG_METHOD_FUNPR(UIRender, bool, Resize, (int width, int height) );
		REG_METHOD_FUNPR(UIRender, bool, Resize, (const CDuiRect &rc) );

		//擦除当前画布。
		REG_METHOD_FUNPR(UIRender, void, Clear, () );

		REG_METHOD_FUNPR(UIRender, void, SaveDC, () );
		REG_METHOD_FUNPR(UIRender, void, RestoreDC, () );

		//REG_METHOD_FUNPR(UIRender, UIObject*, SelectObject, (UIObject *pObject) );
		//REG_METHOD_FUNPR(UIRender, void, RestoreObject, (UIObject *pObject) );
		//REG_METHOD_FUNPR(UIRender, void, RestoreDefaultObject, () );

		//BitBlt (bit block transfer)
		REG_METHOD_FUNPR(UIRender, BOOL, BitBlt, (int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, DWORD dwRop) );

		//stretch BitBlt
		REG_METHOD_FUNPR(UIRender, BOOL, StretchBlt, (int x, int y, int nWidth, int nHeight, UIRender *pSrcRender, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, DWORD dwRop) );

		//绘制位图
		//REG_METHOD_FUNPR(UIRender, void, DrawBitmap, (HBITMAP hBitmap, const CDuiRect& rc, const CDuiRect& rcPaint, const CDuiRect& rcBmpPart, const CDuiRect& rcCorners, bool bAlpha, BYTE uFade, bool hole, bool xtiled, bool ytiled) );

		//画颜色(填充颜色)
		REG_METHOD_FUNPR(UIRender, void, DrawColor, (const CDuiRect& rc, const CDuiSize &round, CDuiColor color) );

		//画渐变色
		REG_METHOD_FUNPR(UIRender, void, DrawGradient, (const CDuiRect& rc, CDuiColor dwFirst, CDuiColor dwSecond, bool bVertical, int nSteps) );

		//画线  MoveTo(rc.left, rc.top)，LineTo(rc.right, rc.bottom)
		REG_METHOD_FUNPR(UIRender, void, DrawLine, (const CDuiRect& rc, int nSize, CDuiColor dwPenColor,int nStyle) );

		//画边框
		REG_METHOD_FUNPR(UIRender, void, DrawRect, (const CDuiRect& rc, int nSize, CDuiColor dwPenColor,int nStyle) );

		//画圆角边框
		REG_METHOD_FUNPR(UIRender, void, DrawRoundRect, (const CDuiRect& rc, int nSize, const CDuiSize &round, CDuiColor dwPenColor,int nStyle) );

		//画椭圆
		REG_METHOD_FUNPR(UIRender, void, DrawEllipse, (const CDuiRect& rc, int nSize, CDuiColor dwPenColor, int nStyle) );

		//绘制文本
		REG_METHOD_FUNPR(UIRender, void, DrawText, (CDuiRect& rc, const CDuiRect &rcTextPadding, LPCTSTR pstrText, CDuiColor dwTextColor, int iFont, UINT uStyle) );

		//绘制文字的占用的空间大小
		REG_METHOD_FUNPR(UIRender, CDuiSize, GetTextSize, (LPCTSTR pstrText, int iFont, UINT uStyle) );

		//////////////////////////////////////////////////////////////////////////

		//画背景色
		REG_METHOD_FUNPR(UIRender, void, DrawBackColor, (const CDuiRect& rc, const CDuiSize &round, CDuiColor dwBackColor, CDuiColor dwBackColor2, CDuiColor dwBackColor3, bool bVertical) );

		//画边框
		REG_METHOD_FUNPR(UIRender, void, DrawBorder, (const CDuiRect &rcItem, int nBorderSize, CDuiSize szBorderRound, CDuiRect rcBorderSize, CDuiColor dwColor, int nBorderStyle) );

		//绘制带背景颜色的文本
		REG_METHOD_FUNPR(UIRender, void, DrawText, (CDuiRect& rc, const CDuiRect &rcTextPadding, LPCTSTR pstrText,CDuiColor dwTextColor, int iFont, UINT uStyle, CDuiColor dwTextBKColor) );

		//根据TDrawInfo绘制图像
		REG_METHOD_FUNPR(UIRender, bool, DrawImageInfo, (const CDuiRect& rcItem, const CDuiRect& rcPaint, const TDrawInfo* pDrawInfo, HINSTANCE instance) );

		//根据字符串属性绘制图像
		REG_METHOD_FUNPR(UIRender, bool, DrawImageString, (const CDuiRect& rcItem, const CDuiRect& rcPaint, LPCTSTR pStrImage, LPCTSTR pStrModify, HINSTANCE instance) );
		
	}
};


} //namespace DuiLib
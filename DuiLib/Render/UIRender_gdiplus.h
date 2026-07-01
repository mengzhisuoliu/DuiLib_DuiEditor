#ifndef __UIRENDER_GDIPLUS_H__
#define __UIRENDER_GDIPLUS_H__

#pragma once
#include "UIRender_gdi.h"
#include "UIObject_gdiplus.h"

#ifdef DUILIB_WIN32
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	class UILIB_API UIRender_gdiplus : public UIRender_gdi
	{
	public:
		UIRender_gdiplus();
		virtual ~UIRender_gdiplus();

// 		virtual UIObject* SelectObject(UIObject *pObject) override;
// 		virtual void RestoreObject(UIObject *pObject = NULL) override;
// 		virtual void RestoreDefaultObject() override;

		virtual void DrawColor(const CDuiRect& rc, const CDuiSize &round, CDuiColor color) override;
		//virtual void DrawGradient(const CDuiRect& rc, CDuiColor dwFirst, CDuiColor dwSecond, bool bVertical, int nSteps) override;

		virtual void DrawLine(int x1, int y1, int x2, int y2, int nSize, CDuiColor dwPenColor,int nStyle = PS_SOLID) override;
		virtual void DrawRect(const CDuiRect& rc, int nSize, CDuiColor dwPenColor,int nStyle = PS_SOLID) override;
		virtual void DrawRoundRect(const CDuiRect& rc, int nSize, const CDuiSize &round, CDuiColor dwPenColor,int nStyle = PS_SOLID) override;
		virtual void DrawEllipse(const CDuiRect& rc, int nSize, CDuiColor dwPenColor,int nStyle = PS_SOLID) override;
		virtual void FillEllipse(const CDuiRect& rc, CDuiColor dwColor) override;
		virtual void DrawText(CDuiRect& rc, LPCTSTR pstrText, CDuiColor dwTextColor, int iFont, UINT uStyle) override;

		virtual UIPath* CreatePath() override;	
		virtual uiBool DrawPath(const UIPath* path, int nSize, CDuiColor dwColor) override;
		virtual uiBool FillPath(const UIPath* path, const CDuiColor dwColor) override;

	};

} // namespace DuiLib
#endif // #ifdef DUILIB_WIN32
#endif // __UIRENDER_H__


#include "StdAfx.h"
#include "UIRender_Sdl.h"

#ifdef DUILIB_SDL
namespace DuiLib {
	//////////////////////////////////////////////////////////////////////////
	//
	//
UIClipSDL::UIClipSDL()
	: m_pRenderer(nullptr)
	, m_bClipSaved(false)
{
	SDL_zero(m_oldClipRect);
	SDL_zero(m_newClipRect);
}

UIClipSDL::~UIClipSDL()
{
	// 恢复裁剪矩形（避免影响后续绘制）
	if (m_pRenderer && m_bClipSaved) 
	{
		SDL_SetRenderClipRect(m_pRenderer, &m_oldClipRect);
	}
}

void UIClipSDL::GenerateClip(UIRender * pRender, RECT rc)
{
	if (!pRender) return;
	m_pRenderer = (SDL_Renderer * )pRender->GetHandle();
	if (!m_pRenderer) return;

	// 保存当前裁剪矩形
	m_bClipSaved = SDL_GetRenderClipRect(m_pRenderer, &m_oldClipRect);

	// 设置新的裁剪矩形
	SDL_Rect newRect;
	newRect.x = rc.left;
	newRect.y = rc.top;
	newRect.w = rc.right - rc.left;
	newRect.h = rc.bottom - rc.top;
	if (m_bClipSaved)
	{
		SDL_Rect intersect;
		if (SDL_GetRectIntersection(&m_oldClipRect, &newRect, &intersect))
		{
			newRect = intersect;
		}
// 		else {
// 			// 无交集，设置裁剪区域为空（所有绘制被屏蔽）
// 			newRect.w = 0;
// 			newRect.h = 0;
// 		}
	}
	m_newClipRect = newRect;

	//应用新的裁剪矩形
	SDL_SetRenderClipRect(m_pRenderer, &m_newClipRect);
}

void UIClipSDL::GenerateRoundClip(UIRender * pRender, RECT rc, RECT rcItem, int roundX, int roundY)
{
	GenerateClip(pRender, rc);
}

void UIClipSDL::UseOldClipBegin(UIRender * pRender)
{
	if (!m_pRenderer || !m_bClipSaved) return;

	// 临时恢复到旧的裁剪矩形
	SDL_SetRenderClipRect(m_pRenderer, &m_oldClipRect);
}

void UIClipSDL::UseOldClipEnd(UIRender * pRender)
{
	if (!m_pRenderer || !m_bClipSaved) return;

	// 恢复回新的裁剪矩形（即当前正在使用的裁剪区域）
	SDL_SetRenderClipRect(m_pRenderer, &m_newClipRect);
}

} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

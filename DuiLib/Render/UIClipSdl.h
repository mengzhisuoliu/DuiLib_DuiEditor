#ifndef __UICLIP_SDL_H__
#define __UICLIP_SDL_H__

#pragma once
#ifdef DUILIB_SDL
namespace DuiLib {
	//////////////////////////////////////////////////////////////////////////
	//
	//
	class UILIB_API UIClipSDL : public UIClipBase
	{
	public:
		UIClipSDL();
		virtual ~UIClipSDL();
	public:
		virtual void GenerateClip(UIRender *pRender, RECT rc) override;
		virtual void GenerateRoundClip(UIRender *pRender, RECT rc, RECT rcItem, int roundX, int roundY) override;
		virtual void UseOldClipBegin(UIRender *pRender) override;
		virtual void UseOldClipEnd(UIRender* pRender) override;
	protected:
		SDL_Renderer* m_pRenderer;
		SDL_Rect      m_oldClipRect;
		SDL_Rect      m_newClipRect;
		bool          m_bClipSaved;
	};
} // namespace DuiLib
#endif //#ifdef DUILIB_SDL

#endif // __UIRENDER_H__

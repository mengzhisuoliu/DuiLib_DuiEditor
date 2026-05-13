#ifndef __UIDATETIME_WND_SDL_H__
#define __UIDATETIME_WND_SDL_H__

#pragma once

#ifdef DUILIB_SDL
namespace DuiLib
{
	class CDateTimeWndSDL : public CWindowSDL
	{
	public:
		CDateTimeWndSDL();

		void Init(CDateTimeUI* pOwner, RECT rcBase, UINT uFormatStyle);
		RECT CalPos(RECT rcBase);

	protected:
		CDateTimeUI* m_pOwner;
		UINT m_uFormatStyle;
		SYSTEMTIME m_oldSysTime;
	};
}
#endif //#ifdef DUILIB_SDL
#endif // __UIDATETIME_H__
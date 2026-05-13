#ifndef __DPI_SDL_H__
#define __DPI_SDL_H__
#pragma once

#ifdef DUILIB_SDL
namespace DuiLib
{
	class UILIB_API CDpiSDL : public CDpiBase
	{
	public:
		CDpiSDL();
	};
}
#endif //#ifdef DUILIB_SDL
#endif //__DPI_H__
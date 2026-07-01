#include "StdAfx.h"

#include "../Render/UIRenderFactory_Sdl.h"
#include "../Render/UIRender_Sdl.h"
#include "../Render/UIObject_Sdl.h"

#ifdef DUILIB_SDL
///////////////////////////////////////////////////////////////////////////////////////
namespace DuiLib {

	//////////////////////////////////////////////////////////////////////////
	//
	//
	UIRender *UIRenderFactory_Sdl::CreateRenderTarget()
	{
		return new UIRender_Sdl();
	}

	UIFont *UIRenderFactory_Sdl::CreateFont()
	{
		return new UIFont_SDL;
	}

	UIPen *UIRenderFactory_Sdl::CreatePen()
	{
		return new UIPen_SDL;
	}

	UIBitmap *UIRenderFactory_Sdl::CreateBitmap()
	{
		return new UIBitmap_SDL;
	}

	UIBrush *UIRenderFactory_Sdl::CreateBrush()
	{
		return new UIBrush_SDL;
	}

	UIImage *UIRenderFactory_Sdl::CreateImage()
	{
		return new UIImage_SDL;
	}

// 	UIPath *UIRenderFactory_Sdl::CreatePath()
// 	{
// 		return new UIPath_Sdl;
// 	}
	

	//////////////////////////////////////////////////////////////////////////
	//
	//



} // namespace DuiLib
#endif //#ifdef DUILIB_SDL


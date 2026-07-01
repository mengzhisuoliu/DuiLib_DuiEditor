#pragma once
#ifndef __UISHADOW_SDL_H__
#define __UISHADOW_SDL_H__

#ifdef DUILIB_SDL
namespace DuiLib
{

class UILIB_API CShadowSDLUI : public CShadowBaseUI
{
public:
	CShadowSDLUI(void);
	virtual ~CShadowSDLUI(void);

public:
	//	初始化并注册阴影类
	static bool Initialize(HINSTANCE hInstance);
};

}
#endif //#ifdef DUILIB_SDL

#endif //__UISHADOW_H__

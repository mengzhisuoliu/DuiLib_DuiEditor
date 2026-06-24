#include "StdAfx.h"

#ifdef DUILIB_SDL
namespace DuiLib{

CUIApplicationSDL::CUIApplicationSDL(void)
{

}

CUIApplicationSDL::~CUIApplicationSDL(void)
{

}

bool CUIApplicationSDL::InitInstance(int argc, char* argv[])
{
	m_strAppPath = CPaintManagerUI::GetInstancePath();

	//小键盘直接根据NUMLOCK灯亮否转码
	SDL_SetHint(SDL_HINT_KEYCODE_OPTIONS, "hide_numpad");

	//输入法候选窗口
	SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "composition,candidates");

	//确保 Alt+F4 能生成窗口关闭事件
	if (!SDL_SetHint(SDL_HINT_WINDOWS_CLOSE_ON_ALT_F4, "1"))
	{
		printf("Warning: Failed to set SDL_HINT_WINDOWS_CLOSE_ON_ALT_F4\n");
	}

	if (!SDL_Init(SDL_INIT_VIDEO))
		return false;

	TTF_Init();
	//GFX_Init();

	// 初始化资源
	InitResource();
	return true;
}

void CUIApplicationSDL::Run()
{
	if (m_pMainWnd)
	{
		m_pMainWnd->m_pApplication = this;
	}

	// 消息循环
	CPaintManagerSDLUI::MessageLoop();
}

int CUIApplicationSDL::ExitInstance()
{
	if (m_pMainWnd != NULL) { delete m_pMainWnd; m_pMainWnd = NULL; }
	CPaintManagerUI::Term();
	//GFX_Quit();
	TTF_Quit();
	SDL_Quit();
	return 0;
}

} //namespace DuiLib{
#endif //#ifdef DUILIB_SDL

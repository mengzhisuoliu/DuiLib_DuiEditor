
#include "App.h"

using namespace DuiLib;

extern "C" int SDL_main(int argc, char* argv[]) {
    /*
    (void)argc;
    (void)argv;
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed (%s)", SDL_GetError());
        return 1;
    }

    if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Hello World",
                                 "!! Your SDL project successfully runs on Android !!", NULL)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_ShowSimpleMessageBox failed (%s)", SDL_GetError());
        return 1;
    }

    SDL_Quit();
    */

    //设置主窗口的名字
    uiApp.SetAppName(_T("Test"));

    //设置程序为单一执行实例，允许多实例的话，就不需要这行了。
    //	uiApp.SetSingleApplication(true, _T("CBD4DD1F-3B18-4DA4-A043-0EA9AEC8D570"));

    //设置xml资源载入方式，选择一种就行
    uiApp.SetResType_File(_T("skin"));								//从文件夹中载入xml文件
    CPaintManagerUI::SetResourcePath("skin");

    CPaintManagerUI::SetRenderEngineType(DuiLib_Render_Sdl);

    //初始化duilib, 注册插件，创建主窗口
    if (!uiApp.InitInstance(argc, argv))
        return 0;

    //进入消息循环
    uiApp.Run();

    //清理资源。
    uiApp.ExitInstance();
    return 0;
}
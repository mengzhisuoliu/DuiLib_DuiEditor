// DuiLibTemplate.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Test.h"

volatile bool g_EnableAllocHook = true;

int MyAllocHook(int allocType, void* userData, size_t size, int blockType,
	long requestNumber, const unsigned char* filename, int lineNumber) {
	if (g_EnableAllocHook && size == 24 && blockType == _NORMAL_BLOCK && requestNumber > 931) {
		// 命中 24 字节分配，触发断点或输出信息
		_CrtDbgBreak();  // 触发调试中断
		// 或者打印堆栈：可使用 CaptureStackBackTrace 等 API
	}
	return TRUE;  // 继续正常分配
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if(!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(),0))
	{
		return 1;
	}

	//_CrtSetBreakAlloc();

	// 在 main 函数开头启用钩子
	//_CrtSetAllocHook(MyAllocHook);

	//设置主窗口的名字
	uiApp.SetAppName(_T("Test"));

	CPaintManagerUI::SetInstance(hInstance);
	//CPaintManagerUI::SetInstancePath(CPaintManagerUI::GetInstancePath());

	//设置程序为单一执行实例，允许多实例的话，就不需要这行了。
//	uiApp.SetSingleApplication(true, _T("CBD4DD1F-3B18-4DA4-A043-0EA9AEC8D570"));

	//设置xml资源载入方式，选择一种就行	
#ifdef _DEBUG
	uiApp.SetResType_File(_T("skin"));								//从文件夹中载入xml文件
#else
	uiApp.SetResType_ZipResource(_T("skin"), _T(""), IDR_ZIPRES1, _T("ZIPRES"));	//从程序资源文件中载入xml文件
#endif

	CPaintManagerUI::SetRenderEngineType(DuiLib_Render_Sdl);

	//初始化duilib, 注册插件，创建主窗口
	if(!uiApp.InitInstance(0,NULL)) 
		return 0;

	//进入消息循环
	uiApp.Run();

	//清理资源。
	uiApp.ExitInstance();
	return 0;
}

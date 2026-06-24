//
//  main.m
//  TestApp
//
//  Created by test on 2026/6/15.
//

#include "App.h"
#import <Cocoa/Cocoa.h>

int main(int argc, char * argv[]) {
    @autoreleasepool {
        
        NSBundle *bundle = [NSBundle mainBundle];
        NSString *bundlePath = [bundle bundlePath];
        NSString *resDir = [bundle resourcePath];
        // 如果资源放在 App 包的 Contents/Resources/skin 下
        NSString *skinDir = [resDir stringByAppendingPathComponent:@"skin"];
        
        //设置主窗口的名字
        uiApp.SetAppName(_T("Test"));
        
        //设置xml资源载入方式，选择一种就行
        uiApp.SetResType_File(_T("skin"));                                //从文件夹中载入xml文件
        
        CPaintManagerUI::SetInstancePath([bundlePath UTF8String]);
        CPaintManagerUI::SetResourcePath([skinDir UTF8String]);
        CPaintManagerUI::SetRenderEngineType(DuiLib_Render_Sdl);

        //初始化duilib, 注册插件，创建主窗口
        if (!uiApp.InitInstance(argc, argv))
            return 0;

        //进入消息循环
        uiApp.Run();

        //清理资源。
        uiApp.ExitInstance();
    }
    return 0;
}

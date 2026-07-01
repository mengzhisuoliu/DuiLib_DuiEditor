#pragma once
#include "MainFrame.h"

class CApp : public CUIApplication
{
public:
	CApp(void);
	virtual ~CApp(void);

	virtual bool InitInstance(int argc, char* argv[]);
	virtual int ExitInstance();

   virtual void InitResource() override;
};


extern CApp uiApp;
inline CMainFrame *GetMainWnd(){ return (CMainFrame *)uiApp.GetMainWnd(); }
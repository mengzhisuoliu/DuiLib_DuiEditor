#include "StdAfx.h"

#ifdef DUILIB_GTK
namespace DuiLib{

CUIApplicationGtk::CUIApplicationGtk(void)
{

}

CUIApplicationGtk::~CUIApplicationGtk(void)
{

}

bool CUIApplicationGtk::InitInstance(int argc, char* argv[])
{
	m_strAppPath = CPaintManagerUI::GetInstancePath();

	// 初始化资源
	InitResource();

	//gtk_init(&argc, &argv);
	gtk_init(0, NULL);

	//关闭edit焦点边框
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(provider),
		"entry { box-shadow: none; border: none; background-color: transparent; }", -1, NULL);
	GdkDisplay *display = gdk_display_get_default();
	GdkScreen *screen = gdk_display_get_default_screen(display);
	gtk_style_context_add_provider_for_screen(screen,
		GTK_STYLE_PROVIDER(provider),
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	g_object_unref(provider);

	return true;
}

void CUIApplicationGtk::Run()
{
	if (m_pMainWnd)
	{
		m_pMainWnd->m_pApplication = this;
	}

	// 消息循环
	CPaintManagerGtkUI::MessageLoop();
}

int CUIApplicationGtk::ExitInstance()
{
	if (m_pMainWnd != NULL) { delete m_pMainWnd; m_pMainWnd = NULL; }
	CPaintManagerUI::Term();
	return 0;
}

} //namespace DuiLib{
#endif //#ifdef DUILIB_GTK

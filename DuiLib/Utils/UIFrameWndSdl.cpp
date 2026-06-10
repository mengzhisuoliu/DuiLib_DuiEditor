#include "StdAfx.h"

#ifdef DUILIB_SDL
namespace DuiLib{


//////////////////////////////////////////////////////////////////////////
// 静态 HitTest 回调函数
	static SDL_HitTestResult SDLCALL HitTestCallback(SDL_Window* window, const SDL_Point* pt, void* userdata)
	{
		CUIFrameWndSDL* pThis = (CUIFrameWndSDL*)userdata;
		if (!pThis) return SDL_HITTEST_NORMAL;

		// 获取窗口客户区大小
		int w, h;
		SDL_GetWindowSize(window, &w, &h);

		const int border = 4; // 边缘敏感区域宽度

		// 边缘区域检测
		bool onTop = (pt->y < border);
		bool onBottom = (pt->y >= h - border);
		bool onLeft = (pt->x < border);
		bool onRight = (pt->x >= w - border);

		// 角优先判断
		if (onTop && onLeft) return SDL_HITTEST_RESIZE_TOPLEFT;
		if (onTop && onRight) return SDL_HITTEST_RESIZE_TOPRIGHT;
		if (onBottom && onLeft) return SDL_HITTEST_RESIZE_BOTTOMLEFT;
		if (onBottom && onRight) return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
		if (onTop) return SDL_HITTEST_RESIZE_TOP;
		if (onBottom) return SDL_HITTEST_RESIZE_BOTTOM;
		if (onLeft) return SDL_HITTEST_RESIZE_LEFT;
		if (onRight) return SDL_HITTEST_RESIZE_RIGHT;

		// 标题栏区域检测
		CPaintManagerSDLUI* pManager = pThis->GetManager();
		if (pManager)
		{
			CDuiRect rcClient;
			pManager->GetClientRect(&rcClient);
			CDuiRect rcCaption = pManager->GetCaptionRect();
			rcCaption = pManager->GetDPIObj()->ScaleRect(rcCaption);
			rcCaption.left = rcClient.left + rcCaption.left;
			rcCaption.right = rcClient.right - rcCaption.right;
			rcCaption.top = rcClient.top + rcCaption.top;
			if (rcCaption.bottom < 0)
				rcCaption.bottom = rcClient.bottom;
			else
				rcCaption.bottom = rcClient.top + rcCaption.bottom;

			// 判断鼠标是否在标题栏内，且不在任何控件上（可选）
			if (rcCaption.PtInRect(CDuiPoint(pt->x, pt->y)))
			{
				CControlUI* pControl = pManager->FindControl(CDuiPoint(pt->x, pt->y));
				if (pThis->IsInStaticControl(pControl))
					return SDL_HITTEST_DRAGGABLE;
			}
		}

		return SDL_HITTEST_NORMAL;
	}

CUIFrameWndSDL::CUIFrameWndSDL(void)
{
	
}

CUIFrameWndSDL::~CUIFrameWndSDL(void)
{
	
}

LRESULT CUIFrameWndSDL::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return CUIFrameWndBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

LRESULT CUIFrameWndSDL::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 关联UI管理器
	GetManager()->Init(GetHWND(), GetManagerName(), this);
	// 注册PreMessage回调
	GetManager()->AddPreMessageFilter(this);

	// 创建主窗口
	CControlUI* pRoot=NULL;
	CDialogBuilder builder;
	CDuiString sSkinType = GetSkinType();
	if (!sSkinType.IsEmpty()) {
		STRINGorID xml(_ttoi(GetSkinFile().GetData()));
		pRoot = builder.Create(xml, sSkinType, this, GetManager());
	}
	else {
		pRoot = builder.Create(GetSkinFile().GetData(), NULL, this, GetManager());
	}

	if (pRoot == NULL) 
	{
#ifdef WIN32
		CDuiString sError = _T("Loading resource failed :");
		sError += GetSkinFile();
		MessageBox(NULL, sError, _T("Duilib"), MB_OK | MB_ICONERROR);
		ExitProcess(1);
#endif
		return 0;
	}
	GetManager()->AttachDialog(pRoot);
	// 添加Notify事件接口
	GetManager()->AddNotifier(this);

	// 设置 HitTest 回调，用于无边框窗口的拖拽调整大小和移动
	SDL_SetWindowHitTest(GetHWND(), HitTestCallback, this);

	// 窗口初始化完毕
	__InitWindow();

	ShowWindow();
	return 0;
}

LRESULT CUIFrameWndSDL::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDuiPoint ptEvent(lParam);

	CDuiRect rcClient;
	GetManager()->GetClientRect(&rcClient);

	//改变窗口大小
	int sep = 4;
	CDuiRect rcLeft(rcClient.left - sep, rcClient.top, rcClient.left + sep, rcClient.bottom);
	CDuiRect rcTop(rcClient.left, rcClient.top - sep, rcClient.right, rcClient.top + sep);
	CDuiRect rcRight(rcClient.right - sep, rcClient.top, rcClient.right + sep, rcClient.bottom);
	CDuiRect rcBottom(rcClient.left, rcClient.bottom - sep, rcClient.right, rcClient.bottom + sep);
	CDuiRect rcLeftTop(rcClient.left - sep, rcClient.top - sep, rcClient.left + sep, rcClient.top + sep);
	CDuiRect rcLeftBottom(rcClient.left - sep, rcClient.bottom - sep, rcClient.left + sep, rcClient.bottom + sep);
	CDuiRect rcRightTop(rcClient.right - sep, rcClient.top - sep, rcClient.right + sep, rcClient.top + sep);
	CDuiRect rcRightBottom(rcClient.right - sep, rcClient.bottom - sep, rcClient.right + sep, rcClient.bottom + sep);

	if (rcLeft.PtInRect(ptEvent) || rcTop.PtInRect(ptEvent) ||
		rcRight.PtInRect(ptEvent) || rcBottom.PtInRect(ptEvent) ||
		rcLeftTop.PtInRect(ptEvent) || rcLeftBottom.PtInRect(ptEvent) ||
		rcRightTop.PtInRect(ptEvent) || rcRightBottom.PtInRect(ptEvent))
	{
		if (rcLeftTop.PtInRect(ptEvent))				SetCursor(DUI_SIZENWSE);
		else if (rcRightBottom.PtInRect(ptEvent))	SetCursor(DUI_SIZENWSE);
		else if (rcRightTop.PtInRect(ptEvent))		SetCursor(DUI_SIZENESW);
		else if (rcLeftBottom.PtInRect(ptEvent))		SetCursor(DUI_SIZENESW);
		else if (rcLeft.PtInRect(ptEvent))			SetCursor(DUI_SIZEWE);
		else if (rcTop.PtInRect(ptEvent))			SetCursor(DUI_SIZENS);
		else if (rcRight.PtInRect(ptEvent))			SetCursor(DUI_SIZEWE);
		else if (rcBottom.PtInRect(ptEvent))			SetCursor(DUI_SIZENS);

		bHandled = TRUE;
		return 0;
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CUIFrameWndSDL::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == SC_MINIMIZE)
	{
		SDL_MinimizeWindow(m_hWnd);
	}
	else if (wParam == SC_MAXIMIZE)
	{
		if (SDL_MaximizeWindow(m_hWnd))
		{
			CControlUI* pControl = static_cast<CControlUI*>(GetManager()->FindControl(_T("windowmaxbtn")));
			if (pControl) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(GetManager()->FindControl(_T("windowrestorebtn")));
			if (pControl) pControl->SetVisible(true);
		}
	}
	else if (wParam == SC_RESTORE)
	{
		if (SDL_RestoreWindow(m_hWnd))
		{
			CControlUI* pControl = static_cast<CControlUI*>(GetManager()->FindControl(_T("windowmaxbtn")));
			if (pControl) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(GetManager()->FindControl(_T("windowrestorebtn")));
			if (pControl) pControl->SetVisible(false);
		}
	}

	return 0;
}

} //namespace DuiLib{
#endif //#ifdef DUILIB_SDL

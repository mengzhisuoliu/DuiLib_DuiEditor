#include "StdAfx.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

void UILIB_API DUI__Trace(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
	TCHAR szBuffer[2048] = {0};
    va_list args;
    va_start(args, pstrFormat);
	_vsntprintf(szBuffer, 2047, pstrFormat, args); 
    va_end(args);
    
	_tcscat(szBuffer, _T("\n"));
	#ifdef DUILIB_WIN32
		CDuiString line;
		line.Format(_T("%d: "), ::GetTickCount());
		line += szBuffer;
		OutputDebugString(line);
	#endif //#ifdef WIN32

	#ifdef DUILIB_SDL
		CDuiStringUtf8 line;
		line.Format("%d: ", SDL_GetTicks());
		line += CDuiString(szBuffer);
		SDL_Log(line.toString());
	#endif
#endif //#ifdef _DEBUG
}

LPCTSTR DUI__TraceMsg(UINT uMsg)
{
#define MSGDEF(x) if(uMsg==x) return _T(#x)
    MSGDEF(WM_SETCURSOR);
    MSGDEF(WM_NCHITTEST);
    MSGDEF(WM_NCPAINT);
    MSGDEF(WM_PAINT);
    MSGDEF(WM_ERASEBKGND);
    MSGDEF(WM_NCMOUSEMOVE);  
    MSGDEF(WM_MOUSEMOVE);
    MSGDEF(WM_MOUSELEAVE);
    MSGDEF(WM_MOUSEHOVER);   
    MSGDEF(WM_NOTIFY);
    MSGDEF(WM_COMMAND);
    MSGDEF(WM_MEASUREITEM);
    MSGDEF(WM_DRAWITEM);   
    MSGDEF(WM_LBUTTONDOWN);
    MSGDEF(WM_LBUTTONUP);
    MSGDEF(WM_LBUTTONDBLCLK);
    MSGDEF(WM_RBUTTONDOWN);
    MSGDEF(WM_RBUTTONUP);
    MSGDEF(WM_RBUTTONDBLCLK);
    MSGDEF(WM_SETFOCUS);
    MSGDEF(WM_KILLFOCUS);  
    MSGDEF(WM_MOVE);
    MSGDEF(WM_SIZE);
    MSGDEF(WM_SIZING);
    MSGDEF(WM_MOVING);
    MSGDEF(WM_GETMINMAXINFO);
    MSGDEF(WM_CAPTURECHANGED);
    MSGDEF(WM_WINDOWPOSCHANGED);
    MSGDEF(WM_WINDOWPOSCHANGING);   
    MSGDEF(WM_NCCALCSIZE);
    MSGDEF(WM_NCCREATE);
    MSGDEF(WM_NCDESTROY);
    MSGDEF(WM_TIMER);
    MSGDEF(WM_KEYDOWN);
    MSGDEF(WM_KEYUP);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SYSKEYDOWN);
    MSGDEF(WM_SYSKEYUP);
    MSGDEF(WM_SYSCOMMAND);
    MSGDEF(WM_SYSCHAR);
    MSGDEF(WM_VSCROLL);
    MSGDEF(WM_HSCROLL);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SHOWWINDOW);
    MSGDEF(WM_PARENTNOTIFY);
    MSGDEF(WM_CREATE);
    MSGDEF(WM_NCACTIVATE);
    MSGDEF(WM_ACTIVATE);
    MSGDEF(WM_ACTIVATEAPP);   
    MSGDEF(WM_CLOSE);
    MSGDEF(WM_DESTROY);
    MSGDEF(WM_GETICON);   
    MSGDEF(WM_GETTEXT);
    MSGDEF(WM_GETTEXTLENGTH);   
    static TCHAR szMsg[10];
	_stprintf(szMsg, _T("0x%04X"), uMsg);
    return szMsg;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

//////////////////////////////////////////////////////////////////////////
//
DUI_BASE_BEGIN_MESSAGE_MAP(CNotifyPump)
DUI_END_MESSAGE_MAP()

static const DUI_MSGMAP_ENTRY* DuiFindMessageEntry(const DUI_MSGMAP_ENTRY* lpEntry,TNotifyUI& msg )
{
	CDuiString sMsgType = msg.sType;
	CDuiString sCtrlName = msg.pSender->GetName();
	const DUI_MSGMAP_ENTRY* pMsgTypeEntry = NULL;
	while (lpEntry->nSig != DuiSig_end)
	{
		if(lpEntry->sMsgType==sMsgType)
		{
			if(!lpEntry->sCtrlName.IsEmpty())
			{
				if(lpEntry->sCtrlName==sCtrlName)
				{
					return lpEntry;
				}
			}
			else
			{
				pMsgTypeEntry = lpEntry;
			}
		}
		lpEntry++;
	}
	return pMsgTypeEntry;
}

bool CNotifyPump::AddVirtualWnd(CDuiString strName,CNotifyPump* pObject)
{
	if( m_VirtualWndMap.Find(strName) == NULL )
	{
		m_VirtualWndMap.Insert(strName.GetData(),(LPVOID)pObject);
		return true;
	}
	return false;
}

bool CNotifyPump::RemoveVirtualWnd(CDuiString strName)
{
	if( m_VirtualWndMap.Find(strName) != NULL )
	{
		m_VirtualWndMap.Remove(strName);
		return true;
	}
	return false;
}

bool CNotifyPump::LoopDispatch(TNotifyUI& msg)
{
	const DUI_MSGMAP_ENTRY* lpEntry = NULL;
	const DUI_MSGMAP* pMessageMap = NULL;

#ifndef UILIB_STATIC
	for(pMessageMap = GetMessageMap(); pMessageMap!=NULL; pMessageMap = (*pMessageMap->pfnGetBaseMap)())
#else
	for(pMessageMap = GetMessageMap(); pMessageMap!=NULL; pMessageMap = pMessageMap->pBaseMap)
#endif
	{
#ifndef UILIB_STATIC
		ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
#else
		ASSERT(pMessageMap != pMessageMap->pBaseMap);
#endif
		if ((lpEntry = DuiFindMessageEntry(pMessageMap->lpEntries,msg)) != NULL)
		{
			goto LDispatch;
		}
	}
	return false;

LDispatch:
	union DuiMessageMapFunctions mmf;
	mmf.pfn = lpEntry->pfn;

	bool bRet = false;
	int nSig;
	nSig = lpEntry->nSig;
	switch (nSig)
	{
	default:
		ASSERT(uiFalse);
		break;
	case DuiSig_lwl:
		(this->*mmf.pfn_Notify_lwl)(msg.wParam,msg.lParam);
		bRet = true;
		break;
	case DuiSig_vn:
		(this->*mmf.pfn_Notify_vn)(msg);
		bRet = true;
		break;
	}
	return bRet;
}

void CNotifyPump::NotifyPump(TNotifyUI& msg)
{
	///遍历虚拟窗口
	if( !msg.sVirtualWnd.IsEmpty() ){
		for( int i = 0; i< m_VirtualWndMap.GetSize(); i++ ) {
			if( LPCTSTR key = m_VirtualWndMap.GetAt(i) ) {
				if( _tcsicmp(key, msg.sVirtualWnd.GetData()) == 0 ){
					CNotifyPump* pObject = static_cast<CNotifyPump*>(m_VirtualWndMap.Find(key, false));
					if( pObject && pObject->LoopDispatch(msg) )
						return;
				}
			}
		}
	}

	///
	//遍历主窗口
	LoopDispatch( msg );
}

//////////////////////////////////////////////////////////////////////////
///
CMacroToStringMap CWindowWnd::m_wmEventString;
CWindowWnd::CWindowWnd() : m_hWnd(NULL)
{
	if (m_wmEventString.GetSize() <= 0)
	{
		_init_wm_defined();
	}
}

CWindowWnd::~CWindowWnd()
{
}

UIWND CWindowWnd::GetHWND() const 
{ 
	return m_hWnd; 
}

CWindowWnd::operator UIWND() const
{
	return m_hWnd;
}

CPaintManagerUI *CWindowWnd::GetManager()
{
	return &m_pm;
}

LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	uiBool bHandled = uiTrue;
	switch (uMsg)
	{
	case WM_CREATE:			
		lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:			
		lRes = OnSize(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_CHAR:			lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
	case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
	case WM_RBUTTONUP:		lRes = OnRButtonUp(uMsg, wParam, lParam, bHandled); break;
	case WM_RBUTTONDOWN:	lRes = OnRButtonDown(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEHOVER:		lRes = OnMouseHover(uMsg, wParam, lParam, bHandled); break;
	case WM_TIMER:			lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEACTIVATE:	lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_ERASEBKGND:		lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
	case WM_PAINT:			lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_PRINT:			lRes = OnPrint(uMsg, wParam, lParam, bHandled); break;
	default:				bHandled = uiFalse; break;
	}

	if (bHandled) return lRes;

	lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	if (bHandled) return lRes;

	//菜单命令
	lRes = HandleMenuCommandMessage(uMsg, wParam, lParam, bHandled);
	if (bHandled) return lRes;

	if (GetManager()->MessageHandler(uMsg, wParam, lParam, lRes))
	{
		return lRes;
	}

	return DefaultWndProc(uMsg, wParam, lParam);
}


LRESULT CWindowWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::HandleMenuCommandMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{	
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}


LRESULT CWindowWnd::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

LRESULT CWindowWnd::OnPrint(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled)
{
	bHandled = uiFalse;
	return 0;
}

void CWindowWnd::_init_wm_defined()
{
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NULL);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CREATE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_DESTROY);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOVE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SIZE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_ACTIVATE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SETFOCUS);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_KILLFOCUS);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_ENABLE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SETREDRAW);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SETTEXT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_GETTEXT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_GETTEXTLENGTH);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_PAINT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CLOSE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_QUERYENDSESSION);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_QUERYOPEN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_ENDSESSION);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_QUIT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_ERASEBKGND);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SYSCOLORCHANGE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SHOWWINDOW);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_WININICHANGE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_DEVMODECHANGE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_ACTIVATEAPP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_FONTCHANGE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_TIMECHANGE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CANCELMODE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SETCURSOR);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOUSEACTIVATE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CHILDACTIVATE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_QUEUESYNC);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_GETMINMAXINFO);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_PAINTICON);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_ICONERASEBKGND);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NEXTDLGCTL);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SPOOLERSTATUS);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_DRAWITEM);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MEASUREITEM);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_DELETEITEM);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_VKEYTOITEM);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CHARTOITEM);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SETFONT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_GETFONT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SETHOTKEY);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_GETHOTKEY);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_QUERYDRAGICON);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_COMPAREITEM);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_COMPACTING);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_COMMNOTIFY);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_WINDOWPOSCHANGING);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_WINDOWPOSCHANGED);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_POWER);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_COPYDATA);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CANCELJOURNAL);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NOTIFY);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_INPUTLANGCHANGEREQUEST);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_INPUTLANGCHANGE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_TCARD);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_HELP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_USERCHANGED);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NOTIFYFORMAT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CONTEXTMENU);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_STYLECHANGING);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_STYLECHANGED);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_DISPLAYCHANGE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_GETICON);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SETICON);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCCREATE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCDESTROY);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCCALCSIZE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCHITTEST);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCPAINT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCACTIVATE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_GETDLGCODE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCMOUSEMOVE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCLBUTTONDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCLBUTTONUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCLBUTTONDBLCLK);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCRBUTTONDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCRBUTTONUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCRBUTTONDBLCLK);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCMBUTTONDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCMBUTTONUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCMBUTTONDBLCLK);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCXBUTTONDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCXBUTTONUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCXBUTTONDBLCLK);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_KEYFIRST);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_KEYDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_KEYUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CHAR);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_DEADCHAR);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SYSKEYDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SYSKEYUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SYSCHAR);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SYSDEADCHAR);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_INITDIALOG);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_COMMAND);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SYSCOMMAND);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_TIMER);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_HSCROLL);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_VSCROLL);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_INITMENU);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_INITMENUPOPUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MENUSELECT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MENUCHAR);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_ENTERIDLE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MENURBUTTONUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MENUDRAG);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MENUGETOBJECT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_UNINITMENUPOPUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MENUCOMMAND);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CTLCOLORMSGBOX);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CTLCOLOREDIT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CTLCOLORLISTBOX);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CTLCOLORBTN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CTLCOLORDLG);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CTLCOLORSCROLLBAR);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CTLCOLORSTATIC);
	MACROTOSTRINGMAP_ADD(m_wmEventString, MN_GETHMENU);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOUSEFIRST);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOUSEMOVE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_LBUTTONDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_LBUTTONUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_LBUTTONDBLCLK);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_RBUTTONDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_RBUTTONUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_RBUTTONDBLCLK);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MBUTTONDOWN);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MBUTTONUP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MBUTTONDBLCLK);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOUSEWHEEL);
#ifdef WM_MOUSEHWHEEL
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOUSEHWHEEL);
#else
	m_wmEventString.Add(0x020E, _T("WM_MOUSEHWHEEL"));
#endif
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_PARENTNOTIFY);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_ENTERMENULOOP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_EXITMENULOOP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NEXTMENU);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_SIZING);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_CAPTURECHANGED);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOVING);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOUSEHOVER);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_MOUSELEAVE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCMOUSEHOVER);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_NCMOUSELEAVE);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_PRINT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_PRINTCLIENT);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_APP);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_APP + 1);
	MACROTOSTRINGMAP_ADD(m_wmEventString, WM_USER);
}

} // namespace DuiLib


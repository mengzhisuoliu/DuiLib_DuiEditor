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
		ASSERT(FALSE);
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
CWindowWnd::CWindowWnd() : m_hWnd(NULL), m_bHandleMessage(FALSE)
{
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

void CWindowWnd::SetHandleMessage(BOOL bHandled)
{
	m_bHandleMessage = bHandled;
}

BOOL CWindowWnd::IsHandleMessage()
{
	return m_bHandleMessage;
}

LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	SetHandleMessage(TRUE);
	switch (uMsg)
	{
	case WM_CREATE:			
		lRes = OnCreate(uMsg, wParam, lParam, m_bHandleMessage); 
		break;
	case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_SIZE:			
		lRes = OnSize(uMsg, wParam, lParam, m_bHandleMessage); 
		break;
	case WM_CHAR:			lRes = OnChar(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_RBUTTONUP:		lRes = OnRButtonUp(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_RBUTTONDOWN:	lRes = OnRButtonDown(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, m_bHandleMessage); break;
	case WM_MOUSEHOVER:		lRes = OnMouseHover(uMsg, wParam, lParam, m_bHandleMessage); break;
	default:				m_bHandleMessage = FALSE; break;
	}
	if (IsHandleMessage()) return lRes;

	lRes = HandleCustomMessage(uMsg, wParam, lParam, m_bHandleMessage);
	if (m_bHandleMessage) return lRes;

	//菜单命令
	lRes = HandleMenuCommandMessage(uMsg, wParam, lParam, m_bHandleMessage);
	if (m_bHandleMessage) return lRes;

	if (GetManager()->MessageHandler(uMsg, wParam, lParam, lRes))
	{
		SetHandleMessage(TRUE);
		return lRes;
	}

	return 0;
}


LRESULT CWindowWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::HandleMenuCommandMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	return 0;
}

LRESULT CWindowWnd::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}


LRESULT CWindowWnd::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

LRESULT CWindowWnd::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetHandleMessage(FALSE);
	return 0;
}

} // namespace DuiLib


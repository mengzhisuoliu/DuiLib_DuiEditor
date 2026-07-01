#pragma once

#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

#ifdef DUILIB_WIN32
namespace DuiLib
{
	class UILIB_API WindowImplBase
		: public CWindowWin32
		, public CNotifyPump
		, public INotifyUI
		, public IMessageFilterUI
		, public IDialogBuilderCallback
		, public IQueryControlText
	{
	public:
		WindowImplBase();
		virtual ~WindowImplBase(){};
		// 只需主窗口重写（初始化资源与多语言接口）
		virtual void InitResource(){};
		// 每个窗口都可以重写
		virtual void __InitWindow(){};
		virtual void OnFinalMessage( HWND hWnd );
		virtual void Notify(TNotifyUI& msg);

		//modify by liqs99
		//DUI_DECLARE_MESSAGE_MAP()
		//virtual void OnClick(TNotifyUI& msg);
		virtual uiBool IsInStaticControl(CControlUI *pControl);
	protected:
		std::vector<CDuiString> m_vctStaticName;
	protected:
		virtual CDuiString GetSkinType() { return _T(""); }
		virtual CDuiString GetSkinFile() = 0;
		virtual LPCTSTR GetWindowClassName(void) const = 0 ;
		virtual LPCTSTR GetManagerName() { return NULL; }
		virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

	public:
		virtual UINT GetClassStyle() const;
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);
		virtual CDuiString QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType);

		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);
		virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
		virtual LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		virtual LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& /*bHandled*/);
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		virtual LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
#endif
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		virtual LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, uiBool& bHandled);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled);
		virtual LONG GetStyle();
	};
}

#endif //#ifdef DUILIB_WIN32
#endif // WIN_IMPL_BASE_HPP


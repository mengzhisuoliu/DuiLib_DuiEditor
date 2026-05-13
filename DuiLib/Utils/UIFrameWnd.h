#pragma once
#include <list>

namespace DuiLib {

typedef MenuCmd MENUCOMMAND;

class CUIForm;
class UILIB_API CUIFrameWndBase 
	: public CUIFrmBase
	, public DuiLibWindowWnd
	, public IDialogBuilderCallback
	, public IQueryControlText
	, public IMessageFilterUI
{
public:
	CUIFrameWndBase(void);
	virtual ~CUIFrameWndBase(void);

	virtual DuiLibPaintManagerUI *GetManager() override;
	virtual void OnFinalMessage( UIWND hWnd ) override;

	virtual LPCTSTR GetWindowClassName() const = 0;
	virtual CDuiString GetSkinFile() = 0;
	virtual CDuiString GetSkinType() { return _T(""); }
	virtual LPCTSTR GetManagerName() { return NULL; }

	virtual CControlUI* CreateControl(LPCTSTR pstrClass) override { return NULL; }

	virtual CDuiString QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) { return _T(""); }

	CControlUI *GetRoot(); 
	void AttachVirtualForm(CUIFrmBase *pForm);
	void DetachVirtualForm(CUIFrmBase *pForm);

	virtual void Notify(TNotifyUI& msg) override;

	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);;
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);;
	virtual LRESULT HandleMenuCommandMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	virtual void InitWindow(){}

	virtual BOOL IsInStaticControl(CControlUI* pControl);
protected:
	virtual void __InitWindow();
	virtual void UIAction(TUIAction *act, bool bAsync);

public:
	CUIApplication *m_pApplication;

protected:
	std::vector<CDuiString> m_vctStaticName;
	CStdPtrArray	m_listForm;
};

} //namespace DuiLib {
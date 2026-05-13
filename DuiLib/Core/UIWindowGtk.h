#ifndef __UI_WINDOW_GTK_H__
#define __UI_WINDOW_GTK_H__


#pragma once

#ifdef DUILIB_GTK
namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//
class UILIB_API CWindowGtk : public CWindowWnd
{
public:
	CWindowGtk();
	virtual ~CWindowGtk();

	virtual UIWND Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc);
	virtual UIWND Create(UIWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = 0, int y = 0, int cx = 0, int cy = 0);
	UIWND CreateDuiWindow(UIWND hwndParent, LPCTSTR pstrWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);

	static BOOL IsWindow(UIWND hWnd);
	static LRESULT SendMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
	static BOOL PostMessage(UIWND hWnd, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);

	virtual BOOL SetWindowPos(int x, int y, int cx, int cy, UINT uFlags) override;
	virtual BOOL GetWindowRect(LPRECT lpRect) override;
	virtual BOOL GetClientRect(LPRECT lpRect) override;

	virtual void Close(UINT nRet = IDOK) override;
	virtual void SetCursor(int nCursor) override;
	void ShowWindow(bool bShow = true, bool bTakeFocus = true);

	void CenterWindow();	// 居中，支持扩展屏幕

	PVOID GetFixedContainer();

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
	void RegisterSignal();
protected:
	PVOID m_pGtkFixed;
};	

} // namespace DuiLib
#endif //#ifdef DUILIB_GTK

#endif // __UIBASE_H__

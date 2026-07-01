#pragma once
#include <list>

#ifdef DUILIB_SDL
namespace DuiLib {

class UILIB_API CUIFrameWndSDL : public CUIFrameWndBase
{
public:
	CUIFrameWndSDL(void);
	virtual ~CUIFrameWndSDL(void);

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, uiBool& bHandled) override;
protected:
};

} //namespace DuiLib {
#endif //#ifdef DUILIB_SDL


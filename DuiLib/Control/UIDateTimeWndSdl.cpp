#include "StdAfx.h"
#include "UIDateTimeWndSdl.h"

#ifdef DUILIB_SDL

namespace DuiLib
{
	CDateTimeWndSDL::CDateTimeWndSDL()
	{
	}

	void CDateTimeWndSDL::Init(CDateTimeUI* pOwner, RECT rcBase, UINT uFormatStyle)
	{
		
	}

	RECT CDateTimeWndSDL::CalPos(RECT rcBase)
	{
		CDuiRect rcPos = rcBase;//m_pOwner->GetPos();

		CControlUI* pParent = m_pOwner;
		RECT rcParent;
		while (pParent = pParent->GetParent()) {
			if (!pParent->IsVisible()) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
			rcParent = pParent->GetClientPos();
			//if( !::IntersectRect(&rcPos, &rcPos, &rcParent) ) {
			if (!rcPos.Intersect(rcPos, rcParent)) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
		}

		return rcPos;
	}
}

#endif //#ifdef DUILIB_SDL

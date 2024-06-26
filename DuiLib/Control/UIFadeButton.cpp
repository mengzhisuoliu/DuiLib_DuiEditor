#include "StdAfx.h"
#include "UIFadeButton.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL(CFadeButtonUI)

	CFadeButtonUI::CFadeButtonUI() : m_bMouseHove( FALSE ), m_bMouseLeave( FALSE )
	{
	}

	CFadeButtonUI::~CFadeButtonUI()
	{
		StopAnimation();
	}

	LPCTSTR CFadeButtonUI::GetClass() const
	{
		return _T("FadeButtonUI");
	}

	LPVOID CFadeButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("FadeButton")) == 0 ) 
			return static_cast<CFadeButtonUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void CFadeButtonUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		m_sLastImage = m_sNormalImage;
	}

	void CFadeButtonUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_MOUSEENTER && !IsAnimationRunning( FADE_IN_ID ) )
		{
			m_bFadeAlpha = 0;
			m_bMouseLeave = FALSE;
			m_bMouseHove = TRUE;
			StopAnimation( FADE_OUT_ID );
			StartAnimation( FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_IN_ID );
			Invalidate();
			return;
		}
		else if( event.Type == UIEVENT_MOUSELEAVE && !IsAnimationRunning( FADE_OUT_ID ) )
		{
			m_bFadeAlpha = 0;
			m_bMouseHove = FALSE;
			m_bMouseLeave = TRUE;
			StopAnimation(FADE_IN_ID);
			StartAnimation(FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_OUT_ID);
			Invalidate();
			return;
		}
		else if( event.Type == UIEVENT_TIMER ) 
		{
			OnTimer(  event.wParam );
		}
		CButtonUI::DoEvent( event );
	}

	void CFadeButtonUI::OnTimer( int nTimerID )
	{
		OnAnimationElapse( nTimerID );
	}

	void CFadeButtonUI::PaintStatusImage(UIRender *pRender)
	{
		if( !IsEnabled() ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(pRender, (LPCTSTR)m_sDisabledImage) ) {}
				else return;
			}
		}
		else if( IsPushedState() ) {
			if( !m_sPushedImage.IsEmpty() ) {
				if( !DrawImage(pRender, (LPCTSTR)m_sPushedImage) ) {}
				else return;
			}
		}
		else if( IsFocused() ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(pRender, (LPCTSTR)m_sFocusedImage) ) {}
				else return;
			}
		}  

		if( !m_sNormalImage.IsEmpty() ) {
			if( IsAnimationRunning(FADE_IN_ID) || IsAnimationRunning(FADE_OUT_ID)) {
    CDuiString sFadeOut, sFadeIn;
    sFadeIn.Format(_T("fade='255'"));
				 sFadeOut.Format(_T("fade='%d'"), m_bFadeAlpha);
 
				if( IsAnimationRunning(FADE_OUT_ID) ) {
				 sFadeOut.Format(_T("fade='%d'"), 255 - m_bFadeAlpha); 
				}
				 
		 	if( !DrawImage(pRender, (LPCTSTR)m_sNormalImage, sFadeIn) ) {}
				if( !DrawImage(pRender, (LPCTSTR)m_sHotImage, sFadeOut) ) {}
	
				return;
			}
			else {
				if( m_bMouseHove ) {
					m_bMouseHove = FALSE;
					m_sLastImage = m_sHotImage;
					if( !DrawImage(pRender, (LPCTSTR)m_sHotImage) ) {}
					return;
				}

				if( m_bMouseLeave ) {
					m_bMouseLeave = FALSE;
					m_sLastImage = m_sNormalImage;
					if( !DrawImage(pRender, (LPCTSTR)m_sNormalImage) ) {}
					return;
				}

				if(m_sLastImage.IsEmpty()) m_sLastImage = m_sNormalImage;
				if( !DrawImage(pRender, (LPCTSTR)m_sLastImage) ) {}
				return;
			}
		}
	}

	void CFadeButtonUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
	{
		m_bFadeAlpha = (BYTE)((nCurFrame / (double)nTotalFrame) * 255);
	//	m_bFadeAlpha = m_bFadeAlpha == 0 ? 10 : m_bFadeAlpha;
		Invalidate();
	}

} // namespace DuiLib

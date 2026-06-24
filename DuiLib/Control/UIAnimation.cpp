#include "StdAfx.h"
#include "UIAnimation.h"
#include <vector>
#include <algorithm>

namespace DuiLib {
	struct CUIAnimation::Imp
	{
		std::vector<CAnimationData*> m_arAnimations;
	};
	
	CUIAnimation::CUIAnimation(CControlUI* pOwner):m_pImp(new CUIAnimation::Imp())
	{
		ASSERT(pOwner != NULL);
		m_pControl = pOwner;
	}
	CUIAnimation:: ~CUIAnimation()
	{
		if(m_pImp)
		{
			delete m_pImp;
			m_pImp = NULL;
		}
	}

	void CUIAnimation::AttachAnimationControl(CControlUI* pOwner)
	{
		m_pControl = pOwner;
	}

	uiBool CUIAnimation::StartAnimation(int nElapse, int nTotalFrame, int nAnimationID /*= 0*/, uiBool bLoop/* = FALSE*/)
	{
		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL != pData 
			|| nElapse <= 0
			|| nTotalFrame <= 0
			|| NULL == m_pControl )
		{
			ASSERT(uiFalse);
			return uiFalse;
		}

		CAnimationData* pAnimation = new CAnimationData(nElapse, nTotalFrame, nAnimationID, bLoop);
		if( NULL == pAnimation ) return uiFalse;
		
		if(m_pControl->GetManager()->SetTimer( m_pControl, nAnimationID, nElapse ))
		{
			m_pImp->m_arAnimations.push_back(pAnimation);
			return uiTrue;
		}
		return uiFalse;
	}

	void CUIAnimation::StopAnimation(int nAnimationID /*= 0*/)
	{
		if(m_pControl == NULL) return;

		if(nAnimationID  != 0)
		{
			CAnimationData* pData = GetAnimationDataByID(nAnimationID);
			if( NULL != pData )
			{
				m_pControl->GetManager()->KillTimer( m_pControl, nAnimationID );
				m_pImp->m_arAnimations.erase(std::remove(m_pImp->m_arAnimations.begin(), m_pImp->m_arAnimations.end(), pData), m_pImp->m_arAnimations.end());
				if(pData != NULL){
					delete pData;
					pData = NULL;
				}
				return;
			}
		}
		else
		{
			int nCount = static_cast<int>(m_pImp->m_arAnimations.size());
			for(int i=0; i<nCount; ++i)
			{
				CAnimationData* pData = m_pImp->m_arAnimations[i];
				if(pData) {
					m_pControl->GetManager()->KillTimer(m_pControl, pData->m_nAnimationID);
					if(pData != NULL){
						delete pData;
						pData = NULL;
					}
				}
			}
			m_pImp->m_arAnimations.clear();
		}
	}

	uiBool CUIAnimation::IsAnimationRunning(int nAnimationID)
	{
		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		return NULL != pData;
	}

	int CUIAnimation::GetCurrentFrame(int nAnimationID/* = 0*/)
	{
		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData )
		{
			ASSERT(uiFalse);
			return -1;
		}
		return pData->m_nCurFrame;
	}

	uiBool CUIAnimation::SetCurrentFrame(int nFrame, int nAnimationID/* = 0*/)
	{
		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData)
		{
			ASSERT(uiFalse);
			return uiFalse;
		}

		if(nFrame >= 0 && nFrame <= pData->m_nTotalFrame)
		{
			pData->m_nCurFrame = nFrame;
			return uiTrue;
		}
		else
		{
			ASSERT(uiFalse);
		}
		return uiFalse;
	}

	void CUIAnimation::OnAnimationElapse(int nAnimationID)
	{
		if(m_pControl == NULL) return;

		CAnimationData* pData = GetAnimationDataByID(nAnimationID);
		if( NULL == pData ) return;

		int nCurFrame = pData->m_nCurFrame;
		if(nCurFrame == 0)
		{
			OnAnimationStart(nAnimationID, pData->m_bFirstLoop);
			pData->m_bFirstLoop = uiFalse;
		}

		OnAnimationStep(pData->m_nTotalFrame, nCurFrame, nAnimationID);

		if(nCurFrame >= pData->m_nTotalFrame)
		{
			OnAnimationStop(nAnimationID);
			if(pData->m_bLoop)
			{
				pData->m_nCurFrame = 0;
			}
			else
			{
				m_pControl->GetManager()->KillTimer( m_pControl, nAnimationID );
				m_pImp->m_arAnimations.erase(std::remove(m_pImp->m_arAnimations.begin(), m_pImp->m_arAnimations.end(), pData), m_pImp->m_arAnimations.end());
				delete pData;
				pData = NULL;
			}
		}

		if( NULL != pData )
		{
			++(pData->m_nCurFrame);
		}
	}

	CAnimationData* CUIAnimation::GetAnimationDataByID(int nAnimationID)
	{
		CAnimationData* pRet = NULL;
		int nCount = static_cast<int>(m_pImp->m_arAnimations.size());
		for(int i=0; i<nCount; ++i)
		{
			if(m_pImp->m_arAnimations[i]->m_nAnimationID == nAnimationID)
			{
				pRet = m_pImp->m_arAnimations[i];
				break;
			}
		}

		return pRet;
	}

} // namespace DuiLib

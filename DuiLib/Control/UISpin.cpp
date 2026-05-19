#include "StdAfx.h"
#include "UISpin.h"

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CSpinUI)

	CSpinUI::CSpinUI()
	: m_nMin(0)
	, m_nMax(0)
	, m_nStep(1)
	{
		SetLayout(Layout_VerticalLayout);
		Add(m_btnUp = new CButtonUI);
		Add(m_btnDown = new CButtonUI);
		m_btnUp->OnEvent += MakeDelegate(this, &CSpinUI::OnClick);
		m_btnDown->OnEvent += MakeDelegate(this, &CSpinUI::OnClick);
	}

	CSpinUI::~CSpinUI()
	{

	}

	LPCTSTR CSpinUI::GetClass() const
	{
		return _T("SpinUI");
	}

	LPVOID CSpinUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_SPIN) == 0)
			return static_cast<CSpinUI*>(this);
		return CDynamicLayoutUI::GetInterface(pstrName);
	}

	void CSpinUI::SetMin(int nMin)
	{
		m_nMin = nMin;
	}

	int CSpinUI::GetMin() const
	{
		return m_nMin;
	}

	void CSpinUI::SetMax(int nMax)
	{
		m_nMax = nMax;
	}

	int CSpinUI::GetMax() const
	{
		return m_nMax;
	}

	void CSpinUI::SetStep(int nStep)
	{
		if (nStep <= 0) nStep = 1;
		m_nStep = nStep;
	}

	int CSpinUI::GetStep() const
	{
		return m_nStep;
	}

	void CSpinUI::SetBindControlName(LPCTSTR sBindName)
	{
		m_sBindControlName = sBindName;
	}

	CDuiString CSpinUI::GetBindControlName()
	{
		return m_sBindControlName;
	}

	void CSpinUI::DoEvent(TEventUI& event)
	{
		if(event.Type == UIEVENT_TIMER)
		{
			if(event.wParam == 101)
			{
				UpdateText(true);
			}
			else if(event.wParam == 102)
			{
				UpdateText(false);
			}
		}
		CDynamicLayoutUI::DoEvent(event);
	}

	void CSpinUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("min")) == 0)
			SetMin(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("max")) == 0)
			SetMax(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("step")) == 0)
			SetStep(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("bindcontrolname")) == 0)
			SetBindControlName(pstrValue);
		else if (_tcsicmp(pstrName, _T("btnup")) == 0)
			m_btnUp->ApplyAttributeList(pstrValue);
		else if (_tcsicmp(pstrName, _T("btndown")) == 0)
			m_btnDown->ApplyAttributeList(pstrValue);
		else
			CDynamicLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	bool CSpinUI::OnClick(void* param)
	{
		TEventUI *pEvent = (TEventUI *)param;
		if( pEvent->Type == UIEVENT_BUTTONDOWN ) 
		{
			if(pEvent->pSender == m_btnUp)
			{
				UpdateText(true);
				GetManager()->SetTimer(this, 101, 200);
			}
			else if(pEvent->pSender == m_btnDown)
			{
				UpdateText(false);
				GetManager()->SetTimer(this, 102, 200);
			}
		}
		if( pEvent->Type == UIEVENT_BUTTONUP )
		{
			if(pEvent->pSender == m_btnUp)
				GetManager()->KillTimer(this, 101);
			else if(pEvent->pSender == m_btnDown)
				GetManager()->KillTimer(this, 102);
		}
		return true;
	}

	void CSpinUI::UpdateText(bool bUp)
	{
		CControlUI *pControl = GetManager()->FindControl(m_sBindControlName);
		if(pControl)
		{
			int nInterger = pControl->GetText().toInt();

			if(bUp)
				nInterger++;
			else
				nInterger--;

			if(m_nMax > m_nMin)
			{
				if(nInterger > m_nMax) nInterger = m_nMax;
				if(nInterger < m_nMin) nInterger = m_nMin;
			}

			pControl->SetTextN(nInterger);
		}
	}
}
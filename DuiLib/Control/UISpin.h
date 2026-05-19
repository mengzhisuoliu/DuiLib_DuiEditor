#ifndef __UISPIN_H__
#define __UISPIN_H__

#pragma once

namespace DuiLib
{
    class UILIB_API CSpinUI : public CDynamicLayoutUI
    {
        DECLARE_DUICONTROL(CSpinUI)

    public:
        CSpinUI();
        virtual ~CSpinUI();

        virtual LPCTSTR GetClass() const override;
        virtual LPVOID GetInterface(LPCTSTR pstrName) override;

        void SetMin(int nMin);
        int GetMin() const;
        void SetMax(int nMax);
        int GetMax() const;
        void SetStep(int nStep);
        int GetStep() const;
		void SetBindControlName(LPCTSTR sBindName);
		CDuiString GetBindControlName();

        virtual void DoEvent(TEventUI& event) override;
        virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    protected:
        bool OnClick(void* param);
		void UpdateText(bool bUp);

        CButtonUI*  m_btnUp;
        CButtonUI*  m_btnDown;

        int         m_nMin;
        int         m_nMax;
        int         m_nStep;
		CDuiString  m_sBindControlName;
    };
}

#endif // __UISPIN_H__
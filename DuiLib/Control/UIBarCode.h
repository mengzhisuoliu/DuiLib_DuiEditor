#pragma once

namespace DuiLib
{

class CBarCodeUI : public CDynamicLayoutUI
{
	struct Imp;
	DECLARE_DUICONTROL(CBarCodeUI)
public:
	CBarCodeUI(void);
	virtual ~CBarCodeUI(void);

	virtual LPCTSTR GetClass() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;

	void SetBarCodeType(LPCTSTR sValue);
	CDuiString GetBarCodeType() const;

	void SetBarCodeSize(int nValue);
	int GetBarCodeSize() const;

	virtual void SetPos(CDuiRect rc, bool bNeedInvalidate = true) override;
	virtual void PaintText(UIRender *pRender) override;

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
protected:
	void MakeBarcodeImage();

protected:
	CDuiRect m_rcCode;
	CStdRefPtr<UIRender> m_qrRender;

	CDuiString m_sBarCodeType; //条码类型, code39, code93, code128
	int m_nBarCodeSize;
protected:
	Imp *m_pImpl;
};

}


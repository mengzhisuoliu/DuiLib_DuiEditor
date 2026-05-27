#pragma once
class CDialogModal : public CUIDialog, public CUIDataExchange
{
public:
	CDialogModal(void);
	virtual ~CDialogModal(void);

	virtual LPCTSTR GetWindowClassName() const  { return _T("Dialog1"); }
	virtual CDuiString GetSkinFile() 			{ return _T("XmlFolder\\Dialog1.xml"); }

	virtual void InitWindow();
	virtual void OnClickOK();
	virtual void OnClickCancel();

	CDuiString m_sEdit;
	CDuiString m_sCombo;
	int m_nComboSel;
	BOOL m_bCheck;
	COleDateTime m_dtDateTime;
	COleDateTime m_dtDate;
	COleDateTime m_dtTime;
	COleCurrency m_cyMoney;

	CString m_sDateTime;
	CString m_sDate;
	CString m_sTime;
};


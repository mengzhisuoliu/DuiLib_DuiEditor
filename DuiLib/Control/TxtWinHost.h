#ifndef __TXTWINHOST_H__
#define __TXTWINHOST_H__

#pragma once
#ifdef DUILIB_WIN32

#ifdef _USEIMM
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#endif

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

#include <textserv.h>

namespace DuiLib {

	class CTxtWinHost : public ITextHost
	{
	public:
		CTxtWinHost();
		HRESULT InitDefaultCharFormat(CHARFORMAT2W* pcf, HFONT hfont);
		HRESULT InitDefaultParaFormat(PARAFORMAT2* ppf);
		uiBool Init(CRichEditUI *re , const CREATESTRUCT *pcs);
		virtual ~CTxtWinHost();

		ITextServices* GetTextServices(void) { return pserv; }
		void SetClientRect(CDuiRect *prc);
		CDuiRect* GetClientRect() { return &rcClient; }
		uiBool IsWordWrap(void) { return fWordWrap; }
		void SetWordWrap(uiBool fWordWrap);
		uiBool IsReadOnly();
		void SetReadOnly(uiBool fReadOnly);

		void SetFont(HFONT hFont);
		void SetColor(CDuiColor dwColor);

		SIZEL* GetExtent();
		void SetExtent(SIZEL *psizelExtent);

		void LimitText(LONG nChars);

		uiBool IsCaptured();

		uiBool IsShowCaret();

		void NeedFreshCaret();

		INT GetCaretWidth();
		INT GetCaretHeight();

		uiBool GetAllowBeep();
		void SetAllowBeep(uiBool fAllowBeep);

		WORD GetDefaultAlign();
		void SetDefaultAlign(WORD wNewAlign);

		uiBool GetRichTextFlag();
		void SetRichTextFlag(uiBool fNew);

		LONG GetDefaultLeftIndent();
		void SetDefaultLeftIndent(LONG lNewIndent);

		uiBool SetSaveSelection(uiBool fSaveSelection);

		HRESULT OnTxInPlaceDeactivate();
		HRESULT OnTxInPlaceActivate(LPCRECT prcClient);

		uiBool GetActiveState(void) { return fInplaceActive; }

		uiBool DoSetCursor(CDuiRect *prc, CDuiPoint *pt);

		void SetTransparent(uiBool fTransparent);

		void GetControlRect(LPRECT prc);

		LONG SetAccelPos(LONG laccelpos);

		WCHAR SetPasswordChar(WCHAR chPasswordChar);

		void SetDisabled(uiBool fOn);

		LONG SetSelBarWidth(LONG lSelBarWidth);

		uiBool GetTimerState();

		void SetParagraphIndent(LONG lIdentSize);

		void SetCharFormat(CHARFORMAT2W &c);

		void SetParaFormat(PARAFORMAT2 &p);

		// -----------------------------
		//	IUnknown interface
		// -----------------------------
		virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG _stdcall AddRef(void);
		virtual ULONG _stdcall Release(void);

		// -----------------------------
		//	ITextHost interface
		// -----------------------------
		virtual HDC TxGetDC();
		virtual INT TxReleaseDC(HDC hdc);
		virtual uiBool TxShowScrollBar(INT fnBar, uiBool fShow);
		virtual uiBool TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
		virtual uiBool TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, uiBool fRedraw);
		virtual uiBool TxSetScrollPos (INT fnBar, INT nPos, uiBool fRedraw);
		virtual void TxInvalidateRect(LPCRECT prc, uiBool fMode);
		virtual void TxViewChange(uiBool fUpdate);
		virtual uiBool TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
		virtual uiBool TxShowCaret(uiBool fShow);
		virtual uiBool TxSetCaretPos(INT x, INT y);
		virtual uiBool TxSetTimer(UINT idTimer, UINT uTimeout);
		virtual void TxKillTimer(UINT idTimer);
		virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
		virtual void TxSetCapture(uiBool fCapture);
		virtual void TxSetFocus();
		virtual void TxSetCursor(HCURSOR hcur, uiBool fText);
		virtual uiBool TxScreenToClient (LPPOINT lppt);
		virtual uiBool TxClientToScreen (LPPOINT lppt);
		virtual HRESULT TxActivate( LONG * plOldState );
		virtual HRESULT TxDeactivate( LONG lNewState );
		virtual HRESULT TxGetClientRect(LPRECT prc);
		virtual HRESULT TxGetViewInset(LPRECT prc);
		virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
		virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
		virtual COLORREF TxGetSysColor(int nIndex);
		virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
		virtual HRESULT TxGetMaxLength(DWORD *plength);
		virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
		virtual HRESULT TxGetPasswordChar(TCHAR *pch);
		virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
		virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
		virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
		virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
		virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
		virtual HRESULT TxNotify(DWORD iNotify, void *pv);
		virtual HIMC TxImmGetContext(void);
		virtual void TxImmReleaseContext(HIMC himc);
		virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

	private:
		CRichEditUI *m_re;
		ULONG	cRefs;					// Reference Count
		ITextServices	*pserv;		    // pointer to Text Services object
		// Properties

		DWORD		dwStyle;				// style bits

		unsigned	fEnableAutoWordSel	:1;	// enable Word style auto word selection?
		unsigned	fWordWrap			:1;	// Whether control should word wrap
		unsigned	fAllowBeep			:1;	// Whether beep is allowed
		unsigned	fRich				:1;	// Whether control is rich text
		unsigned	fSaveSelection		:1;	// Whether to save the selection when inactive
		unsigned	fInplaceActive		:1; // Whether control is inplace active
		unsigned	fTransparent		:1; // Whether control is transparent
		unsigned	fTimer				:1;	// A timer is set
		unsigned    fCaptured           :1;
		unsigned    fShowCaret          :1;
		unsigned    fNeedFreshCaret     :1; // 修正改变大小后点击其他位置原来光标不能消除的问题

		INT         iCaretWidth;
		INT         iCaretHeight;
		INT         iCaretLastWidth;
		INT         iCaretLastHeight;
		LONG		lSelBarWidth;			// Width of the selection bar
		LONG  		cchTextMost;			// maximum text size
		DWORD		dwEventMask;			// DoEvent mask to pass on to parent window
		LONG		icf;
		LONG		ipf;
		CDuiRect		rcClient;				// Client Rect for this control
		SIZEL		sizelExtent;			// Extent array
		CHARFORMAT2W cf;					// Default character format
		PARAFORMAT2	pf;					    // Default paragraph format
		LONG		laccelpos;				// Accelerator position
		WCHAR		chPasswordChar;		    // Password character
	};

} // namespace DuiLib

#endif //#ifdef DUILIB_WIN32
#endif // __TXTWINHOST_H__



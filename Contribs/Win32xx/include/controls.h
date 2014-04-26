// Win32++   Version 7.3
// Released: 30th November 2011
//
//      David Nash
//      email: dnash@bigpond.net.au
//      url: https://sourceforge.net/projects/win32-framework
//
//
// Copyright (c) 2005-2011  David Nash
//
// Permission is hereby granted, free of charge, to
// any person obtaining a copy of this software and
// associated documentation files (the "Software"),
// to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice
// shall be included in all copies or substantial portions
// of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// controls.h
//  Declaration of the following classes:
//  CAnimation, CComboBox, CComboBoxEx, CDateTime, CIPAddress,
//  CHeader, CHotKey, CMonthCalendar, CProgressBar, CScrollBar,
//  CSlider, CSpinButton, and CToolTip.


#ifndef _WIN32XX_CONTROLS_H_
#define _WIN32XX_CONTROLS_H_

#include "wincore.h"
#include "stdcontrols.h"

namespace Win32xx
{

	// Forward declarations
	class CMonthCalendar;
	class CToolTip;

	class CAnimation : public CWnd
	{
	public:
		CAnimation() {}
		virtual ~CAnimation() {}

		BOOL Close() const;
		BOOL Open(LPTSTR lpszName) const;
		BOOL Play(UINT wFrom, UINT wTo, UINT cRepeat) const;
		BOOL Seek(UINT wFrame) const;
		BOOL Stop() const;

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = ANIMATE_CLASS; }
	};


	class CComboBox : public CWnd
	{
	public:
		CComboBox() {}
		virtual ~CComboBox() { ::InitCommonControls(); }

		int   AddString(LPCTSTR lpszString) const;
		void  Clear() const;
		void  Copy() const;
		void  Cut() const;
		int   DeleteString(int nIndex) const;
		int   Dir(UINT attr, LPCTSTR lpszWildCard ) const;
		int   FindString(int nIndexStart, LPCTSTR lpszString) const;
		int   FindStringExact(int nIndexStart, LPCTSTR lpszString) const;
		int   GetCount() const;
		int   GetCurSel() const;
		CRect GetDroppedControlRect() const;
		BOOL  GetDroppedState() const;
		int   GetDroppedWidth() const;
		DWORD GetEditSel() const;
		BOOL  GetExtendedUI() const;
		int   GetHorizontalExtent() const;
		DWORD GetItemData(int nIndex) const;
		int   GetItemHeight(int nIndex) const;
		int   GetLBText(int nIndex, LPTSTR lpszText) const;
		int   GetLBTextLen(int nIndex) const;
		LCID  GetLocale() const;
		int   GetTopIndex() const;
		int   InitStorage(int nItems, int nBytes) const;
		int   InsertString(int nIndex, LPCTSTR lpszString) const;
		void  LimitText(int nMaxChars) const;
		void  Paste() const;
		void  ResetContent() const;
		int   SelectString(int nStartAfter, LPCTSTR lpszString) const;
		int   SetCurSel(int nIndex) const;
		int   SetDroppedWidth(int nWidth) const;
		BOOL  SetEditSel(int nStartChar, int nEndChar) const;
		int   SetExtendedUI(BOOL bExtended = TRUE) const;
		void  SetHorizontalExtent(UINT nExtent ) const;
		int   SetItemData(int nIndex, DWORD dwItemData) const;
		int   SetItemHeight(int nIndex, UINT cyItemHeight) const;
		LCID  SetLocale( LCID NewLocale ) const;
		int   SetTopIndex(int nIndex) const;
		void  ShowDropDown(BOOL bShow = TRUE) const;

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = _T("ComboBox"); }
	};


	class CComboBoxEx : public CComboBox
	{
	public:
		CComboBoxEx() 
		{
			INITCOMMONCONTROLSEX icce;
			icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icce.dwICC = ICC_USEREX_CLASSES;
			::InitCommonControlsEx(&icce);		
		}
		virtual ~CComboBoxEx() {}

		int  	DeleteItem(int nIndex ) const;
		CWnd* 	GetComboBoxCtrl() const;
		CEdit* 	GetEditCtrl() const;
		DWORD 	GetExtendedStyle() const;
		HIMAGELIST GetImageList() const;
		BOOL 	GetItem(COMBOBOXEXITEM* pCBItem) const;
		BOOL 	HasEditChanged () const;
		int     InsertItem(COMBOBOXEXITEM* lpcCBItem) const;
		DWORD 	SetExtendedStyle(DWORD dwExMask, DWORD dwExStyles ) const;
		HIMAGELIST SetImageList(HIMAGELIST himl) const;
		BOOL 	SetItem(PCOMBOBOXEXITEM lpcCBItem) const;

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = WC_COMBOBOXEX; }
	};


	class CDateTime : public CWnd
	{
	public:
		CDateTime()
		{
			INITCOMMONCONTROLSEX icce;
			icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icce.dwICC = ICC_DATE_CLASSES;
			::InitCommonControlsEx(&icce);
		}
		virtual ~CDateTime() {}

		COLORREF GetMonthCalColor(int iColor) const;
		COLORREF SetMonthCalColor(int iColor, COLORREF ref);
		BOOL SetFormat(LPCTSTR pstrFormat);
		CMonthCalendar* GetMonthCalCtrl() const;
		CFont* GetMonthCalFont() const;
		void SetMonthCalFont(HFONT hFont, BOOL bRedraw = TRUE);
		DWORD GetRange(LPSYSTEMTIME lpSysTimeArray) const;
		BOOL SetRange(DWORD flags, LPSYSTEMTIME lpSysTimeArray);
		DWORD GetTime(LPSYSTEMTIME pTimeDest) const;
		BOOL SetTime(DWORD flag, LPSYSTEMTIME pTimeNew = NULL);

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = DATETIMEPICK_CLASS; }
	};

	class CHeader : public CWnd
	{
	public:
		CHeader()
		{
			INITCOMMONCONTROLSEX icex;
			icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icex.dwICC = ICC_WIN95_CLASSES;
			::InitCommonControlsEx(&icex);
		}
		virtual ~CHeader() {}

		// Attributes
		HIMAGELIST GetImageList() const;
		BOOL GetItem(int nPos, HDITEM* pHeaderItem) const;
		int GetItemCount() const;
		CRect GetItemRect(int nIndex) const;
		BOOL GetOrderArray(LPINT piArray, int iCount);
		int OrderToIndex(int nOrder) const;
		HIMAGELIST SetImageList(HIMAGELIST himl);
		BOOL SetItem(int nPos, HDITEM* pHeaderItem);
		BOOL SetOrderArray(int iCount, LPINT piArray);
		int GetBitmapMargin() const;
		int SetBitmapMargin(int nWidth);

		// Operations
		HIMAGELIST CreateDragImage(int nIndex);
		BOOL DeleteItem(int nPos);
		int InsertItem(int nPos, HDITEM* phdi);
		BOOL Layout(HDLAYOUT* pHeaderLayout);
#ifdef Header_SetHotDivider
		int SetHotDivider(CPoint pt);
		int SetHotDivider(int nIndex);
#endif
#ifdef Header_ClearFilter
		int ClearAllFilters();
		int ClearFilter(int nColumn);
		int EditFilter(int nColumn, BOOL bDiscardChanges);
		int SetFilterChangeTimeout(DWORD dwTimeOut);
#endif

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = WC_HEADER ; }
	};

	class CHotKey : public CWnd
	{
	public:
		CHotKey() { ::InitCommonControls(); }
		virtual ~CHotKey() {}

		DWORD GetHotKey() const;
		CString GetKeyName(UINT vk, BOOL fExtended) const;
		void SetHotKey(DWORD dwKey);
		void SetRules(WORD wInvalidComb, WORD wModifiers);

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = HOTKEY_CLASS; }
	};

	class CIPAddress : public CWnd
	{
	public:
		CIPAddress()
		{
			INITCOMMONCONTROLSEX icex;
			icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icex.dwICC = ICC_INTERNET_CLASSES;
			::InitCommonControlsEx(&icex);
		}
		virtual ~CIPAddress() {}

		void ClearAddress();
		int GetAddress(BYTE& nField0, BYTE& nField1, BYTE& nField2, BYTE& nField3);
		int GetAddress(DWORD* dwAddress);
		BOOL IsBlank() const;
		void SetAddress(BYTE nField0, BYTE nField1, BYTE nField2, BYTE nField3);
		void SetAddress(DWORD dwAddress);
		void SetFieldFocus(WORD nField);
		void SetFieldRange(int nField, BYTE nLower, BYTE nUpper);

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = WC_IPADDRESS; }

	};

	class CMonthCalendar : public CWnd
	{
	public:
		CMonthCalendar()
		{
			INITCOMMONCONTROLSEX icex;
			icex.dwSize = sizeof(icex);
			icex.dwICC  = ICC_DATE_CLASSES;
			InitCommonControlsEx(&icex);
		}
		virtual ~CMonthCalendar() {}

		// Attributes
		COLORREF GetColor(int nRegion) const;
		int GetFirstDayOfWeek(BOOL* pbLocal = NULL) const;
		CRect GetMinReqRect() const;
		int GetMonthDelta() const;
		COLORREF SetColor(int nRegion, COLORREF ref);
		BOOL SetFirstDayOfWeek(int iDay, int* lpnOld = NULL);
		int SetMonthDelta(int iDelta);

		// Operations
		BOOL GetCurSel(LPSYSTEMTIME pDateTime) const;
		int GetMaxSelCount() const;
		int GetMonthRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange, DWORD dwFlags) const;
		DWORD GetRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange) const;
		BOOL GetSelRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange) const;
		BOOL GetToday(LPSYSTEMTIME pDateTime) const;
		DWORD HitTest(PMCHITTESTINFO pMCHitTest);
		BOOL SetCurSel(const LPSYSTEMTIME pDateTime);
		BOOL SetDayState(int nMonths, LPMONTHDAYSTATE pStates);
		BOOL SetMaxSelCount(int nMax);
		BOOL SetRange(const LPSYSTEMTIME pMinRange, const LPSYSTEMTIME pMaxRange);
		BOOL SetSelRange(const LPSYSTEMTIME pMinRange, const LPSYSTEMTIME pMaxRange);
		void SetToday(const LPSYSTEMTIME pDateTime);

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = MONTHCAL_CLASS; }
	};

	class CProgressBar : public CWnd
	{
	public:
		CProgressBar() { ::InitCommonControls(); }
		virtual ~CProgressBar() {}

		int  GetPos() const;
		int  GetRange(BOOL fWhichLimit, PPBRANGE ppBRange) const;
		int  OffsetPos(int nIncrement) const;
		int  SetPos(int nNewPos) const;
		int  SetRange(short nMinRange, short nMaxRange) const;
		int  SetStep(int nStepInc) const;
		int  StepIt() const;

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = PROGRESS_CLASS; }
	};

	class CScrollBar : public CWnd
	{
	public:
		CScrollBar() {}
		virtual ~CScrollBar() {}

		BOOL EnableScrollBar( UINT nArrowFlags = ESB_ENABLE_BOTH )  const;
		BOOL GetScrollInfo(LPSCROLLINFO lpsi)  const;
		int  GetScrollPos()  const;
		BOOL GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos )  const;
		BOOL SetScrollInfo(LPSCROLLINFO lpsi, BOOL bRedraw = TRUE )  const;
		int  SetScrollPos(int nPos, BOOL bRedraw)  const;
		BOOL SetScrollRange( int nMinPos, int nMaxPos, BOOL bRedraw = TRUE )  const;
		BOOL ShowScrollBar(BOOL bShow)  const;

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = _T("SCROLLBAR"); ; }
	};

	class CSlider : public CWnd
	{
	public:
		CSlider() { ::InitCommonControls(); }
		virtual ~CSlider() {}

		void ClearSel() const;
		void ClearTics(BOOL bRedraw = FALSE ) const;
		CWnd* GetBuddy(BOOL fLocation = TRUE ) const;
		CRect GetChannelRect() const;
		int  GetLineSize() const;
		int  GetNumTics() const;
		int  GetPageSize() const;
		int  GetPos() const;
		int  GetRangeMax() const;
		int  GetRangeMin() const;
		int  GetSelEnd() const;
		int  GetSelStart() const;
		int  GetThumbLength() const;
		CRect GetThumbRect() const;
		int  GetTic(int nTic ) const;
		int  GetTicPos(int nTic) const;
		CToolTip* GetToolTips() const;
		CWnd* SetBuddy(CWnd* pBuddy, BOOL fLocation = TRUE ) const;
		int  SetLineSize(int nSize) const;
		int  SetPageSize(int nSize) const;
		void SetPos(int nPos, BOOL bRedraw = FALSE) const;
		void SetRangeMax(int nMax, BOOL bRedraw = FALSE) const;
		void SetRangeMin(int nMax, BOOL bRedraw = FALSE) const;
		void SetSelection(int nMin, int nMax, BOOL bRedraw = FALSE) const;
		BOOL SetTic(int nTic) const;
		void SetTicFreq(int nFreq)  const;
		int  SetTipSide(int nLocation) const;
		void SetToolTips(CToolTip* pToolTip) const;

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = TRACKBAR_CLASS; }
	};


	// Also known as an Up/Down control
	class CSpinButton : public CWnd
	{
	public:
		CSpinButton() { ::InitCommonControls(); }
		virtual ~CSpinButton() {}

		int  GetAccel(int cAccels, LPUDACCEL paAccels) const;
		int  GetBase() const;
		CWnd* GetBuddy() const;
		int  GetPos() const;
		DWORD GetRange() const;
		BOOL SetAccel(int cAccels, LPUDACCEL paAccels) const;
		int  SetBase(int nBase) const;
		CWnd* SetBuddy(CWnd* hwndBuddy) const;
		int  SetPos(int nPos) const;
		void SetRange(int nLower, int nUpper) const;

	protected:
		// Overridables
		virtual void PreCreate(CREATESTRUCT &cs);
		virtual void PreRegisterClass(WNDCLASS &wc);
	};

	class CToolTip : public CWnd
	{
	public:
		CToolTip();
		virtual ~CToolTip();

		//Attributes
		int GetDelayTime(DWORD dwDuration) const;
		void GetMargin(LPRECT lprc) const;
		int GetMaxTipWidth() const;
		void GetText(CString& str, CWnd* pWnd, UINT_PTR nIDTool = 0) const;
		COLORREF GetTipBkColor() const;
		COLORREF GetTipTextColor() const;
		int GetToolCount() const;
		BOOL GetToolInfo(TOOLINFO& ToolInfo, CWnd* pWnd, UINT_PTR nIDTool = 0) const;
		void SetDelayTime(UINT nDelay);
		void SetDelayTime(DWORD dwDuration, int iTime);
		void SetMargin(LPRECT lprc);
		int SetMaxTipWidth(int iWidth);
		void SetTipBkColor(COLORREF clr);
		void SetTipTextColor(COLORREF clr);
		void SetToolInfo(LPTOOLINFO lpToolInfo);
#if (defined TTM_SETTITLE) && (_WIN32_IE >=0x0500)
		CSize GetBubbleSize(LPTOOLINFO lpToolInfo) const;
#endif

		//Operations
		void Activate(BOOL bActivate);
		BOOL AddTool(CWnd* pWnd, UINT nIDText, LPCRECT lpRectTool = NULL, UINT_PTR nIDTool = 0);
		BOOL AddTool(CWnd* pWnd, LPCTSTR lpszText = LPSTR_TEXTCALLBACK, LPCRECT lpRectTool = NULL, UINT_PTR nIDTool = 0);
		void DelTool(CWnd* pWnd, UINT_PTR nIDTool = 0);
		BOOL HitTest(CWnd* pWnd, CPoint pt, LPTOOLINFO lpToolInfo) const;
		void Pop();
		void RelayEvent(LPMSG lpMsg);

		void SetToolRect(CWnd* pWnd, UINT_PTR nIDTool, LPCRECT lpRect);
		void Update();
		void UpdateTipText(LPCTSTR lpszText, CWnd* pWnd, UINT_PTR nIDTool = 0);
		void UpdateTipText(UINT nIDText, CWnd* pWnd, UINT_PTR nIDTool = 0);

#if (defined TTM_SETTITLE) && (_WIN32_IE >=0x0500)
		BOOL AdjustRect(LPRECT lprc, BOOL bLarger = TRUE);
		BOOL SetTitle(UINT uIcon, LPCTSTR lpstrTitle);
#endif
#if (defined TTM_SETWINDOWTHEME) && (WINVER >= 0x0501)
		HRESULT SetWindowTheme(LPCWSTR lpstrTheme);
#endif

	protected:
		// Overridables
		virtual void PreRegisterClass(WNDCLASS &wc) { wc.lpszClassName = TOOLTIPS_CLASS; ; }
		virtual void PreCreate(CREATESTRUCT &cs) 
		{ 
			cs.dwExStyle = WS_EX_TOOLWINDOW; 
			cs.style = WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP;
		}

	private:
		void LoadToolInfo(TOOLINFO& ti, CWnd* pWnd, UINT_PTR nIDTool) const;
	};


} // namespace Win32xx

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace Win32xx
{

	////////////////////////////////////////
	// Definitions for the CAnimation class
	//
	inline BOOL CAnimation::Close() const
	// Closes an AVI clip.
	{
		assert(IsWindow());
		return Animate_Close(m_hWnd);
	}

	inline BOOL CAnimation::Open(LPTSTR lpszName) const
	// Opens an AVI clip and displays its first frame in an animation control.
	{
		assert(IsWindow());
		return Animate_Open(m_hWnd, lpszName);
	}

	inline BOOL CAnimation::Play(UINT wFrom, UINT wTo, UINT cRepeat) const
	// Plays an AVI clip in an animation control. The control plays the clip
	//	in the background while the thread continues executing.
	{
		assert(IsWindow());
		return Animate_Play(m_hWnd, wFrom, wTo, cRepeat);
	}

	inline BOOL CAnimation::Seek(UINT wFrame) const
	// Directs an animation control to display a particular frame of an AVI clip.
	// The control displays the clip in the background while the thread continues executing.
	{
		assert(IsWindow());
		return Animate_Seek(m_hWnd, wFrame);
	}

	inline BOOL CAnimation::Stop() const
	// Stops playing an AVI clip in an animation control.
	{
		assert(IsWindow());
		return Animate_Stop(m_hWnd);
	}


	////////////////////////////////////////
	// Definitions for the CComboBox class
	//
	inline int  CComboBox::AddString(LPCTSTR lpszString) const
	// Adds a string to the list box of a combo box. If the combo box does not
	// have the CBS_SORT style, the string is added to the end of the list.
	// Otherwise, the string is inserted into the list, and the list is sorted.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_ADDSTRING, 0, (LPARAM)lpszString);
	}

	inline void CComboBox::Clear() const
	// Deletes the current selection, if any, from the combo box's edit control.
	{
		assert(IsWindow());
		SendMessage(WM_CLEAR, 0, 0);
	}

	inline void CComboBox::Copy() const
	// Copies the current selection to the clipboard in CF_TEXT format.
	{
		assert(IsWindow());
		SendMessage(WM_COPY, 0, 0);
	}

	inline void CComboBox::Cut() const
	// Deletes the current selection, if any, in the edit control and copies
	// the deleted text to the clipboard in CF_TEXT format.
	{
		assert(IsWindow());
		SendMessage(WM_CUT, 0, 0);
	}

	inline int  CComboBox::DeleteString(int nIndex) const
	// Deletes a string in the list box of a combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_DELETESTRING, (WPARAM)nIndex, 0);
	}

	inline int  CComboBox::Dir(UINT attr, LPCTSTR lpszWildCard ) const
	// Adds the names of directories and files that match a specified string
	// and set of file attributes.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_DIR, (WPARAM)attr, (LPARAM)lpszWildCard);
	}

	inline int  CComboBox::FindString(int nIndexStart, LPCTSTR lpszString) const
	// Search the list box of a combo box for an item beginning with the
	// characters in a specified string.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_FINDSTRING, (WPARAM)nIndexStart, (LPARAM)lpszString);
	}

	inline int  CComboBox::FindStringExact(int nIndexStart, LPCTSTR lpszString) const
	// Find the first list box string in a combo box that matches the string specified in lpszString.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_FINDSTRINGEXACT, (WPARAM)nIndexStart, (LPARAM)lpszString);
	}

	inline int  CComboBox::GetCount() const
	// Retrieves the number of items in the list box of the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETCOUNT, 0,0);
	}

	inline int  CComboBox::GetCurSel() const
	// Retrieves the index of the currently selected item, if any, in the list box of the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETCURSEL, 0,0);
	}

	inline CRect CComboBox::GetDroppedControlRect() const
	// Retrieves the screen coordinates of the combo box in its dropped-down state.
	{
		assert(IsWindow());
		CRect rc;
		SendMessage(CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)&rc);
		return rc;
	}

	inline BOOL CComboBox::GetDroppedState() const
	// Determines whether the list box of the combo box is dropped down.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(CB_GETDROPPEDSTATE, 0, 0);
	}

	inline int  CComboBox::GetDroppedWidth() const
	// Retrieves the minimum allowable width, in pixels, of the list box of the combo box
	// with the CBS_DROPDOWN or CBS_DROPDOWNLIST style.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETDROPPEDWIDTH, 0, 0);
	}

	inline DWORD CComboBox::GetEditSel() const
	// Gets the starting and ending character positions of the current selection
	// in the edit control of the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETEDITSEL, 0, 0);
	}

	inline BOOL CComboBox::GetExtendedUI() const
	// Determines whether the combo box has the default user interface or the extended user interface.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(CB_GETEXTENDEDUI, 0, 0);
	}

	inline int  CComboBox::GetHorizontalExtent() const
	// Retrieve from the combo box the width, in pixels, by which the list box can
	// be scrolled horizontally (the scrollable width).
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETHORIZONTALEXTENT, 0, 0);
	}

	inline DWORD CComboBox::GetItemData(int nIndex) const
	// Retrieves the application-supplied value associated with the specified item in the combo box.
	{
		assert(IsWindow());
		return (DWORD)SendMessage(CB_GETITEMDATA, (WPARAM)nIndex, 0);
	}

	inline int  CComboBox::GetItemHeight(int nIndex) const
	// Determines the height of list items or the selection field in the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETITEMHEIGHT, (WPARAM)nIndex, 0);
	}

	inline int  CComboBox::GetLBText(int nIndex, LPTSTR lpszText) const
	//  Retrieves a string from the list of the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETLBTEXT, (WPARAM)nIndex, (LPARAM)lpszText);
	}

	inline int  CComboBox::GetLBTextLen(int nIndex) const
	// Retrieves the length, in characters, of a string in the list of the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETLBTEXTLEN, (WPARAM)nIndex, 0);
	}

	inline LCID CComboBox::GetLocale() const
	// Retrieves the current locale of the combo box.
	{
		assert(IsWindow());
		return (LCID)SendMessage(CB_GETLOCALE, 0, 0);
	}

	inline int  CComboBox::GetTopIndex() const
	// Retrieves the zero-based index of the first visible item in the list box portion of the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_GETTOPINDEX, 0, 0);
	}

	inline int  CComboBox::InitStorage(int nItems, int nBytes) const
	// Allocates memory for storing list box items. Use this before adding a
	// large number of items to the list box portion of a combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_INITSTORAGE, (WPARAM)nItems, (LPARAM)nBytes);
	}

	inline int  CComboBox::InsertString(int nIndex, LPCTSTR lpszString) const
	// Inserts a string into the list box of the combo box. Unlike the AddString,
	// a list with the CBS_SORT style is not sorted.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_INSERTSTRING, (WPARAM)nIndex, (LPARAM)lpszString);
	}

	inline void CComboBox::Paste() const
	// Copies the current content of the clipboard to the combo box's edit control at the current caret position.
	{
		assert(IsWindow());
		SendMessage(WM_PASTE, 0, 0);
	}

	inline void CComboBox::LimitText(int nMaxChars) const
	// Limits the length of the text the user may type into the edit control of the combo box.
	{
		assert(IsWindow());
		SendMessage(CB_LIMITTEXT, (WPARAM)nMaxChars, 0);
	}

	inline void CComboBox::ResetContent() const
	// Removes all items from the list box and edit control of the combo box.
	{
		assert(IsWindow());
		SendMessage(CB_RESETCONTENT, 0, 0);
	}

	inline int  CComboBox::SelectString(int nStartAfter, LPCTSTR lpszString) const
	// Searches the list of a combo box for an item that begins with the characters in a
	// specified string. If a matching item is found, it is selected and copied to the edit control.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_SELECTSTRING, (WPARAM)nStartAfter, (LPARAM)lpszString);
	}

	inline int  CComboBox::SetCurSel(int nIndex) const
	// Selects a string in the list of the combo box. If necessary, the list scrolls the string into view.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_SETCURSEL, (WPARAM)nIndex, 0);
	}

	inline int  CComboBox::SetDroppedWidth(int nWidth) const
	// Sets the maximum allowable width, in pixels, of the list box of the combo box with
	// the CBS_DROPDOWN or CBS_DROPDOWNLIST style.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_SETDROPPEDWIDTH, (WPARAM)nWidth, 0);
	}

	inline BOOL CComboBox::SetEditSel(int nStartChar, int nEndChar) const
	// Selects characters in the edit control of the combo box.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(CB_SETEDITSEL, 0, (LPARAM)MAKELONG(nStartChar,nEndChar));
	}

	inline int  CComboBox::SetExtendedUI(BOOL bExtended) const
	// Selects either the default user interface or the extended user interface for the combo box that
	// has the CBS_DROPDOWN or CBS_DROPDOWNLIST style.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_SETEXTENDEDUI, (WPARAM)bExtended, 0);
	}

	inline void CComboBox::SetHorizontalExtent(UINT nExtent ) const
	// Sets the width, in pixels, by which the list box can be scrolled horizontally (the scrollable width).
	{
		assert(IsWindow());
		SendMessage(CB_SETHORIZONTALEXTENT, (WPARAM)nExtent, 0);
	}

	inline int  CComboBox::SetItemData(int nIndex, DWORD dwItemData) const
	// Sets the value associated with the specified item in the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_SETITEMDATA, (WPARAM)nIndex, (LPARAM)dwItemData);
	}

	inline int  CComboBox::SetItemHeight(int nIndex, UINT cyItemHeight) const
	// Sets the height of list items or the selection field in the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_SETITEMHEIGHT, (WPARAM)nIndex, (LPARAM)cyItemHeight);
	}

	inline LCID CComboBox::SetLocale( LCID NewLocale ) const
	// Sets the current locale of the combo box.
	{
		assert(IsWindow());
		return (LCID)SendMessage(CB_SETLOCALE, (WPARAM)NewLocale, 0);
	}

	inline int  CComboBox::SetTopIndex(int nIndex) const
	// Ensure that a particular item is visible in the list box of the combo box.
	{
		assert(IsWindow());
		return (int)SendMessage(CB_SETTOPINDEX, (WPARAM)nIndex, 0);
	}

	inline void CComboBox::ShowDropDown(BOOL bShow) const
	// Shows or hides the list box of the combo box that has the CBS_DROPDOWN or CBS_DROPDOWNLIST style.
	{
		assert(IsWindow());
		SendMessage(CB_SHOWDROPDOWN, (WPARAM)bShow, 0);
	}


	////////////////////////////////////////
	// Definitions for the CComboBoxEx class
	//
	inline int  CComboBoxEx::DeleteItem(int nIndex ) const
	// Removes an item from the ComboBoxEx control.
	{
		assert(IsWindow());
		return (int)SendMessage(CBEM_DELETEITEM, (WPARAM)nIndex, 0);
	}

	inline CWnd* CComboBoxEx::GetComboBoxCtrl() const
	// Retrieves the handle to the child combo box control.
	{
		assert(IsWindow());
		return FromHandle((HWND)SendMessage(CBEM_GETCOMBOCONTROL, 0, 0));
	}

	inline CEdit* CComboBoxEx::GetEditCtrl() const
	// Retrieves the handle to the edit control portion of the ComboBoxEx control.
	{
		assert(IsWindow());
		return (CEdit*)FromHandle((HWND)SendMessage(CBEM_GETEDITCONTROL, 0, 0));
	}

	inline DWORD CComboBoxEx::GetExtendedStyle() const
	// Retrieves the extended styles that are in use for the ComboBoxEx control.
	{
		assert(IsWindow());
		return (DWORD)SendMessage(CBEM_GETEXTENDEDSTYLE, 0, 0);
	}

	inline HIMAGELIST CComboBoxEx::GetImageList() const
	// Retrieves the handle to an image list assigned to the ComboBoxEx control.
	{
		assert(IsWindow());
		return (HIMAGELIST)SendMessage(CBEM_GETIMAGELIST, 0, 0);
	}

	inline BOOL CComboBoxEx::GetItem(COMBOBOXEXITEM* pCBItem) const
	// Retrieves item information for the given ComboBoxEx item.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(CBEM_GETITEM, 0, (LPARAM)pCBItem);
	}

	inline BOOL CComboBoxEx::HasEditChanged () const
	// Determines whether or not the user has changed the text of the ComboBoxEx edit control.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(CBEM_HASEDITCHANGED, 0, 0);
	}

	inline int CComboBoxEx::InsertItem(COMBOBOXEXITEM* lpcCBItem) const
	// Inserts a new item in the ComboBoxEx control.
	{
		assert(IsWindow());
		return (int)SendMessage(CBEM_INSERTITEM, 0, (LPARAM)lpcCBItem);
	}

	inline DWORD CComboBoxEx::SetExtendedStyle(DWORD dwExMask, DWORD dwExStyles ) const
	// Sets extended styles within the ComboBoxEx control.
	{
		assert(IsWindow());
		return (DWORD)SendMessage(CBEM_SETEXTENDEDSTYLE, (WPARAM)dwExMask, (LPARAM)dwExStyles);
	}

	inline HIMAGELIST CComboBoxEx::SetImageList(HIMAGELIST himl) const
	// Sets an image list for the ComboBoxEx control.
	{
		assert(IsWindow());
		return (HIMAGELIST)SendMessage(CBEM_SETIMAGELIST, 0, (LPARAM)himl);
	}

	inline BOOL CComboBoxEx::SetItem(PCOMBOBOXEXITEM lpcCBItem) const
	// Sets the attributes for an item in the ComboBoxEx control.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(CBEM_SETITEM, 0, (LPARAM)lpcCBItem);
	}

	////////////////////////////////////////
	// Definitions for the CDateTime class
	//
	inline COLORREF CDateTime::GetMonthCalColor(int iColor) const
	{
		assert(IsWindow());
		return (COLORREF)DateTime_GetMonthCalColor(m_hWnd, iColor);
	}

	inline COLORREF CDateTime::SetMonthCalColor(int iColor, COLORREF clr)
	{
		assert(IsWindow());
		return (COLORREF)DateTime_SetMonthCalColor(m_hWnd, iColor, clr);
	}

	inline BOOL CDateTime::SetFormat(LPCTSTR pszFormat)
	{
		assert(IsWindow());
		return DateTime_SetFormat(m_hWnd, pszFormat);
	}

	inline CMonthCalendar* CDateTime::GetMonthCalCtrl() const
	{
		assert(IsWindow());
		return (CMonthCalendar*)FromHandle((HWND)DateTime_GetMonthCal(m_hWnd));
	}

	inline CFont* CDateTime::GetMonthCalFont() const
	{
		assert(IsWindow());
		return FromHandle((HFONT)DateTime_GetMonthCalFont(m_hWnd));
	}

	inline void CDateTime::SetMonthCalFont(HFONT hFont, BOOL bRedraw /*= TRUE*/)
	{
		assert(IsWindow());
		DateTime_SetMonthCalFont(m_hWnd, hFont, MAKELONG(bRedraw, 0));
	}

	inline DWORD CDateTime::GetRange(LPSYSTEMTIME lpSysTimeArray) const
	{
		assert(IsWindow());
		return DateTime_GetRange(m_hWnd, lpSysTimeArray);
	}

	inline BOOL CDateTime::SetRange(DWORD flags, LPSYSTEMTIME lpSysTimeArray)
	{
		assert(IsWindow());
		return DateTime_SetRange(m_hWnd, flags, lpSysTimeArray);
	}

	inline DWORD CDateTime::GetTime(LPSYSTEMTIME pTimeDest) const
	{
		assert(IsWindow());
		return DateTime_GetSystemtime(m_hWnd, pTimeDest);
	}

	inline BOOL CDateTime::SetTime(DWORD flag, LPSYSTEMTIME pTimeNew /*= NULL*/)
	{
		assert(IsWindow());
		return DateTime_SetSystemtime(m_hWnd, flag, pTimeNew);
	}


	////////////////////////////////////////
	// Definitions for the CHotKey class
	//
	inline DWORD CHotKey::GetHotKey() const
	{
		assert(IsWindow());
		return (DWORD)SendMessage(HKM_GETHOTKEY, 0, 0);
	}

	inline CString CHotKey::GetKeyName(UINT vk, BOOL fExtended) const
	{
		// Translate the virtual-key code to a scan code
		LONG lScan = MapVirtualKey(vk, 0);

		// Construct an LPARAM with the scan code in Bits 16-23, and an extended flag in bit 24
		LPARAM lParam = lScan << 16;
		if (fExtended)
			lParam |= 0x01000000L;

		CString str;
		int nBufferLen = 64;
		int nStrLen = nBufferLen;

		// Loop until we have retrieved the entire string
		while(nStrLen == nBufferLen)
		{
			nBufferLen *= 4;
			LPTSTR pszStr = str.GetBuffer(nBufferLen);
			nStrLen = ::GetKeyNameText((LONG)lParam, pszStr, nBufferLen + 1);
			str.ReleaseBuffer();
		}

		return str;
	}

	inline void CHotKey::SetHotKey(DWORD dwKey)
	{
		assert(IsWindow());
		SendMessage(HKM_SETHOTKEY, (WPARAM)dwKey, 0);
	}

	inline void CHotKey::SetRules(WORD wInvalidComb, WORD wModifiers)
	{
		assert(IsWindow());
		SendMessage(HKM_SETRULES, wInvalidComb, wModifiers);
	}


	////////////////////////////////////////
	// Definitions for the CHeader class
	//
	inline HIMAGELIST CHeader::CreateDragImage(int nIndex)
	{
		assert(IsWindow());
		return Header_CreateDragImage(m_hWnd, nIndex);
	}

	inline BOOL CHeader::DeleteItem(int nPos)
	{
		assert(IsWindow());
		return Header_DeleteItem(m_hWnd, nPos);
	}

	inline HIMAGELIST CHeader::GetImageList() const
	{
		assert(IsWindow());
		return Header_GetImageList(m_hWnd);
	}

	inline BOOL CHeader::GetItem(int nPos, HDITEM* pHeaderItem) const
	{
		assert(IsWindow());
		return Header_GetItem(m_hWnd, nPos, pHeaderItem);
	}

	inline int CHeader::GetItemCount() const
	{
		assert(IsWindow());
		return Header_GetItemCount(m_hWnd);
	}

	inline CRect CHeader::GetItemRect(int nIndex) const
	{
		assert(IsWindow());
		RECT rc;
		Header_GetItemRect(m_hWnd, nIndex, &rc);
		return rc;
	}

	inline BOOL CHeader::GetOrderArray(LPINT piArray, int iCount)
	{
		assert(IsWindow());
		return Header_GetOrderArray(m_hWnd, piArray, iCount);
	}

	inline int CHeader::InsertItem(int nPos, HDITEM* phdi)
	{
		assert(IsWindow());
		return Header_InsertItem(m_hWnd, nPos, phdi);
	}

	inline BOOL CHeader::Layout(HDLAYOUT* pHeaderLayout)
	{
		assert(IsWindow());
		return Header_Layout(m_hWnd, pHeaderLayout);
	}

	inline int CHeader::OrderToIndex(int nOrder) const
	{
		assert(IsWindow());
		return Header_OrderToIndex( m_hWnd, nOrder);
	}

#ifdef Header_SetHotDivider
	inline int CHeader::SetHotDivider(CPoint pt)
	{
		assert(IsWindow());
		return Header_SetHotDivider(m_hWnd, TRUE, MAKELPARAM(pt.x, pt.y));
	}

	inline int CHeader::SetHotDivider(int nIndex)
	{
		assert(IsWindow());
		return Header_SetHotDivider(m_hWnd, FALSE, nIndex);
	}
#endif

	inline HIMAGELIST CHeader::SetImageList(HIMAGELIST himl)
	{
		assert(IsWindow());
		return Header_SetImageList(m_hWnd, himl);
	}

	inline BOOL CHeader::SetItem(int nPos, HDITEM* pHeaderItem)
	{
		assert(IsWindow());
		return Header_SetItem(m_hWnd, nPos, pHeaderItem);
	}

	inline BOOL CHeader::SetOrderArray(int iCount, LPINT piArray)
	{
		assert(IsWindow());
		return Header_SetOrderArray(m_hWnd, iCount, piArray);
	}

#ifdef Header_ClearFilter
	inline int CHeader::ClearFilter(int nColumn)
	{
		assert(IsWindow());
		return Header_ClearFilter(m_hWnd, nColumn);
	}

	inline int CHeader::ClearAllFilters()
	{
		assert(IsWindow());
		return Header_ClearAllFilters(m_hWnd);
	}

	inline int CHeader::EditFilter(int nColumn, BOOL bDiscardChanges)
	{
		assert(IsWindow());
		return Header_EditFilter(m_hWnd, nColumn, LPARAM MAKELPARAM(bDiscardChanges, 0));
	}

	inline int CHeader::GetBitmapMargin() const
	{
		assert(IsWindow());
		return Header_GetBitmapMargin(m_hWnd);
	}

	inline int CHeader::SetBitmapMargin(int nWidth)
	{
		assert(IsWindow());
		return Header_SetBitmapMargin(m_hWnd, nWidth);
	}

	inline int CHeader::SetFilterChangeTimeout(DWORD dwTimeOut)
	{
		assert(IsWindow());
		return Header_SetFilterChangeTimeout(m_hWnd, dwTimeOut);
	}
#endif

	////////////////////////////////////////
	// Definitions for the CIPAddress class
	//
	inline void CIPAddress::ClearAddress()
	{
		assert(IsWindow());
		SendMessage(IPM_CLEARADDRESS, 0, 0);
	}

	inline int CIPAddress::GetAddress(BYTE& nField0, BYTE& nField1, BYTE& nField2, BYTE& nField3)
	{
		DWORD dwAddr;
		int iValue = GetAddress(&dwAddr);
		nField0 = (BYTE)FIRST_IPADDRESS(dwAddr);
		nField1 = (BYTE)SECOND_IPADDRESS(dwAddr);
		nField2 = (BYTE)THIRD_IPADDRESS(dwAddr);
		nField3 = (BYTE)FOURTH_IPADDRESS(dwAddr);
		return iValue;
	}

	inline int CIPAddress::GetAddress(DWORD* dwAddress)
	{
		assert(IsWindow());
		return (int)SendMessage(IPM_GETADDRESS, 0, (LPARAM)&dwAddress);
	}

	inline BOOL CIPAddress::IsBlank() const
	{
		assert(IsWindow());
		return (BOOL)SendMessage(IPM_ISBLANK, 0, 0);
	}

	inline void CIPAddress::SetAddress(BYTE nField0, BYTE nField1, BYTE nField2, BYTE nField3)
	{
		assert(IsWindow());
		SendMessage(IPM_SETADDRESS, 0, (LPARAM)MAKEIPADDRESS(nField0, nField1, nField2, nField3));
	}

	inline void CIPAddress::SetAddress(DWORD dwAddress)
	{
		assert(IsWindow());
		SendMessage(IPM_SETADDRESS, 0, (LPARAM)dwAddress);
	}

	inline void CIPAddress::SetFieldFocus(WORD nField)
	{
		assert(IsWindow());
		SendMessage(IPM_SETFOCUS, nField, 0);
	}

	inline void CIPAddress::SetFieldRange(int nField, BYTE nLower, BYTE nUpper)
	{
		assert(IsWindow());
		SendMessage(IPM_SETRANGE, MAKEIPRANGE(nLower, nUpper), nField);
	}


	///////////////////////////////////////////
	// Definitions for the CMonthCalendar class
	//
	inline COLORREF CMonthCalendar::GetColor(int nRegion) const
	{
		assert(IsWindow());
		return (COLORREF)MonthCal_GetColor(m_hWnd, nRegion);
	}

	inline BOOL CMonthCalendar::GetCurSel(LPSYSTEMTIME pDateTime) const
	{
		assert(IsWindow());
		return MonthCal_GetCurSel(m_hWnd, pDateTime);
	}

	inline int CMonthCalendar::GetFirstDayOfWeek(BOOL* pbLocal /*= NULL*/) const
	{
		assert(IsWindow());
		DWORD dwValue = MonthCal_GetFirstDayOfWeek(m_hWnd);

		if (pbLocal)
			*pbLocal = HIWORD(dwValue);

		return LOWORD(dwValue);
	}

	inline int CMonthCalendar::GetMaxSelCount() const
	{
		assert(IsWindow());
		return MonthCal_GetMaxSelCount(m_hWnd);
	}

	inline CRect CMonthCalendar::GetMinReqRect() const
	{
		assert(IsWindow());
		RECT rc;
		MonthCal_GetMinReqRect(m_hWnd, &rc);
		return rc;
	}

	inline int CMonthCalendar::GetMonthDelta() const
	{
		assert(IsWindow());
		return MonthCal_GetMonthDelta(m_hWnd);
	}

	inline int CMonthCalendar::GetMonthRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange, DWORD dwFlags) const
	{
		assert(IsWindow());
		SYSTEMTIME MinMax[2];
		memcpy(&MinMax[0], pMinRange, sizeof(SYSTEMTIME));
		memcpy(&MinMax[1], pMaxRange, sizeof(SYSTEMTIME));
		int nCount = MonthCal_GetMonthRange(m_hWnd, dwFlags, MinMax);
		return nCount;
	}

	inline DWORD CMonthCalendar::GetRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange) const
	{
		assert(IsWindow());
		SYSTEMTIME MinMax[2];
		DWORD dwValue = MonthCal_GetRange(m_hWnd, &MinMax);
		memcpy(pMinRange, &MinMax[0], sizeof(SYSTEMTIME));
		memcpy(pMaxRange, &MinMax[1], sizeof(SYSTEMTIME));
		return dwValue;
	}

	inline BOOL CMonthCalendar::GetSelRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange) const
	{
		assert(IsWindow());
		SYSTEMTIME MinMax[2];
		memcpy(&MinMax[0], pMinRange, sizeof(SYSTEMTIME));
		memcpy(&MinMax[1], pMaxRange, sizeof(SYSTEMTIME));
		return (BOOL)MonthCal_GetSelRange(m_hWnd, &MinMax);
	}

	inline BOOL CMonthCalendar::GetToday(LPSYSTEMTIME pDateTime) const
	{
		assert(IsWindow());
		return MonthCal_GetToday(m_hWnd, pDateTime);
	}

	inline DWORD CMonthCalendar::HitTest(PMCHITTESTINFO pMCHitTest)
	{
		assert(IsWindow());
		return (DWORD)MonthCal_HitTest(m_hWnd, pMCHitTest);
	}

	inline COLORREF CMonthCalendar::SetColor(int nRegion, COLORREF clr)
	{
		assert(IsWindow());
		return (COLORREF)MonthCal_SetColor(m_hWnd, nRegion, clr);
	}

	inline BOOL CMonthCalendar::SetCurSel(const LPSYSTEMTIME pDateTime)
	{
		assert(IsWindow());
		return MonthCal_SetCurSel(m_hWnd, pDateTime);
	}

	inline BOOL CMonthCalendar::SetDayState(int nMonths, LPMONTHDAYSTATE pStates)
	{
		assert(IsWindow());
		return (BOOL)MonthCal_SetDayState(m_hWnd, nMonths, pStates);
	}

	inline BOOL CMonthCalendar::SetFirstDayOfWeek(int iDay, int* pnOld/* = NULL*/)
	{
		assert(IsWindow());
		DWORD dwValue = (DWORD)MonthCal_SetFirstDayOfWeek(m_hWnd, iDay);

		if(pnOld)
			*pnOld = LOWORD(dwValue);

		return (BOOL)HIWORD(dwValue);
	}

	inline BOOL CMonthCalendar::SetMaxSelCount(int nMax)
	{
		assert(IsWindow());
		return MonthCal_SetMaxSelCount(m_hWnd, nMax);
	}

	inline int CMonthCalendar::SetMonthDelta(int iDelta)
	{
		assert(IsWindow());
		return MonthCal_SetMonthDelta(m_hWnd, iDelta);
	}

	inline BOOL CMonthCalendar::SetRange(const LPSYSTEMTIME pMinRange, const LPSYSTEMTIME pMaxRange)
	{

		SYSTEMTIME MinMax[2];
		DWORD dwLimit;

		if (pMinRange != NULL)
		{
			memcpy(&MinMax[0], pMinRange, sizeof(SYSTEMTIME));
			dwLimit = GDTR_MIN;
		}

		if (pMaxRange)
		{
			memcpy(&MinMax[1], pMaxRange, sizeof(SYSTEMTIME));
			dwLimit |= GDTR_MAX;
		}

		return (BOOL)MonthCal_SetRange(m_hWnd, dwLimit, &MinMax);
	}

	inline BOOL CMonthCalendar::SetSelRange(const LPSYSTEMTIME pMinRange, const LPSYSTEMTIME pMaxRange)
	{
		SYSTEMTIME MinMax[2];
		memcpy(&MinMax[0], pMinRange, sizeof(SYSTEMTIME));
		memcpy(&MinMax[1], pMaxRange, sizeof(SYSTEMTIME));
		return (BOOL)MonthCal_SetSelRange(m_hWnd, &MinMax);
	}

	inline void CMonthCalendar::SetToday(const LPSYSTEMTIME pDateTime)
	{
		assert(IsWindow());
		MonthCal_SetToday(m_hWnd, pDateTime);
	}


	////////////////////////////////////////
	// Definitions for the CProgressBar class
	//
	inline int CProgressBar::GetPos() const
	// Retrieves the current position of the progress bar.
	{
		assert(IsWindow());
		return (int)SendMessage(PBM_GETPOS, 0, 0);
	}

	inline int CProgressBar::GetRange(BOOL fWhichLimit, PPBRANGE ppBRange) const
	// Retrieves information about the current high and low limits of the progress bar control.
	{
		assert(IsWindow());
		return (int)SendMessage(PBM_GETRANGE, (WPARAM)fWhichLimit, (LPARAM) (PPBRANGE) ppBRange);
	}

	inline int CProgressBar::OffsetPos(int nIncrement) const
	// Advances the current position of the progress bar by a specified increment and redraws
	// the bar to reflect the new position.
	{
		assert(IsWindow());
		return (int)SendMessage(PBM_DELTAPOS, (WPARAM)nIncrement, 0);
	}

	inline int CProgressBar::SetPos(int nNewPos) const
	// Sets the current position for the progress bar and redraws the bar to reflect the new position.
	{
		assert(IsWindow());
		return (int)SendMessage(PBM_SETPOS, (WPARAM)nNewPos, 0);
	}

	inline int CProgressBar::SetRange(short nMinRange, short nMaxRange) const
	// Sets the minimum and maximum values for the progress bar and redraws the bar to reflect the new range.
	{
		assert(IsWindow());
		return (int)SendMessage(PBM_SETRANGE, 0, (LPARAM) MAKELPARAM (nMinRange, nMaxRange));
	}

	inline int CProgressBar::SetStep(int nStepInc) const
	// Specifies the step increment for the progress bar.
	{
		assert(IsWindow());
		return (int)SendMessage(PBM_SETSTEP, (WPARAM)nStepInc, 0);
	}

	inline int CProgressBar::StepIt() const
	// Advances the current position for a progress bar by the step increment and
	// redraws the bar to reflect the new position.
	{
		assert(IsWindow());
		return (int)SendMessage(PBM_STEPIT, 0, 0);
	}


	////////////////////////////////////////
	// Definitions for the CScrollBar class
	//
	inline BOOL CScrollBar::EnableScrollBar( UINT nArrowFlags )  const
	// Enables or disables the scroll bar arrows.
	{
		assert(IsWindow());
		return ::EnableScrollBar(m_hWnd, SB_CTL, nArrowFlags);
	}

	inline BOOL CScrollBar::GetScrollInfo(LPSCROLLINFO lpsi)  const
	// Retrieves the parameters of a scroll bar, including the minimum and maximum
	// scrolling positions, the page size, and the position of the scroll box (thumb).
	{
		assert(IsWindow());
		return ::GetScrollInfo(m_hWnd, SB_CTL, lpsi);
	}

	inline int CScrollBar::GetScrollPos()  const
	// Retrieves the current position of the scroll box (thumb) in the scroll bar.
	{
		assert(IsWindow());
		return ::GetScrollPos(m_hWnd, SB_CTL);
	}

	inline BOOL CScrollBar::GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos )  const
	// Retrieves the current minimum and maximum scroll box (thumb) positions for the scroll bar.
	{
		assert(IsWindow());
		return ::GetScrollRange(m_hWnd, SB_CTL, lpMinPos, lpMaxPos);
	}

	inline BOOL CScrollBar::SetScrollInfo(LPSCROLLINFO lpsi, BOOL bRedraw )  const
	// Sets the parameters of the scroll bar, including the minimum and maximum scrolling positions,
	// the page size, and the position of the scroll box (thumb).
	{
		assert(IsWindow());
		return ::SetScrollInfo(m_hWnd, SB_CTL, lpsi, bRedraw);
	}

	inline int CScrollBar::SetScrollPos(int nPos, BOOL bRedraw)  const
	// Sets the position of the scroll box (thumb) in the scroll bar and, if requested,
	// redraws the scroll bar to reflect the new position of the scroll box.
	{
		assert(IsWindow());
		return ::SetScrollPos(m_hWnd, SB_CTL, nPos, bRedraw);
	}

	inline BOOL CScrollBar::SetScrollRange( int nMinPos, int nMaxPos, BOOL bRedraw )  const
	// Sets the minimum and maximum scroll box positions for the scroll bar.
	{
		assert(IsWindow());
		return ::SetScrollRange(m_hWnd, SB_CTL, nMinPos, nMaxPos, bRedraw);
	}

	inline BOOL CScrollBar::ShowScrollBar(BOOL bShow)  const
	// Shows or hides the scroll bar.
	{
		assert(IsWindow());
		return ::ShowScrollBar(m_hWnd, SB_CTL, bShow);
	}

	////////////////////////////////////////
	// Definitions for the CSlider class
	//
	inline void CSlider::ClearSel() const
	// Clears the current selection range in the trackbar.
	{
		assert(IsWindow());
		SendMessage(TBM_CLEARSEL, 0, 0);
	}

	inline void CSlider::ClearTics(BOOL bRedraw) const
	// Removes the current tick marks from the trackbar.
	{
		assert(IsWindow());
		SendMessage(TBM_CLEARTICS, (WPARAM)bRedraw, 0);
	}

	inline CWnd* CSlider::GetBuddy(BOOL fLocation) const
	// Retrieves the handle to the trackbar control buddy window at a given location.
	{
		assert(IsWindow());
		return FromHandle((HWND)SendMessage(TBM_GETBUDDY, (WPARAM)fLocation, 0));
	}

	inline CRect CSlider::GetChannelRect() const
	// Retrieves the size and position of the bounding rectangle for the trackbar's channel.
	{
		assert(IsWindow());
		CRect rc;
		SendMessage(TBM_GETCHANNELRECT, 0, (LPARAM)&rc);
		return rc;
	}

	inline int  CSlider::GetLineSize() const
	// Retrieves the number of logical positions the trackbar's slider moves in response
	// to keyboard input from the arrow keys.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETLINESIZE, 0, 0);
	}

	inline int  CSlider::GetNumTics() const
	// Retrieves the number of tick marks in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETNUMTICS, 0, 0);
	}

	inline int  CSlider::GetPageSize() const
	// Retrieves the number of logical positions the trackbar's slider moves in response to
	// keyboard input, or mouse input, such as clicks in the trackbar's channel.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETPAGESIZE, 0, 0);
	}

	inline int  CSlider::GetPos() const
	// Retrieves the current logical position of the slider in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETPOS, 0, 0);
	}

	inline int  CSlider::GetRangeMax() const
	// Retrieves the maximum position for the slider in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETRANGEMAX, 0, 0);
	}

	inline int  CSlider::GetRangeMin() const
	// Retrieves the minimum position for the slider in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETRANGEMIN, 0, 0);
	}

	inline int  CSlider::GetSelEnd() const
	// Retrieves the ending position of the current selection range in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETSELEND, 0, 0);
	}

	inline int  CSlider::GetSelStart() const
	// Retrieves the starting position of the current selection range in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETSELSTART, 0, 0);
	}

	inline int  CSlider::GetThumbLength() const
	// Retrieves the length of the slider in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETTHUMBLENGTH, 0, 0);
	}

	inline CRect CSlider::GetThumbRect() const
	// Retrieves the size and position of the bounding rectangle for the slider in the trackbar.
	{
		CRect rc;
		SendMessage(TBM_GETTHUMBRECT, 0, (LPARAM)&rc);
		return rc;
	}

	inline int CSlider::GetTic(int nTic ) const
	// Retrieves the logical position of a tick mark in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETTIC, (WPARAM)nTic, 0);
	}

	inline int  CSlider::GetTicPos(int nTic) const
	// Retrieves the current physical position of a tick mark in the trackbar.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_GETTICPOS, (WPARAM)nTic, 0);
	}

	inline CToolTip* CSlider::GetToolTips() const
	// Retrieves the handle to the ToolTip control assigned to the trackbar, if any.
	{
		assert(IsWindow());
		return (CToolTip*)FromHandle((HWND)SendMessage(TBM_GETTOOLTIPS, 0, 0));
	}

	inline CWnd* CSlider::SetBuddy(CWnd* pBuddy, BOOL fLocation /*= TRUE*/ ) const
	// Assigns a window as the buddy window for the trackbar control.
	{
		assert(IsWindow());
		return FromHandle((HWND)SendMessage(TBM_SETBUDDY, (WPARAM)fLocation, (LPARAM)pBuddy->GetHwnd()));
	}

	inline int  CSlider::SetLineSize(int nSize) const
	// Sets the number of logical positions the trackbar's slider moves in response to
	// keyboard input from the arrow keys.
	{
		assert(IsWindow());
		return(int)SendMessage(TBM_SETLINESIZE, 0, (LPARAM)nSize);
	}

	inline int  CSlider::SetPageSize(int nSize) const
	// Sets the number of logical positions the trackbar's slider moves in response to
	// keyboard input, or mouse input such as clicks in the trackbar's channel.
	{
		assert(IsWindow());
		return(int)SendMessage(TBM_SETPAGESIZE, 0, (LPARAM)nSize);
	}

	inline void CSlider::SetPos(int nPos, BOOL bRedraw) const
	// Sets the current logical position of the slider in the trackbar.
	{
		assert(IsWindow());
		SendMessage(TBM_SETPOS, (WPARAM)bRedraw, (LPARAM)nPos);
	}

	inline void CSlider::SetRangeMax(int nMax, BOOL bRedraw) const
	// Sets the maximum logical position for the slider in the trackbar.
	{
		assert(IsWindow());
		SendMessage(TBM_SETRANGEMAX, (WPARAM)bRedraw, (LPARAM)nMax);
	}

	inline void CSlider::SetRangeMin(int nMax, BOOL bRedraw) const
	// Sets the minimum logical position for the slider in the trackbar.
	{
		assert(IsWindow());
		SendMessage(TBM_SETRANGEMIN, (WPARAM)bRedraw, (LPARAM)nMax);
	}

	inline void CSlider::SetSelection(int nMin, int nMax, BOOL bRedraw) const
	// Sets the starting and ending positions for the available selection range in the trackbar.
	{
		assert(IsWindow());
		SendMessage(TBM_SETSEL, (WPARAM)bRedraw, (LPARAM)MAKELONG(nMax, nMin));
	}

	inline BOOL CSlider::SetTic(int nTic) const
	// Sets a tick mark in the trackbar at the specified logical position.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(TBM_SETTIC, 0, nTic);
	}

	inline void CSlider::SetTicFreq(int nFreq)  const
	// Sets the interval frequency for tick marks in the trackbar.
	{
		assert(IsWindow());
		SendMessage(TBM_SETTICFREQ, (WPARAM)nFreq, 0);
	}

	inline int  CSlider::SetTipSide(int nLocation) const
	// Positions a ToolTip control used by the trackbar control.
	{
		assert(IsWindow());
		return (int)SendMessage(TBM_SETTIPSIDE, (WPARAM)nLocation, 0);
	}

	inline void CSlider::SetToolTips(CToolTip* pToolTip) const
	// Assigns a ToolTip control to the trackbar control.
	{
		assert(IsWindow());
		HWND hToolTip = pToolTip? pToolTip->GetHwnd() : 0;
		SendMessage(TBM_SETTOOLTIPS, (WPARAM)hToolTip, 0);
	}

	////////////////////////////////////////
	// Definitions for the CSpinButton class
	//
	inline int CSpinButton::GetAccel(int cAccels, LPUDACCEL paAccels) const
	// Retrieves acceleration information for the up-down control.
	{
		assert(IsWindow());
		return (int)SendMessage(UDM_GETACCEL, (WPARAM)cAccels, (LPARAM)paAccels);
	}

	inline int CSpinButton::GetBase() const
	// Retrieves the current radix base (that is, either base 10 or 16) for the up-down control.
	{
		assert(IsWindow());
		return (int)SendMessage(UDM_GETBASE, 0, 0);
	}

	inline CWnd* CSpinButton::GetBuddy() const
	// Retrieves the handle to the current buddy window.
	{
		assert(IsWindow());
		return FromHandle((HWND)SendMessage(UDM_GETBUDDY, 0, 0));
	}

	inline int CSpinButton::GetPos() const
	// Retrieves the current position of the up-down control with 16-bit precision.
	{
		assert(IsWindow());
		return (int)SendMessage(UDM_GETPOS, 0, 0);
	}

	inline DWORD CSpinButton::GetRange() const
	// Retrieves the minimum and maximum positions (range) for the up-down control.
	{
		assert(IsWindow());
		return (DWORD)SendMessage(UDM_GETRANGE, 0, 0);
	}

	inline void CSpinButton::PreCreate(CREATESTRUCT &cs)
    {
		cs.style = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VISIBLE |UDS_SETBUDDYINT;
	}

	inline void CSpinButton::PreRegisterClass(WNDCLASS &wc)
	{
		wc.lpszClassName = UPDOWN_CLASS;
	}

	inline BOOL CSpinButton::SetAccel(int cAccels, LPUDACCEL paAccels) const
	// Sets the acceleration for the up-down control.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(UDM_SETACCEL, (WPARAM)cAccels, (LPARAM)paAccels);
	}

	inline int CSpinButton::SetBase(int nBase) const
	// Sets the radix base for the up-down control.
	{
		assert(IsWindow());
		return (int)SendMessage(UDM_SETBASE, (WPARAM)nBase, 0);
	}

	inline CWnd* CSpinButton::SetBuddy(CWnd* pBuddy) const
	// Sets the buddy window for the up-down control.
	{
		assert(IsWindow());
		return FromHandle((HWND)SendMessage(UDM_SETBUDDY, (WPARAM)pBuddy->GetHwnd(), 0));
	}

	inline int CSpinButton::SetPos(int nPos) const
	// Sets the current position for the up-down control with 16-bit precision.
	{
		assert(IsWindow());
		return (int)SendMessage(UDM_SETPOS, 0, (LPARAM)MAKELONG ((short) nPos, 0));
	}

	inline void CSpinButton::SetRange(int nLower, int nUpper) const
	// Sets the minimum and maximum positions (range) for the up-down control.
	{
		assert(IsWindow());
		SendMessage(UDM_SETRANGE, 0, (LPARAM)MAKELONG(nUpper, nLower));
	}

	////////////////////////////////////////
	// Definitions for the CToolTip class
	//

	inline CToolTip::CToolTip()
	{}

	inline CToolTip::~CToolTip()
	{}

	inline void CToolTip::Activate(BOOL bActivate)
	// Activates or deactivates a ToolTip control.
	{
		assert(IsWindow());
		SendMessage(TTM_ACTIVATE, bActivate, 0);
	}

	inline BOOL CToolTip::AddTool(CWnd* pWnd, UINT nIDText, LPCRECT lpRectTool /*= NULL*/, UINT_PTR nIDTool /*= 0*/)
	// Registers a tool with a ToolTip control.
	{
		assert(IsWindow());
		assert(pWnd);
		TOOLINFO ti;
		LoadToolInfo(ti, pWnd, nIDTool);
		ti.hinst = GetApp()->GetResourceHandle();
		ti.lpszText = (LPTSTR)MAKEINTRESOURCE(nIDText);
		if (lpRectTool)
			ti.rect = *lpRectTool;
		return (BOOL)SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
	}

	inline BOOL CToolTip::AddTool(CWnd* pWnd, LPCTSTR lpszText /*= LPSTR_TEXTCALLBACK*/, LPCRECT lpRectTool /*= NULL*/, UINT_PTR nIDTool /*= 0*/)
	// Registers a tool with a ToolTip control.
	{
		assert(IsWindow());
		assert(pWnd);
		TOOLINFO ti;
		LoadToolInfo(ti, pWnd, nIDTool);
		ti.lpszText = (LPTSTR)lpszText;
		if (lpRectTool)
			ti.rect = *lpRectTool;
		return (BOOL)SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
	}

	inline void CToolTip::DelTool(CWnd* pWnd, UINT_PTR nIDTool /*= 0*/)
	// Removes a tool from a ToolTip control.
	{
		assert(IsWindow());
		TOOLINFO ti;
		LoadToolInfo(ti, pWnd, nIDTool);
		SendMessage(TTM_DELTOOL, 0, (LPARAM)&ti);
	}

	inline int CToolTip::GetDelayTime(DWORD dwDuration) const
	// Retrieves the initial, pop-up, and reshow durations currently set for a ToolTip control.
	{
		assert(IsWindow());
		return (int)SendMessage(TTM_GETDELAYTIME, dwDuration, 0);
	}

	inline void CToolTip::GetMargin(LPRECT lprc) const
	// Retrieves the top, left, bottom, and right margins set for a ToolTip window.
	{
		assert(IsWindow());
		SendMessage(TTM_GETMARGIN, 0, (LPARAM)lprc);
	}

	inline int CToolTip::GetMaxTipWidth() const
	// Retrieves the maximum width for a ToolTip window.
	{
		assert(IsWindow());
		return (int)SendMessage(TTM_GETMAXTIPWIDTH, 0, 0);
	}

	inline void CToolTip::GetText(CString& str, CWnd* pWnd, UINT_PTR nIDTool /*= 0*/) const
	// Retrieves the information a ToolTip control maintains about a tool.
	{
		assert(IsWindow());
		assert(pWnd);
		LPTSTR pszText = str.GetBuffer(80);	// Maximum allowed Tooltip is 80 characters for Windows XP and below
		TOOLINFO ti;
		LoadToolInfo(ti, pWnd, nIDTool);
		ti.lpszText = pszText;
		SendMessage(TTM_GETTEXT, 0, (LPARAM)&ti);
		str.ReleaseBuffer();
	}

	inline COLORREF CToolTip::GetTipBkColor() const
	// Retrieves the background color in a ToolTip window.
	{
		assert(IsWindow());
		return (COLORREF)SendMessage(TTM_GETTIPBKCOLOR, 0,0);
	}

	inline COLORREF CToolTip::GetTipTextColor() const
	// Retrieves the text color in a ToolTip window.
	{
		assert(IsWindow());
		return (COLORREF)SendMessage(TTM_GETTIPTEXTCOLOR, 0, 0);
	}

	inline int CToolTip::GetToolCount() const
	// Retrieves a count of the tools maintained by a ToolTip control.
	{
		assert(IsWindow());
		return (int)SendMessage(TTM_GETTOOLCOUNT, 0, 0);
	}

	inline BOOL CToolTip::GetToolInfo(TOOLINFO& ToolInfo, CWnd* pWnd, UINT_PTR nIDTool /*= 0*/) const
	// Retrieves the information that a ToolTip control maintains about a tool.
	{
		assert(IsWindow());
		assert(pWnd);
		LoadToolInfo(ToolInfo, pWnd, nIDTool);
		return (BOOL)SendMessage(TTM_GETTOOLINFO, 0, (LPARAM)&ToolInfo);
	}

	inline BOOL CToolTip::HitTest(CWnd* pWnd, CPoint pt, LPTOOLINFO lpToolInfo) const
	// Tests a point to determine whether it is within the bounding rectangle of the 
	//  specified tool and, if it is, retrieves information about the tool.
	{
		assert(IsWindow());
		assert(pWnd);
		assert(lpToolInfo);
		TTHITTESTINFO hti = {0};
		hti.hwnd = pWnd->GetHwnd();
		hti.pt = pt;
		hti.ti = *lpToolInfo;
		return (BOOL)SendMessage(TTM_HITTEST, 0, (LPARAM)&hti);
	}

	inline void CToolTip::LoadToolInfo(TOOLINFO& ti, CWnd* pWnd, UINT_PTR nIDTool) const
	{
		ZeroMemory(&ti, sizeof(TOOLINFO));
		ti.cbSize = sizeof(TOOLINFO);
		HWND hWnd = pWnd->GetHwnd();
		if (nIDTool == 0)
		{
			ti.hwnd = ::GetParent(hWnd);
			ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
			ti.uId = (UINT)hWnd;
		}
		else
		{
			ti.hwnd = hWnd;
			ti.uId = nIDTool;
		}
	}

	inline void CToolTip::Pop()
	// Removes a displayed ToolTip window from view.
	{
		assert(IsWindow());
		SendMessage(TTM_POP, 0, 0);
	}

	inline void CToolTip::RelayEvent(LPMSG lpMsg)
	// Passes a mouse message to a ToolTip control for processing.
	{
		assert(IsWindow());
		SendMessage(TTM_RELAYEVENT, 0, (LPARAM)lpMsg);
	}

	inline void CToolTip::SetDelayTime(UINT nDelay)
	// Sets the initial delay for a ToolTip control.
	{
		assert(IsWindow());
		SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, nDelay);
	}

	inline void CToolTip::SetDelayTime(DWORD dwDuration, int iTime)
	// Sets the initial, pop-up, and reshow durations for a ToolTip control.
	{
		assert(IsWindow());
		SendMessage(TTM_SETDELAYTIME, dwDuration, iTime);
	}

	inline void CToolTip::SetMargin(LPRECT lprc)
	// Sets the top, left, bottom, and right margins for a ToolTip window.
	{
		assert(IsWindow());
		assert(lprc);
		SendMessage(TTM_SETMARGIN, 0, (LPARAM)lprc);
	}

	inline int CToolTip::SetMaxTipWidth(int iWidth)
	// Sets the maximum width for a ToolTip window.
	{
		assert(IsWindow());
		return (int)SendMessage(TTM_SETMAXTIPWIDTH, 0, iWidth);
	}

	inline void CToolTip::SetTipBkColor(COLORREF clr)
	// Sets the background color in a ToolTip window.
	// Ignored when XP themes are active.
	{
		assert(IsWindow());
		SendMessage(TTM_SETTIPBKCOLOR, clr, 0);
	}

	inline void CToolTip::SetTipTextColor(COLORREF clr)
	// Sets the text color in a ToolTip window.
	// Ignored when XP themes are active.
	{
		assert(IsWindow());
		SendMessage(TTM_SETTIPTEXTCOLOR, clr, 0);
	}

	inline void CToolTip::SetToolInfo(LPTOOLINFO lpToolInfo)
	// Sets the information that a ToolTip control maintains for a tool.
	{
		assert(IsWindow());
		SendMessage(TTM_SETTOOLINFO, 0, (LPARAM)lpToolInfo);
	}

	inline void CToolTip::SetToolRect(CWnd* pWnd, UINT_PTR nIDTool, LPCRECT lpRect)
	// Sets a new bounding rectangle for a tool.
	{
		assert(IsWindow());
		assert(pWnd);
		assert(lpRect);
		TOOLINFO ti;
		LoadToolInfo(ti, pWnd, nIDTool);
		ti.rect = *lpRect;
		SendMessage(TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
	}

	inline void CToolTip::Update()
	// Forces the current tool to be redrawn.
	{
		assert(IsWindow());
		SendMessage(TTM_UPDATE, 0, 0);
	}

	inline void CToolTip::UpdateTipText(LPCTSTR lpszText, CWnd* pWnd, UINT_PTR nIDTool /*= 0*/)
	// Sets the ToolTip text for a tool.
	{
		assert(IsWindow());
		assert(pWnd);
		TOOLINFO ti;
		LoadToolInfo(ti, pWnd, nIDTool);
		ti.lpszText = (LPTSTR)lpszText;
		SendMessage(TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
	}

	inline void CToolTip::UpdateTipText(UINT nIDText, CWnd* pWnd, UINT_PTR nIDTool /*= 0*/)
	// Sets the ToolTip text for a tool.
	{
		assert(IsWindow());
		assert(pWnd);
		TOOLINFO ti;
		LoadToolInfo(ti, pWnd, nIDTool);
		ti.hinst = GetApp()->GetResourceHandle();
		ti.lpszText = (LPTSTR)MAKEINTRESOURCE(nIDText);
		SendMessage(TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
	}

#if (defined TTM_SETTITLE) && (_WIN32_IE >=0x0500)
	inline BOOL CToolTip::AdjustRect(LPRECT lprc, BOOL bLarger /*= TRUE*/)
	// Calculates a ToolTip control's text display rectangle from its window rectangle, or the 
	//  ToolTip window rectangle needed to display a specified text display rectangle.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(TTM_ADJUSTRECT, bLarger, (LPARAM)lprc);
	}

	inline CSize CToolTip::GetBubbleSize(LPTOOLINFO lpToolInfo) const
	// Returns the width and height of a ToolTip control.
	{
		assert(IsWindow());
		LRESULT lr = SendMessage(TTM_GETBUBBLESIZE, (LPARAM)lpToolInfo);
		CSize sz(LOWORD(lr), HIWORD(lr));
		return sz;
	}

	inline BOOL CToolTip::SetTitle(UINT uIcon, LPCTSTR lpstrTitle)
	// Adds a standard icon and title string to a ToolTip.
	{
		assert(IsWindow());
		return (BOOL)SendMessage(TTM_SETTITLE, uIcon, (LPARAM)lpstrTitle);
	}
#endif

#if (defined TTM_SETWINDOWTHEME) && (WINVER >= 0x0501)
	inline HRESULT CToolTip::SetWindowTheme(LPCWSTR lpstrTheme)
	// Sets the visual style of a ToolTip control.
	{
		assert(IsWindow());
		SendMessage(TTM_SETWINDOWTHEME, 0, (LPARAM)lpstrTheme);
	}
#endif


} // namespace Win32xx

#endif //  define _WIN32XX_CONTROLS_H_


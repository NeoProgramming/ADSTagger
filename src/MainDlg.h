// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <sstream>
#include <iomanip>
#include <atltime.h>
#include "listctrl/ListCtrl.h"
#include "TaggerCore.h"

class CMainDlg : 
	public CDialogImpl<CMainDlg>, 
	public CDialogResize<CMainDlg>
{
public:
	CMainDlg();
	
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
	//	MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_ADDTAGS, OnAddTags)
		CHAIN_MSG_MAP(CDialogResize<CMainDlg>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CMainDlg)
		DLGRESIZE_CONTROL(IDC_ADDTAGS, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_NEWTAGS, DLSZ_SIZE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		// группа работает кривенько
	//	BEGIN_DLGRESIZE_GROUP()
		DLGRESIZE_CONTROL(IDC_USEDTAGS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_FREETAGS, DLSZ_SIZE_X | DLSZ_SIZE_Y | DLSZ_MOVE_X)
	//	END_DLGRESIZE_GROUP()
	END_DLGRESIZE_MAP()


protected:
	// списки; список может хранить userdata по умолчанию (DWORD) или более осмысленный
	CListCtrlData< TagPtr > m_wndUsedTags;	// список; физически это CListCtrlData< DWORD >
	CListCtrlData< TagPtr > m_wndFreeTags;	// список с данными - шаблонный аргумент, хранилище внутри

	CEdit m_wndNewTags;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	void LoadLists();
	void LoadUsedTags(TagIter b, TagIter e);
	void LoadFreeTags(TagIter b, TagIter e, int chk);
	void SaveLists();
	
	void AddTags();
	void AppAbout();

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddTags(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

#include "stdafx.h"
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include "resource.h"
#include "MainDlg.h"
#include "TaggerCore.h"

CMainDlg::CMainDlg()
{
	m_wndUsedTags.RegisterClass();
	m_wndFreeTags.RegisterClass();
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);
	
	HMENU pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu)
	{
	//	::InsertMenu(pSysMenu, 0, MF_BYPOSITION | MF_STRING, ID_APP_ABOUT, _T("About NeoTagger..."));
	//	::InsertMenu(pSysMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, _T(""));
		AppendMenu(pSysMenu, MF_SEPARATOR, 0, _T(""));
		AppendMenu(pSysMenu, MF_STRING, ID_APP_ABOUT, _T("About NeoTagger..."));
	}

	m_wndNewTags = GetDlgItem(IDC_NEWTAGS);
	m_wndUsedTags.SubclassWindow(GetDlgItem(IDC_USEDTAGS));
	m_wndFreeTags.SubclassWindow(GetDlgItem(IDC_FREETAGS));

	DlgResize_Init();

	Core.init();
	LoadIni();
	LoadLists();
		
	return TRUE;
}

LRESULT CMainDlg::OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if (wParam == ID_APP_ABOUT) {
		AppAbout();
		bHandled = TRUE;
		return TRUE;
	}
	bHandled = FALSE;
	return DefWindowProc();
}

//LRESULT CDlgResize::OnSize(UINT nType, int cx, int cy){}
LRESULT CMainDlg::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// перехватываем все здесь
	/*
	const int d = 6;
	const int bh = 25;
	const int bw1 = 60;
	const int bw2 = 80;
	const int lw = (cx - d * 3) / 2;
	if (m_listUsedTags.GetSafeHwnd()) 
		m_listUsedTags.MoveWindow(d, d, lw, cy - 3*d - bh);
	if (m_listFreeTags.GetSafeHwnd())
		m_listFreeTags.MoveWindow(lw+2*d, d, lw, cy - 3 * d - bh);
	if(m_btnAddTag.GetSafeHwnd())
		m_btnAddTag.MoveWindow(d, cy - d - bh, bw1, bh);
	if (m_editNewTag.GetSafeHwnd())
		m_editNewTag.MoveWindow(2*d+bw1, cy - d - bh, lw - d - bw1, bh);
	if (m_btnOK.GetSafeHwnd())
		m_btnOK.MoveWindow(cx - 2*d - 2*bw2, cy - d - bh, bw2, bh);
	if (m_btnCancel.GetSafeHwnd())
		m_btnCancel.MoveWindow(cx - d - bw2, cy - d - bh, bw2, bh);
	*/
	return TRUE;
}

void CMainDlg::LoadUsedTags(TagIter b, TagIter e)
{
	for (; b != e; ++b) {
		if (b->chk != 0) {
			int nNewItem = m_wndUsedTags.AddItem(_T(""));
			m_wndUsedTags.SetItemCheck(nNewItem, 0, b->chk);
			m_wndUsedTags.SetItemText(nNewItem, 1, Core.m_converter.from_bytes(b->tag).c_str());
			m_wndUsedTags.SetItemText(nNewItem, 2, Core.m_converter.from_bytes(b->val).c_str());
			TagPtr p = &(*b);
			m_wndUsedTags.SetItemData(nNewItem, p);
		}
	}
}

void CMainDlg::LoadFreeTags(TagIter b, TagIter e, int chk)
{
	for (; b != e; ++b) {
		if (b->chk == chk) {
			int nNewItem = m_wndFreeTags.AddItem(_T(""));
			m_wndFreeTags.SetItemCheck(nNewItem, 0, b->chk);
			m_wndFreeTags.SetItemText(nNewItem, 1, Core.m_converter.from_bytes(b->tag).c_str());
			TagPtr p = &(*b);
			m_wndFreeTags.SetItemData(nNewItem, p);
		}
	}
}

void CMainDlg::LoadLists()
{
	m_wndUsedTags.SetFocusSubItem(TRUE);

	m_wndUsedTags.AddColumn(_T(""), 0, ITEM_IMAGE_3STATE, TRUE, ITEM_FORMAT_CHECKBOX_3STATE);
	m_wndUsedTags.AddColumn(_T("Used Tags"), 130);
	m_wndUsedTags.AddColumn(_T("Values"), 80, -1, FALSE, ITEM_FORMAT_NONE);

	LoadUsedTags(Core.m_Tags.begin(), Core.m_Tags.end());
	
	m_wndFreeTags.AddColumn(_T(""), 0, ITEM_IMAGE_CHECKBOX, TRUE, ITEM_FORMAT_CHECKBOX);
	m_wndFreeTags.AddColumn(_T("Free Tags"), 130);
	
	LoadFreeTags(Core.m_Tags.begin(), Core.m_Tags.end(), 0);
	
}

void CMainDlg::SaveLists()
{
	// пройтись по спискам и собрать чекбоксы
	for (auto L : { &m_wndUsedTags, &m_wndFreeTags }) {
		for (int i = 0, n = L->GetItemCount(); i < n; i++) {
			TagPtr p = nullptr;
			if(L->GetItemData(i, p) && p)
				p->chk = _ttoi( L->GetItemText(i, 0) );
		}
	}
}

void CMainDlg::AppAbout()
{
	CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
	dlg.DoModal();
}

void CMainDlg::LoadIni()
{
	Core.loadIni();
	RECT rect;
	rect.left = Core.Cfg.x1;
	rect.top = Core.Cfg.y1;
	rect.right = Core.Cfg.x2;
	rect.bottom = Core.Cfg.y2;
	SetWindowPos(0, &rect, SWP_NOZORDER);
}

void CMainDlg::SaveIni()
{
	RECT rect;
	BOOL r = GetWindowRect(&rect);
	Core.Cfg.x1 = rect.left;
	Core.Cfg.y1 = rect.top;
	Core.Cfg.x2 = rect.right;
	Core.Cfg.y2 = rect.bottom;
	Core.saveIni();
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	AddTags();
	SaveLists();
	Core.apply();
	SaveIni();
	EndDialog(wID);

	// cm_RereadSource
	HWND h = FindWindow(_T("TTOTAL_CMD"), 0);
	if(h != INVALID_HANDLE_VALUE)
		::PostMessage(h, EM_DISPLAYBAND, 540, 0);

	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SaveIni();
	EndDialog(wID);
	return 0;
}

void CMainDlg::AddTags()
{
	ATL::CString str;
	m_wndNewTags.GetWindowTextW(str);
	std::string line = Core.m_converter.to_bytes(str);
	TagIter it = Core.parseTags(line.c_str(), nullptr);
	LoadFreeTags(it, Core.m_Tags.end(), 1);
}

LRESULT CMainDlg::OnAddTags(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// add new tags to UsedTags
	AddTags();

	return 0;
}

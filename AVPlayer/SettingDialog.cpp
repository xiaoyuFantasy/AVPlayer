#include "stdafx.h"
#include "SettingDialog.h"
#include "FileDialogEx.h"

CSettingDialog::CSettingDialog()
{
}

CSettingDialog::~CSettingDialog()
{
}

LPCTSTR CSettingDialog::GetWindowClassName() const
{
	return L"SettingDialog";
}

CDuiString CSettingDialog::GetSkinFolder()
{
	return L"";
}

CDuiString CSettingDialog::GetSkinFile()
{
	return L"setting_frame.xml";
}

void CSettingDialog::OnFinalMessage(HWND hWnd)
{
}

void CSettingDialog::InitWindow()
{
	m_pEditPath = (CEditUI*)m_PaintManager.FindControl(L"edit_path");
	m_pOptRender = (COptionUI*)m_PaintManager.FindControl(L"normal_render");
}

void CSettingDialog::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (_tcscmp(msg.pSender->GetName(), _T("btn_browse")) == 0)
		{
			std::wstring file_type = L"文件格式";
			LPCTSTR filter = L"*.mp4;*.mp3;*.avi;*.3g2;*.3gp;*.dvd;*.flv;*.hls;";
			std::map<LPCTSTR, LPCTSTR> filters;
			filters[file_type.c_str()] = filter;
			CFileDialogEx* file_dlg = new CFileDialogEx();
			file_dlg->SetFilter(filters);
			file_dlg->SetMultiSel(TRUE);
			file_dlg->SetParentWnd(m_hWnd);
			file_dlg->ShowOpenFileDlg(std::bind(&CSettingDialog::OnFileSelected, this, std::placeholders::_1, std::placeholders::_2));
			delete file_dlg;
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btn_ok")) == 0)
		{
			Close();
			if (m_funCallback)
				m_funCallback(std::wstring(m_pEditPath->GetText()), m_pOptRender->IsSelected() ? 1 : 2);
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btn_cancel")) == 0)
			Close();
	}
	
	__super::Notify(msg);
}

void CSettingDialog::SetCallback(std::function<void(std::wstring wstrPath, int type)> callback)
{
	m_funCallback = callback;
}


void CSettingDialog::OnFileSelected(bool bRet, std::wstring filePath)
{
	if (bRet && !filePath.empty())
		m_pEditPath->SetText(filePath.c_str());
}

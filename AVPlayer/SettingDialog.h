#pragma once
#include <functional>

class CSettingDialog : public WindowImplBase
{
public:
	CSettingDialog();
	virtual ~CSettingDialog();

public:
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFolder();
	CDuiString GetSkinFile();
	void OnFinalMessage(HWND hWnd);
	void InitWindow();
	void Notify(TNotifyUI &msg);

	void SetCallback(std::function<void(std::wstring wstrPath, int type)> callback);

protected:
	void OnFileSelected(bool bRet, std::wstring filePath);

private:
	COptionUI*	m_pOptRender = nullptr;
	CEditUI*	m_pEditPath = nullptr;
	std::function<void(std::wstring wstrPath, int type)> m_funCallback;
};


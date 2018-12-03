#pragma once
#include <functional>

using CallbackFunc = std::function<void(std::wstring wstrPath, int video_type, int decode_type)>;

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

	void SetCallback(CallbackFunc callback);

protected:
	void OnFileSelected(bool bRet, std::wstring filePath);

private:
	COptionUI*	m_pOptRender = nullptr;
	CEditUI*	m_pEditPath = nullptr;
	CComboUI*	m_pDecodeType = nullptr;
	CallbackFunc m_funCallback;
};


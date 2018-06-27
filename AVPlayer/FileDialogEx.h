#pragma once
#include <functional>
#include <map>
#include <Commdlg.h>

struct __POSITION {};
typedef __POSITION* POSITION;

class CFileDialogEx
{
public:
	typedef std::function<void(bool, std::wstring)> FileDialogCallback2;

	enum FileDialogType
	{
		FDT_None,
		FDT_OpenFile,
		FDT_SaveFile
	};
	CFileDialogEx();
	virtual ~CFileDialogEx();

public:
public:
	void SetDefExt(LPCTSTR lpszDefExt);
	void SetFileName(LPCTSTR lpszFileName);
	void SetFlags(DWORD dwFlags);
	void SetFilter(LPCTSTR lpszFilter);
	void SetFilter(std::map<LPCTSTR, LPCTSTR>& filters);
	void SetMultiSel(BOOL bMultiSel = TRUE);
	void SetParentWnd(HWND hParentWnd);
	void SetTitle(LPCTSTR lpszTitle);
	void SetFileNameBufferSize(DWORD dwSize);

	void ShowOpenFileDlg(FileDialogCallback2 file_dialog_callback1);
	void ShowSaveFileDlg(FileDialogCallback2 file_dialog_callback2);
	
	std::wstring GetPathName();
	std::wstring GetFileName();
	std::wstring GetFileExt();
	std::wstring GetFileTitle();
	std::wstring GetFolderPath();

	POSITION GetStartPosition();
	std::wstring GetNextPathName(POSITION& pos);

public:
	OPENFILENAME m_stOFN;

protected:
	void SyncShowModal();

private:
	wchar_t m_szDefExt[64];
	wchar_t m_szFilter[MAX_PATH];
	wchar_t m_szFileName[MAX_PATH];
	wchar_t * m_lpszFileName;
	FileDialogType file_dialog_type_;
	FileDialogCallback2 file_dialog_callback2_;
};


#include "stdafx.h"
#include "AVPlayerWnd.h"
#include "FileDialogEx.h"
#include <atlconv.h>
#include <Shellapi.h>
#pragma comment(lib, "Shell32.lib")

std::wstring StringToWString(const std::string &str)
{
	std::wstring wstr(str.length(), L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr;
}

//只拷贝低字节至string中
std::string WStringToString(const std::wstring &wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}

#define PLAYER_MSG_DURATION (WM_USER + 1)
#define PLAYER_MSG_PROGRESS (WM_USER + 2)

CAVPlayerWnd::CAVPlayerWnd()
{
}


CAVPlayerWnd::~CAVPlayerWnd()
{
}

LPCTSTR CAVPlayerWnd::GetWindowClassName() const
{
	return L"AVPlayerWnd";
}

CDuiString CAVPlayerWnd::GetSkinFolder()
{
	return L"";
}

CDuiString CAVPlayerWnd::GetSkinFile()
{
	return L"player_frame.xml";
}

CControlUI * CAVPlayerWnd::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, _T("PlayerWindow")) == 0)
	{
		CWndUI *pUI = new CWndUI;
		m_pVideo = new CVideoWnd;
		m_pVideo->Create(m_hWnd, L"VideoWnd", UI_WNDSTYLE_CHILD, UI_CLASSSTYLE_CHILD);
		pUI->Attach(m_pVideo->GetHWND());
		return pUI;
	}

	return nullptr;
}

void CAVPlayerWnd::OnFinalMessage(HWND hWnd)
{
	PostQuitMessage(IDOK);
}

void CAVPlayerWnd::InitWindow()
{
	m_pVideoLayout = (CWndUI*)m_PaintManager.FindControl(L"video_player");
	m_pLabelName = (CLabelUI*)m_PaintManager.FindControl(L"video_name");
	m_pSliderPlay = (CSliderUI*)m_PaintManager.FindControl(L"sliderPlay");
	m_pBtnStop = (CButtonUI*)m_PaintManager.FindControl(L"btnStop");
	m_pBtnPrevious = (CButtonUI*)m_PaintManager.FindControl(L"btnPrevious");
	m_pBtnPlay = (CButtonUI*)m_PaintManager.FindControl(L"btnPlay");
	m_pBtnPause = (CButtonUI*)m_PaintManager.FindControl(L"btnPause");
	m_pBtnNext = (CButtonUI*)m_PaintManager.FindControl(L"btnNext");
	m_pBtnVolume = (CButtonUI*)m_PaintManager.FindControl(L"btnVolume");
	m_pBtnVolumeZero = (CButtonUI*)m_PaintManager.FindControl(L"btnVolumeZero");
	m_pSliderVolume = (CSliderUI*)m_PaintManager.FindControl(L"sliderVol");

	if (IsWindow(m_pVideo->GetHWND()))
	{
		m_opts.hWnd = m_pVideo->GetHWND();
		m_opts.hInstance = CPaintManagerUI::GetInstance();
		m_opts.durationCb = CAVPlayerWnd::DurationCallback;
		m_opts.progreeeCb = CAVPlayerWnd::ProgressCallback;
		m_opts.videoCb = CAVPlayerWnd::VideoCallback;
		m_opts.errorCb = CAVPlayerWnd::ErrorCallback;
		m_opts.user_data = this;
	}

	ParseCmdLine(GetCommandLineW(), m_mapCmd);
	auto itor = m_mapCmd.find(L"/spath");
	if (itor != m_mapCmd.end())
	{
		OnFileSelected(true, itor->second);
	}
}

void CAVPlayerWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (_tcscmp(msg.pSender->GetName(), _T("btnPlay")) == 0)
		{
			if (m_pVideo->m_funcIsPlaying && m_pVideo->m_funcIsPlaying(m_pVideo->m_hPlayer))
			{
				m_pVideo->m_funcPause(m_pVideo->m_hPlayer, false);
				m_pBtnPlay->SetVisible(false);
				m_pBtnPause->SetVisible();
			}
			else
			{
				std::wstring file_type = L"文件格式";
				LPCTSTR filter = L"*.mp4;*.mp3;*.avi;*.3g2;*.3gp;*.dvd;*.flv;*.hls;";
				std::map<LPCTSTR, LPCTSTR> filters;
				filters[file_type.c_str()] = filter;
				CFileDialogEx* file_dlg = new CFileDialogEx();
				file_dlg->SetFilter(filters);
				file_dlg->SetMultiSel(TRUE);
				file_dlg->SetParentWnd(m_hWnd);
				file_dlg->ShowOpenFileDlg(std::bind(&CAVPlayerWnd::OnFileSelected, this, std::placeholders::_1, std::placeholders::_2));
				delete file_dlg;
			}
			
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnPause")) == 0)
		{
			if (m_pVideo->m_funcIsPlaying(m_pVideo->m_hPlayer))
			{
				m_pVideo->m_funcPause(m_pVideo->m_hPlayer, true);
				m_pBtnPlay->SetVisible();
				m_pBtnPause->SetVisible(false);
			}
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnStop")) == 0)
		{
			if (m_pVideo->m_funcIsPlaying(m_pVideo->m_hPlayer))
			{
				m_pVideo->m_funcStop(m_pVideo->m_hPlayer);
				m_pBtnPlay->SetVisible();
				m_pBtnPause->SetVisible(false);
				m_pBtnStop->SetEnabled(false);
				m_pVideoLayout->SetVisible(false);
			}
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnPrevious")) == 0)
		{
			int value = m_pSliderPlay->GetValue();
			m_pVideo->m_funcSeek(m_pVideo->m_hPlayer, value - 5);
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnNext")) == 0)
		{
			int value = m_pSliderPlay->GetValue();
			m_pVideo->m_funcSeek(m_pVideo->m_hPlayer, value + 5);
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnVolume")) == 0)
		{
			m_pBtnVolume->SetVisible(false);
			m_pBtnVolumeZero->SetVisible();
			m_pVideo->m_funcSetMuted(m_pVideo->m_hPlayer, true);
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnVolumeZero")) == 0)
		{
			m_pBtnVolume->SetVisible();
			m_pBtnVolumeZero->SetVisible(false);
			m_pVideo->m_funcSetMuted(m_pVideo->m_hPlayer, false);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_VALUECHANGED)
	{
		if (_tcscmp(msg.pSender->GetName(), _T("sliderPlay")) == 0)
		{
			int value = m_pSliderPlay->GetValue();
			m_pVideo->m_funcSeek(m_pVideo->m_hPlayer, value);
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("sliderVol")) == 0)
		{
			int value = m_pSliderVolume->GetValue();
			m_pVideo->m_funcSetVolume(m_pVideo->m_hPlayer, value);
		}
	}
	__super::Notify(msg);
}

LRESULT CAVPlayerWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if (uMsg == PLAYER_MSG_DURATION)
	{
		m_pSliderPlay->SetMinValue(0);
		m_pSliderPlay->SetMaxValue(wParam);
		m_pSliderPlay->SetEnabled();
	}
	else if (uMsg == PLAYER_MSG_PROGRESS)
	{
		m_pSliderPlay->SetValue(wParam);
	}
	else if (uMsg == WM_MOUSEWHEEL)
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta < 0)
			m_scale += 0.1f;
		else
			m_scale -= 0.1f;

		if (m_scale < 0.5f)
			m_scale = 0.5f;

		if (m_scale > 1.5f)
			m_scale = 1.5f;

		m_pVideo->m_funcSetPanoScale(m_pVideo->m_hPlayer, m_scale);
		/*int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		player.m_funcSetPanoRotate(player.m_hPlayer, m_xCurrPos, m_yCurrPos);*/
	}
	return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

int CAVPlayerWnd::ParseCmdLine(const wchar_t * lpCmdLine, std::map<std::wstring, std::wstring>& pMapCmdLine)
{
	int nArgs = 0;
	LPWSTR * szArglist = CommandLineToArgvW(lpCmdLine, &nArgs);

	for (int i = 0; i < nArgs; i++)
	{
		if (wcsncmp(L"//", szArglist[i], 1) != 0)
		{
			continue;
		}
		if (i + 1 < nArgs) //结束  
		{
			if (wcsncmp(L"//", szArglist[i + 1], 1) != 0)
			{
				pMapCmdLine.insert(std::make_pair(szArglist[i], szArglist[i + 1]));
				i++;
				continue;
			}
		}
		pMapCmdLine.insert(std::make_pair(szArglist[i], L"1"));
	}
	LocalFree(szArglist);
	return 0;
}

void CAVPlayerWnd::DurationCallback(void * userdata, int64_t duration)
{
	CAVPlayerWnd *player = (CAVPlayerWnd*)userdata;
	::PostMessage(player->GetHWND(), PLAYER_MSG_DURATION, duration, NULL);
}

void CAVPlayerWnd::ProgressCallback(void * userdata, int64_t progress)
{
	CAVPlayerWnd *player = (CAVPlayerWnd*)userdata;
	::PostMessage(player->GetHWND(), PLAYER_MSG_PROGRESS, progress, NULL);
}

void CAVPlayerWnd::VideoCallback(void * userdata, int nWidth, int nHeight)
{
	CAVPlayerWnd *player = (CAVPlayerWnd*)userdata;
	player->m_proportion = (double)nWidth / (double)nHeight;
	player->m_pVideoLayout->SetProportion(player->m_proportion);
}

void CAVPlayerWnd::ErrorCallback(void * userdata, int err, std::string strMsg)
{
}

void CAVPlayerWnd::OnFileSelected(bool bRet, std::wstring filePath)
{
	if (bRet && !filePath.empty())
	{
		wchar_t szUrl[MAX_PATH + 1] = { 0 };
		_tcscpy_s(szUrl, MAX_PATH, filePath.c_str());
		::PathStripPath(szUrl);
		m_pLabelName->SetText(szUrl);
		m_opts.video_type = VIDEO_TYPE::NORMAL_TYPE;
		m_opts.bEnableAudio = false;
		//m_opts.bEnableVideo = false;
		m_opts.strPath = CW2A(filePath.c_str(), CP_UTF8);
		//m_opts.strPath = "rtmp://playrtmp.simope.com:1935/live/524622521d?liveID=100031600";
		//m_opts.strPath = "rtmp://192.168.160.132/live/vr";
		m_pVideo->m_funcPlay(m_pVideo->m_hPlayer, m_opts);
		m_pBtnPlay->SetVisible(false);
		m_pBtnPause->SetVisible();
		m_pBtnStop->SetEnabled();
		m_pVideoLayout->SetVisible();
	}
}


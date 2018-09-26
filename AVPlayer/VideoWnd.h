#pragma once
#include "AVPlayerInterface.h"
#include "ipc/ipc_listener.h"
#include "ipc/ipc_endpoint.h"

#ifdef DEBUG
#pragma comment(lib, "ipc_d.lib")
#else
#pragma comment(lib, "ipc.lib")
#endif // DEBUG

class CVideoWnd : public WindowImplBase, public IPC::Listener
{
public:
	CVideoWnd();
	virtual ~CVideoWnd();

public:
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFolder();
	CDuiString GetSkinFile();
	void OnFinalMessage(HWND hWnd);
	void InitWindow();
	void Notify(TNotifyUI &msg);

	void SetCmdLine(std::map<std::wstring, std::wstring> &mapCmd);
	void Play();
	void SetOptions(PLAYER_OPTS &opts);
	void Stop();
	void SetDragEnable(bool bEnable);

	bool OnMessageReceived(IPC::Message* msg);
	void OnChannelConnected(int32 peer_pid);
	void OnChannelError();

protected:
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void ResizeVideo();

protected:
	void ParseCommandLine();
	void InitIPC();
	void Send(const std::string& cmd);
	static void FuncPlayerEvent(void* user_data, const PLAYER_EVENT e, LPVOID pData);

public:
	PLAYER_OPTS		m_opts;
	std::map<std::wstring, std::wstring> m_mapCmd;
	bool			m_bLockScreenPlayedPre = false;
	HMODULE			m_hPlayerModule = NULL;
	funcInit		m_funcInit = nullptr;
	funcUnInit		m_funcUnInit = nullptr;
	funcCreatePlayer	m_funcCreatePlayer = nullptr;
	funcDestoryPlayer	m_funcDesotryPlayer = nullptr;
	funcOpen			m_funcOpen = nullptr;
	funcPlay			m_funcPlay = nullptr;
	funcPause			m_funcPause = nullptr;
	funcResume			m_funcResume = nullptr;
	funcStop			m_funcStop = nullptr;
	funcStatus			m_funcStatus = nullptr;
	funcWaitForCompletion	m_funcWaitForCompletion = nullptr;
	funcClose			m_funcClose = nullptr;
	funcSeekTo			m_funcSeekTo = nullptr;
	funcVolume			m_funcVolume = nullptr;
	funcMute			m_funcMute = nullptr;
	funcFullScreen		m_funcFullScreen = nullptr;
	funcCurrentPlayTime m_funcCurrentPlayTime = nullptr;
	funcDuration		m_funcDuration = nullptr;
	funcVideoSize		m_funcVideoSize = nullptr;
	funcBuffering		m_funcBuffering = nullptr;
	funcSetScale		m_funcSetScale = nullptr;
	funcSetRotate		m_funcSetRotate = nullptr;
	funcSetRenderMode	m_funcSetRenderMode = nullptr;
	HANDLE				m_hPlayer = nullptr;

	bool	m_bLButtonDown = false;
	int		m_xPos = 0;
	int		m_yPos = 0;

	//cmd args
	bool	m_bChildWnd = false;
	int		m_nVideoType = 1;
	int		m_nIndex = 0;
	std::wstring m_wstrPlayUrl;
	std::wstring m_wstrChannelName;
	//ipc
	std::shared_ptr<IPC::Endpoint> m_pEndpoint;
	std::atomic_bool	m_bIPCConnected;
	//control
	CLabelUI*	m_pLabelMsg = nullptr;
	CVerticalLayoutUI*	m_pVideoLayout = nullptr;
};


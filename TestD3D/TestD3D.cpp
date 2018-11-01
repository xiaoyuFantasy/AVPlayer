// TestD3D.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "D3DRender.h"
#include <Windows.h>
#include <process.h>


struct LPUnit
{
	int index;
	HINSTANCE hinst;
};

#define WIDTH 2160
#define HEIGHT 1080

unsigned __stdcall video_frame_yuv(void *opaque)
{
	JYD3DRender pD3DRender;
	pD3DRender.Init((HWND)opaque, WIDTH, HEIGHT, true);

	FILE *fileYuv = _fsopen("panor_2160x1080.yuv", "rb", _SH_DENYWR);
	if (!fileYuv)
		return 0;
	int size = WIDTH * HEIGHT * 3 / 2;
	char *buffer = (char *)malloc(size * 3 / 2);

	while (1) {
		if (fread(buffer, 1, size, fileYuv) != size) {
			if (feof(fileYuv))
				fseek(fileYuv, 0, 0);
		}
		pD3DRender.Render(buffer);
		Sleep(30);
	}
	fclose(fileYuv);
	fileYuv = NULL;
	return 0;
}

unsigned __stdcall video_frame_rgb(void *opaque)
{
	JYD3DRender pD3DRender;
	pD3DRender.Init((HWND)opaque, WIDTH, HEIGHT, true);

	FILE *fileRGB = _fsopen("panor_2160x1080.yuv", "rb", _SH_DENYWR);
	if (!fileRGB)
		return 0;
	int size = WIDTH * HEIGHT * 4;
	char *buffer = (char *)malloc(size * 4);

	while (1) {
		if (fread(buffer, 1, size, fileRGB) != size) {
			fseek(fileRGB, 0, 0);
			fread(buffer, 1, size, fileRGB);
		}
		pD3DRender.Render(buffer);
		Sleep(30);
	}
	fclose(fileRGB);
	fileRGB = NULL;
	return 0;
}

unsigned __stdcall RenderFunc(LPVOID lp)
{
	LPUnit* lpu = (LPUnit*)lp;
	HWND hWnd = ::CreateWindow(L"#32770", L"VideoWnd", WS_POPUP | WS_VISIBLE, 0, 0, 1920, 1080, nullptr, nullptr, nullptr, nullptr);
	ShowWindow(hWnd, SW_SHOW);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	_beginthreadex(NULL, 0, video_frame_yuv, hWnd, 0, nullptr);

	MSG msg;
	while (GetMessage(&msg, hWnd, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, nullptr, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	for (size_t i = 0; i < 1; i++)
	{
		LPUnit* lpu = new LPUnit;
		lpu->index = i;
		_beginthreadex(NULL, 0, RenderFunc, lpu, 0, nullptr);
	}
	
	system("pause");
	return 0;
}



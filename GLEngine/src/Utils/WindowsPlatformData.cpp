#include "Utils/WindowsPlatformData.h"

#include <SDL/SDL_syswm.h>

#pragma comment(lib, "user32.lib")

void* WindowsPlatformData::s_windowHandle = NULL;
rde::string WindowsPlatformData::s_className;

BEGIN_UNNAMED_NAMESPACE()

LRESULT CALLBACK WndProc(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_msg)
	{
	case WM_CLOSE:
		DestroyWindow(a_hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(a_hwnd, a_msg, a_wParam, a_lParam);
	}
	return 0;
}

END_UNNAMED_NAMESPACE()

bool WindowsPlatformData::registerWindowsClass(const char* a_className)
{
	s_className = a_className;

	WNDCLASSEX wc;
	//Step 1: Registering the Window Class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = s_className.c_str();
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	ATOM atom = RegisterClassEx(&wc);

	if (!atom)
	{
		print("Window Registration Failed! \n");
		return false;
	}
	return true;
}

void WindowsPlatformData::setWindowHandle(SDL_Window* a_window)
{
	SDL_SysWMinfo pInfo;
	SDL_GetWindowWMInfo(a_window, &pInfo);
	s_windowHandle = pInfo.info.win.window;
}
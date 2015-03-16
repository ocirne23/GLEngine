#include "Utils/WindowsPlatformData.h"

#include <SDL/SDL_syswm.h>

#pragma comment(lib, "user32.lib")

void* WindowsPlatformData::s_editorWindowHandle = NULL;
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

HWND isWindowOtherThanAbove(HWND w, HWND exclude)
{
    HWND from = w;
    while (true)
    {
	from = GetWindow(from, GW_HWNDPREV);
	if (!from)
	    return NULL;
	if (from != exclude)
	    return from;
    }
}

void WindowsPlatformData::setEditorTop()
{
    if (!s_editorWindowHandle)
    {
	s_editorWindowHandle = FindWindow(NULL, "MainWindow");
	if (s_editorWindowHandle)
	    SetForegroundWindow((HWND) s_editorWindowHandle);
    }

    if (s_editorWindowHandle)
    {
	HWND window = (HWND) s_windowHandle;
	HWND editor = (HWND) s_editorWindowHandle;
	HWND console = GetConsoleWindow();
	HWND focus = GetFocus();
	HWND foreground = GetForegroundWindow();

	RECT rect;
	GetWindowRect(window, &rect);

	if (foreground == console)
	{
	    SetWindowPos(console, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}
	if (focus != window && focus != editor && !(foreground == window || foreground == editor))
	{
	    SetWindowPos(editor, HWND_NOTOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	    SetWindowPos(console, HWND_NOTOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	    SetWindowPos(window, editor, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
	}
	else if (focus == window || focus == editor)
	{
	    SetWindowPos(editor, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
	    SetWindowPos(window, editor, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOSIZE | SWP_NOMOVE);
	}
    }
}

void WindowsPlatformData::quitEditor()
{
    if (s_editorWindowHandle)
	SendMessage((HWND) s_editorWindowHandle, WM_CLOSE, 0, 0);
}
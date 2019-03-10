#pragma once

namespace WinPlatform
{
	void* createHighestGLContext(void* hdcv, int maxMajorVersion, int maxMinorVersion);
	void deleteContext(void* context);

	void* getHINSTANCE(void* hwnd);
	void* getWNDPROC(void* hwnd);
	void* getHDC(void* hwnd);

	void swapBuffers(void* hdc);
}


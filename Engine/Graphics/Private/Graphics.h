#pragma once

#include "Core.h"

class IWindow;
enum class EWindowMode;

class ICamera;
class IFilesystem;

class Graphics
{
public:

	Graphics() {}
	Graphics(const Graphics&) = delete;
	~Graphics();

	IWindow createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode);

private:

	//IFilesystem* filesystem = nullptr;
};
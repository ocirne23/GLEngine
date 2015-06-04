#pragma once

#include "Graphics/GLTestScreenImpl.h"

class GLTestScreen
{
public:

	void render(float deltaSec);

private:

	GLTestScreenImpl m_impl;
};
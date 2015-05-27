#pragma once

#include "Core.h"

#include "Graphics/GL/GLSpriteBatch.h"
#include "UI/Style.h"

#include "3rdparty/json/json.h"
#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/vector.h"

class Widget;

/** UI Container */
class Frame
{
public:

	Frame(const char* fileName);
	~Frame();

	void resize(uint width, uint height);
	void render();

private:

	uint m_xPos = 0;
	uint m_yPos = 0;
	uint m_width = 0;
	uint m_height = 0;

	Style m_style;

	Json::Value m_root;
	rde::string m_fileName;

	rde::vector<Widget*> m_widgets;

	GLSpriteBatch m_spriteBatch;
};
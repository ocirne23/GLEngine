#pragma once

#include "Core.h"

#include "Graphics/GL/GLSpriteBatch.h"
#include "UI/Style.h"

#include "3rdparty/json/json.h"
#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/hash_map.h"

#include <glm/glm.hpp>

class Widget;

/** UI Container */
class Frame
{
public:

	~Frame();

	void initialize(const char* fileName);
	void resize(uint width, uint height);
	void render();

private:

	Style m_style;
	Json::Value m_root;
	rde::string m_fileName;
	rde::hash_map<rde::string, Widget*> m_widgets;
	GLSpriteBatch m_spriteBatch;

	glm::mat4 m_vpMatrix;
};
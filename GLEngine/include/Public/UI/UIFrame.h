#pragma once

#include "Core.h"

#include "Graphics/GL/GLSpriteBatch.h"
#include "UI/Style.h"
#include "json/json.h"
#include "EASTL/string.h"
#include "EASTL/hash_map.h"

#include <glm/glm.hpp>

class Widget;
class GLSpriteBatch;

/** UI Container */
class UIFrame
{
public:

	~UIFrame();

	void initialize(const char* filePath);
	void resize(float width, float height);
	void render(GLSpriteBatch& spriteBatch) const;
	void updateLayout();

	float getWidth() const  { return m_width; }
	float getHeight() const { return m_height; }
	const eastl::string& getJsonFilePath() const { return m_filePath; }

private:

	eastl::string m_filePath;
	Style m_style;

	eastl::hash_map<eastl::string, Widget*> m_widgets;
	glm::mat4 m_vpMatrix;

	float m_width      = 0;
	float m_height     = 0;
	bool m_layoutValid = false;
};
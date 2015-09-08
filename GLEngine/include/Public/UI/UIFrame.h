#pragma once

#include "Core.h"

#include "Graphics/GL/GLSpriteBatch.h"
#include "UI/Style.h"

#include "3rdparty/json/json.h"
#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/hash_map.h"

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
	const rde::string& getJsonFilePath() const { return m_filePath; }

private:

	rde::string m_filePath;
	Style m_style;

	rde::hash_map<rde::string, Widget*> m_widgets;
	glm::mat4 m_vpMatrix;

	float m_width      = 0;
	float m_height     = 0;
	bool m_layoutValid = false;
};
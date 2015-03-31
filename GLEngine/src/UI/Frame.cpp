#include "UI/Frame.h"

#include "UI/Widgets/ImageButton.h"
#include "UI/Widgets/TextButton.h"
#include "Utils/FileHandle.h"

#include <glm/gtc/matrix_transform.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const uint SPRITE_BATCH_SIZE = 100;

END_UNNAMED_NAMESPACE()

Frame::Frame(const char* a_fileName) : m_fileName(a_fileName), m_spriteBatch(SPRITE_BATCH_SIZE)
{
	Json::Reader reader;
	{
		FileHandle handle(a_fileName);
		rde::string contents = handle.readString();
		bool success = reader.parse(contents.begin(), contents.end(), m_root);
		assert(success);
	}

	m_style.load(m_root["style"].asCString());
	Json::Value contents = m_root["contents"];
	for (Json::Value item : contents)
	{
		Widget* widget = NULL;
		rde::string type = rde::string(item["type"].asCString());

		if (type == getStringForWidgetType(EWidgetType::TEXTBUTTON))
			widget = new TextButton();
		else if (type == getStringForWidgetType(EWidgetType::IMAGEBUTTON))
			widget = new ImageButton();

		assert(widget);
		widget->fromJson(item);
		m_widgets.push_back(widget);
	}
}

void Frame::render()
{
	glm::mat4 projectionMatrix = glm::ortho(0.0f, (float) m_width, (float) m_height, 0.0f, 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 mvp = projectionMatrix * viewMatrix;

	m_spriteBatch.begin(mvp);
	m_spriteBatch.end();
}

Frame::~Frame()
{
	for (Widget* widget : m_widgets)
		delete widget;
	m_widgets.clear();
}
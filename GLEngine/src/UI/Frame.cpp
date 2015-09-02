#include "UI/Frame.h"

#include "UI/Widgets/ImageButton.h"
#include "UI/Widgets/TextButton.h"
#include "Utils/FileHandle.h"

#include <glm/gtc/matrix_transform.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const uint SPRITE_BATCH_SIZE = 100;

END_UNNAMED_NAMESPACE()

void Frame::initialize(const char* a_fileName, float a_width, float a_height, float a_xPos, float a_yPos)
{
	m_width = a_width;
	m_height = a_height;
	m_xPos = a_xPos;
	m_yPos = a_yPos;

	m_spriteBatch.initialize(SPRITE_BATCH_SIZE);

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
		EWidgetType type = getWidgetTypeForString(rde::string(item["type"].asCString()));
		switch (type)
		{
		case EWidgetType::TEXTBUTTON:
			widget = new TextButton();
			break;
		case EWidgetType::IMAGEBUTTON:
			widget = new ImageButton();
			break;
		case EWidgetType::INVALID_TYPE: // Fall through
		default:
			assert(false);
			break;
		}
		assert(widget);
		widget->fromJson(item);
		m_widgets.push_back(widget);
	}

	glm::mat4 projectionMatrix = glm::ortho(0.0f, (float) m_width, (float) m_height, 0.0f, 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_vpMatrix = projectionMatrix * viewMatrix;
}

void Frame::render()
{
	m_spriteBatch.begin(m_vpMatrix);

	for (Widget* widget : m_widgets)
	{
		switch (widget->getType())
		{
		case EWidgetType::TEXTBUTTON: break;
		case EWidgetType::IMAGEBUTTON:
		{
			ImageButton* imageButton = (ImageButton*) widget;
			m_spriteBatch.draw(m_style.getImageButtonTex(), 50.0f, 150.0f, 400.0f, 300.0f);
			break;
		}
		}
	}

	m_spriteBatch.end();
}

Frame::~Frame()
{
	for (Widget* widget : m_widgets)
		delete widget;
	m_widgets.clear();
}

void Frame::resize(uint width, uint height)
{
	//TODO
}

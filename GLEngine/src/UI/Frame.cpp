#include "UI/Frame.h"

#include "UI/Widgets/ImageButton.h"
#include "UI/Widgets/TextButton.h"
#include "Utils/FileHandle.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"

#include <glm/gtc/matrix_transform.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const uint SPRITE_BATCH_SIZE = 100;

END_UNNAMED_NAMESPACE()

void Frame::initialize(const char* a_fileName)
{
	m_spriteBatch.initialize(SPRITE_BATCH_SIZE);

	float screenWidth = (float) GLEngine::graphics->getViewportWidth();
	float screenHeight = (float) GLEngine::graphics->getViewportHeight();
	glm::mat4 projectionMatrix = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_vpMatrix = projectionMatrix * viewMatrix;

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
		m_widgets.insert({widget->getName(), widget});
	}
}

void Frame::render()
{
	m_spriteBatch.begin(m_vpMatrix);

	for (const auto& it : m_widgets)
	{
		Widget* widget = it.second;
		switch (widget->getType())
		{
		case EWidgetType::TEXTBUTTON: break;
		case EWidgetType::IMAGEBUTTON:
		{
			ImageButton* button = (ImageButton*) widget;
			const Layout& layout = button->getLayout();
			m_spriteBatch.draw(m_style.getImageButtonTex(), layout.width, layout.height, layout.marginLeft, layout.marginTop);
			break;
		}
		}
	}

	m_spriteBatch.end();
}

Frame::~Frame()
{
	for (auto& it: m_widgets)
		delete it.second;
	m_widgets.clear();
}

void Frame::resize(uint width, uint height)
{
	//TODO
}

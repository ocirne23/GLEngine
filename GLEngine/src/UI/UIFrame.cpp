#include "UI/UIFrame.h"

#include "UI/Widgets/ImageButton.h"
#include "UI/Widgets/TextButton.h"
#include "Utils/FileHandle.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"

void UIFrame::initialize(const char* a_filePath)
{
	m_filePath = a_filePath;

	m_widgets.clear();

	float screenWidth = (float)GLEngine::graphics->getViewportWidth();
	float screenHeight = (float)GLEngine::graphics->getViewportHeight();
	resize(screenWidth, screenHeight);

	Json::Value root;
	Json::Reader reader;
	{
		FileHandle handle(a_filePath);
		eastl::string contents = handle.readString();
		bool success = reader.parse(contents.begin(), contents.end(), root);
		assert(success);
	}

	m_style.load(root["style"].asCString());
	Json::Value contents = root["contents"];
	for (Json::Value item : contents)
	{
		Widget* widget = NULL;
		EWidgetType type = getWidgetTypeForString(eastl::string(item["type"].asCString()));
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

/*
void UIFrame::render()
{
	if (!m_layoutValid)
		updateLayout();

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
			glm::vec4 bounds = button->getAbsoluteBounds();
			m_spriteBatch.draw(m_style.getImageButtonTex(), bounds.x, bounds.y, bounds.z, bounds.w);
			break;
		}
		}
	}

	m_spriteBatch.end();
}
*/

UIFrame::~UIFrame()
{
	for (auto& it: m_widgets)
		delete it.second;
	m_widgets.clear();
}

void UIFrame::resize(float a_width, float a_height)
{
	m_width = a_width;
	m_height = a_height;
	m_layoutValid = false;
}

void UIFrame::updateLayout()
{
	for (const auto& it : m_widgets)
		it.second->updateLayout(*this);
	m_layoutValid = true;
}

void UIFrame::render(GLSpriteBatch& a_spriteBatch) const
{
	for (const auto& it : m_widgets)
	{
		Widget* widget = it.second;
		glm::vec4 bounds = widget->getAbsoluteBounds();

		switch (widget->getType())
		{
		case EWidgetType::TEXTBUTTON:
			a_spriteBatch.draw(m_style.getTextButtonTex(), bounds.x, bounds.y, bounds.z, bounds.w);
			break;
		case EWidgetType::IMAGEBUTTON:
			a_spriteBatch.draw(m_style.getImageButtonTex(), bounds.x, bounds.y, bounds.z, bounds.w);
			break;
		}
	}
}

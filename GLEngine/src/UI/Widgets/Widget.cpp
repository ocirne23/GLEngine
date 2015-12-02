#include "UI/Widgets/Widget.h"

#include "UI/UIFrame.h"

#include <glm/glm.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const eastl::string WIDGET_TYPE_STRINGS[] =
{
	eastl::string("textbutton"),
	eastl::string("imagebutton")
};

END_UNNAMED_NAMESPACE()

const eastl::string& getStringForWidgetType(EWidgetType a_widgetType)
{
	return WIDGET_TYPE_STRINGS[(int) a_widgetType];
}

const EWidgetType getWidgetTypeForString(const eastl::string& a_str)
{
	for (int i = 0; i < (int) EWidgetType::NUM_WIDGET_TYPES; ++i)
	{
		if (WIDGET_TYPE_STRINGS[i] == a_str)
			return (EWidgetType) i;
	}
	return EWidgetType::INVALID_TYPE;
}

void Widget::fromJson(const Json::Value& a_json)
{
	m_type = getWidgetTypeForString(eastl::string(a_json["type"].asCString()));
	m_name = eastl::string(a_json["name"].asCString());
	m_layout.fromJson(a_json["layout"]);
}

Json::Value Widget::toJson()
{
	Json::Value v;
	v["layout"] = m_layout.toJson();
	v["name"] = m_name.c_str();
	v["type"] = getStringForWidgetType(m_type).c_str();
	return v;
}

void Widget::updateLayout(const UIFrame& a_frame)
{
	float frameWidth = a_frame.getWidth();
	float frameHeight = a_frame.getHeight();

	if (m_layout.horzontalAlignment == Layout::EHorizontalAlignment::RIGHT)
		m_absBounds.x = frameWidth - m_layout.marginRight - m_layout.width;
	else
		m_absBounds.x = m_layout.marginLeft;

	if (m_layout.verticalAlignment == Layout::EVerticalAlignment::BOTTOM)
		m_absBounds.y = frameHeight - m_layout.marginBottom - m_layout.height;
	else
		m_absBounds.y = m_layout.marginTop;

	m_absBounds.z = m_layout.width;
	m_absBounds.w = m_layout.height;
}

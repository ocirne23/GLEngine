#include "UI/Widgets/Widget.h"

#include <glm/glm.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const rde::string WIDGET_TYPE_STRINGS[] =
{
	rde::string("textbutton"),
	rde::string("imagebutton")
};

END_UNNAMED_NAMESPACE()

const rde::string& getStringForWidgetType(EWidgetType a_widgetType)
{
	return WIDGET_TYPE_STRINGS[(int) a_widgetType];
}

const EWidgetType getWidgetTypeForString(const rde::string& a_str)
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
	m_type = getWidgetTypeForString(rde::string(a_json["type"].asCString()));
	m_name = rde::string(a_json["name"].asCString());
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

glm::vec4 Widget::getAbsoluteBounds(Frame& frame) const
{
	return glm::vec4(0);
}

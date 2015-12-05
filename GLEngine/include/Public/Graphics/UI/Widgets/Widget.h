#pragma once

#include "Core.h"
#include "Graphics/UI/Layout.h"
#include "EASTL/string.h"

#include <glm/glm.hpp>

class UIFrame;

enum class EWidgetType
{
	TEXTBUTTON,
	IMAGEBUTTON,
	NUM_WIDGET_TYPES,
	INVALID_TYPE
};

const eastl::string& getStringForWidgetType(EWidgetType widgetType);
const EWidgetType getWidgetTypeForString(const eastl::string& str);

class Widget
{
public:

	virtual ~Widget() {};
	virtual void fromJson(const Json::Value& json);
	virtual Json::Value toJson();

	void updateLayout(const UIFrame& frame);

	EWidgetType getType() const                { return m_type; }
	const eastl::string& getName() const       { return m_name; }
	const Layout& getLayout() const            { return m_layout; }
	const Widget* getParent() const            { return m_parent; }
	const glm::vec4& getAbsoluteBounds() const { return m_absBounds; }

protected:

	EWidgetType m_type;
	eastl::string m_name;
	Layout m_layout;
	Widget* m_parent = NULL;
	glm::vec4 m_absBounds = glm::vec4(0);
};







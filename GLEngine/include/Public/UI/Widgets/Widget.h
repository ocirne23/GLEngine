#pragma once

#include "Core.h"

#include "3rdparty/rde/rde_string.h"
#include "UI/Layout.h"

enum class EWidgetType
{
	TEXTBUTTON,
	IMAGEBUTTON,
	NUM_WIDGET_TYPES,
	INVALID_TYPE
};

const rde::string& getStringForWidgetType(EWidgetType widgetType);
const EWidgetType getWidgetTypeForString(const rde::string& str);

class Widget
{
public:

	virtual ~Widget() {};
	virtual void fromJson(const Json::Value& json);
	virtual Json::Value toJson();

	EWidgetType getType() const        { return m_type; }
	const Layout& getLayout() const    { return m_layout; }
	const rde::string& getName() const { return m_name; }

protected:

	EWidgetType m_type;
	rde::string m_name;
	Layout m_layout;
};







#pragma once

#include "UI/Widgets/Widget.h"
#include <functional>

class Style;

class TextButton : public Widget
{
public:
	~TextButton() override;
	virtual void fromJson(const Json::Value& json) override;
	virtual Json::Value toJson() override;
private:
	rde::string m_text;
};
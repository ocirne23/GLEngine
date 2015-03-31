#pragma once

#include "UI/Widgets/Widget.h"
#include <functional>

class Style;

class ImageButton : public Widget
{
public:
	~ImageButton() override;
	virtual void fromJson(const Json::Value& json) override;
	virtual Json::Value toJson() override;
private:
};
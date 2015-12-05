#pragma once

#include "Graphics/UI/Widgets/Widget.h"
#include "EASTL/string.h"
#include <functional>

class Style;

class TextButton : public Widget
{
public:
	~TextButton() override;
	virtual void fromJson(const Json::Value& json) override;
	virtual Json::Value toJson() override;
private:
	eastl::string m_text;
};
#include "Graphics/UI/Widgets/ImageButton.h"

ImageButton::~ImageButton()
{

}

void ImageButton::fromJson(const Json::Value& a_json)
{
	Widget::fromJson(a_json);
}

Json::Value ImageButton::toJson()
{
	Json::Value v = Widget::toJson();
	return v;
}
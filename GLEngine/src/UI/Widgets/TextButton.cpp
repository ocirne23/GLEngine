#include "UI/Widgets/TextButton.h"

TextButton::~TextButton()
{

}

void TextButton::fromJson(const Json::Value& a_json)
{
	Widget::fromJson(a_json);
	m_text = rde::string(a_json["properties"]["text"].asCString());
}

Json::Value TextButton::toJson()
{
	Json::Value v = Widget::toJson();
	v["properties"]["text"] = m_text.c_str();
	return v;
}
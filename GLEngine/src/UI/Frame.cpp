#include "UI/Frame.h"

#include "UI/Widgets/ImageButton.h"
#include "UI/Widgets/TextButton.h"

#include "Utils/FileHandle.h"

Frame::Frame(const char* a_fileName) : m_fileName(a_fileName)
{
	Json::Reader reader;
	{
		FileHandle handle(a_fileName);
		rde::string contents = handle.readString();
		bool success = reader.parse(contents.begin(), contents.end(), m_root);
		assert(success);
	}

	Json::Value contents = m_root["contents"];
	for (Json::Value item : contents)
	{
		Widget* widget = NULL;
		rde::string type = rde::string(item["type"].asCString());

		if (type == getStringForWidgetType(EWidgetType::TEXTBUTTON))
			widget = new TextButton();
		else if (type == getStringForWidgetType(EWidgetType::IMAGEBUTTON))
			widget = new ImageButton();

		assert(widget);
		widget->fromJson(item);
		m_widgets.push_back(widget);
	}
}

Frame::~Frame()
{
	for (Widget* widget : m_widgets)
		delete widget;
	m_widgets.clear();
}
#include "Graphics/UI/Layout.h"

#include "Core.h"
#include "EASTL/string.h"

BEGIN_UNNAMED_NAMESPACE()

const eastl::string HORIZONTAL_ALIGNMENTS[] =
{
	eastl::string("LEFT"),
	eastl::string("CENTER"),
	eastl::string("RIGHT"),
	eastl::string("STRETCH")
};

const eastl::string VERTICAL_ALIGNMENTS[] =
{
	eastl::string("TOP"),
	eastl::string("CENTER"),
	eastl::string("BOTTOM"),
	eastl::string("STRETCH")
};

const Layout::EHorizontalAlignment getHorizontalAlignmentForString(const eastl::string& a_str)
{
	for (int i = 0; i < (int) Layout::EHorizontalAlignment::NUM_HORIZONTAL_ALIGNMENTS; ++i)
		if (HORIZONTAL_ALIGNMENTS[i] == a_str)
			return (Layout::EHorizontalAlignment) i;
	return Layout::EHorizontalAlignment::INVALID_ALIGNMENT;
}

const eastl::string& getStringForHorizontalAlignment(Layout::EHorizontalAlignment a_alignment)
{
	return HORIZONTAL_ALIGNMENTS[(int) a_alignment];
}

const Layout::EVerticalAlignment getVerticalAlignmentForString(const eastl::string& a_str)
{
	for (int i = 0; i < (int) Layout::EVerticalAlignment::NUM_VERTICAL_ALIGNMENTS; ++i)
		if (VERTICAL_ALIGNMENTS[i] == a_str)
			return (Layout::EVerticalAlignment) i;
	return Layout::EVerticalAlignment::INVALID_ALIGNMENT;
}

const eastl::string& getStringForVerticalAlignment(Layout::EVerticalAlignment a_alignment)
{
	return VERTICAL_ALIGNMENTS[(int) a_alignment];
}

END_UNNAMED_NAMESPACE()

void Layout::fromJson(const Json::Value& a_json)
{
	width = a_json["width"].asFloat();
	height = a_json["height"].asFloat();
	marginLeft = a_json["marginLeft"].asFloat();
	marginRight = a_json["marginRight"].asFloat();
	marginTop = a_json["marginTop"].asFloat();
	marginBottom = a_json["marginBottom"].asFloat();
	horzontalAlignment = getHorizontalAlignmentForString(eastl::string(a_json["horizontalAlignment"].asCString()));
	verticalAlignment = getVerticalAlignmentForString(eastl::string(a_json["verticalAlignment"].asCString()));
}

Json::Value Layout::toJson()
{
	Json::Value v;
	v["width"] = width;
	v["height"] = height;
	v["marginLeft"] = marginLeft;
	v["marginRight"] = marginRight;
	v["marginTop"] = marginTop;
	v["marginBottom"] = marginBottom;
	v["horizontalAlignment"] = getStringForHorizontalAlignment(horzontalAlignment).c_str();
	v["verticalAlignment"] = getStringForVerticalAlignment(verticalAlignment).c_str();
	return v;
}
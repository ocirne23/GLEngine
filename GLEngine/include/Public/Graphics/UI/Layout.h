#pragma once

#include "json/json.h"

struct Layout
{
	enum class EHorizontalAlignment
	{
		LEFT, CENTER, RIGHT, STRETCH, NUM_HORIZONTAL_ALIGNMENTS, INVALID_ALIGNMENT
	};
	enum class EVerticalAlignment
	{
		TOP, CENTER, BOTTOM, STRETCH, NUM_VERTICAL_ALIGNMENTS, INVALID_ALIGNMENT
	};

	float width;
	float height;

	EHorizontalAlignment horzontalAlignment;
	EVerticalAlignment verticalAlignment;

	float marginLeft;
	float marginRight;
	float marginTop;
	float marginBottom;

	void fromJson(const Json::Value& json);
	Json::Value toJson();
};

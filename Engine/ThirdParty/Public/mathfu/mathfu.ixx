#include "mathfu/glsl_mappings.h"

export module ThirdParty.Mathfu;

export float fuu()
{
	mathfu::vec3 v(2.0f, 3.0f, 4.0f);
	return v.Length();
}
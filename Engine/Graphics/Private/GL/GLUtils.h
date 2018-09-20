#pragma once

#include "Core.h"

enum class EBufferUsageType;

BEGIN_NAMESPACE(GLUtils)

uint getGLUsageFlags(const EBufferUsageType& usageType);

END_NAMESPACE(GLUtils)
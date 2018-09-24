#pragma once

#include "Core.h"

enum class EBufferUsageType;
enum class EVertexAttributeFormat;

BEGIN_NAMESPACE(GLUtils)

uint getGLUsageFlags(const EBufferUsageType& usageType);
uint getGLVertexAttributeSize(const EVertexAttributeFormat& attribute);
uint getGLVertexAttributeType(const EVertexAttributeFormat& attribute);

END_NAMESPACE(GLUtils)
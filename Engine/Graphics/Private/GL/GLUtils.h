#pragma once

#include "Core.h"

enum class EBufferUsageType;
enum class EVertexAttributeFormat;
enum class ETextureAttachment;

BEGIN_NAMESPACE(GLUtils)

uint getGLUsageFlags(const EBufferUsageType& usageType);
uint getGLVertexAttributeSize(const EVertexAttributeFormat& attribute);
uint getGLVertexAttributeType(const EVertexAttributeFormat& attribute);
uint getGLAttachment(const ETextureAttachment& attachment);

END_NAMESPACE(GLUtils)
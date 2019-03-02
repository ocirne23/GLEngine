#pragma once

#include "Core.h"

enum class EBufferUsageType;
enum class EVertexAttributeFormat;
enum class ETextureAttachment;
enum class ETextureType;
enum class EMultisampleType;
enum class ETextureMinFilter;
enum class ETextureMagFilter;
enum class ETextureWrap;
enum class ETextureCompareMode;

BEGIN_NAMESPACE(GLUtils)

uint getGLUsageFlags(const EBufferUsageType& usageType);
uint getGLVertexAttributeSize(const EVertexAttributeFormat& attribute);
uint getGLVertexAttributeType(const EVertexAttributeFormat& attribute);
uint getGLAttachment(const ETextureAttachment& attachment);
uint getGLTextureType(const ETextureType& type);
uint getGLMultisampleType(const EMultisampleType& type);
bool getGLMultisampleFixed(const EMultisampleType& type);
uint getGLTextureFilter(const ETextureMinFilter& filter);
uint getGLTextureFilter(const ETextureMagFilter& filter);
uint getGLTextureWrap(const ETextureWrap& wrap);
uint getGLTextureCompare(const ETextureCompareMode& mode);
uint getGLSizedInternalFormat(const ETextureComponentFormat& format, uint numComponents);
bool isCompressed(const ETextureComponentFormat& format);

END_NAMESPACE(GLUtils)
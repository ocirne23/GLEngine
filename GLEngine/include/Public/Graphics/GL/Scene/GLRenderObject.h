#pragma once

class GLRenderer;
class GLConstantBuffer;

class GLRenderObject
{
public:
	virtual ~GLRenderObject() {}

	virtual void render(const GLRenderer& renderer, GLConstantBuffer& modelMatrixUBO, bool depthOnly) = 0;
};
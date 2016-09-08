#pragma once

class GLRenderer;
class GLConstantBuffer;

class GLRenderObject
{
public:
	virtual ~GLRenderObject() {}

	virtual void render(GLRenderer& renderer, bool depthOnly) = 0;
};
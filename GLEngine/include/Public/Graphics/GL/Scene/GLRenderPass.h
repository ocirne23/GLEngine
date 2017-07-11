#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"

#include "EASTL/fixed_vector.h"
#include "EASTL/vector.h"
#include "gsl/gsl.h"

enum RenderPassFramebufferContentTags
{
	EMPTY = 0,
	COLOR = 1,
	NORMALS = 2,
	DEPTH = 3,
	SCENECOLOR = 4
};

struct GLRenderPassFramebuffer
{
	int contentTag = 0;
	GLFramebuffer renderPass;
};

class GLRenderPass;

class GLRenderPassManager
{
public:

	void registerRenderPass(GLRenderPass* pass, eastl::vector<int> inputContentTags, eastl::vector<int> outputContentTags) {}

private:

	eastl::vector<GLRenderPass*> m_renderPasses;
	eastl::vector<owner<GLFramebuffer*>> m_framebuffers;
	eastl::vector<int> m_contentTags;
};

class GLRenderPass
{
public:

	GLRenderPass() {}
	~GLRenderPass() {}

	owner<GLFramebuffer*> provideFramebuffer(int contentTag)
	{
		return NULL;
	}

	void render(eastl::vector<GLFramebuffer*> inputs, eastl::vector<GLFramebuffer*> outputs)
	{

	}

private:

};
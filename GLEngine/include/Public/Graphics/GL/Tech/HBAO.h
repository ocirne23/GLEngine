#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTexture.h"

#include <glm/glm.hpp>

class PerspectiveCamera;

class HBAO
{
public:

	struct GlobalsUBO
	{
		glm::vec2 fullResolution;
		glm::vec2 invFullResolution;

		glm::vec2 aoResolution;
		glm::vec2 invAOResolution;

		glm::vec2 focalLen;
		glm::vec2 invFocalLen;

		glm::vec2 uvToViewA;
		glm::vec2 uvToViewB;

		float r;
		float r2;
		float negInvR2;
		float maxRadiusPixels;

		float angleBias;
		float tanAngleBias;
		float strength;
		float padding;

		glm::vec2 noiseTexScale;
		glm::vec2 ndcDepthConv;
	};

public:
	HBAO() {}
	~HBAO();

	void initialize(const PerspectiveCamera& camera, uint xRes, uint yRes, uint hbaoGlobalsUBOBindingPoint);

	void begin();
	void endAndRender();

private:

	bool m_initialized;

	GLFramebuffer m_fboFullRes;
	GLFramebuffer m_blurXFbo;
	GLFramebuffer m_blurYFbo;
	GLShader m_hbaoFullShader;
	GLShader m_blurXShader;
	GLShader m_blurYShader;
	GLTexture m_noiseTexture;
	GLConstantBuffer m_hbaoGlobalsBuffer;
};
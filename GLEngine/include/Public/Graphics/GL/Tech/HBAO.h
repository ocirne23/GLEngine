#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Tech/BilateralBlur.h"

#include <glm/glm.hpp>

class PerspectiveCamera;

class HBAO
{
public:

	struct GlobalsUBO
	{
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
	~HBAO() {}

	void initialize(const PerspectiveCamera& camera, uint screenWidth, uint screenHeight);
	void reloadShader();
	GLFramebuffer& getHBAOResultFBO(GLFramebuffer& a_sceneFBO);

private:

	bool m_initialized = false;

	glm::ivec2 m_screenRes;
	GLShader m_downsampleDepthShader;
	GLShader m_hbaoFullShader;
	GLFramebuffer m_downsampleDepthFBO;
	GLFramebuffer m_hbaoResultFBO;
	GLConstantBuffer m_hbaoGlobalsBuffer;
	GLTexture m_noiseTexture;
	BilateralBlur m_bilateralBlur;
};
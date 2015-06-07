#pragma once

#include "Core.h"
#include "Input/Input.h"

class GLShader;
class GLMesh;
class GLFramebuffer;
class GLTexture;
class PerspectiveCamera;

class HBAOTest
{
public:

	HBAOTest() {};

	void init(PerspectiveCamera* camera);
	void render(float a_deltaSec);

private:

	void keyDown(EKey key);

private:

	GLFramebuffer* m_fboFullRes    = NULL;
	GLFramebuffer* m_fboHalfRes    = NULL;
	GLMesh* m_dragon               = NULL;
	GLMesh* m_building             = NULL;
	GLMesh* m_skybox               = NULL;
	GLShader* m_geometryShader     = NULL;
	GLShader* m_geometryBackShader = NULL;
	GLShader* m_compositShader     = NULL;
	GLShader* m_hbaoHalfShader     = NULL;
	GLShader* m_hbaoFullShader     = NULL;
	GLShader* m_blurXShader        = NULL;
	GLShader* m_blurYShader        = NULL;
	GLShader* m_downsampleShader   = NULL;
	GLShader* m_upsampleShader     = NULL;
	GLTexture* m_noiseTexture      = NULL;
	PerspectiveCamera* m_camera    = NULL;

	Input::KeyDownListener m_keyDownListener;
};
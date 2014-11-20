#pragma once

#include "Core.h"
#include "../Core/IScreen.h"
#include "Input/KeyListener.h"

#include "Graphics/LightManager.h"

#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/Core/GLConstantBuffer.h"
#include "Graphics/GL/Core/GLShader.h"
#include "Graphics/GL/Core/GLTexture.h"
#include "Graphics/GL/Core/GLTextureBuffer.h"
#include "Graphics/GL/Core/GLUniform.h"
#include "Graphics/GL/Tech/ClusteredShading.h"

class GameScreen : public IScreen, public KeyListener
{
public:
	GameScreen(ScreenManager* screenManager);
	virtual ~GameScreen() OVERRIDE;

	virtual void render(float deltaSec) OVERRIDE;
	virtual void show(uint width, uint height) OVERRIDE;
	virtual void resize(uint width, uint height) OVERRIDE;
	virtual void hide() OVERRIDE;
	virtual bool keyDown(Key key) OVERRIDE;

private:

	GLTexture m_dfvTexture;
	GLMesh m_mesh;
	GLShader m_modelShader;

	LightManager m_lightManager;
	GLConstantBuffer m_lightPositionRangeBuffer;
	GLConstantBuffer m_lightColorBuffer;

	ClusteredShading m_clusteredShading;
	GLTextureBuffer m_lightIndiceBuffer;
	GLTextureBuffer m_lightGridBuffer;

	GLUniform<float> m_recLogSD1Uniform;
	GLUniform<float> m_recNearUniform;
};
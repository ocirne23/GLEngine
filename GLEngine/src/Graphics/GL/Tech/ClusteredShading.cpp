#include "Graphics/GL/Tech/ClusteredShading.h"

#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/Utils/ClusteredTiledShadingUtils.h"
#include "Graphics/Utils/LightManager.h"
#include "Graphics/Utils/PerspectiveCamera.h"

#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

const uint CLUSTERED_SHADING_TILE_WIDTH = 64;
const uint CLUSTERED_SHADING_TILE_HEIGHT = 64;

END_UNNAMED_NAMESPACE()

ClusteredShading::~ClusteredShading()
{
}

void ClusteredShading::initialize(const PerspectiveCamera& a_camera, uint a_screenWidth, uint a_screenHeight)
{
	m_screenWidth    = a_screenWidth;
	m_screenHeight   = a_screenHeight;
	m_pixelsPerTileW = CLUSTERED_SHADING_TILE_WIDTH;
	m_pixelsPerTileH = CLUSTERED_SHADING_TILE_HEIGHT;
	m_gridWidth      = a_screenWidth / m_pixelsPerTileW + 1;
	m_gridHeight     = a_screenHeight / m_pixelsPerTileH + 1;
	m_recNear        = 1.0f / a_camera.getNear();
	
	const uint grid2dDimY = (a_screenHeight + m_pixelsPerTileH - 1) / m_pixelsPerTileH;
	const float sD        = 2.0f * glm::tan(glm::radians(a_camera.getVFov()) * 0.5f) / float(grid2dDimY);
	m_recLogSD1           = 1.0f / logf(sD + 1.0f);
	
	const float zGridLocFar = logf(a_camera.getFar() / a_camera.getNear()) / logf(1.0f + sD);
	m_gridDepth             = uint(ceilf(zGridLocFar) + 0.5f);
	m_gridSize              = m_gridWidth * m_gridHeight * m_gridDepth;
	m_maxNumLightIndices    = m_gridSize * MAX_NUM_INDICES_PER_TILE;

	m_tileLightIndices.resize(m_gridSize);

	m_lightPositionRangesUBO.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::ClusteredLightPositionRange));
	m_lightColorIntensitiesUBO.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::ClusteredLightColorIntensities));
	m_clusteredShadingGlobalsUBO.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::ClusteredGlobals));

	m_lightGridTextureBuffer.initialize(m_gridSize * sizeof(glm::uvec2), GLTextureBuffer::ESizedFormat::RG32I, GLTextureBuffer::EDrawUsage::STREAM);
	m_lightIndiceTextureBuffer.initialize(m_maxNumLightIndices * sizeof(ushort), GLTextureBuffer::ESizedFormat::R16I, GLTextureBuffer::EDrawUsage::STREAM);

	GlobalsUBO ubo;
	ubo.u_recNear    = m_recNear;
	ubo.u_recLogSD1  = m_recLogSD1;
	ubo.u_tileWidth  = m_pixelsPerTileW;
	ubo.u_tileHeight = m_pixelsPerTileH;
	ubo.u_gridHeight = m_gridHeight;
	ubo.u_gridDepth  = m_gridDepth;
	m_clusteredShadingGlobalsUBO.upload(sizeof(GlobalsUBO), &ubo);

	m_initialized = true;
}

void ClusteredShading::update(const PerspectiveCamera& a_camera, const LightManager& a_lightManager)
{
	assert(m_initialized);

	for (TileIndiceList& indiceList : m_tileLightIndices)
		indiceList.count = 0;

	const glm::mat4& viewMatrix                  = a_camera.getViewMatrix();
	const uint numLights                         = a_lightManager.getNumLights();
	const glm::vec4* lightPositionRanges         = a_lightManager.getLightPositionRanges();
	glm::vec4* lightPositionRangeViewspaceBuffer = rcast<glm::vec4*>(m_lightPositionRangesUBO.mapBuffer());

	uint lightIndiceCtr = 0;
	for (ushort i = 0; i < numLights; ++i)
	{
		lightPositionRangeViewspaceBuffer[i] = viewMatrix * glm::vec4(glm::vec3(lightPositionRanges[i]), 1.0);
		lightPositionRangeViewspaceBuffer[i].w = lightPositionRanges[i].w;
		const glm::vec3 lightPositionViewSpace(lightPositionRangeViewspaceBuffer[i]);
		const float range = lightPositionRangeViewspaceBuffer[i].w;
		IBounds3D bounds3D = ClusteredTiledShadingUtils::sphereToScreenSpaceBounds3D(a_camera, 
		                                                                             lightPositionViewSpace, range, 
		                                                                             m_screenWidth, m_screenHeight, 
		                                                                             m_pixelsPerTileW, m_pixelsPerTileH, 
		                                                                             m_recLogSD1);
		lightIndiceCtr += writeLightIndices(bounds3D, i);
		if (lightIndiceCtr > m_maxNumLightIndices)
			break;
	}

	m_lightPositionRangesUBO.unmapBuffer();
	m_lightColorIntensitiesUBO.upload(numLights * sizeof(glm::vec4), &a_lightManager.getLightColorIntensities()[0]);

	glm::uvec2* gridBuffer = rcast<glm::uvec2*>(m_lightGridTextureBuffer.mapBuffer());
	ushort* indiceBuffer = rcast<ushort*>(m_lightIndiceTextureBuffer.mapBuffer());

	uint indiceCtr = 0;
	for (uint i = 0; i < m_gridSize; ++i)
	{
		ushort numIndicesForTile = m_tileLightIndices[i].count;
		gridBuffer[i].x = indiceCtr;                     // start idx
		gridBuffer[i].y = indiceCtr + numIndicesForTile; // end idx
		if (numIndicesForTile)
		{
			memcpy(&indiceBuffer[indiceCtr], &m_tileLightIndices[i].indices[0], numIndicesForTile * sizeof(ushort));
			indiceCtr += numIndicesForTile;
		}
	}

	m_lightIndiceTextureBuffer.unmapBuffer();
	m_lightGridTextureBuffer.unmapBuffer();
}

uint ClusteredShading::writeLightIndices(const IBounds3D& a_lightBounds, ushort a_lightIdx)
{
	for (int x = a_lightBounds.min.x; x < a_lightBounds.max.x; ++x)
	{
		for (int y = a_lightBounds.min.y; y < a_lightBounds.max.y; ++y)
		{
			for (int z = a_lightBounds.min.z; z < a_lightBounds.max.z; ++z)
			{
				const uint gridIdx = (x * m_gridHeight + y) * m_gridDepth + z;
				m_tileLightIndices[gridIdx].push_back(a_lightIdx);
			}
		}
	}
	return a_lightBounds.volume();
}

void ClusteredShading::bindTextureBuffers()
{
	m_lightIndiceTextureBuffer.bind(GLConfig::getTextureBindingPoint(GLConfig::ETextures::ClusteredLightIndice));
	m_lightGridTextureBuffer.bind(GLConfig::getTextureBindingPoint(GLConfig::ETextures::ClusteredLightGrid));
}

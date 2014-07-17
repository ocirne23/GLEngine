/****************************************************************************/
/* Copyright (c) 2011, Markus Billeter, Ola Olsson, Erik Sintorn and Ulf Assarsson
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
/****************************************************************************/
#include "Utils\SphereToSSBounds.h"

#include "Graphics\PerspectiveCamera.h"

namespace
{

void updateClipRegionRoot(float nc,
	float lc,
	float lz,
	float lightRadius,
	float cameraScale,
	float &clipMin,
	float &clipMax)
{
	float nz = (lightRadius - nc * lc) / lz;
	float pz = (lc * lc + lz * lz - lightRadius * lightRadius) / (lz - (nz / nc) * lc);

	if (pz < 0.0f)
	{
		float c = -nz * cameraScale / nc;
		if (nc < 0.0f)
		{
			clipMin = glm::max(clipMin, c);
		}
		else
		{
			clipMax = glm::min(clipMax, c);
		}
	}
}

void updateClipRegion(float lc,
	float lz,
	float lightRadius,
	float cameraScale,
	float &clipMin,
	float &clipMax)
{
	float rSq = lightRadius * lightRadius;
	float lcSqPluslzSq = lc * lc + lz * lz;
	float d = rSq * lc * lc - lcSqPluslzSq * (rSq - lz * lz);

	if (d >= 0.0f)
	{
		float a = lightRadius * lc;
		float b = sqrt(d);
		float nx0 = (a + b) / lcSqPluslzSq;
		float nx1 = (a - b) / lcSqPluslzSq;

		updateClipRegionRoot(nx0, lc, lz, lightRadius, cameraScale, clipMin, clipMax);
		updateClipRegionRoot(nx1, lc, lz, lightRadius, cameraScale, clipMin, clipMax);
	}
}

glm::vec4 computeClipRegion(const glm::vec3 &lightPosView, float lightRadius, float cameraNear, const glm::mat4 &projection)
{
	glm::vec4 clipRegion(1.0f, 1.0f, -1.0f, -1.0f);
	if (lightPosView.z - lightRadius <= -cameraNear)
	{
		glm::vec2 clipMin(-1.0f, -1.0f);
		glm::vec2 clipMax(1.0f, 1.0f);

		updateClipRegion(lightPosView.x, lightPosView.z, lightRadius, projection[0][0], clipMin.x, clipMax.x);
		updateClipRegion(lightPosView.y, lightPosView.z, lightRadius, projection[1][1], clipMin.y, clipMax.y);

		clipRegion = glm::vec4(clipMin, clipMax);
	}

	return clipRegion;
}

inline void swap(float& a, float& b) { float tmp = a; a = b; b = tmp; }

float calcClusterZ(float viewSpaceZ, float recNear, float recLogSD1)
{
	float gridLocZ = logf(-viewSpaceZ * recNear) * recLogSD1;
	return gridLocZ;
}

}

IBounds2D findScreenSpaceBounds(const Camera& camera, glm::vec3 lightPosViewSpace, float lightRadius, Viewport viewport)
{
	glm::vec4 reg = computeClipRegion(lightPosViewSpace, lightRadius, camera.m_near, camera.m_projectionMatrix);
	reg = -reg;

	swap(reg.x, reg.z);
	swap(reg.y, reg.w);
	reg *= 0.5f;
	reg += 0.5f;

	IBounds2D result;

	result.minX = int(reg.x * float(viewport.width));
	result.minY = int(reg.y * float(viewport.height));
	result.maxX = int(reg.z * float(viewport.width));
	result.maxY = int(reg.w * float(viewport.height));
	result.minX = glm::min(result.minX, result.maxX);
	result.minY = glm::min(result.minY, result.maxY);

	return result;
}

IBounds3D findScreenSpace3DTile(const PerspectiveCamera& camera, glm::vec3 lightPosViewSpace, float lightRadius, Viewport viewport, uint pixelsPerTileW, uint pixelsPerTileH, float recLogSD1)
{
	IBounds2D bounds2D = findScreenSpaceBounds(camera, lightPosViewSpace, lightRadius, viewport);
	float recNear = 1.0f / camera.m_near;

	int minZ = glm::max(int(calcClusterZ(lightPosViewSpace.z + lightRadius, recNear, recLogSD1)), 0);
	int maxZ = glm::max(int(ceilf(calcClusterZ(lightPosViewSpace.z - lightRadius, recNear, recLogSD1)) + 0.5f), 0);
	minZ = glm::min(minZ, maxZ);

	IBounds3D bounds3D;
	bounds3D.minX = bounds2D.minX / pixelsPerTileW;
	bounds3D.maxX = (bounds2D.maxX / pixelsPerTileW) + 1;
	bounds3D.minY = bounds2D.minY / pixelsPerTileH;
	bounds3D.maxY = (bounds2D.maxY / pixelsPerTileH) + 1;
	bounds3D.minZ = minZ;
	bounds3D.maxZ = maxZ;

	return bounds3D;
}
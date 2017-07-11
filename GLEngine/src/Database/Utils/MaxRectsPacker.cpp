/** Port of https://github.com/libgdx/libgdx/blob/master/extensions/gdx-tools/src/com/badlogic/gdx/tools/texturepacker/MaxRectsPacker.java */
/*******************************************************************************
* Copyright 2011 See AUTHORS file. (https://github.com/libgdx/libgdx/blob/master/AUTHORS)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

#include "Database/Utils/MaxRectsPacker.h"

#include "EASTL/sort.h"

BEGIN_UNNAMED_NAMESPACE()

const int SEARCH_FUZZINESS = 15;

bool isPowerOfTwo(uint x)
{
	return ((x != 0) && !(x & (x - 1)));
}

uint getNextPOT(uint i)
{
	if (!isPowerOfTwo(i))
		i--;
	i |= i >> 1;
	i |= i >> 2;
	i |= i >> 4;
	i |= i >> 8;
	i |= i >> 16;
	i++;
	return i;
}

struct BinarySearch 
{
	int min, max, fuzziness, low, high, current;
	bool pot;

	BinarySearch(int a_min, int a_max, int a_fuzziness, bool a_pot) 
	{
		pot = a_pot;
		fuzziness = a_pot ? 0 : a_fuzziness;
		min = a_pot ? int(log(getNextPOT(a_min)) / log(2)) : a_min;
		max = a_pot ? int(log(getNextPOT(a_max)) / log(2)) : a_max;
	}

	int reset() 
	{
		low = min;
		high = max;
		current = (low + high) >> 1;
		return pot ? int(pow(2, current)) : current;
	}

	int next(bool result) 
	{
		if (low >= high) 
			return -1;
		if (result)
			low = current + 1;
		else
			high = current - 1;
		current = (low + high) >> 1;
		if (abs(low - high) < fuzziness) 
			return -1;
		return pot ? int(pow(2, current)) : current;
	}
};

END_UNNAMED_NAMESPACE()

MaxRectsPacker::MaxRectsPacker(const Settings& a_settings) : m_settings(a_settings)
{
}

MaxRectsPacker::~MaxRectsPacker()
{
}

eastl::vector<Page> MaxRectsPacker::pack(eastl::vector<Rect> a_rects)
{
	m_settings.paddingPx = 0;
	uint padding = (a_rects.size() == 1) ? 0 : 16;

	for (Rect& r : a_rects)
	{
		r.width += padding * 2;
		r.height += padding * 2;
	}

	eastl::sort(a_rects.begin(), a_rects.end(), [](const Rect& a, const Rect& b) {
		return int(a.width) < int(b.width);
	});

	eastl::vector<Page> pages;
	while (!a_rects.empty())
	{
		Page result = packPage(a_rects);
		pages.push_back(result);
		a_rects = result.remainingRects;
	}

	for (Page& page : pages)
	{
		for (Rect& r : page.rects)
		{
			r.x += padding;
			r.y += padding;
			r.width -= padding * 2;
			r.height -= padding * 2;
		}
	}

	return pages;
}

Page MaxRectsPacker::packPage(eastl::vector<Rect>& a_rects)
{
	int paddingX = m_settings.paddingPx;
	int paddingY = m_settings.paddingPx;
	float maxWidth = m_settings.maxWidth - float(paddingX) * 2;
	float maxHeight = m_settings.maxHeight - float(paddingY) * 2;
	uint minWidth = UINT32_MAX;
	uint minHeight = UINT32_MAX;

	for (const Rect& rect : a_rects)
	{
		minWidth = eastl::min(minWidth, rect.width);
		minHeight = eastl::min(minHeight, rect.height);
		assert(rect.width <= maxWidth && rect.height <= maxHeight);
	}

	BinarySearch widthSearch(minWidth, m_settings.maxWidth, SEARCH_FUZZINESS, m_settings.powerOfTwo);
	BinarySearch heightSearch(minHeight, m_settings.maxHeight, SEARCH_FUZZINESS, m_settings.powerOfTwo);
	int width = widthSearch.reset();
	int height = heightSearch.reset();

	Page bestResult;
	while (true)
	{
		Page bestWidthResult;
		while (width != -1)
		{
			Page result = packAtSize(true, width - m_settings.paddingPx, height - m_settings.paddingPx, a_rects);
			bestWidthResult = getBest(bestWidthResult, result);
			width = widthSearch.next(result.rects.empty());
		}
		bestResult = getBest(bestResult, bestWidthResult);
		height = heightSearch.next(bestWidthResult.rects.empty());
		if (height == -1)
			break;
		width = widthSearch.reset();
	}
	if (bestResult.rects.empty())
		bestResult = packAtSize(false, m_settings.maxWidth - m_settings.paddingPx, m_settings.maxHeight - m_settings.paddingPx, a_rects);
	return bestResult;
}

Page MaxRectsPacker::packAtSize(bool a_fully, int a_width, int a_height, const eastl::vector<Rect>& a_rects)
{
	Page bestResult;
	for (uint i = 0; i < uint(FreeRectChoiceHeuristic::NUM_HEURISTICS); ++i)
	{
		m_maxRects.init(a_width, a_height);

		Page result = m_maxRects.pack(a_rects, scast<FreeRectChoiceHeuristic>(i));

		if (a_fully && result.remainingRects.size())
			continue;
		if (result.rects.empty())
			continue;
		bestResult = getBest(bestResult, result);
	}
	return bestResult;
}

Page MaxRectsPacker::getBest(Page a, Page b) const
{
	return a.occupancy > b.occupancy ? a : b;
}

void MaxRects::init(uint a_width, uint a_height)
{
	binWidth = a_width;
	binHeight = a_height;

	usedRectangles.clear();
	freeRectangles.clear();
	freeRectangles.emplace_back(0, 0, 0, a_width, a_height);
}

Rect MaxRects::insert(Rect a_rect, FreeRectChoiceHeuristic a_heuristic)
{
	Rect newNode = scoreRect(a_rect, a_heuristic);
	if (newNode.height == 0) return newNode;

	uint numRectanglesToProcess = uint(freeRectangles.size());
	for (uint i = 0; i < numRectanglesToProcess; ++i)
	{
		if (splitFreeNode(freeRectangles[i], newNode))
		{
			freeRectangles.erase(&freeRectangles[i]);
			--i;
			--numRectanglesToProcess;
		}
	}

	pruneFreeList();
	usedRectangles.push_back(newNode);
	return newNode;
}

Page MaxRects::pack(eastl::vector<Rect> a_rects, FreeRectChoiceHeuristic a_heuristic)
{
	while (!a_rects.empty())
	{
		int bestRectIndex = -1;
		Rect bestNode;
		bestNode.score1 = UINT32_MAX;
		bestNode.score2 = UINT32_MAX;

		for (uint i = 0; i < a_rects.size(); ++i)
		{
			Rect newNode = scoreRect(a_rects[i], a_heuristic);
			if (newNode.score1 < bestNode.score1 || (newNode.score1 == bestNode.score1 && newNode.score2 < bestNode.score2))
			{
				bestNode = newNode;
				bestRectIndex = i;
			}
		}

		if (bestRectIndex == -1) break;

		placeRect(bestNode);
		a_rects.erase(&a_rects[bestRectIndex]);
	}
	Page result = getResult();
	result.remainingRects = a_rects;
	return result;
}

Page MaxRects::getResult()
{
	uint w = 0;
	uint h = 0;
	for (uint i = 0; i < usedRectangles.size(); ++i)
	{
		const Rect rect = usedRectangles[i];
		w = glm::max(w, rect.x + rect.width);
		h = glm::max(h, rect.y + rect.height);
	}
	Page result;
	result.rects = usedRectangles;
	result.occupancy = getOccupancy();
	result.width = w;
	result.height = h;
	return result;
}

void MaxRects::placeRect(Rect a_node)
{
	uint numRectanglesToProcess = uint(freeRectangles.size());
	for (uint i = 0; i < numRectanglesToProcess; ++i)
	{
		if (splitFreeNode(freeRectangles[i], a_node))
		{
			freeRectangles.erase(&freeRectangles[i]);
			--i;
			--numRectanglesToProcess;
		}
	}
	pruneFreeList();
	usedRectangles.push_back(a_node);
}

Rect MaxRects::scoreRect(Rect a_rect, FreeRectChoiceHeuristic a_heuristic)
{
	uint width = a_rect.width;
	uint height = a_rect.height;
	Rect newNode;
	switch (a_heuristic)
	{
	case FreeRectChoiceHeuristic::BestShortSideFit:
		newNode = findPositionForNewNodeBestShortSideFit(width, height);
		break;
	case FreeRectChoiceHeuristic::BottomLeftRule:
		newNode = findPositionForNewNodeBottomLeft(width, height);
		break;
	case FreeRectChoiceHeuristic::BestLongSideFit:
		newNode = findPositionForNewNodeBestLongSideFit(width, height);
		break;
	case FreeRectChoiceHeuristic::BestAreaFit:
		newNode = findPositionForNewNodeBestAreaFit(width, height);
		break;
	default: break;
	}
	newNode.id = a_rect.id;

	if (newNode.height == 0) {
		newNode.score1 = UINT32_MAX;
		newNode.score2 = UINT32_MAX;
	}
	return newNode;
}

float MaxRects::getOccupancy()
{
	uint usedSurfaceArea = 0;
	for (uint i = 0; i < usedRectangles.size(); ++i)
		usedSurfaceArea += usedRectangles[i].width * usedRectangles[i].height;
	return float(usedSurfaceArea) / (binWidth * binHeight);
}

Rect MaxRects::findPositionForNewNodeBottomLeft(uint a_width, uint a_height)
{
	Rect bestNode;
	bestNode.score1 = UINT32_MAX; // best y, score2 is best x
	for (uint i = 0; i < uint(freeRectangles.size()); i++)
	{
		// Try to place the rectangle in upright (non-rotated) orientation.
		if (freeRectangles[i].width >= a_width && freeRectangles[i].height >= a_height)
		{
			uint topSideY = freeRectangles[i].y + a_height;
			if (topSideY < bestNode.score1 || (topSideY == bestNode.score1 && freeRectangles[i].x < bestNode.score2))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = a_width;
				bestNode.height = a_height;
				bestNode.score1 = topSideY;
				bestNode.score2 = freeRectangles[i].x;
			}
		}
	}
	return bestNode;
}

Rect MaxRects::findPositionForNewNodeBestShortSideFit(uint a_width, uint a_height)
{
	Rect bestNode;
	bestNode.score1 = UINT32_MAX;

	for (uint i = 0; i < uint(freeRectangles.size()); i++) {
		// Try to place the rectangle in upright (non-rotated) orientation.
		if (freeRectangles[i].width >= a_width && freeRectangles[i].height >= a_height) {
			uint leftoverHoriz = glm::abs(freeRectangles[i].width - a_width);
			uint leftoverVert = glm::abs(freeRectangles[i].height - a_height);
			uint shortSideFit = glm::min(leftoverHoriz, leftoverVert);
			uint longSideFit = glm::max(leftoverHoriz, leftoverVert);

			if (shortSideFit < bestNode.score1 || (shortSideFit == bestNode.score1 && longSideFit < bestNode.score2)) {
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = a_width;
				bestNode.height = a_height;
				bestNode.score1 = shortSideFit;
				bestNode.score2 = longSideFit;
			}
		}
	}
	return bestNode;
}

Rect MaxRects::findPositionForNewNodeBestLongSideFit(uint a_width, uint a_height)
{
	Rect bestNode;
	bestNode.score2 = UINT32_MAX;
	for (uint i = 0; i < uint(freeRectangles.size()); i++) {
		// Try to place the rectangle in upright (non-rotated) orientation.
		if (freeRectangles[i].width >= a_width && freeRectangles[i].height >= a_height) {
			uint leftoverHoriz = glm::abs(freeRectangles[i].width - a_width);
			uint leftoverVert = glm::abs(freeRectangles[i].height - a_height);
			uint shortSideFit = glm::min(leftoverHoriz, leftoverVert);
			uint longSideFit = glm::max(leftoverHoriz, leftoverVert);

			if (longSideFit < bestNode.score2 || (longSideFit == bestNode.score2 && shortSideFit < bestNode.score1)) {
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = a_width;
				bestNode.height = a_height;
				bestNode.score1 = shortSideFit;
				bestNode.score2 = longSideFit;
			}
		}
	}
	return bestNode;
}

Rect MaxRects::findPositionForNewNodeBestAreaFit(uint a_width, uint a_height)
{
	Rect bestNode;
	bestNode.score1 = UINT32_MAX; // best area fit, score2 is best short side fit

	for (uint i = 0; i < freeRectangles.size(); i++) {
		uint areaFit = freeRectangles[i].width * freeRectangles[i].height - a_width * a_height;

		// Try to place the rectangle in upright (non-rotated) orientation.
		if (freeRectangles[i].width >= a_width && freeRectangles[i].height >= a_height) {
			uint leftoverHoriz = glm::abs(freeRectangles[i].width - a_width);
			uint leftoverVert = glm::abs(freeRectangles[i].height - a_height);
			uint shortSideFit = glm::min(leftoverHoriz, leftoverVert);

			if (areaFit < bestNode.score1 || (areaFit == bestNode.score1 && shortSideFit < bestNode.score2))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.width = a_width;
				bestNode.height = a_height;
				bestNode.score2 = shortSideFit;
				bestNode.score1 = areaFit;
			}
		}
	}
	return bestNode;
}

bool MaxRects::splitFreeNode(Rect freeNode, Rect usedNode)
{
	// Test with SAT if the rectangles even intersect.
	if (usedNode.x >= freeNode.x + freeNode.width || usedNode.x + usedNode.width <= freeNode.x
		|| usedNode.y >= freeNode.y + freeNode.height || usedNode.y + usedNode.height <= freeNode.y) return false;

	if (usedNode.x < freeNode.x + freeNode.width && usedNode.x + usedNode.width > freeNode.x) {
		// New node at the top side of the used node.
		if (usedNode.y > freeNode.y && usedNode.y < freeNode.y + freeNode.height) {
			Rect newNode = freeNode;
			newNode.height = usedNode.y - newNode.y;
			freeRectangles.push_back(newNode);
		}

		// New node at the bottom side of the used node.
		if (usedNode.y + usedNode.height < freeNode.y + freeNode.height) {
			Rect newNode = freeNode;
			newNode.y = usedNode.y + usedNode.height;
			newNode.height = freeNode.y + freeNode.height - (usedNode.y + usedNode.height);
			freeRectangles.push_back(newNode);
		}
	}

	if (usedNode.y < freeNode.y + freeNode.height && usedNode.y + usedNode.height > freeNode.y) {
		// New node at the left side of the used node.
		if (usedNode.x > freeNode.x && usedNode.x < freeNode.x + freeNode.width) {
			Rect newNode = freeNode;
			newNode.width = usedNode.x - newNode.x;
			freeRectangles.push_back(newNode);
		}

		// New node at the right side of the used node.
		if (usedNode.x + usedNode.width < freeNode.x + freeNode.width) {
			Rect newNode = freeNode;
			newNode.x = usedNode.x + usedNode.width;
			newNode.width = freeNode.x + freeNode.width - (usedNode.x + usedNode.width);
			freeRectangles.push_back(newNode);
		}
	}
	return true;
}

void MaxRects::pruneFreeList() {
	/*
	* /// Would be nice to do something like this, to avoid a Theta(n^2) loop through each pair. /// But unfortunately it
	* doesn't quite cut it, since we also want to detect containment. /// Perhaps there's another way to do this faster than
	* Theta(n^2).
	*
	* if (freeRectangles.size > 0) clb::sort::QuickSort(&freeRectangles[0], freeRectangles.size, NodeSortCmp);
	*
	* for(int i = 0; i < freeRectangles.size-1; i++) if (freeRectangles[i].x == freeRectangles[i+1].x && freeRectangles[i].y
	* == freeRectangles[i+1].y && freeRectangles[i].width == freeRectangles[i+1].width && freeRectangles[i].height ==
	* freeRectangles[i+1].height) { freeRectangles.erase(freeRectangles.begin() + i); --i; }
	*/

	// Go through each pair and remove any rectangle that is redundant.
	for (int i = 0, n = uint(freeRectangles.size()); i < n; i++)
		for (int j = i + 1; j < n; ++j)
		{
			Rect rect1 = freeRectangles[i];
			Rect rect2 = freeRectangles[j];
			if (isContainedIn(rect1, rect2))
			{
				freeRectangles.erase(&freeRectangles[i]);
				--i;
				--n;
				break;
			}
			if (isContainedIn(rect2, rect1))
			{
				freeRectangles.erase(&freeRectangles[j]);
				--j;
				--n;
			}
		}
}

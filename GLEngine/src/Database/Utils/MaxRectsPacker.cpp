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
		current = (low + high) >> > 1;
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

eastl::vector<MaxRectsPacker::Page> MaxRectsPacker::pack(eastl::vector<Rect> a_rects) const
{
	eastl::sort(a_rects.begin(), a_rects.end(), [](Rect& a, Rect& b) {
		return int(a.width) - int(b.width);
	});

	eastl::vector<Page> pages;
	while (!a_rects.empty())
		pages.push_back(packPage(a_rects));
	return pages;
}

MaxRectsPacker::Page MaxRectsPacker::packPage(eastl::vector<Rect>& a_rects) const
{
	int paddingX = m_settings.paddingPx;
	int paddingY = m_settings.paddingPx;
	float maxWidth = m_settings.maxWidth - paddingX * 2;
	float maxHeight = m_settings.maxHeight - paddingY * 2;
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

MaxRectsPacker::Page MaxRectsPacker::packAtSize(bool a_fully, int a_width, int a_height, const eastl::vector<Rect>& a_rects) const
{
	Page bestResult;
	for (uint i = 0; i < FreeRectChoiceHeuristic::NUM_HEURISTICS; ++i)
	{
		Page result = pack(a_rects, scast<FreeRectChoiceHeuristic>(i));
		if (a_fully && result.remainingRects.size())
			continue;
		if (result.rects.empty())
			continue;
		bestResult = getBest(bestResult, result);
	}
	return bestResult;
}

MaxRectsPacker::Page MaxRectsPacker::getBest(Page a, Page b) const
{
	return a.occupancy > b.occupancy ? a : b;
}

MaxRectsPacker::Page MaxRectsPacker::pack(const eastl::vector<Rect>& a_rects, FreeRectChoiceHeuristic a_method) const
{
	return Page();
}

#pragma once

#include "Core.h"
#include "EASTL/vector.h"

class MaxRectsPacker
{
public:

	struct Settings
	{
		uint paddingPx  = 0;
		bool powerOfTwo = false;
	//	bool square     = false;
		uint maxWidth   = 4096;
		uint maxHeight  = 4096;
	};

	struct Rect
	{
		Rect(uint id, uint x, uint y, uint w, uint h) : id(id), x(x), y(y), width(w), height(h) {}

		const uint id = 0;
		uint x        = 0;
		uint y        = 0;
		uint width    = 0;
		uint height   = 0;
	};

	struct Page
	{
		eastl::vector<Rect> rects;
		eastl::vector<Rect> remainingRects;
		float occupancy = 0;
		int width       = 0;
		int height      = 0;
		int padding     = 0;
	};

	enum class FreeRectChoiceHeuristic
	{
		BestShortSideFit,
		BestLongSideFit,
		BestAreaFit,
		BottomLeftRule,
		ContactPointRule,
		NUM_HEURISTICS
	};

public:

	MaxRectsPacker(const Settings& settings);
	~MaxRectsPacker();
	MaxRectsPacker(const MaxRectsPacker& copy) = delete;

	eastl::vector<Page> pack(eastl::vector<Rect> rects) const;

private:

	Page packPage(eastl::vector<Rect>& rects) const;
	Page packAtSize(bool fully, int width, int height, const eastl::vector<Rect>& rects) const;
	Page getBest(Page a, Page b) const;
	Page pack(const eastl::vector<Rect>& rects, FreeRectChoiceHeuristic method) const;

private:

	Settings m_settings;
	int binWidth = 0;
	int binHeight = 0;
	eastl::vector<Rect> usedRectangles;
	eastl::vector<Rect> freeRectangles;

};
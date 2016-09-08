#pragma once

#include "Core.h"
#include "EASTL/vector.h"
#include <glm/glm.hpp>

struct Rect
{
	Rect() {}
	Rect(uint id, uint x, uint y, uint w, uint h) : id(id), x(x), y(y), width(w), height(h) {}

	uint id = 0;
	uint x = 0;
	uint y = 0;
	uint width = 0;
	uint height = 0;
	uint score1 = 0;
	uint score2 = 0;
};

struct Page
{
	eastl::vector<Rect> rects;
	eastl::vector<Rect> remainingRects;
	float occupancy = 0;
	int width = 0;
	int height = 0;
	int padding = 0;
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

class MaxRects
{
public:

	void init(uint a_width, uint a_height);
	Rect insert(Rect a_rect, FreeRectChoiceHeuristic a_heuristic);
	Page pack(eastl::vector<Rect> a_rects, FreeRectChoiceHeuristic a_heuristic);
	Page getResult();

private:

	void placeRect(Rect a_node);
	Rect scoreRect(Rect a_rect, FreeRectChoiceHeuristic a_heuristic);
	float getOccupancy();
	Rect findPositionForNewNodeBottomLeft(uint a_width, uint a_height);
	Rect findPositionForNewNodeBestShortSideFit(uint a_width, uint a_height);
	Rect findPositionForNewNodeBestLongSideFit(uint a_width, uint a_height);
	Rect findPositionForNewNodeBestAreaFit(uint a_width, uint a_height);
	bool splitFreeNode(Rect freeNode, Rect usedNode);
	void pruneFreeList();
	bool isContainedIn(Rect a, Rect b) const
	{
		return a.x >= b.x && a.y >= b.y && a.x + a.width <= b.x + b.width && a.y + a.height <= b.y + b.height;
	}

private:

	uint binWidth = 0;
	uint binHeight = 0;
	eastl::vector<Rect> usedRectangles;
	eastl::vector<Rect> freeRectangles;
};

class MaxRectsPacker
{
public:

	struct Settings
	{
		uint paddingPx  = 0;
		bool powerOfTwo = false;
		uint maxWidth   = 4096;
		uint maxHeight  = 4096;
	};

public:

	MaxRectsPacker(const Settings& settings);
	~MaxRectsPacker();
	MaxRectsPacker(const MaxRectsPacker& copy) = delete;

	eastl::vector<Page> pack(eastl::vector<Rect> rects);

private:

	Page packPage(eastl::vector<Rect>& rects);
	Page packAtSize(bool fully, int width, int height, const eastl::vector<Rect>& rects);
	Page getBest(Page a, Page b) const;

private:

	Settings m_settings;
	MaxRects m_maxRects;
	int binWidth = 0;
	int binHeight = 0;
	eastl::vector<Rect> usedRectangles;
	eastl::vector<Rect> freeRectangles;

};
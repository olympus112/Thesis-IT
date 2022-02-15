#pragma once

#include "graphics/canvas.h"
#include "graphics/seedPoint.h"

typedef int TSPGIndex;
enum TSPGIndex_ {
	TSPGIndex_Jittered,
	TSPGIndex_Greedy
};

struct TSPG {
public:
	static std::unordered_map<TSPGIndex, SRef<TSPG>> get;

	virtual ~TSPG() = default;

	virtual void renderOverlay(Canvas& source, Canvas& target) {}
	virtual void renderSettings(Canvas& source, Canvas& target) {}
	virtual std::vector<SeedPoint> generate(Canvas& source, Canvas& target) = 0;
};


#pragma once

#include "graphics/canvas.h"
#include "graphics/mondriaanPatch.h"
#include "graphics/seedPoint.h"

class SeedPointsTab {
public:
	Canvas source;
	Canvas target;

	std::vector<SeedPoint> seedPoints;
	std::vector<MondriaanPatch> patches;

	int intersectedIndex = -1;
	int selectedIndex = -1;
	Vec2f intersectedPoint;
	Vec2f selectedPoint;

	SeedPointsTab() = default;

	void init();
	void update();
	void render();

	void onSourceChanged();
	void onTargetChanged();
	void reload();
};

#pragma once

#include <random>

#include "graphics/canvas.h"
#include "graphics/mondriaanPatch.h"
#include "graphics/seedPoint.h"

class SeedPointsTab {
private:
	bool showConnections = false;

	int targetSeedPoints = 10;
	int targetSeedPointInterDistance = 100;
	bool targetSeedPointsGridMethod = false;
	int targetSeedPointsGridDivisions = 10;

	std::mt19937 generator;
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
	void renderCursor();
	void renderTooltip();
	void renderPatchViewer();
	void renderSeedPointViewer();
	void renderSeedPoints();
	void renderPatches();
	void renderTextures();
	void renderSettings();
	void renderTargetGrid();

	void resetSelection();

	void spawnTargetSeedpoints();
	void spawnSourceSeedpoints();
	void spawnPatches();
	void mutatePatches();

	void onSourceChanged();
	void onTargetChanged();
	void reload();
};

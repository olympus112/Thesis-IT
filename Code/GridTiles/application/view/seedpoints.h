#pragma once

#include <random>

#include "generation/TSPG/TSPG.h"
#include "generation/SSPG/SSPG.h"
#include "graphics/canvas.h"
#include "graphics/seedPoint.h"

class SeedPointsTab {
private:
	TSPGIndex tspGenerationMethod;
	SSPGIndex sspGenerationMethod;

	bool showConnections = false;

	std::mt19937 generator;
public:
	Canvas source;
	Canvas target;

	std::vector<SeedPoint> seedPoints;

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
	int checkPatch(Patch* newPatch, Patch* oldPatch);
	void generateImage();

	void onSourceChanged();
	void onTargetChanged();
	void reload();
};

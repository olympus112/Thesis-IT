#pragma once

#include <random>

#include "generation/TSPG/TSPG.h"
#include "generation/SSPG/SSPG.h"
#include "graphics/canvas.h"
#include "graphics/mondriaanPatch.h"

class EditorView {
private:
	TSPGIndex tspGenerationMethod;
	SSPGIndex sspGenerationMethod;

	bool showConnections = false;

	std::mt19937 generator;
public:
	Canvas source;
	Canvas target;

	std::vector<MondriaanPatch> patches;

	int intersectedIndex = -1;
	int selectedIndex = -1;
	Vec2f intersectedPoint;
	Vec2f selectedPoint;

	EditorView() = default;

	void init();
	void update();

	void render();
	void renderCursor();
	void renderTooltip();
	void renderPatchViewer();
	void renderPatches();
	void renderTextures();
	void renderSettings();

	void resetSelection();

	void spawnTargetPatches();
	void mutateSourcePatches();
	void mutatePatches();
	int checkPatch(MondriaanPatch* newPatch, MondriaanPatch* oldPatch);
	void generateImage();

	void reload();
};

#pragma once

#include <random>

#include "generation/TSPG/TSPG.h"
#include "generation/SSPG/SSPG.h"
#include "graphics/canvas.h"
#include "graphics/mondriaanPatch.h"
#include "util/regularGrid.h"

class EditorView {
private:
	TSPGIndex tspGenerationMethod;
	SSPGIndex sspGenerationMethod;

	bool showConnections = false;
	bool showRegularGrid = false;

	int nMutations;
	int nMutatedPatches;
	int nSplits;
	int metric;

	std::mt19937 generator;
public:
	Canvas source;
	Canvas target;

	RegularGrid<10, 10> grid;

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

	void renderVoronoi();
	void computeVoronoi();

	void resetSelection();

	void spawnTargetPatches();
	void mutateSourcePatches();
	void generateImage();

	void mutatePatches();
	void mutatePatchesRandom();
	void splitPatches();

	void spawnNewPatch();
	bool checkPatch(const MondriaanPatch& oldPatch, const MondriaanPatch& newPatch);
	bool checkPatchSourceLocation(const MondriaanPatch& patch);
	bool checkPatchTargetLocation(const MondriaanPatch& patch);
	bool checkPatchOverlap(const MondriaanPatch& patch, const MondriaanPatch& oldPatch);

	void reload();
};

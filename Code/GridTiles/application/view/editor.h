#pragma once

#include <random>

#include "generation/TSPG/TSPG.h"
#include "generation/SSPG/SSPG.h"
#include "graphics/canvas.h"
#include "graphics/mondriaanPatch.h"
#include "thread_pool/thread_pool.hpp"
#include "util/RegularTree.h"

class EditorView {
private:
	thread_pool pool;

	TSPGIndex tspGenerationMethod;
	SSPGIndex sspGenerationMethod;

	bool showConnections = false;
	bool showRegularGrid = false;

	int nSplits;
	int metric;

	bool stop;

	std::mt19937 generator;
public:
	Canvas source;
	Canvas target;

	RegularTree<10, 10> grid;

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
	void splitPatchesRollingGuidance();
	void generateRegularPatches();

	void spawnNewPatch();
	bool checkPatch(const MondriaanPatch& oldPatch, const MondriaanPatch& newPatch);
	bool checkPatchSourceLocation(const MondriaanPatch& patch);
	bool checkPatchTargetLocation(const MondriaanPatch& patch);
	bool checkPatchOverlap(const MondriaanPatch& patch, const MondriaanPatch& oldPatch);

	void reload();
};

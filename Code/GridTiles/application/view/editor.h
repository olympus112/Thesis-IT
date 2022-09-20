#pragma once

#include <random>

#include "generation/TSPG/TSPG.h"
#include "generation/SSPG/SSPG.h"
#include "graphics/canvas.h"
#include "graphics/mondriaanPatch.h"
#include "thread_pool/thread_pool.hpp"
#include "util/RegularTree.h"

typedef int ChoiceMethod;
enum ChoiceMethod_ {
	ChoiceMethod_Area,
	ChoiceMethod_Axis,
	ChoiceMethod_Feature
};

typedef int FeatureChoice;
enum FeatureChoice_ {
	FeatureChoice_Salience,
	FeatureChoice_Edge,
	FeatureChoice_EdgeLevels
};

typedef int FeatureMethod;
enum FeatureMethod_ {
	FeatureMethod_Max,
	FeatureMethod_Sum,
	FeatureMethod_Mean
};

typedef int SplitMethod;
enum SplitMethod_ {
	SplitMethod_Random,
	SplitMethod_Axis,
	SplitMethod_Greedy
};

typedef int FractionMethod;
enum FractionMethod_ {
	FractionMethod_Rational,
	FractionMethod_Golden_Ratio,
	FractionMethod_Greedy,
	FractionMethod_Constant
};

typedef int FractionFeature;
enum FractionFeature_ {
	FractionFeature_Saliency,
	FractionFeature_Edge,
	FractionFeature_EdgeLevels,
	FractionFeature_DilatedLevels
};

typedef int PDFChoice;
enum PDFChoice_ {
	PDFChoice_Best,
	PDFChoice_Constant,
	PDFChoice_Linear,
	PDFChoice_Quadratic
};

typedef int SortMethod;
enum SortMethod_ {
	SortMethod_Saliency,
	SortMethod_Center,
	SortMethod_Area
};

class EditorView {
private:
	thread_pool pool;

	TSPGIndex tspGenerationMethod;
	SSPGIndex sspGenerationMethod;

	bool showConnections = false;
	bool showRegularGrid = false;

	int nSplits;
	int metric;
	int splitMetric = cv::TemplateMatchModes::TM_CCORR_NORMED;
	ChoiceMethod choiceMethod = ChoiceMethod_Area;
	FeatureMethod featureMethod = FeatureMethod_Sum;
	FeatureChoice featureChoice = FeatureChoice_Salience;
	PDFChoice pdfChoice = PDFChoice_Linear;
	SplitMethod splitMethod = SplitMethod_Axis;
	FractionMethod fractionMethod = FractionMethod_Greedy;
	FractionFeature fractionFeature = FractionFeature_EdgeLevels;
	SortMethod sortMethod = SortMethod_Saliency;
	bool showSort = false;
	float constantFraction = 0.5;

	bool stop;
	bool showMondrianGrid = false;
	ImVec2 puzzlePos;

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
	void splitPatches(std::size_t patchToSplit = -1);
	void splitPatchesRollingGuidance(std::size_t patchToSplit = -1);
	void generateRegularPatches();
	void matchPatches(std::size_t selectedIndex);
	void sortPatches();
	void exportImage();

	void spawnNewPatch();
	bool checkPatch(const MondriaanPatch& oldPatch, const MondriaanPatch& newPatch);
	bool checkPatchSourceLocation(const MondriaanPatch& patch);
	bool checkPatchTargetLocation(const MondriaanPatch& patch);
	bool checkPatchOverlap(const MondriaanPatch& patch, const MondriaanPatch& oldPatch);

	void reload();
};

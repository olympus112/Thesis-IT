#pragma once

#include "graphics/canvas.h"
#include "graphics/seedPoint.h"

enum TSPGenerationMethod_ {
	TSPGenerationMethod_RegularGrid,
	TSPGenerationMethod_Greedy
};

struct TSPGenerationMethod {
public:
	virtual ~TSPGenerationMethod() = default;

	virtual void renderOverlay(Canvas& source, Canvas& target) {}
	virtual void renderSettings(Canvas& source, Canvas& target) {}
	virtual std::vector<SeedPoint> generate(Canvas& source, Canvas& target) = 0;
};

struct TSPG_RegularGrid : public TSPGenerationMethod {
public:
	int divisions;
	bool showGrid;

	void renderOverlay(Canvas& source, Canvas& target) override;
	void renderSettings(Canvas& source, Canvas& target) override;

	std::vector<SeedPoint> generate(Canvas& source, Canvas& target) override;
};

struct TSPG_Greedy : public TSPGenerationMethod {
public:
	int count;
	int interdistance;

	void renderSettings(Canvas& source, Canvas& target) override;

	std::vector<SeedPoint> generate(Canvas& source, Canvas& target) override;
};
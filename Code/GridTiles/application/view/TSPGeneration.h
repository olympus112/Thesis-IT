#pragma once

#include "graphics/canvas.h"
#include "graphics/seedPoint.h"

struct TSPG_Method {
public:
	virtual ~TSPG_Method() = default;

	virtual void renderOverlay(Canvas& source, Canvas& target) {}
	virtual void renderSettings(Canvas& source, Canvas& target) {}
	virtual std::vector<SeedPoint> generate(Canvas& source, Canvas& target) = 0;
};

struct TSPG_Jittered : public TSPG_Method {
public:
	typedef int JitterMethod;
	enum JitterMethod_ {
		JitterMethod_Random,
		JitterMethod_Center,
		JitterMethod_Salience
	};

	JitterMethod jitterMethod = JitterMethod_Salience;
	int divisions = 10;
	bool showGrid = true;

	TSPG_Jittered() = default;

	void renderOverlay(Canvas& source, Canvas& target) override;
	void renderSettings(Canvas& source, Canvas& target) override;

	std::vector<SeedPoint> generate(Canvas& source, Canvas& target) override;
};

struct TSPG_Greedy : public TSPG_Method {
public:
	typedef int GreedyMethod;
	enum GreedyMethod_ {
		GreedyMethod_Random,
		GreedyMethod_Salience
	};

	GreedyMethod greedyMethod = GreedyMethod_Salience;
	int count = 20;
	int interdistance = 100;

	TSPG_Greedy() = default;

	void renderSettings(Canvas& source, Canvas& target) override;

	std::vector<SeedPoint> generate(Canvas& source, Canvas& target) override;
};

typedef int TSPGenerationMethod;
enum TSPGenerationMethod_ {
	TSPGenerationMethod_Jittered,
	TSPGenerationMethod_Greedy
};

extern std::unordered_map<TSPGenerationMethod, SRef<TSPG_Method>> tspGenerationMethods;
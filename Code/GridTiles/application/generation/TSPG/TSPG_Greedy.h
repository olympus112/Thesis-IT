#pragma once

#include "TSPG.h"

struct TSPG_Greedy : public TSPG {
public:
	inline static TSPGIndex ID = TSPGIndex_Greedy;

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

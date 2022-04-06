#pragma once

#include "TSPG.h"

struct TSPG_Jittered : public TSPG {
public:
	inline static TSPGIndex ID = TSPGIndex_Jittered;

	typedef int JitterMethod;
	enum JitterMethod_ {
		JitterMethod_Random,
		JitterMethod_Center,
		JitterMethod_Salience,
		JitterMethod_Start
	};

	JitterMethod jitterMethod = JitterMethod_Salience;
	int divisions = 10;
	bool showGrid = false;
	bool contain = true;


	TSPG_Jittered() = default;

	void renderOverlay(Canvas& source, Canvas& target) override;
	void renderSettings(Canvas& source, Canvas& target) override;

	std::vector<MondriaanPatch> generate() override;
};

#pragma once
#include "SSPG.h"

struct SSPG_TemplateMatch : public SSPG {
	inline static SSPGIndex ID = SSPGIndex_TemplateMatch;

	int interdistance = 100;
	int metric = 1;

	SSPG_TemplateMatch() = default;

	void renderSettings(Canvas& source, Canvas& target) override;

	void mutate(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) override;
};

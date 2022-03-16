#pragma once
#include "SSPG.h"

struct SSPG_TemplateMatch : public SSPG {
	inline static SSPGIndex ID = SSPGIndex_TemplateMatch;

	int rotations = 5;
	int interdistance = 100;
	int metric = 1;

	SSPG_TemplateMatch() = default;

	void renderSettings(Canvas& source, Canvas& target) override;

	void mutate(std::vector<MondriaanPatch>& patches) override;
};

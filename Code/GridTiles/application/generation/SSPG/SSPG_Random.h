#pragma once

#include "SSPG.h"
#include "graphics/seedPoint.h"

struct SSPG_Random : public SSPG {
	inline static SSPGIndex ID = SSPGIndex_Random;

	int interdistance = 100;

	SSPG_Random() = default;

	void renderSettings(Canvas& source, Canvas& target) override;

	void mutate(std::vector<MondriaanPatch>& patches) override;
};

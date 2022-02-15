#pragma once

#include "SSPG.h"
#include "graphics/seedPoint.h"

struct SSPG_Sift : public SSPG {
	inline static SSPGIndex ID = SSPGIndex_Sift;

	SSPG_Sift() = default;

	void renderSettings(Canvas& source, Canvas& target) override;

	void mutate(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) override;
};

#pragma once

#include "graphics/canvas.h"
#include "graphics/seedPoint.h"

enum SSPGenerationMethod_ {
	SSPGenerationMethod_TemplateMatch
};

struct SSPGenerationMethod {
public:
	virtual ~SSPGenerationMethod() = default;

	virtual void renderOverlay(Canvas& source, Canvas& target) {}
	virtual void renderSettings(Canvas& source, Canvas& target) {}
	virtual void mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) = 0;
};

struct SSPG_TemplateMatch : public SSPGenerationMethod {
	int interdistance;
	int metric;

	void renderSettings(Canvas& source, Canvas& target) override;

	void mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) override;
};
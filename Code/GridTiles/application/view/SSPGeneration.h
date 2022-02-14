#pragma once

#include "graphics/canvas.h"
#include "graphics/seedPoint.h"

struct SSPG_Method {
public:
	virtual ~SSPG_Method() = default;

	virtual void renderOverlay(Canvas& source, Canvas& target) {}
	virtual void renderSettings(Canvas& source, Canvas& target) {}
	virtual void mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) = 0;
};

struct SSPG_Random : public SSPG_Method {
	int interdistance = 100;

	SSPG_Random() = default;

	void renderSettings(Canvas& source, Canvas& target) override;

	void mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) override;
};

struct SSPG_TemplateMatch : public SSPG_Method {
	int interdistance = 100;
	int metric = 1;

	SSPG_TemplateMatch() = default;

	void renderSettings(Canvas& source, Canvas& target) override;

	void mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) override;
};

struct SSPG_SIFT : public SSPG_Method {
	SSPG_SIFT() = default;

	void renderSettings(Canvas& source, Canvas& target) override;

	void mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) override;
};

typedef int SSPGenerationMethod;
enum SSPGenerationMethod_ {
	SSPGenerationMethod_Random,
	SSPGenerationMethod_TemplateMatch,
	SSPGenerationMethod_SIFT
};

extern std::unordered_map<SSPGenerationMethod, SRef<SSPG_Method>> sspGenerationMethods;
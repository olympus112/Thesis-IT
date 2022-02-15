#pragma once

#include "graphics/canvas.h"
#include "graphics/seedPoint.h"

typedef int SSPGIndex;
enum SSPGIndex_ {
	SSPGIndex_Random,
	SSPGIndex_TemplateMatch,
	SSPGIndex_Sift
};

struct SSPG {
public:
	static std::unordered_map<SSPGIndex, SRef<SSPG>> get;

	virtual ~SSPG() = default;

	virtual void renderOverlay(Canvas& source, Canvas& target) {}
	virtual void renderSettings(Canvas& source, Canvas& target) {}
	virtual void mutate(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) = 0;
};
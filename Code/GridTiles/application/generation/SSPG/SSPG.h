#pragma once

#include "graphics/canvas.h"
#include "graphics/mondriaanPatch.h"

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
	virtual void mutate(std::vector<MondriaanPatch>& patches) = 0;
};
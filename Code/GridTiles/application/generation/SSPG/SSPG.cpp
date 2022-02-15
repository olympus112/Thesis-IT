#include <core.h>
#include "SSPG.h"

#include "SSPG_Random.h"
#include "SSPG_TemplateMatch.h"
#include "SSPG_Sift.h"

std::unordered_map<SSPGIndex, SRef<SSPG>> SSPG::get = {
	{ SSPG_Random::ID, std::make_shared<SSPG_Random>() },
	{ SSPG_TemplateMatch::ID, std::make_shared<SSPG_TemplateMatch>() },
	{ SSPG_Sift::ID, std::make_shared<SSPG_Sift>() }
};

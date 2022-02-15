#include <core.h>
#include "TSPG.h"

#include <unordered_map>

#include "TSPG_Greedy.h"
#include "TSPG_Jittered.h"

std::unordered_map<TSPGIndex, SRef<TSPG>> TSPG::get = {
	{TSPG_Jittered::ID, std::make_shared<TSPG_Jittered>()},
	{TSPG_Greedy::ID, std::make_shared<TSPG_Greedy>()}
};

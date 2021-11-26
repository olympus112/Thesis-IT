#pragma once
#include "pipeline.h"
#include "seedpoints.h"

class Editor {
public:
	URef<PipelineTab> pipelineTab;
	URef<SeedPointsTab> seedPointsTab;

	Editor();

	void init();
	void update();
	void render();
};

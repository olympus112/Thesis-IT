#pragma once
#include "pipeline.h"
#include "seedpoints.h"

class EditorView {
public:
	PipelineTab pipeline;
	SeedPointsTab seedpoints;

	EditorView();

	void init();
	void update();
	void render();
};

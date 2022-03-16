#pragma once

#include "editor.h"
#include "pipeline.h"
#include "settingsView.h"

class Screen {
public:
	PipelineView pipeline;
	EditorView editor;
	SettingsView settingsView;

	Screen();

	void init();
	void update();
	void render();
};

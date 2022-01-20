#pragma once

#include "editorView.h"
#include "settingsView.h"

class Screen {
public:
	EditorView editor;
	SettingsView settingsView;

	Screen();

	void init();
	void update();
	void render();
};

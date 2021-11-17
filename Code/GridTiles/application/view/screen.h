#pragma once

#include "editor.h"
#include "settings.h"

class Screen {
public:
	URef<Editor> editor;
	URef<Settings> settings;

	Screen();

	void init();
	void update();
	void render();
};

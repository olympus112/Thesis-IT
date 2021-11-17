#pragma once

class Screen;
extern URef<Screen> screen;

bool init();
void update();
void render();
void close();
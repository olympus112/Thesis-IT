#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Screen;

extern URef<Screen> screen;
extern GLFWwindow* window;

bool init();
void update();
void render();
void close();
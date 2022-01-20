#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "view/screen.h"
#include "view/settings.h"


extern Settings settings;
extern Screen screen;
extern GLFWwindow* window;

bool init();
void update();
void render();
void close();
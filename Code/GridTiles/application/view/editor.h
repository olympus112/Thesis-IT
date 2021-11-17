#pragma once

class Editor {

public:
	Editor();

	void init();
	void update();
	void render();

	void reloadSource();
	void reloadTarget();
private:
	void histogram();
	void seedpoints();
};

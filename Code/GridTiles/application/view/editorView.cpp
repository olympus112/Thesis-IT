#include "core.h"
#include "editorView.h"

EditorView::EditorView() = default;

void EditorView::init() {
	pipeline.init();
	seedpoints.init();
}

void EditorView::update() {
	pipeline.update();
	seedpoints.update();
}

void EditorView::render() {
	seedpoints.render();
	pipeline.render();
}

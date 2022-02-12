#pragma once

#include "graphics/textures/texture.h"
#include "imgui/imfilebrowser.h"

namespace ImGui {
	class TexturePicker {
	public:
		std::string name;
		std::string path;
		bool hovered;
		Texture* texture;
		URef<FileBrowser> browser;

		TexturePicker(const std::string& name);

		bool render();
		void load(Texture* texture, const std::string& path = "");

		ImTextureID it();
	};
}

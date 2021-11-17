#pragma once

#include "graphics/texture.h"
#include "imgui/imfilebrowser.h"

namespace ImGui {
	class TexturePicker {
	public:
		std::string name;
		std::string path;
		URef<Texture> texture;
		URef<FileBrowser> browser;

		TexturePicker(const std::string& name);

		bool render();
		void load(const std::string& path);

		ImTextureID asImTexture();
	};
}

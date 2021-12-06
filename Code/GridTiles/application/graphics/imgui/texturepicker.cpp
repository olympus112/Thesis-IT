#include <core.h>
#include "texturepicker.h"

#include "math/utils.h"

ImGui::TexturePicker::TexturePicker(const std::string& name) {
	this->name = name;
	this->browser = std::make_unique<FileBrowser>();

	browser->SetTitle("Choose " + name);
	browser->SetTypeFilters({".png", ".jpg"});
}

bool ImGui::TexturePicker::render() {
	bool result = false;

	NewLine();
	Text("%s:", name.c_str());

	if (texture) {
		if (!path.empty())
			TextDisabled(path.c_str());

		float width = Utils::clamp(GetContentRegionAvailWidth() - GetStyle().FramePadding.x * 2, 100.0f, 300.0f);
		if (ImageButton(reinterpret_cast<ImTextureID>(texture->id), ImVec2(width, width)))
			browser->Open();
	} else {
		if (Button("Choose image"))
			browser->Open();
	}

	NewLine();
	Separator();

	browser->Display();

	if (browser->HasSelected()) {
		path = browser->GetSelected().string();
		texture = std::make_unique<Texture>(path);
		browser->ClearSelected();

		result = true;
	}

	return result;
}

void ImGui::TexturePicker::load(const std::string& path) {
	this->path = path;
	texture = std::make_unique<Texture>(path);
}

ImTextureID ImGui::TexturePicker::asImTexture() {
	return texture->asImTexture();
}

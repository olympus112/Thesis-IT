#include <core.h>
#include "texturepicker.h"

#include "math/utils.h"

ImGui::TexturePicker::TexturePicker(const std::string& name) {
	this->name = name;
	this->browser = std::make_unique<FileBrowser>();

	browser->SetTitle("Choose " + name);
	browser->SetTypeFilters({".png", ".jpg"});
}

bool ImGui::TexturePicker::render(Texture* texture) {
	bool result = false;

	NewLine();
	ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
	Text("%s:", name.c_str());
	ImGui::PopStyleColor();

	if (texture) {
		if (!path.empty())
			TextDisabled(path.c_str());

		float width = Utils::clamp(GetContentRegionAvail().x - GetStyle().FramePadding.x * 2, 100.0f, 300.0f);
		if (ImageButton(reinterpret_cast<ImTextureID>(texture->id), ImVec2(width, width  / texture->aspect())))
			browser->Open();

		ImGui::Text("Aspect: %f", texture->aspect());
		ImGui::Text("Texture dimension: %d px X %d px", texture->data.cols, texture->data.rows);

	} else {
		if (Button("Choose image"))
			browser->Open();
	}

	hovered = ImGui::IsItemHovered();
		browser->Display();

	if (browser->HasSelected()) {
		path = browser->GetSelected().string();
		browser->ClearSelected();

		result = true;
	}

	return result;
}


ImTextureID ImGui::TexturePicker::it() {
	return texture->it();
}

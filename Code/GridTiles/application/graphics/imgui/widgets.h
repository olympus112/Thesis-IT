#pragma once

#include "../color.h"
#include "imgui/imgui.h"
#include "util/globals.h"

namespace ImGui {
	void image(const char* text, ImTextureID user_texture_id, const ImVec2& size = Globals::imageSize, const ImVec2& uv0 = ImVec2(0, 0),
	           const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
	           const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	void arrow(const Vec2f& p1 = {0, Globals::imageWidth / 2},
	           const Vec2f& p2 = {Globals::arrowWidth, Globals::imageWidth / 2}, float thickness = 1.0f,
	           const Color& color = Colors::WHITE);
}

#pragma once
#include "graphics/color.h"

namespace ImGuiUtils {
	inline void pushButtonColor(const Color& color) {
		ImGui::PushStyleColor(ImGuiCol_Button, color.withOpacity(0.6).iv4());
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color.withOpacity(0.7).iv4());
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, color.withOpacity(0.8).iv4());
	}

	inline void pushButtonColor(float color) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(color, 0.6f, 0.6f).Value);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(color, 0.7f, 0.7f).Value);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(color, 0.8f, 0.8f).Value);
	}

	inline void popButtonColor() {
		ImGui::PopStyleColor(3);
	}

	inline void ImageNoSpacing(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0)) {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(9, ImGui::GetStyle().ItemSpacing.y));
		ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
		ImGui::PopStyleVar();
	}
}

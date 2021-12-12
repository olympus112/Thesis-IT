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
}

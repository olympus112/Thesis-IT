#include <core.h>
#include "graphics/imgui/widgets.h"

#include "imgui/imgui_internal.h"

namespace ImGui {
	void arrow(const Vec2f& p1, const Vec2f& p2, float thickness, const Color& color) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		Vec2f direction = p2 - p1;
		float len = length(direction);
		if (len == 0.0f)
			return;

		ImVec2 size = abs(direction).asImVec();
		ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);

		ImGui::ItemSize(bb);
		if (!ImGui::ItemAdd(bb, 0))
			return;

		window->DrawList->AddLine(
			bb.Min + p1,
			bb.Min + p2,
			color.u32(),
			thickness
		);

		direction /= len / 5.0f;

		window->DrawList->AddTriangleFilled(
			bb.Min + Vec2f(p2.x, p2.y),
			bb.Min + Vec2f(p2.x - direction.y - 2.0f * direction.x, p2.y + direction.x - 2.0f * direction.y),
			bb.Min + Vec2f(p2.x + direction.y - 2.0f * direction.x, p2.y - direction.x - 2.0f * direction.y), 
			color.u32());

	}

	void image(const char* text, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0,
	           const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImVec2 textSize = ImGui::CalcTextSize(text);
		ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + ImVec2(0, textSize.y));
		if (border_col.w > 0.0f)
			bb.Max = bb.Max + ImVec2(2, 2);
		ImGui::ItemSize(bb);
		if (!ImGui::ItemAdd(bb, 0))
			return;

		if (border_col.w > 0.0f) {
			window->DrawList->AddRect(bb.Min + ImVec2(0, textSize.y), bb.Max - ImVec2(0, textSize.y), ImGui::GetColorU32(border_col), 0.0f);
			window->DrawList->AddImage(user_texture_id, bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1) - ImVec2(0, textSize.y), uv0, uv1, ImGui::GetColorU32(tint_col));
			window->DrawList->AddText(bb.Max - ImVec2((bb.GetWidth() - textSize.x) / 2.0f, textSize.y), ImGui::GetColorU32(tint_col), text);
		} else {
			window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max - ImVec2(0, textSize.y), uv0, uv1, ImGui::GetColorU32(tint_col));
			window->DrawList->AddText(ImVec2(bb.Min.x + (bb.GetWidth() - textSize.x) / 2.0f, bb.Max.y - textSize.y), ImGui::GetColorU32(tint_col), text);
		}
	}
}

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
			window->DrawList->AddRect(bb.Min + ImVec2(0, textSize.y), bb.Max - ImVec2(0, textSize.y),
			                          ImGui::GetColorU32(border_col), 0.0f);
			window->DrawList->AddImage(user_texture_id, bb.Min + ImVec2(1, 1),
			                           bb.Max - ImVec2(1, 1) - ImVec2(0, textSize.y), uv0, uv1,
			                           ImGui::GetColorU32(tint_col));
			window->DrawList->AddText(bb.Max - ImVec2((bb.GetWidth() - textSize.x) / 2.0f, textSize.y),
			                          ImGui::GetColorU32(tint_col), text);
		}
		else {
			window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max - ImVec2(0, textSize.y), uv0, uv1,
			                           ImGui::GetColorU32(tint_col));
			window->DrawList->AddText(ImVec2(bb.Min.x + (bb.GetWidth() - textSize.x) / 2.0f, bb.Max.y - textSize.y),
			                          ImGui::GetColorU32(tint_col), text);
		}
	}

	bool SliderFloatWithSteps(const char* label, float* v, float v_min, float v_max, float v_step,
	                                 const char* display_format) {
		if (!display_format)
			display_format = "%.3f";

		char text_buf[64] = {};
		ImFormatString(text_buf, IM_ARRAYSIZE(text_buf), display_format, *v);

		// Map from [v_min,v_max] to [0,N]
		const int countValues = int((v_max - v_min) / v_step);
		int v_i = int((*v - v_min) / v_step);
		const bool value_changed = SliderInt(label, &v_i, 0, countValues, text_buf);

		// Remap from [0,N] to [v_min,v_max]
		*v = v_min + float(v_i) * v_step;
		return value_changed;
	}
}

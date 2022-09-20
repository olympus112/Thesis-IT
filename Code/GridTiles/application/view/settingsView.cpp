#include "core.h"
#include "settingsView.h"

#include "main.h"
#include "screen.h"
#include "graphics/imgui/imguiUtils.h"
#include "graphics/imgui/texturepicker.h"
#include "graphics/imgui/widgets.h"
#include "util/stringUtil.h"

static ImGui::TexturePicker sourceTexture("Source texture");
static ImGui::TexturePicker targetTexture("Target texture");

SettingsView::SettingsView() = default;

void SettingsView::init() {
}

void SettingsView::update() {
}

void SettingsView::render() {
	ImGui::Begin("Settings");

	if (ImGui::Button("Reload pipeline", ImVec2(ImGui::GetContentRegionAvail().x, 80))) {
		settings.reloadPrescaledTextures();
		screen.pipeline.reload();
		screen.editor.reload();
	}

	if (ImGui::Button("Reload levels" , ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
		screen.pipeline.reloadLevels();
	}

	if (ImGui::CollapsingHeader("Global variables")) {
		ImGui::Text("Source hover: %s", screen.editor.source.hover ? "Yes" : "No");
		ImGui::Text("Source drag: %s", screen.editor.source.drag ? "Yes" : "No");
		ImGui::Text("Target hover: %s", screen.editor.target.hover ? "Yes" : "No");
		ImGui::Text("Target drag: %s", screen.editor.target.drag ? "Yes" : "No");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Intersected index: %d", screen.editor.intersectedIndex);
		ImGui::Text("Intersected point: %s", Util::str(screen.editor.intersectedPoint).c_str());
		ImGui::Text("Selected index: %d", screen.editor.intersectedIndex);
		ImGui::Text("Selected point: %s", Util::str(screen.editor.selectedPoint).c_str());
	}

	if (ImGui::CollapsingHeader("Feature settings")) {
		// Intensity weight
		if (ImGui::SliderFloat("Intensity weight", &settings.intensityWeight, 0.0f, 1.0f)) {
			settings.edgeWeight = 1.0f - settings.intensityWeight;
		}
		// Edge weight
		if (ImGui::SliderFloat("Edge weight", &settings.edgeWeight, 0.0f, 1.0f)) {
			settings.intensityWeight = 1.0f - settings.edgeWeight;
		}

		// Equalization weight
		ImGui::SliderFloat("Equalization weight", &settings.equalizationWeight, 0.0f, 1.0f);
		ImGui::Checkbox("Use RGB", &settings.useRGB);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		{
			static std::array edgeMethods = { "Sobel", "Canny" };
			ImGui::Combo("Edge method", &settings.edgeMethod, edgeMethods.data(), edgeMethods.size());
		}

		ImGui::TextColored(Colors::BLUE.iv4(), "Sobel");
		ImGui::RadioButton("X", &settings.sobelType, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Y", &settings.sobelType, 1);
		ImGui::SameLine();
		ImGui::RadioButton("XY", &settings.sobelType, 2);
		ImGui::SameLine();
		ImGui::RadioButton("MAG", &settings.sobelType, 3);
		ImGui::SameLine();
		ImGui::Text("Sobel type");

		ImGui::RadioButton("1##sobel", &settings.sobelSize, 1);
		ImGui::SameLine();
		ImGui::RadioButton("3##sobel", &settings.sobelSize, 3);
		ImGui::SameLine();
	    ImGui::RadioButton("5##sobel", &settings.sobelSize, 5);
		ImGui::SameLine();
		ImGui::RadioButton("7##sobel", &settings.sobelSize, 7);
		ImGui::SameLine();
		ImGui::Text("Sobel size");
		ImGui::SliderInt("Sobel derivative", &settings.sobelDerivative, 1, 5);

		ImGui::Separator();

		ImGui::TextColored(Colors::BLUE.iv4(), "Canny");
		ImGui::SliderFloat("Canny T1", &settings.cannyThreshold1, 0, settings.cannyThreshold2);
		ImGui::SliderFloat("Canny T2", &settings.cannyThreshold2, settings.cannyThreshold1, 1000);
		ImGui::RadioButton("3##canny", &settings.cannyAperture, 3);
		ImGui::SameLine();
		ImGui::RadioButton("5##canny", &settings.cannyAperture, 5);
		ImGui::SameLine();
		ImGui::RadioButton("7##canny", &settings.cannyAperture, 7);
		ImGui::SameLine();
		ImGui::Text("Canny aperture");
		ImGui::Checkbox("Canny L2 gradient", &settings.cannyL2gradient);

		ImGui::Separator();
		ImGui::TextColored(Colors::BLUE.iv4(), "Dilation");
		ImGui::SliderInt("Dilation", &settings.dilation, 1, 20);
	}

	if (ImGui::CollapsingHeader("Texture settings")) {
		// Postscale
		ImGui::DragFloat("Postscale", &settings.postscale, 0.01f, 0.05f, 1.5);

		// Rotations
		ImGui::DragInt("Rotations", &settings.rotations, 1, 1, 10);

		// Source to target pixel ratio
		ImGui::Text("Source to target pixel ratio: %.2f", settings.sourceToTargetPixelRatio);
		// Source mm2px ration
		ImGui::Text("Source px to mm ratio: %.2f", settings.sourceMillimeterToPixelRatio);
		// Target mm2px ratio
		ImGui::Text("Target px to mm ratio: %.2f", settings.targetMillimeterToPixelRatio);

		// Preferred patch range
		ImGui::NewLine();
		ImGui::Text("Preferred patch range");
		if (ImGui::DragIntRange2("##ppr", &settings.preferredPatchCountRange.x, &settings.preferredPatchCountRange.y, 1, 1, 1000)) {
			settings.validateTextureSettings(Settings::SettingValidation_ActualTargetDimension);
		}

		// Minimum patch dimension
		ImGui::Text("Minimum patch dimension");
		if (ImGui::DragFloat2("##mps", settings.minimumPatchDimension_mm.data, 1, 1, 1000)) {
			settings.validateTextureSettings(Settings::SettingValidation_ActualTargetDimension);
			settings.minimumPatchDimension_px = Vec2i(std::ceil(settings.tmm2px(settings.minimumPatchDimension_mm.x)), std::ceil(settings.tmm2px(settings.minimumPatchDimension_mm.y)));
		}
		ImGui::Text("Patch size = %d x %d px", settings.minimumPatchDimension_px.x, settings.minimumPatchDimension_px.y);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// Source texture
		if (sourceTexture.render(*settings.source)) {
			// Load new source and validate actual source dimension
			settings.originalSource = Texture(sourceTexture.path);
			settings.validateTextureSettings(Settings::SettingValidation_ActualSourceDimension);

			// Reload prescaled textures
			settings.reloadPrescaledTextures();

			// Notify pipeline and editor
			screen.pipeline.reload();
			screen.editor.reload();
		}

		// Actual source dimension
		ImGui::Text("Actual dimension");
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		if (ImGui::DragFloat("##asdx", &settings.actualSourceDimension_mm.x, 1.0, 1.0, 10000.0, "%.0f mm")) {
			settings.actualSourceDimension_mm.y = settings.validateTextureAspect(&settings.actualSourceDimension_mm.x, nullptr, settings.source->aspect());
			settings.validateTextureSettings(Settings::SettingValidation_ActualSourceDimension);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("X");
		ImGui::SameLine();
		if (ImGui::DragFloat("##asdy", &settings.actualSourceDimension_mm.y, 1.0, 1.0f, 10000.0f, "%.0f mm")) {
			settings.actualSourceDimension_mm.x = settings.validateTextureAspect(nullptr, &settings.actualSourceDimension_mm.y, settings.source->aspect());
			settings.validateTextureSettings(Settings::SettingValidation_ActualSourceDimension);
		}
		ImGui::PopItemWidth();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// Target texture
		if (targetTexture.render(*settings.target)) {
			// Load new target and validate actual target dimension
			settings.originalTarget = Texture(targetTexture.path);
			settings.validateTextureSettings(Settings::SettingValidation_ActualTargetDimension);

			// Reload prescaled textures
			settings.reloadPrescaledTextures();

			// Notify pipeline and editor
			screen.pipeline.reload();
			screen.editor.reload();
		}

		// Actual target dimension
		ImGui::Text("Preferred dimension");
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		if (ImGui::DragFloat("##ptdx", &settings.actualTargetDimension_mm.x, 1.0f, 1.0f, 10000.0f, "%.0f mm")) {
			settings.actualTargetDimension_mm.y = settings.validateTextureAspect(&settings.actualTargetDimension_mm.x, nullptr, settings.target->aspect());
			settings.validateTextureSettings(Settings::SettingValidation_ActualTargetDimension);
		}
		ImGui::SameLine();
		ImGui::Text("X");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::DragFloat("##ptdy", &settings.actualTargetDimension_mm.y, 1.0f, 1.0f, 10000.0f, "%.0f mm")) {
			settings.actualTargetDimension_mm.x = settings.validateTextureAspect(nullptr, &settings.actualTargetDimension_mm.y, settings.target->aspect());
			settings.validateTextureSettings(Settings::SettingValidation_ActualTargetDimension);
		}
		ImGui::PopItemWidth();

		// Source tooltip
		if (sourceTexture.hovered) {
			ImGui::BeginTooltip();

			/*for (std::size_t i = 0; i <= Feature::get.size(); i++) {
				if (i == Feature::get.size())
					ImGui::image("Original", settings.source->it());
				else
					ImGui::image(Feature::get[i]->name().c_str(), settings.source[i].it());
			}*/

			for (int i = 0; i < settings.source.textures.size(); i++) {
				ImVec2 size(Globals::imageWidth, Globals::imageWidth / settings.source.textures[i].aspect());
				ImGui::image(std::to_string(settings.source.textures[i].id).c_str(), settings.source.textures[i].it(), size);

				if (i != settings.source.textures.size() - 1)
					ImGui::SameLine();
			}

			if (!settings.source.features.empty()) {
				for (int f = 0; f < Feature::get.size(); f++) {
					for (int r = 0; r < settings.source.rotations; r++) {
						ImVec2 size(Globals::imageWidth, Globals::imageWidth / settings.source.features[r][f].aspect());
						ImGui::image(std::to_string(settings.source.features[r][f].id).c_str(), settings.source.features[r][f].it(), size);

						if (r != settings.source.textures.size() - 1)
							ImGui::SameLine();
					}
				}
			}

			for (int i = 0; i < settings.source.masks.size(); i++) {
				ImVec2 size(Globals::imageWidth, Globals::imageWidth / settings.source.masks[i].aspect());
				ImGui::image(std::to_string(360.0 / settings.source.rotations * i).c_str(), settings.source.masks[i].it(), size);

				if (i != settings.source.masks.size() - 1)
					ImGui::SameLine();

			}

			ImGui::EndTooltip();
		}

		// Target tooltip
		if (targetTexture.hovered) {
			ImGui::BeginTooltip();

			for (std::size_t i = 0; i <= Feature::get.size(); i++) {
				if (i == Feature::get.size())
					ImGui::image("Original", settings.target->it());
				else
					ImGui::image(Feature::get[i]->name().c_str(), settings.target[i].it());
			}

			ImGui::EndTooltip();
		}
	}

	ImGui::End();
}

#include "core.h"
#include "settingsView.h"

#include "main.h"
#include "screen.h"
#include "graphics/imgui/imguiUtils.h"
#include "graphics/imgui/texturepicker.h"
#include "graphics/imgui/widgets.h"
#include "imgui/imgui_notify.h"
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
		ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Reloading pipeline..." });
		settings.reloadPrescaledTextures();
		screen.pipeline.reload();
		screen.editor.reload();
		ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Done." });
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

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		bool blurChanged = false;
		blurChanged |= ImGui::RadioButton("X", &settings.sobelType, 0);
		ImGui::SameLine();
		blurChanged |= ImGui::RadioButton("Y", &settings.sobelType, 1);
		ImGui::SameLine();
		blurChanged |= ImGui::RadioButton("XY", &settings.sobelType, 2);
		ImGui::SameLine();
		ImGui::Text("Sobel type");

		blurChanged |= ImGui::RadioButton("1##sobel", &settings.sobelSize, 1);
		ImGui::SameLine();
		blurChanged |= ImGui::RadioButton("3##sobel", &settings.sobelSize, 3);
		ImGui::SameLine();
		blurChanged |= ImGui::RadioButton("5##sobel", &settings.sobelSize, 5);
		ImGui::SameLine();
		blurChanged |= ImGui::RadioButton("7##sobel", &settings.sobelSize, 7);
		ImGui::SameLine();
		ImGui::Text("Sobel size");
		blurChanged |= ImGui::SliderInt("Sobel derivative", &settings.sobelDerivative, 1, 5);

		ImGui::Separator();

		blurChanged |= ImGui::SliderFloat("Canny T1", &settings.cannyThreshold1, 0, settings.cannyThreshold2);
		blurChanged |= ImGui::SliderFloat("Canny T2", &settings.cannyThreshold2, settings.cannyThreshold1, 1000);
		blurChanged |= ImGui::RadioButton("3##canny", &settings.cannyAperture, 3);
		ImGui::SameLine();
		blurChanged |= ImGui::RadioButton("5##canny", &settings.cannyAperture, 5);
		ImGui::SameLine();
		blurChanged |= ImGui::RadioButton("7##canny", &settings.cannyAperture, 7);
		ImGui::SameLine();
		ImGui::Text("Canny aperture");
		blurChanged |= ImGui::Checkbox("Canny L2 gradient", &settings.cannyL2gradient);
	}

	if (ImGui::CollapsingHeader("Texture settings")) {
		// Postscale
		ImGui::DragFloat("Postscale", &settings.postscale, 0.1f, 0.1f, 1.5);

		// Source to target pixel ratio
		ImGui::Text("Source to target pixel ratio: %.2f", settings.sourceToTargetPixelRatio);

		// Preferred patch range
		ImGui::Text("Preferred patch range");
		if (ImGui::DragIntRange2("##ppr", &settings.preferredPatchCountRange.x, &settings.preferredPatchCountRange.y, 1, 1, 1000)) {
			settings.validateTextureSettings(Settings::SettingValidation_ActualTargetDimension);
		}

		// Minimum patch dimension
		ImGui::Text("Minimum patch dimension");
		if (ImGui::DragFloat2("##mps", settings.minimumPatchDimension_mm.data)) {
			settings.validateTextureSettings(Settings::SettingValidation_ActualTargetDimension);
		}

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

		// Source mm2px ration
		ImGui::Text("Source px to mm ratio: %.2f", settings.sourceMillimeterToPixelRatio);

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

		// Target mm2px ratio
		ImGui::Text("Target px to mm ratio: %.2f", settings.targetMillimeterToPixelRatio);

		// Source tooltip
		if (sourceTexture.hovered) {
			ImGui::BeginTooltip();

			for (std::size_t i = 0; i <= Feature::get.size(); i++) {
				/*for (const RotatedFeatureTexture& texture : settings.source.textures) {
					std::string name;
					ImTextureID id;
					if (i == Feature::get.size()) {
						name = std::format("{:.2f} deg", texture.angle / CV_PI * 180);
						id = texture.it();
					} else {
						name = "";
						id = texture.features.features[i].it();
					}

					ImGui::image(name.c_str(), id, ImVec2(100, 100));
					ImGui::SameLine();
				}

				std::string label;
				if (i == Feature::get.size()) {
					label = "Original";
				} else {
					label = Feature::get[i]->name();
				}
				ImGui::Text(label.c_str());
				ImGui::NewLine();*/

				if (i == Feature::get.size())
					ImGui::image("Original", settings.source->it());
				else
					ImGui::image(Feature::get[i]->name().c_str(), settings.source[i].it());
			}

			ImGui::EndTooltip();
		}
	}

	ImGui::End();
}

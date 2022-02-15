#include "core.h"
#include "settingsView.h"

#include "main.h"
#include "screen.h"
#include "graphics/imgui/texturepicker.h"
#include "graphics/imgui/widgets.h"
#include "util/stringUtil.h"

static ImGui::TexturePicker sourceTexture("Source texture");
static ImGui::TexturePicker targetTexture("Target texture");

SettingsView::SettingsView() = default;

void SettingsView::init() {

	sourceTexture.load(*settings.source);
	targetTexture.load(*settings.target);

	screen.editor.pipeline.reload();
	screen.editor.seedpoints.reload();
}

void SettingsView::update() {
}

void SettingsView::render() {
	ImGui::Begin("Settings");

	if (ImGui::CollapsingHeader("Global variables")) {
		ImGui::Text("Source hover: %s", screen.editor.seedpoints.source.hover ? "Yes" : "No");
		ImGui::Text("Source drag: %s", screen.editor.seedpoints.source.drag ? "Yes" : "No");
		ImGui::Text("Target hover: %s", screen.editor.seedpoints.target.hover ? "Yes" : "No");
		ImGui::Text("Target drag: %s", screen.editor.seedpoints.target.drag ? "Yes" : "No");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Intersected index: %d", screen.editor.seedpoints.intersectedIndex);
		ImGui::Text("Intersected point: %s", Util::str(screen.editor.seedpoints.intersectedPoint).c_str());
		ImGui::Text("Selected index: %d", screen.editor.seedpoints.intersectedIndex);
		ImGui::Text("Selected point: %s", Util::str(screen.editor.seedpoints.selectedPoint).c_str());
	}

	if (ImGui::CollapsingHeader("Feature settings")) {
		if (ImGui::SliderFloat("Intensity weight", &settings.intensityWeight, 0.0f, 1.0f)) {
			settings.edgeWeight = 1.0f - settings.intensityWeight;
		}
		if (ImGui::SliderFloat("Edge weight", &settings.edgeWeight, 0.0f, 1.0f)) {
			settings.intensityWeight = 1.0f - settings.edgeWeight;
		}
		if (ImGui::SliderFloat("Equalization weight", &settings.equalizationWeight, 0.0f, 1.0f)) {
			screen.editor.pipeline.onEqualizationWeightChanged(true);
		}

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

		if (blurChanged) {
			screen.editor.pipeline.onTargetBlurChanged(true);
			screen.editor.pipeline.onSourceBlurChanged(true);
		}
	}

	if (ImGui::CollapsingHeader("Texture settings")) {
		ImGui::Text("Rotations");
		if (ImGui::SliderInt("##Rotations", &settings.rotations, 1, 30)) {
			float interval = 360.0f / static_cast<float>(settings.rotations);
			float distanceFromFloor = fmod(settings.sourceRotation, interval);
			settings.sourceRotation -= distanceFromFloor < interval / 2.0f ? distanceFromFloor : -(interval - distanceFromFloor);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Preferred patch range");
		if (ImGui::DragIntRange2("##ppr", &settings.preferredPatchCountRange.x, &settings.preferredPatchCountRange.y, 1, 1)) {
			settings.validateTextureSettings(Settings::SettingValidation_PreferredTargetDimension);
		}
		ImGui::Text("Minimum patch dimension");
		if (ImGui::DragFloat2("##mps", settings.minimumPatchDimension.data)) {
			settings.validateTextureSettings(Settings::SettingValidation_PreferredTargetDimension);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Preferred target dimension");
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		if (ImGui::SliderFloat("##ptdx", &settings.preferredTargetDimension.x, 1.0, 10000, "%.0f mm")) {
			settings.preferredTargetDimension.y = settings.validateTextureAspect(&settings.preferredTargetDimension.x, nullptr, settings.target->aspect());
			settings.validateTextureSettings(Settings::SettingValidation_PreferredTargetDimension);
		}
		ImGui::SameLine();
		ImGui::Text("X");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::SliderFloat("##ptdy", &settings.preferredTargetDimension.y, 1.0, 10000, "%.0f mm")) {
			settings.preferredTargetDimension.x = settings.validateTextureAspect(nullptr, &settings.preferredTargetDimension.y, settings.target->aspect());
			settings.validateTextureSettings(Settings::SettingValidation_PreferredTargetDimension);
		}
		ImGui::PopItemWidth();
		if (targetTexture.render()) {
			settings.target = ExtendedTexture("Target", targetTexture.path);
			screen.editor.pipeline.onTargetChanged(true);
			screen.editor.seedpoints.onTargetChanged();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Actual source dimension");
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		if (ImGui::SliderFloat("##asdx", &settings.actualSourceDimension.x, 1.0, 10000, "%.0f mm")) {
			settings.actualSourceDimension.y = settings.validateTextureAspect(&settings.actualSourceDimension.x, nullptr, settings.source->aspect());
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("X");
		ImGui::SameLine();
		if (ImGui::SliderFloat("##asdy", &settings.actualSourceDimension.y, 1.0, 10000, "%.0f mm")) {
			settings.actualSourceDimension.x = settings.validateTextureAspect(nullptr, &settings.actualSourceDimension.y, settings.source->aspect());
		}
		ImGui::PopItemWidth();
		if (sourceTexture.render()) {
			settings.source = RotatedFeatureTextures(sourceTexture.path, settings.rotations);
			screen.editor.pipeline.onSourceChanged(true);
			screen.editor.seedpoints.onSourceChanged();
		}

		if (sourceTexture.hovered) {
			ImGui::BeginTooltip();

			for (const RotatedTexture& texture : settings.source.textures) {
				
			}

			for (std::size_t i = 0; i <= Feature::get.size(); i++) {
				for (const RotatedFeatureTexture& texture : settings.source.textures) {
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
				ImGui::NewLine();
			}

			ImGui::EndTooltip();
		}
	}

	ImGui::End();
}

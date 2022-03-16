#include <core.h>
#include "editor.h"
#include <format>
#include <algorithm>
#include <map>
#include <queue>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "main.h"
#include "screen.h"
#include "graphics/color.h"
#include "graphics/imgui/widgets.h"
#include "graphics/imgui/imguiUtils.h"
#include "omp.h"
#include "graphics/mondriaanPatch.h"

void EditorView::init() {
	generator = std::mt19937(std::random_device()());
	tspGenerationMethod = TSPGIndex_Jittered;
	sspGenerationMethod = SSPGIndex_TemplateMatch;

	patches.push_back(MondriaanPatch(Vec2(100, 100), Vec2(200, 200), Vec2(25, 25), 0));
}

void EditorView::update() {
	ImVec2 relativeOffset = ImGui::GetMousePos() - (source.hover ? source : target).offset;

	// Mouse move
	if (source.hover || target.hover) {
		intersectedIndex = -1;
		intersectedPoint = relativeOffset;

		for (int index = 0; index < patches.size(); index++) {
			MondriaanPatch& patch = patches[index];

			if (source.hover && patch.sourceBounds().contains(source.toTextureSpace(intersectedPoint))) {
				intersectedIndex = index;
				break;
			}

			// TODO rotation
			if (target.hover && patch.targetBounds().contains(target.toTextureSpace(intersectedPoint))) {
				intersectedIndex = index;
				break;
			}
		}
	} else {
		source.drag = false;
		target.drag = false;
	}

	// Mouse press
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		if (source.hover || target.hover) {
			selectedPoint = intersectedPoint;
			selectedIndex = intersectedIndex;

			if (target.hover)
				target.drag = true;

			if (source.hover)
				source.drag = true;
		}
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && (source.hover || target.hover)) {
		patches.emplace_back(source.toTextureSpace(relativeOffset), target.toTextureSpace(relativeOffset), settings.minimumPatchDimension_mm, 0);
	}

	// Mouse drag
	if ((source.drag || target.drag) && selectedIndex != -1) {
		Vec2 delta = relativeOffset - selectedPoint;
		if (source.drag)
			patches[selectedIndex].sourceOffset += source.toTextureSpace(delta);

		if (target.drag)
			patches[selectedIndex].targetOffset += target.toTextureSpace(delta);

		selectedPoint += delta;
	}

	// Mouse release
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		target.drag = false;
		source.drag = false;
	}
}

void EditorView::renderCursor() {
	// Cursor
	if (source.hover || target.hover) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		double size = 7;
		float thickness = 3;
		Vec2 cursor = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddLine(cursor - ImVec2(static_cast<float>(size), 0),
		                                    cursor + ImVec2(static_cast<float>(size), 0),
		                                    Colors::WHITE.u32(),
		                                    thickness);
		ImGui::GetWindowDrawList()->AddLine(cursor - ImVec2(0, static_cast<float>(size)),
		                                    cursor + ImVec2(0, static_cast<float>(size)),
		                                    Colors::WHITE.u32(),
		                                    thickness);

		if (source.hover) {
			ImGui::GetWindowDrawList()->AddLine(Vec2(source.minX(), cursor.y).iv(),
			                                    Vec2(source.minX() + size, cursor.y).iv(),
			                                    Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(source.maxX(), cursor.y).iv(),
			                                    Vec2(source.maxX() - size, cursor.y).iv(),
			                                    Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(cursor.x, source.minY()).iv(),
			                                    Vec2(cursor.x, source.minY() + size).iv(),
			                                    Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(cursor.x, source.maxY()).iv(),
			                                    Vec2(cursor.x, source.maxY() - size).iv(),
			                                    Colors::WHITE.u32(),
			                                    thickness);
		}

		if (target.hover) {
			ImGui::GetWindowDrawList()->AddLine(Vec2(target.minX(), cursor.y).iv(),
			                                    Vec2(target.minX() + size, cursor.y).iv(),
			                                    Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(target.maxX(), cursor.y).iv(),
			                                    Vec2(target.maxX() - size, cursor.y).iv(),
			                                    Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(cursor.x, target.minY()).iv(),
			                                    Vec2(cursor.x, target.minY() + size).iv(),
			                                    Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(cursor.x, target.maxY()).iv(),
			                                    Vec2(cursor.x, target.maxY() - size).iv(),
			                                    Colors::WHITE.u32(),
			                                    thickness);
		}
	}
}

void EditorView::renderTooltip() {
	// Tooltip
	if (intersectedIndex != -1 || selectedIndex != -1) {
		const MondriaanPatch& patch = patches[intersectedIndex != -1 ? intersectedIndex : selectedIndex];
		auto tooltipPosition = intersectedIndex != -1 ? ImGui::GetCursorPos() : source.min() + patch.sourceOffset;

		Bounds sourceUV = patch.sourceUV();
		Bounds targetUV = patch.targetUV();

		ImVec2 restore = ImGui::GetCursorPos();

		ImGui::SetCursorPos(tooltipPosition.iv());
		ImGui::SetNextWindowSize(ImVec2(190, 0));
		ImGui::BeginTooltip();

		ImGui::TextColored(Colors::BLUE.iv4(), "General");
		ImGui::Separator();
		ImGui::Text("Width: %.2f", patch.dimension_mm.x);
		ImGui::Text("Height: %.2f", patch.dimension_mm.y);
		ImGui::Text("Rotation: %.2f", patch.sourceRotation);
		ImGui::Separator();

		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Text("X: %.0f", patch.sourceOffset.x);
		ImGui::Text("Y: %.0f", patch.sourceOffset.y);
		ImGui::image("Source", settings.source->it(), ImVec2(80, 80), sourceUV.min().iv(), sourceUV.max().iv());

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Text("X: %.0f", patch.targetOffset.x);
		ImGui::Text("Y: %.0f", patch.targetOffset.y);
		ImGui::image("Target", settings.target->it(), ImVec2(80, 80), targetUV.min().iv(), targetUV.max().iv());

		ImGui::Columns(1);

		ImGui::EndTooltip();

		ImGui::SetCursorPos(restore);
	}
}

void EditorView::renderPatchViewer() {
	// Seedpoint viewer
	ImGui::Begin("Patch viewer");
	for (int index = 0; index < patches.size(); index++) {
		const MondriaanPatch& patch = patches[index];
		auto sourceScreenPosition = source.toAbsoluteScreenSpace(patch.sourceOffset);
		auto targetScreenPosition = target.toAbsoluteScreenSpace(patch.targetOffset);
		Bounds sourceUV = patch.sourceUV();
		Bounds targetUV = patch.targetUV();

		float size = 80.0f;
		ImGui::TextColored(Colors::BLUE.iv4(), "Patch #%d", index + 1);
		ImGui::Separator();
		ImGui::Text("Width: %.2f", patch.dimension_mm.x);
		ImGui::Text("Height: %.2f", patch.dimension_mm.y);
		ImGui::Text("Rotation: %.2f", patch.sourceRotation);

		ImGui::Columns(4, 0, false);
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Image(settings.source->it(), ImVec2(size, size), sourceUV.min().iv(), sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::Text("X: %.0f", patch.sourceOffset.x);
		ImGui::Text("Y: %.0f", patch.sourceOffset.y);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Image(settings.target->it(), ImVec2(size, size), targetUV.min().iv(), targetUV.max().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Text("");
		ImGui::Text("X: %.0f", patch.targetOffset.x);
		ImGui::Text("Y: %.0f", patch.targetOffset.y);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);

		ImGui::Image(settings.source[FeatureIndex_Intensity].it(), ImVec2(size, size), sourceUV.min().iv(), sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Image(settings.source[FeatureIndex_Edge].it(), ImVec2(size, size), sourceUV.min().iv(), sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Image(settings.target[FeatureIndex_Intensity].it(), ImVec2(size, size), targetUV.min().iv(), targetUV.max().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Image(settings.target[FeatureIndex_Edge].it(), ImVec2(size, size), targetUV.min().iv(), targetUV.max().iv());
		ImGui::NextColumn();

		ImGui::Columns(1);

		ImGui::Separator();
	}
	ImGui::End();
}

void EditorView::renderPatches() {
	// Render seedpoints
	for (int index = 0; index < patches.size(); index++)
		patches[index].render(source,
		                      target,
		                      intersectedIndex == index,
		                      selectedIndex == index,
		                      showConnections,
		                      Color::blend(Colors::YELLOW, Colors::RGB_R, static_cast<float>(index + 1) / static_cast<float>(patches.size())));
}

void EditorView::renderTextures() {
	ImGui::Columns(3, nullptr, false);
	ImGui::SetColumnWidth(0, source.dimension.iv().x + 20);
	ImGui::SetColumnWidth(1, target.dimension.iv().x + 20);
	ImGui::SetColumnWidth(2, target.dimension.iv().x + 20);

	// Source image
	ImGui::image("Source", settings.source->it(), source.dimension.iv());
	source.offset = ImGui::GetItemRectMin();
	source.hover = ImGui::IsItemHovered();

	ImGui::NextColumn();

	// Target image
	ImGui::image("Target", settings.target->it(), target.dimension.iv());
	target.offset = ImGui::GetItemRectMin();
	target.hover = ImGui::IsItemHovered();

	ImGui::NextColumn();

	// Mask
	ImGui::image("Mask", settings.mask.it(), source.dimension.iv());

	ImGui::NextColumn();
	ImGui::Columns(1);

	// Overlays
	TSPG::get[tspGenerationMethod]->renderOverlay(source, target);
	TSPG::get[sspGenerationMethod]->renderOverlay(source, target);

	// Bounding boxes
	ImGui::GetWindowDrawList()->AddRect(source.min().iv(), source.max().iv(), Colors::WHITE.u32(), 0, ImDrawCornerFlags_All, 2);
	ImGui::GetWindowDrawList()->AddRect(target.min().iv(), target.max().iv(), Colors::WHITE.u32(), 0, ImDrawCornerFlags_All, 2);
}

void EditorView::renderSettings() {
	float height = 30.0f;

	ImGui::Columns(3, nullptr, false);
	ImGui::SetColumnWidth(0, source.dimension.iv().x + 20);
	ImGui::SetColumnWidth(1, target.dimension.iv().x + 20);
	ImGui::SetColumnWidth(2, target.dimension.iv().x + 20);

	ImGuiUtils::pushButtonColor(0.0);
	if (ImGui::Button("Delete patches", ImVec2(source.dimension.x, height)))
		patches.clear();
	ImGuiUtils::popButtonColor();

	ImGuiUtils::pushButtonColor(0.28);
	if (ImGui::Button("Move source patches", ImVec2(source.dimension.x, height)))
		mutateSourcePatches();
	ImGuiUtils::popButtonColor();

	ImGui::Checkbox("Show connections", &showConnections);
	if (ImGui::Button("Generate image"))
		generateImage();

	ImGui::NextColumn();

	ImGuiUtils::pushButtonColor(0.0);
	if (ImGui::Button("Delete selected patch", ImVec2(target.dimension.x, height)))
		if (selectedIndex != -1)
			patches.erase(patches.begin() + selectedIndex);
	ImGuiUtils::popButtonColor();

	ImGuiUtils::pushButtonColor(0.28);
	if(ImGui::Button("Spawn target patches", ImVec2(target.dimension.x, height)))
		spawnTargetPatches();
	ImGuiUtils::popButtonColor();

	ImGui::NextColumn();

	// TSP generation method
	ImGui::TextColored(Colors::BLUE.iv4(), "Target seedpoints");
	{
		static std::array methods = { "Jittered", "Greedy" };

		ImGui::Combo("TSPG method", &tspGenerationMethod, methods.data(), methods.size());

		TSPG::get[tspGenerationMethod]->renderSettings(source, target);
	}


	// SSP generation method
	ImGui::TextColored(Colors::BLUE.iv4(), "Source seedpoints");
	{
		static std::array methods = { "Random", "Template Match", "SIFT" };

		ImGui::Combo("SSPG method", &sspGenerationMethod, methods.data(), methods.size());

		SSPG::get[sspGenerationMethod]->renderSettings(source, target);
	}

	ImGui::NextColumn();
	ImGui::Columns(1);
}

void EditorView::render() {
	ImGui::Begin("SeedPoints", 0, ImGuiWindowFlags_AlwaysAutoResize);

	renderTextures();
	renderPatches();
	renderSettings();
	renderCursor();
	renderTooltip();

	ImGui::End();

	renderPatchViewer();
}

void EditorView::spawnTargetPatches() {
	patches.clear();
	resetSelection();

	this->patches = TSPG::get[tspGenerationMethod]->generate();
}

void EditorView::mutateSourcePatches() {
	SSPG::get[sspGenerationMethod]->mutate(patches);
}

std::size_t mutations = 3;
std::vector indices = {0, 1, 2, 3};
int EditorView::checkPatch(MondriaanPatch* newPatch, MondriaanPatch* oldPatch) {
	/*for (MondriaanPatch& patch : patches) {
		if (&patch == oldPatch)
			continue;

		if (newPatch->sourceBounds().ir().Overlaps(patch->sourceBounds().ir()))
			return 1;

		if (newPatch->targetBounds().ir().Overlaps(patch->targetBounds().ir()))
			return 2;

		if (!source.textureBounds().ir().Contains(newPatch->sourceBounds().ir()))
			return 3;

		if (!target.textureBounds().ir().Contains(newPatch->targetBounds().ir()))
			return 4;
	}*/

	return 0;
}

void EditorView::mutatePatches() {
	//std::vector sourceTextures = {screen.editor.pipeline.sourceGrayscaleE->data, screen.editor.pipeline.sourceSobel->data};
	//std::vector targetTextures = {screen.editor.pipeline.wequalizedE->data, screen.editor.pipeline.targetSobel->data};
	//std::vector<double> distribution = {settings.intensityWeight, settings.edgeWeight};

	//for (const SeedPoint& seedPoint : seedPoints) {
	//	MondriaanPatch* patch = reinterpret_cast<MondriaanPatch*>(seedPoint.patch.get());
	//	if (patch == nullptr)
	//		continue;

	//	//double currentDistance = Match(patch, sourceTextures, targetTextures, distribution).distance;

	//	int bestMutation = -1;
	//	double bestDistance = std::numeric_limits<double>::max();
	//	std::ranges::shuffle(indices, generator);
	//	for (std::size_t index = 0; index < mutations; index++) {
	//		MondriaanPatch newPatch = patch->getMutation(indices[index], 5.0);

	//		std::map<int, const char*> d = {
	//		{MondriaanPatch::BOTTOM, "Bottom"},
	//		{MondriaanPatch::LEFT, "Left"},
	//		{MondriaanPatch::RIGHT, "Right"},
	//		{MondriaanPatch::TOP, "Top"}
	//		};
	//		int error = checkPatch(&newPatch, patch);
	//		if (error != 0) {
	//			Log::debug("(%f, %f) %s: %d", seedPoint.sourcePosition.x, seedPoint.sourcePosition.y, d[indices[index]], error);
	//			continue;
	//		}

	//		double newDistance = Match(patch, sourceTextures, targetTextures, distribution).distance;

	//		if (newDistance < bestDistance) {
	//			bestDistance = newDistance;
	//			bestMutation = indices[index];
	//		}
	//	}

	//	if (bestMutation == -1) {
	//		continue;
	//	}

	//	patch->mutate(bestMutation, 1.0);
	//}
}

//void EditorView::generateImage() {
//	int cols = target.texture->data.cols / settings.minimumPatchDimension.x;
//	int rows = target.texture->data.rows / settings.minimumPatchDimension.y;
//
//	cv::Mat output(target.texture->data.rows, target.texture->data.cols, target.texture->data.type());
//	cv::Mat sourceSobel = source.features[Canvas::FEATURE_SOBEL]->data;
//	cv::Mat sourceInt = source.features[Canvas::FEATURE_INT]->data;
//#pragma omp parallel for
//	for (int row = 0; row < rows; row++) {
//#pragma omp parallel for
//		for (int col = 0; col < cols; col++) {
//			cv::Rect rect(col * settings.minimumPatchDimension.x,
//			              row * settings.minimumPatchDimension.y,
//			              settings.minimumPatchDimension.x,
//			              settings.minimumPatchDimension.y);
//
//			cv::Mat targetSobel = target.features[Canvas::FEATURE_SOBEL]->data(rect);
//			cv::Mat targetInt = target.features[Canvas::FEATURE_INT]->data(rect);
//
//			cv::Mat sobelMatch;
//			cv::matchTemplate(sourceSobel, targetSobel, sobelMatch, cv::TM_CCORR);
//			cv::normalize(sourceSobel, sourceSobel, 1.0, 0.0, cv::NORM_MINMAX);
//			cv::Mat intMatch;
//			cv::matchTemplate(sourceSobel, targetSobel, intMatch, cv::TM_CCORR);
//			cv::normalize(intMatch, intMatch, 1.0, 0.0, cv::NORM_MINMAX);
//			cv::Mat featureMatch = settings.edgeWeight * sobelMatch + settings.intensityWeight * intMatch;
//			cv::Point matchLoc;
//			cv::minMaxLoc(featureMatch, nullptr, nullptr, nullptr, &matchLoc);
//
//			//cv::copyTo(target.texture->data(rect), output(rect), cv::Mat());
//			cv::copyTo(source.texture->data(cv::Rect(matchLoc.x, matchLoc.y, settings.minimumPatchDimension.x, settings.minimumPatchDimension.y)),
//			           output(rect),
//			           cv::Mat());
//		}
//	}
//
//	cv::imshow("Output", output);
//}

void EditorView::generateImage() {
	cv::Mat outputTarget(settings.target->rows(), settings.target->cols(), settings.target->data.type(), cv::Scalar(0));
	cv::Mat outputSource(settings.target->rows(), settings.target->cols(), settings.source->data.type(), cv::Scalar(0));

	for (MondriaanPatch& patch : patches) {
		Vec2i alignedPatchDimension(settings.smm2px(patch.dimension_mm));

		cv::Mat whiteMask = cv::Mat(alignedPatchDimension.cv(), CV_8UC1, cv::Scalar(255));

		// Create rotated and aligned patch rectangles in source and target
		cv::Rect rotatedSourceRect = cv::Rect(patch.sourceOffset.x, patch.sourceOffset.y, patch.mask.cols, patch.mask.rows);
		cv::Mat rotatedSourcePatch = settings.source->data(rotatedSourceRect);

		cv::Rect targetRect = cv::Rect(patch.targetOffset.x, patch.targetOffset.y, alignedPatchDimension.x, alignedPatchDimension.y);
		cv::Mat targetPatch = settings.target->data(targetRect);

		// Rotate source patch back to alignment
		cv::Mat alignedSourcePatch;
		cv::Mat transformationMatrix = RotatedTexture::computeTransformationMatrix(alignedPatchDimension.cv(), -patch.sourceRotation);
		cv::warpAffine(rotatedSourcePatch, alignedSourcePatch, transformationMatrix, alignedPatchDimension.cv());

		/*cv::imshow("mask", whiteMask);
		cv::imshow("rotatedSourcePatch", rotatedSourcePatch);
		cv::imshow("alignedSourcePatch", alignedSourcePatch);
		cv::imshow("targetPatch", targetPatch);
		cv::waitKey();*/

		cv::copyTo(targetPatch, outputTarget(targetRect), whiteMask);
		cv::copyTo(alignedSourcePatch, outputSource(targetRect), whiteMask);
	}

	cv::imshow("Output Target", outputTarget);
	cv::imshow("Output Source", outputSource);
}

void EditorView::reload() {
	// Calculate source dimensions
	source.aspect = settings.source->aspect();
	source.tdimension = settings.source->dimension();
	source.dimension = Canvas::computeDimension(source.tdimension, 350);

	// Calculate target dimensions
	target.aspect = settings.target->aspect();
	target.tdimension = settings.target->dimension();
	target.dimension = Canvas::computeDimension(target.tdimension, 350);
}

void EditorView::resetSelection() {
	selectedIndex = -1;
	selectedPoint = Vec2f();
	intersectedIndex = -1;
	intersectedPoint = Vec2f();
}

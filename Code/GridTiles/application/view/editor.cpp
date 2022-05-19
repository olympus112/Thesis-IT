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
#include "fade2D/Fade_2D.h"

void EditorView::init() {
	generator = std::mt19937(std::random_device()());
	tspGenerationMethod = TSPGIndex_Jittered;
	sspGenerationMethod = SSPGIndex_TemplateMatch;

	reload();

	grid.add(Vec2(100, 100), Vec2(200, 200), Vec2(25, 25), 25);
	grid[0].computeTransformationMatrix();

	nMutations = 2;
	nMutatedPatches = Utils::min<int>(10, grid.size());
	metric = 0;
	nSplits = 4;
}

void EditorView::update() {
	ImVec2 relativeOffset = ImGui::GetMousePos() - (source.hover ? source : target).offset;

	// Mouse move
	if (source.hover || target.hover) {
		intersectedIndex = -1;
		intersectedPoint = relativeOffset;

		for (int index = 0; index < grid.size(); index++) {
			MondriaanPatch& patch = grid[index];

			Bounds sourceBounds = patch.sourceRotatedBounds();
			Bounds targetBounds = patch.targetBounds();

			Vec2 sourceIntersection = source.toTextureSpace(intersectedPoint);
			Vec2 targetIntersection = target.toTextureSpace(intersectedPoint);

			if (source.hover && sourceBounds.econtains(sourceIntersection)) {
				intersectedIndex = index;
				break;
			}

			if (target.hover && targetBounds.econtains(targetIntersection)) {
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
		grid.add(source.toTextureSpace(relativeOffset), target.toTextureSpace(relativeOffset), settings.minimumPatchDimension_mm, 0);
	}

	// Mouse drag
	if ((source.drag || target.drag) && selectedIndex != -1) {
		Vec2 delta = relativeOffset - selectedPoint;
		if (source.drag) {
			Vec2 sourceDelta = source.toTextureSpace(delta);
			Bounds oldSourceBounds = grid[selectedIndex].sourceRotatedBounds();
			grid[selectedIndex].sourceOffset += sourceDelta;
			// Check location
			if (checkPatchSourceLocation(grid[selectedIndex])) {
				Bounds newSourceBounds = grid[selectedIndex].sourceRotatedBounds();
				grid.update(selectedIndex, oldSourceBounds, newSourceBounds, Type_Source);
			} else {
				// Undo change
				grid[selectedIndex].sourceOffset -= sourceDelta;
			}
		}

		if (target.drag) {
			Vec2 targetDelta = target.toTextureSpace(delta);
			Bounds oldTargetBounds = grid[selectedIndex].targetBounds();
			grid[selectedIndex].targetOffset += targetDelta;
			// Check location
			if (checkPatchTargetLocation(grid[selectedIndex])) {
				Bounds newTargetBounds = grid[selectedIndex].targetBounds();
				grid.update(selectedIndex, oldTargetBounds, newTargetBounds, Type_Target);
			} else {
				// Undo change
				grid[selectedIndex].targetOffset -= targetDelta;
			}
		}

		selectedPoint = relativeOffset;
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
		const MondriaanPatch& patch = grid[intersectedIndex != -1 ? intersectedIndex : selectedIndex];
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
		ImGui::Text("W: %d", patch.mask.cols);
		ImGui::Text("H: %d", patch.mask.rows);
		ImGui::image("Source", settings.source->it(), ImVec2(80, 80), sourceUV.min().iv(), sourceUV.emax().iv());

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Text("X: %.0f", patch.targetOffset.x);
		ImGui::Text("Y: %.0f", patch.targetOffset.y);
		ImGui::Text("W: %.0f", patch.targetDimension().x);
		ImGui::Text("H: %.0f", patch.targetDimension().y);
		ImGui::image("Target", settings.target->it(), ImVec2(80, 80), targetUV.min().iv(), targetUV.emax().iv());

		ImGui::Columns(1);

		ImGui::EndTooltip();

		ImGui::SetCursorPos(restore);
	}
}

void EditorView::renderPatchViewer() {
	// Seedpoint viewer
	ImGui::Begin("Patch viewer");
	ImGui::Text("# Patches: %d", grid.size());

	for (int index = 0; index < grid.size(); index++) {
		ImGui::PushID(index);
		MondriaanPatch& patch = grid[index];

		Vec2 sourceRotatedDimension = patch.sourceRotatedDimension2f();
		Bounds rotatedSourceUV = patch.sourceRotatedUV();
		Bounds targetUV = patch.targetUV();

		float size = 80.0f;
		ImVec2 sourceSize = Canvas::computeDimension(sourceRotatedDimension, size).iv();
		ImVec2 targetSize = Canvas::computeDimension(patch.targetDimension(), size).iv();

		ImGui::TextColored(Colors::BLUE.iv4(), "Patch #%d", index + 1);
		ImGui::Separator();
		ImGui::DragFloat("Width", &patch.dimension_mm.x, 0.1f, 0.1f, 50.0f, "%.2f mm");
		ImGui::DragFloat("Height", &patch.dimension_mm.y, 0.1f, 0.1f, 50.0f, "%.2f mm");
		if (ImGui::DragFloat("Rotation", &patch.sourceRotation, 1.0f, 0.0f, 360.0f, "%.2f")) {
			patch.computeMask();
		}
		ImGui::Text("Match: %.6f", static_cast<float>(patch.match));

		if (ImGui::Button("Show mask"))
			cv::imshow("Mask", patch.mask);
		ImGui::SameLine();
		if (ImGui::Button("Reload mask"))
			patch.mask = Mask(patch.sourceDimension(), patch.sourceRotation).data;
		if (ImGui::Button("Add to global mask")) {
			patch.addToGlobalMask();
			settings.mask.reloadGL();
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove from global mask")) {
			patch.removeFromGlobalMask();
			settings.mask.reloadGL();
		}
		if (ImGui::Button("Reload match")) {
			patch.computeTransformationMatrix();
			patch.computeMatch();
		}

		ImGui::Columns(4, 0, false);
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");

		auto drawRotatedQuad = [&patch, &sourceSize, &sourceRotatedDimension](const ImVec2& offset) {
			std::vector<Vec2> points = patch.sourceRotatedPointsRelative2f();

			ImGui::GetWindowDrawList()->AddQuad(offset + (Utils::transform(points[0], sourceRotatedDimension, Vec2(sourceSize))).iv(),
			                                    offset + (Utils::transform(points[1], sourceRotatedDimension, Vec2(sourceSize))).iv(),
			                                    offset + (Utils::transform(points[2], sourceRotatedDimension, Vec2(sourceSize))).iv(),
			                                    offset + (Utils::transform(points[3], sourceRotatedDimension, Vec2(sourceSize))).iv(),
			                                    Colors::RGB_R.u32());
		};

		ImVec2 sourcePos = ImGui::GetCursorScreenPos();
		ImGuiUtils::ImageNoSpacing(settings.source->it(), sourceSize, rotatedSourceUV.min().iv(), rotatedSourceUV.emax().iv());
		drawRotatedQuad(sourcePos);

		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::DragFloat("X##sourceX", &patch.sourceOffset.x, 1.0f, 0.0f, settings.source->dimension().x, "%.1f");
		ImGui::DragFloat("Y##sourceY", &patch.sourceOffset.y, 1.0f, 0.0f, settings.source->dimension().y, "%.1f");

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGuiUtils::ImageNoSpacing(settings.target->it(), targetSize, targetUV.min().iv(), targetUV.emax().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Text("");
		ImGui::DragFloat("X##targetX", &patch.targetOffset.x, 1.0f, 0.0f, settings.target->dimension().x, "%.1f");
		ImGui::DragFloat("Y##targetY", &patch.targetOffset.y, 1.0f, 0.0f, settings.target->dimension().y, "%.1f");

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);

		ImVec2 intensityPos = ImGui::GetCursorScreenPos();
		ImGuiUtils::ImageNoSpacing(settings.source[FeatureIndex_Intensity].it(), sourceSize, rotatedSourceUV.min().iv(), rotatedSourceUV.emax().iv());
		drawRotatedQuad(intensityPos);
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImVec2 edgePos = ImGui::GetCursorScreenPos();
		ImGuiUtils::ImageNoSpacing(settings.source[FeatureIndex_Edge].it(), sourceSize, rotatedSourceUV.min().iv(), rotatedSourceUV.emax().iv());
		drawRotatedQuad(edgePos);
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGuiUtils::ImageNoSpacing(settings.target[FeatureIndex_Intensity].it(), targetSize, targetUV.min().iv(), targetUV.emax().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGuiUtils::ImageNoSpacing(settings.target[FeatureIndex_Edge].it(), targetSize, targetUV.min().iv(), targetUV.emax().iv());
		ImGui::NextColumn();

		ImGui::Columns(1);

		ImGui::Separator();
		ImGui::PopID();
	}
	ImGui::End();
}

void EditorView::renderPatches() {
	std::set<std::size_t> targetNeighbours;
	std::set<std::size_t> sourceNeighbours;
	if (selectedIndex != -1) {
		targetNeighbours = grid.neighbours(selectedIndex, Type_Target);
		sourceNeighbours = grid.neighbours(selectedIndex, Type_Source);
	}

	// Render seedpoints
	for (int patchIndex = 0; patchIndex < grid.size(); patchIndex++) {
		Color targetColor = Color::blend(Colors::YELLOW, Colors::RED, static_cast<float>(patchIndex + 1) / static_cast<float>(grid.size()));
		Color sourceColor = targetColor;

		if (targetNeighbours.find(patchIndex) != targetNeighbours.end())
			targetColor = Colors::RGB_B;

		if (sourceNeighbours.find(patchIndex) != sourceNeighbours.end())
			sourceColor = Colors::RGB_B;

		if (grid.overlaps(grid[patchIndex], Type_Target))
			targetColor = Colors::RGB_G;

		if (grid.overlaps(grid[patchIndex], Type_Source))
			sourceColor = Colors::RGB_G;

		grid[patchIndex].render(source,
		                        target,
		                        intersectedIndex == patchIndex,
		                        selectedIndex == patchIndex,
		                        showConnections,
		                        sourceColor,
		                        targetColor);
	}

}

void EditorView::renderSettings() {
	float height = 30.0f;

	ImGui::Columns(3, nullptr, false);
	ImGui::SetColumnWidth(0, source.dimension.iv().x + 20);
	ImGui::SetColumnWidth(1, target.dimension.iv().x + 20);
	ImGui::SetColumnWidth(2, target.dimension.iv().x + 20);

	//
	// Column 1
	//

	// Source image
	ImGui::image("Source", settings.source->it(), source.dimension.iv());
	source.offset = ImGui::GetItemRectMin();
	source.hover = ImGui::IsItemHovered();

	// Target image
	ImGui::image("Target", settings.target->it(), target.dimension.iv());
	target.offset = ImGui::GetItemRectMin();
	target.hover = ImGui::IsItemHovered();

	// Clear mask
	if (ImGui::Button("Clear mask", ImVec2(target.dimension.x, height))) {
		settings.mask.data = cv::Mat(settings.mask.rows(), settings.mask.cols(), CV_8UC1, cv::Scalar(255));
		settings.mask.reloadGL();
	}

	// Delete patches
	ImGuiUtils::pushButtonColor(0.0);
	if (ImGui::Button("Delete patches", ImVec2(source.dimension.x, height))) {
		grid.clear();
		resetSelection();
	}
	ImGuiUtils::popButtonColor();

	// Move and mutate
	ImGuiUtils::pushButtonColor(0.28);
	if (ImGui::Button("Move source patches", ImVec2(source.dimension.x, height)))
		mutateSourcePatches();
	ImGuiUtils::popButtonColor();
	if (ImGui::ButtonEx("Mutate", ImVec2(source.dimension.x, height), ImGuiButtonFlags_Repeat))
		mutatePatches();
	if (ImGui::ButtonEx("Split", ImVec2(source.dimension.x, height), ImGuiButtonFlags_Repeat))
		splitPatches();

	// Show settings
	ImGui::Checkbox("Show connections", &showConnections);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &showRegularGrid);

	// Generate image
	if (ImGui::Button("Generate image"))
		generateImage();
	if (ImGui::Button("Generate regular matching"))
		pool.push_task([this] { generateRegularPatches(); });


	//
	// Column 2
	//
	ImGui::NextColumn();

	// Mask
	ImGui::image("Mask", settings.mask.it(), source.dimension.iv());

	// Puzzle
	ImGui::image("Puzzle", settings.puzzle.it(), target.dimension.iv());

	// Reload mask
	if (ImGui::Button("Reload mask", ImVec2(target.dimension.x, height))) {
		settings.mask.data = cv::Mat(settings.mask.rows(), settings.mask.cols(), CV_8UC1, cv::Scalar(255));
		for (const MondriaanPatch& patch : grid.patches) {
			patch.addToGlobalMask();
		}
		settings.mask.reloadGL();
	}

	// Delete patch
	ImGuiUtils::pushButtonColor(0.0);
	if (ImGui::Button("Delete selected patch", ImVec2(target.dimension.x, height))) {
		if (selectedIndex != -1)
			grid.erase(selectedIndex);

		resetSelection();
	}
	ImGuiUtils::popButtonColor();

	// Spawning
	ImGuiUtils::pushButtonColor(0.28);
	if (ImGui::Button("Spawn target patches", ImVec2(target.dimension.x, height)))
		spawnTargetPatches();
	ImGuiUtils::popButtonColor();

	if (ImGui::Button("Spawn new patch", ImVec2(target.dimension.x, height)))
		spawnNewPatch();

	if (ImGui::Button("Spawn big patch", ImVec2(target.dimension.x, height))) {
		grid.clear();
		resetSelection();
		grid.add(MondriaanPatch(Vec2(), Vec2(), settings.spx2mm(settings.target->dimension() - Vec2(1, 1)), 0));
	}

	//
	// Column 3
	//
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

	// Mutations
	{
		ImGui::TextColored(Colors::BLUE.iv4(), "Mutations");
		ImGui::DragInt("# mutations", &nMutations, 1, 1, MondriaanPatch::mutations - 1);
		ImGui::DragInt("# mutated patches", &nMutatedPatches, 1, 1, grid.size());
		static std::array<const char*, 6> metrics = { "SQDIFF", "SQDIFF_NORMED", "CCORR", "CCORR_NORMED", "CCOEFF", "CCOEFF_NORMED", };
		ImGui::Combo("Distance metric##Mutations", &metric, metrics.data(), metrics.size());
	}

	// Splits
	{
		ImGui::TextColored(Colors::BLUE.iv4(), "Splits");
		ImGui::DragInt("# Splits", &nSplits, 1, 1, MondriaanPatch::mutations - 1);
	}

	//
	// End column
	//
	ImGui::NextColumn();
	ImGui::Columns(1);

	// Overlays
	TSPG::get[tspGenerationMethod]->renderOverlay(source, target);
	TSPG::get[sspGenerationMethod]->renderOverlay(source, target);

	// Bounding boxes
	ImGui::GetWindowDrawList()->AddRect(source.min().iv(), source.max().iv(), Colors::WHITE.u32(), 0, ImDrawCornerFlags_All, 2);
	ImGui::GetWindowDrawList()->AddRect(target.min().iv(), target.max().iv(), Colors::WHITE.u32(), 0, ImDrawCornerFlags_All, 2);
}

void EditorView::render() {
	ImGui::Begin("SeedPoints", 0, ImGuiWindowFlags_AlwaysAutoResize);

	//renderTextures();
	renderSettings();
	renderPatches();
	renderCursor();
	renderTooltip();
	renderVoronoi();

	if (showRegularGrid) {
		grid.render(target, Type_Target);
		grid.render(source, Type_Source);
	}

	ImGui::End();

	renderPatchViewer();
}

void EditorView::spawnTargetPatches() {
	grid.clear();
	resetSelection();

	this->grid = RegularGrid<10>(TSPG::get[tspGenerationMethod]->generate(), source.tdimension, target.tdimension);
}

void EditorView::mutateSourcePatches() {
	SSPG::get[sspGenerationMethod]->mutate(this->grid.patches);
}

bool EditorView::checkPatchSourceLocation(const MondriaanPatch& newPatch) {
	if (!settings.source->bounds().icontains(newPatch.sourceRotatedBounds()))
		return false;

	return true;
}

bool EditorView::checkPatchTargetLocation(const MondriaanPatch& newPatch) {
	if (!settings.target->bounds().icontains(newPatch.targetBounds()))
		return false;

	return true;
}

bool EditorView::checkPatchOverlap(const MondriaanPatch& patch, const MondriaanPatch& oldPatch) {
	if (grid.overlaps(patch, Type_Target | Type_Source, &oldPatch))
		return false;

	return true;
}


bool EditorView::checkPatch(const MondriaanPatch& newPatch, const MondriaanPatch& oldPatch) {
	if (!checkPatchTargetLocation(newPatch))
		return false;

	if (!checkPatchSourceLocation(newPatch))
		return false;

	if (!checkPatchOverlap(newPatch, oldPatch))
		return false;

	// TODO original source location
	// TODO original target location

	return true;
}

void EditorView::mutatePatches() {
	std::vector<std::size_t> patchIndices = Utils::nUniqueRandomSizeTypesInRange(generator, nMutatedPatches, 0, grid.size());
	for (std::size_t patchIndex : patchIndices) {
		MondriaanPatch& currentPatch = grid[patchIndex];

		MondriaanPatch bestPatch;
		bestPatch.match = std::numeric_limits<double>::max();

		std::vector<MondriaanPatch::Mutation> mutationIndices = Utils::nUniqueRandomIntegersInRange(
			generator,
			nMutations,
			0,
			MondriaanPatch::mutations);

		for (int mutationIndex : mutationIndices) {
			Log::print("Patch %d: ", patchIndex);
			MondriaanPatch mutatedPatch = currentPatch.mutated(mutationIndex, currentPatch.step(generator, mutationIndex));

			if (!checkPatch(mutatedPatch, currentPatch)) {
				Log::error("rejected");
				continue;
			}

			mutatedPatch.computeBestMatch(static_cast<cv::TemplateMatchModes>(metric));
			Log::print(" and match %.5f\n", mutatedPatch.match);

			if (mutatedPatch.match < bestPatch.match)
				bestPatch = mutatedPatch;
		}

		if (bestPatch.match != std::numeric_limits<double>::max()) {
			Boundsi oldTargetBounds = currentPatch.targetBounds();
			Boundsi oldSourceRotatedBounds = currentPatch.sourceRotatedBounds();

			currentPatch.removeFromGlobalMask();
			currentPatch = bestPatch;
			currentPatch.addToGlobalMask();

			grid.update(patchIndex, oldTargetBounds, currentPatch.targetBounds(), Type_Target);
			grid.update(patchIndex, oldSourceRotatedBounds, currentPatch.sourceRotatedBounds(), Type_Source);
		}
	}

	settings.mask.reloadGL();
}

void EditorView::mutatePatchesRandom() {
	std::vector<int> patchIndices = Utils::nUniqueRandomIntegersInRange(generator, nMutatedPatches, 0, grid.size());
	for (int patchIndex : patchIndices) {
		MondriaanPatch& currentPatch = grid[patchIndex];

		MondriaanPatch bestPatch;
		bestPatch.match = std::numeric_limits<double>::max();
		std::vector<MondriaanPatch::Mutation> mutationIndices = Utils::nUniqueRandomIntegersInRange(
			generator,
			nMutations,
			0,
			MondriaanPatch::mutations);
		for (int mutationIndex : mutationIndices) {
			Log::print("Patch %d: ", patchIndex);
			MondriaanPatch mutatedPatch = currentPatch.mutated(mutationIndex, currentPatch.step(generator, mutationIndex));

			if (!checkPatch(mutatedPatch, currentPatch)) {
				Log::error("rejected");
				continue;
			}

			mutatedPatch.computeMatch();
			Log::print(" and match %.5f\n", mutatedPatch.match);

			if (mutatedPatch.match < bestPatch.match)
				bestPatch = mutatedPatch;
		}

		if (bestPatch.match != std::numeric_limits<double>::max()) {
			Boundsi oldBounds = currentPatch.targetBounds();
			currentPatch = bestPatch;

			grid.update(patchIndex, oldBounds, currentPatch.targetBounds(), Type_Target);
		}
	}
}

void EditorView::splitPatches() {
	std::vector<std::pair<std::size_t, float>> patchArea;
	for (std::size_t patchIndex = 0; patchIndex < grid.size(); patchIndex++)
		patchArea.emplace_back(patchIndex, grid.patches[patchIndex].dimension_mm.x * grid.patches[patchIndex].dimension_mm.y);

	auto customLess = [](const std::pair<std::size_t, float>& a, const std::pair<std::size_t, float>& b) {
		return a.second > b.second;
	};

	auto pdf = [](float x) {
		return std::cbrt(x - 1) + 1;
	}; // 3(x - 1)^2
	//
	//auto pdf = [] (float x) { return 1.0f - std::sqrtf(1.0f - x); }; // 2(1 - x)

	std::ranges::sort(patchArea.begin(), patchArea.end(), customLess);
	//std::vector<std::size_t> patchIndices = Utils::nUniqueRandomSizeTypesInRange(generator, nMutatedPatches, 0, grid.size());
	std::vector<std::size_t> patchIndices = Utils::nUniqueSampledSizeTypesInRange(generator, nMutatedPatches, 0, grid.size(), pdf);
	std::array splitFractions = {1.0 / 4.0, 1.0 / 3.0, 1.0 / 2.0, 2.0 / 3.0, 3.0 / 4.0};
	//std::array splitFractions = { 0.618033989, 0.381966011 };

	for (std::size_t patchIndex : patchIndices) {
		patchIndex = patchArea[patchIndex].first;

		MondriaanPatch& currentPatch = grid[patchIndex];

		//bool splitX = Utils::randomBool(generator);
		bool splitX = currentPatch.dimension_mm.x > currentPatch.dimension_mm.y;

		MondriaanPatch bestPatchA;
		MondriaanPatch bestPatchB;
		double bestTotalMatch = std::numeric_limits<double>::max();

		std::vector<int> fractionIndices = Utils::nUniqueRandomIntegersInRange(generator,
		                                                                       std::min<int>(4, splitFractions.size()),
		                                                                       0,
		                                                                       splitFractions.size());

		for (int fractionIndex : fractionIndices) {
			//Log::print("Patch %d: ", patchIndex);

			double fraction = splitFractions[fractionIndex];

			MondriaanPatch newPatchA;
			MondriaanPatch newPatchB;
			if (splitX) {
				int originalWidthPx = settings.tmm2px(currentPatch.dimension_mm.x);
				int newWidthAPx = originalWidthPx * fraction;
				int newWidthBPx = originalWidthPx - newWidthAPx;
				double newWidthA = settings.tpx2mm(newWidthAPx);
				double newWidthB = settings.tpx2mm(newWidthBPx);

				if (newWidthA < settings.minimumPatchDimension_mm.x || newWidthB < settings.minimumPatchDimension_mm.x) {
					if (!checkPatch(newPatchA, currentPatch)) {
						//Log::error("rejected width");
						continue;
					}
				}

				Vec2 newTargetOffsetA = currentPatch.targetOffset;
				Vec2 newTargetOffsetB = currentPatch.targetOffset + Vec2(settings.tmm2px(newWidthA), 0);

				newPatchA = MondriaanPatch(currentPatch.sourceOffset,
				                           newTargetOffsetA,
				                           Vec2(newWidthA, currentPatch.dimension_mm.y),
				                           currentPatch.sourceRotation);
				if (!checkPatch(newPatchA, currentPatch)) {
					//Log::error("rejected A");
					continue;
				}

				newPatchB = MondriaanPatch(currentPatch.sourceOffset,
				                           newTargetOffsetB,
				                           Vec2(newWidthB, currentPatch.dimension_mm.y),
				                           currentPatch.sourceRotation);
				if (!checkPatch(newPatchB, currentPatch)) {
					//Log::error("rejected B");
					continue;
				}

			} else {
				int originalHeightPx = settings.tmm2px(currentPatch.dimension_mm.y);
				int newHeightAPx = originalHeightPx * fraction;
				int newHeightBPx = originalHeightPx - newHeightAPx;
				double newHeightA = settings.tpx2mm(newHeightAPx);
				double newHeightB = settings.tpx2mm(newHeightBPx);

				if (newHeightA < settings.minimumPatchDimension_mm.y || newHeightB < settings.minimumPatchDimension_mm.y) {
					if (!checkPatch(newPatchA, currentPatch)) {
						//Log::error("rejected height");
						continue;
					}
				}

				Vec2 newTargetOffsetA = currentPatch.targetOffset;
				Vec2 newTargetOffsetB = currentPatch.targetOffset + Vec2(0, settings.tmm2px(newHeightA));

				newPatchA = MondriaanPatch(currentPatch.sourceOffset,
				                           newTargetOffsetA,
				                           Vec2(currentPatch.dimension_mm.x, newHeightA),
				                           currentPatch.sourceRotation);
				if (!checkPatch(newPatchA, currentPatch)) {
					//Log::error("rejected A");
					continue;
				}

				newPatchB = MondriaanPatch(currentPatch.sourceOffset,
				                           newTargetOffsetB,
				                           Vec2(currentPatch.dimension_mm.x, newHeightB),
				                           currentPatch.sourceRotation);
				if (!checkPatch(newPatchB, currentPatch)) {
					//Log::error("rejected B");
					continue;
				}
			}

			newPatchA.computeBestMatch(static_cast<cv::TemplateMatchModes>(metric));
			newPatchB.computeBestMatch(static_cast<cv::TemplateMatchModes>(metric));
			double totalMatch = newPatchA.match + newPatchB.match;
			//Log::print(" and match %.5f\n", totalMatch);

			if (totalMatch < bestTotalMatch) {
				bestPatchA = newPatchA;
				bestPatchB = newPatchB;
				bestTotalMatch = totalMatch;
			}
		}

		if (bestTotalMatch != std::numeric_limits<double>::max()) {
			Boundsi oldBounds = currentPatch.targetBounds();

			// Update old patch
			currentPatch = bestPatchA;
			grid.update(patchIndex, oldBounds, currentPatch.targetBounds(), Type_Target);

			// Insert new patch
			grid.add(bestPatchB);
		}
	}
}

void EditorView::generateRegularPatches() {
	int width = settings.target->cols();
	int height = settings.target->rows();

	settings.target->data.copyTo(settings.puzzle.data);
	settings.puzzle.reloadGL();

	int size = 15;
//#pragma omp parallel fordd
	for (int col = 0; col < width; col += size) {
//#pragma omp parallel fordd
		for (int row = 0; row < height; row += size) {
			cv::Rect patch = cv::Rect(col, row, std::min(size, width - col), std::min(size, height - row));

			auto[rotationIndex, sourcePosition] = Utils::computeBestMatch(patch, cv::TM_CCORR_NORMED);
			cv::Rect sourcePatch(sourcePosition.x, sourcePosition.y, patch.width, patch.height);

			settings.sourcer.textures[rotationIndex].data(sourcePatch).copyTo(settings.puzzle.data(patch));
			settings.puzzle.reloadGL();
		}
	}
}

GEOM_FADE2D::Fade_2D dt;
std::vector<GEOM_FADE2D::VoroCell2*> cells;

void EditorView::renderVoronoi() {
	using namespace GEOM_FADE2D;
	for (VoroCell2* cell : cells) {
		std::vector<Point2> boundaries;
		cell->getBoundaryPoints(boundaries);

		if (!boundaries.empty()) {
			std::vector<ImVec2> points;
			for (const Point2& point : boundaries)
				points.push_back(target.toAbsoluteScreenSpace(Vec2(point.x(), point.y())).iv());

			ImGui::GetWindowDrawList()->AddPolyline(points.data(), points.size(), Colors::GREEN.u32(), true, 1.0);
		}
	}
}

void EditorView::computeVoronoi() {
	using namespace GEOM_FADE2D;
	std::vector<Point2> vPoints;
	for (const MondriaanPatch& patch : grid.patches) {
		Vec2 center = patch.targetBounds().center();
		vPoints.push_back(Point2(center.x, center.y));
	}

	dt.enableMultithreading();
	dt.insert(vPoints);

	cells.clear();
	Voronoi2* pVoro(dt.getVoronoiDiagram());
	pVoro->getVoronoiCells(cells);
}

void EditorView::spawnNewPatch() {
	int bestRow = 0;
	int bestCol = 0;
	double bestCoverage = std::numeric_limits<double>::max();
	for (int col = 0; col < grid.cols; col++) {
		for (int row = 0; row < grid.rows; row++) {
			double dx = grid.targetDimension.x / static_cast<double>(grid.cols);
			double dy = grid.targetDimension.y / static_cast<double>(grid.rows);
			double x = col * dx;
			double y = row * dy;
			Bounds bounds(x, y, x + dx, y + dy);

			double coverage = 0;
			std::set<std::size_t>& patches = grid.targetCells[row, col, Type_Target];
			for (std::size_t patchIndex : patches) {
				MondriaanPatch& patch = grid[patchIndex];
				Bounds targetBounds = patch.targetBounds();
				// Todo fix
				Bounds overlap(Utils::max(bounds.minX(), targetBounds.minX()),
				               Utils::max(bounds.minY(), targetBounds.minY()),
				               Utils::min(bounds.imaxX(), targetBounds.imaxX()),
				               Utils::min(bounds.imaxY(), targetBounds.imaxY()));
				coverage += overlap.width() * overlap.height();
			}

			if (coverage < bestCoverage) {
				bestCoverage = coverage;
				bestRow = row;
				bestCol = col;
			}
		}
	}

	Log::debug("Found new spot: %d, %d, %f", bestCol, bestRow, bestCoverage);

	double dx = grid.targetDimension.x / static_cast<double>(grid.cols);
	double dy = grid.targetDimension.y / static_cast<double>(grid.rows);
	double x = bestCol * dx;
	double y = bestRow * dy;
	Vec2 targetDimension = settings.tmm2px(settings.minimumPatchDimension_mm);
	Vec2 targetOffset(x + (dx - targetDimension.x) / 2.0, y + (dy - targetDimension.y) / 2.0);
	grid.add(targetOffset, Utils::transform(targetOffset, target.tdimension, source.tdimension), settings.minimumPatchDimension_mm, 0.0);
}

void EditorView::generateImage() {
	cv::Mat outputTarget(settings.target->rows(), settings.target->cols(), settings.target->data.type(), cv::Scalar(0));
	cv::Mat outputSource(settings.target->rows(), settings.target->cols(), settings.source->data.type(), cv::Scalar(0));

	for (MondriaanPatch& patch : grid.patches) {
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

	cv::resize(outputSource, outputSource, cv::Size(500, 500));
	cv::resize(outputTarget, outputTarget, cv::Size(500, 500));

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

	RegularGrid<10, 10> newGrid(grid.patches, source.tdimension, target.tdimension);
	this->grid = newGrid;
}

void EditorView::resetSelection() {
	selectedIndex = -1;
	selectedPoint = Vec2f();
	intersectedIndex = -1;
	intersectedPoint = Vec2f();
}

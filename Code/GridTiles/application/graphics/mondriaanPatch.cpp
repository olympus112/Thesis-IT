#include "core.h"
#include "mondriaanPatch.h"

#include "main.h"

MondriaanPatch::MondriaanPatch(const Vec2& sourceOffset, const Vec2& targetOffset, const Vec2& dimension, double sourceRotation) {
	this->sourceOffset = sourceOffset;
	this->targetOffset = targetOffset;
	this->dimension_mm = dimension;
	this->sourceRotation = sourceRotation;

	this->mask = Mask(dimension, sourceRotation).pixels;
}

void MondriaanPatch::render(const Canvas& source, const Canvas& target, bool intersected, bool selected, bool showConnections, const Color& color) const {
	
	ImVec2 sourceMin = source.toAbsoluteScreenSpace(sourceOffset).iv();
	ImVec2 targetMin = target.toAbsoluteScreenSpace(targetOffset).iv();
	Vec2 sourceSize = source.toRelativeScreenSpace(settings.smm2px(dimension_mm));
	Vec2 targetSize = target.toRelativeScreenSpace(settings.tmm2px(dimension_mm));

	ImGui::GetWindowDrawList()->AddRect(sourceMin, sourceMin + sourceSize.iv(), color.u32(), 0, ImDrawCornerFlags_All, intersected || selected ? 2 : 1);
	ImGui::GetWindowDrawList()->AddRect(targetMin, targetMin + targetSize.iv(), color.u32(), 0, ImDrawCornerFlags_All, intersected || selected ? 2 : 1);

	if (showConnections || selected)
		ImGui::GetWindowDrawList()->AddLine(sourceMin, targetMin, Colors::WHITE.u32());
}

Bounds MondriaanPatch::sourceBounds() const {
	return Bounds(sourceOffset, sourceOffset + settings.smm2px(dimension_mm));
}

Bounds MondriaanPatch::targetBounds() const {
	return Bounds(targetOffset, targetOffset + settings.tmm2px(dimension_mm));
}

Bounds MondriaanPatch::sourceUV() const {
	return settings.source->bounds().subBoundsUV(sourceBounds());
}

Bounds MondriaanPatch::targetUV() const {
	return settings.target->bounds().subBoundsUV(targetBounds());
}


//void MondriaanPatch::mutate(int mutation, double step) {
//	switch (mutation) {
//		case LEFT:
//			sourceOffset.x -= step;
//			targetOffset.x -= step;
//		case RIGHT:
//			shape.dimension.x += step;
//			break;
//		case TOP:
//			sourceOffset.y -= step;
//			targetOffset.y -= step;
//		case BOTTOM:
//			shape.dimension.y += step;
//			break;
//		default:
//			Log::error("Unknown mutation");
//			break;
//	}
//
//	shape = computeShape(shape.dimension);
//	mask = computeMask(shape);
//}
//
//MondriaanPatch MondriaanPatch::getMutation(int mutation, double step) const {
//	MondriaanPatch patch(sourceOffset, targetOffset, shape.dimension);
//	patch.mutate(mutation, step);
//
//	return patch;
//}
//
//int MondriaanPatch::mutations() {
//	return 4;
//}
//
//Shape MondriaanPatch::computeShape(const Vec2& dimension) {
//	return Shape({
//		Vec2(),
//		Vec2(dimension.x, 0),
//		dimension,
//		Vec2(0, dimension.y),
//	});
//}

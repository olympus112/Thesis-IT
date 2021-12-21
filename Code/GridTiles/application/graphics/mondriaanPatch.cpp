#include "core.h"
#include "mondriaanPatch.h"

MondriaanPatch::MondriaanPatch(const Vec2& sourceOffset, const Vec2& targetOffset, const Vec2& dimension) : Patch(computeShape(dimension), sourceOffset, targetOffset) {

}

void MondriaanPatch::mutate(int mutation, double step) {
	switch (mutation) {
		case LEFT:
			sourceOffset.x -= step;
			targetOffset.x -= step;
		case RIGHT:
			shape.dimension.x += step;
			break;
		case TOP:
			sourceOffset.y -= step;
			targetOffset.y -= step;
		case BOTTOM:
			shape.dimension.y += step;
			break;
		default:
			Log::error("Unknown mutation");
			break;
	}

	shape = computeShape(shape.dimension);
	mask = computeMask(shape);
}

MondriaanPatch MondriaanPatch::getMutation(int mutation, double step) const {
	MondriaanPatch patch(sourceOffset, targetOffset, shape.dimension);
	patch.mutate(mutation, step);

	return patch;
}

int MondriaanPatch::mutations() {
	return 4;
}

Shape MondriaanPatch::computeShape(const Vec2& dimension) {
	return Shape({
		Vec2(),
		Vec2(dimension.x, 0),
		dimension,
		Vec2(0, dimension.y),
	});
}

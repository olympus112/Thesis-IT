#include "core.h"
#include "mondriaanPatch.h"

MondriaanPatch::MondriaanPatch(const Vec2& dimension, const Vec2& sourceOffset, const Vec2& targetOffset) : Patch(computeShape(dimension), sourceOffset, targetOffset) {

}

void MondriaanPatch::mutate(int mutation, double step) {
	switch (mutation) {
		case LEFT:
			shape.dimension.x += step;
			shape[Bounds::TL].x -= step;
			shape[Bounds::BL].x -= step;
			break;
		case RIGHT:
			shape.dimension.x += step;
			shape[Bounds::TR].x += step;
			shape[Bounds::BR].x += step;
			break;
		case TOP:
			shape.dimension.y += step;
			shape[Bounds::TL].y -= step;
			shape[Bounds::TR].y -= step;
			break;
		case BOTTOM:
			shape.dimension.y += step;
			shape[Bounds::BL].y += step;
			shape[Bounds::BR].y += step;
			break;
		default:
			break;
	}
	
	mask = computeMask(shape);
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

#include "core.h"
#include "canvas.h"

#include "bounds.h"

Canvas::Canvas(Texture* texture, const Vec2& offset, double preferredSize) {
	this->offset = offset;
	this->hover = false;
	this->drag = false;

	this->aspect = texture->aspect();
	this->tdimension = texture->dimension();
	this->dimension = computeDimension(textureDimension(), preferredSize);
}

Vec2 Canvas::min() const {
	return offset;
}

Vec2 Canvas::max() const {
	return offset + dimension;
}

double Canvas::minX() const {
	return offset.x;
}

double Canvas::maxX() const {
	return offset.x + dimension.x;
}

double Canvas::minY() const {
	return offset.y;
}

double Canvas::maxY() const {
	return offset.y + dimension.y;
}

bool Canvas::covers(const Vec2& center, const Vec2& point) const {
	return Bounds(center, 5).contains(point);
}

Vec2 Canvas::textureDimension() const {
	return tdimension;
}

Vec2 Canvas::screenDimension() const {
	return dimension;
}

Vec2 Canvas::toTextureSpace(const Vec2& screenVector) const {
	return Utils::transform(screenVector, screenDimension(), textureDimension());
}

Vec2 Canvas::toRelativeScreenSpace(const Vec2& textureVector) const {
	return Utils::transform(textureVector, textureDimension(), screenDimension());
}

Vec2 Canvas::toAbsoluteScreenSpace(const Vec2& textureVector) const {
	return offset + toRelativeScreenSpace(textureVector);
}

Bounds Canvas::screenBounds() const {
	return Bounds(
		offset,
		offset + dimension
	);
}

Bounds Canvas::textureBounds() const {
	return Bounds(
		Vec2(),
		textureDimension()
	);
}

Bounds Canvas::uv(const Bounds& subBounds) const {
	return screenBounds().subBoundsUV(subBounds);
}

Vec2 Canvas::computeDimension(const Vec2& textureDimension, double preferredSize) {
	double aspect = textureDimension.x / textureDimension.y;

	if (aspect > 1.0) {
		return Vec2(
			preferredSize,
			preferredSize / aspect
		);
	}

	if (aspect < 1.0) {
		return Vec2(
			preferredSize * aspect,
			preferredSize
		);
	}

	return Vec2(preferredSize, preferredSize);
}

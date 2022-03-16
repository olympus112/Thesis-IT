#pragma once

#include "graphics/textures/texture.h"

template <typename T>
class BoundsTemplate;

class Canvas {
public:
	bool hover;
	bool drag;

	// Screen space
	Vec2 dimension;

	// Texture space
	Vec2 tdimension;

	// Screen space
	Vec2 offset;

	// Screen / texture space
	double aspect;

	Canvas() = default;
	Canvas(Texture* texture, const Vec2& offset, double preferredSize);

	Vec2 min() const;
	Vec2 max() const;

	double minX() const;
	double maxX() const;
	double minY() const;
	double maxY() const;

	bool covers(const Vec2& center, const Vec2& point) const;

	Vec2 textureDimension() const;
	Vec2 screenDimension() const;

	Vec2 toTextureSpace(const Vec2& screenVector) const;
	Vec2 toRelativeScreenSpace(const Vec2& textureVector) const;
	Vec2 toAbsoluteScreenSpace(const Vec2& textureVector) const;

	// Screen space
	BoundsTemplate<double> screenBounds() const;
	// Screen space
	BoundsTemplate<double> uv(const BoundsTemplate<double>& subBounds) const;

	// Texture space
	BoundsTemplate<double> textureBounds() const;

	static Vec2 computeDimension(const Vec2& textureDimension, double preferredSize);
};

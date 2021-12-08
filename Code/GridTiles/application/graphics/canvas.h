#pragma once

#include "texture.h"

template <typename T>
class BoundsTemplate;

class Canvas {
public:
	bool hover;
	bool drag;

	Vec2 dimension;
	Vec2 offset;
	double aspect;

	Texture* texture;

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
	Vec2 toScreenSpace(const Vec2& textureVector) const;
	Vec2 toAbsoluteScreenSpace(const Vec2& textureVector) const;

	BoundsTemplate<double> bounds() const;
	BoundsTemplate<double> uv(const BoundsTemplate<double>& subBounds) const;

	static Vec2 computeDimension(const Vec2& textureDimension, double preferredSize);
};

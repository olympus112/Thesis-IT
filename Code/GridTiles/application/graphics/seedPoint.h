#pragma once
#include "graphics/bounds.h"

struct SeedPoint {
	// Source position in texture space
	Vec2 sourcePosition;
	// Target position in texture space
	Vec2 targetPosition;

	SeedPoint() = default;
	SeedPoint(const Vec2& sourcePosition, const Vec2& targetPosition);

	void render(const Canvas& sourceCanvas, const Canvas& targetCanvas, bool intersected, bool selected) const;
};

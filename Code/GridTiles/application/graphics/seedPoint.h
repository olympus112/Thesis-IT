#pragma once
#include "graphics/bounds.h"

struct SeedPoint {
	// Texture space
	Vec2 sourcePosition;
	// Texture space
	Vec2 targetPosition;
	// Screen space
	int screenSize = 16;

	SeedPoint() = default;
	SeedPoint(const Vec2& sourcePosition, const Vec2& targetPosition);

	void render(const Canvas& sourceCanvas, const Canvas& targetCanvas, bool intersected, bool selected, bool showConnections, const Color& color) const;
};

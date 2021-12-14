#pragma once
#include "patch.h"
#include "graphics/bounds.h"

struct SeedPoint {
	// Texture space
	Vec2 sourcePosition;
	// Texture space
	Vec2 targetPosition;
	// Texture space
	int textureSize;
	// Patch
	URef<Patch> patch = nullptr;

	SeedPoint() = default;
	SeedPoint(const Vec2& sourcePosition, const Vec2& targetPosition, int textureSize = 32);
	SeedPoint(const SeedPoint& sp) {
		this->sourcePosition = sp.sourcePosition;
		this->targetPosition = sp.targetPosition;
		this->textureSize = sp.textureSize;
		this->patch = URef<Patch>(sp.patch.get());
	}
	SeedPoint& operator=(const SeedPoint& sp) {
		this->sourcePosition = sp.sourcePosition;
		this->targetPosition = sp.targetPosition;
		this->textureSize = sp.textureSize;
		this->patch = URef<Patch>(sp.patch.get());

		return *this;
	}

	Bounds sourceBounds(const Texture* source) const;
	Bounds targetBounds(const Texture* target) const;

	void render(const Canvas& sourceCanvas, const Canvas& targetCanvas, bool intersected, bool selected, bool showConnections, const Color& color) const;
};

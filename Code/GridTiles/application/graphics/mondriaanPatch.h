#pragma once
#include "patch.h"

class MondriaanPatch : public Patch {
public:
	constexpr static int LEFT = 0;
	constexpr static int RIGHT = 1;
	constexpr static int TOP = 2;
	constexpr static int BOTTOM = 3;

public:
	MondriaanPatch(const Vec2& sourceOffset, const Vec2& targetOffset, const Vec2& dimension);

	void mutate(int mutation, double step);
	MondriaanPatch getMutation(int mutation, double step) const;

	static int mutations();
	static Shape computeShape(const Vec2& dimension);
};
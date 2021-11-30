#pragma once
#include "graphics/color.h"
#include "imgui/imgui_internal.h"

struct Bounds {
	ImRect rect;

	Bounds();
	Bounds(const ImRect& rect);
	Bounds(const Vec2f& position, float size);

	void render(const Vec2f& offset, const Color& color = Colors::WHITE) const;

	bool contains(const Vec2f& point) const {
		return rect.Contains(point.asImVec());
	}

	float minX() const {
		return rect.Min.x;
	}

	float minY() const {
		return rect.Min.y;
	}

	float maxX() const {
		return rect.Max.x;
	}

	float maxY() const {
		return rect.Max.y;
	}

	ImVec2 min() const {
		return rect.Min;
	}

	ImVec2 max() const {
		return rect.Max;
	}

	float width() const {
		return maxX() - minX();
	}

	float height() const {
		return maxY() - minY();
	}

	ImRect subBoundsUV(const Bounds& bounds) const {
		float w = width();
		float h = height();
		float xMin = (bounds.minX() - minX()) / w;
		float xMax = (bounds.maxX() - minX()) / w;
		float yMin = (bounds.minY() - minY()) / h;
		float yMax = (bounds.maxY() - minY()) / h;

		return ImRect(
			xMin,
			yMin,
			xMax,
			yMax
		);
	}
};

struct SeedPoint {
	cv::Mat patch;
	Vec2f position;

	SeedPoint(const Vec2f& position);
		
	Bounds bounds() const;

	void calculatePatch(const cv::Mat& texture, const Bounds& bounds);
	void render(const Vec2f& offset, bool intersected, bool selected) const;
};

struct SeedPointPair {
	SeedPoint source;
	SeedPoint target;
	float size;
	float matching = 0.0f;

	void render(const Bounds& sourceBox, const Bounds& targetBox, bool intersected, bool selected) const;
	void calculateMatch(const cv::Mat& sourceTexture, const cv::Mat& targetTexture, const Bounds& sourceBounds, const Bounds& targetBounds);
};

class SeedPointsTab {
public:
	Bounds sourceBox;
	bool sourceHover = false;
	bool sourceDrag = false;

	Bounds targetBox;
	bool targetHover = false;
	bool targetDrag = false;

	std::vector<SeedPointPair> seedPointPairs;
	int intersectedIndex = -1;
	int selectedIndex = -1;
	Vec2f intersectedPoint;
	Vec2f selectedPoint;

	SeedPointsTab();

	void init();
	void update();
	void render();
};

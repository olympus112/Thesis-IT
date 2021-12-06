#pragma once
#include "graphics/bounds.h"
#include "graphics/color.h"

struct SeedPoint {
	Vec2f position;

	SeedPoint(const Vec2f& position);

	cv::Mat calculatePatch(const cv::Mat& texture, int size);
	void render(const ImVec2& offset, ImVec2 (*transform)(const Vec2f&), bool intersected, bool selected) const;
};

struct SeedPointPair {
	SeedPoint source;
	SeedPoint target;
	int size;
	float matching;

	void render(const Bounds& sourceBox, const Bounds& targetBox, bool intersected, bool selected, const Color& color = Colors::RED) const;
	void calculateMatch(const std::vector<cv::Mat>& sourceTextures, const std::vector<cv::Mat>& targetTextures, const std::vector<float>& distribution);
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

	void onRecalculateMatching();
};

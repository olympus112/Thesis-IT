#pragma once

#include "bounds.h"

template <typename T>
class ShapeTemplate {
public:
	Vector<T, 2> dimension;
	std::vector<Vector<T, 2>> points;

	ShapeTemplate() = default;

	ShapeTemplate(const std::vector<Vector<T, 2>>& points) {
		BoundsTemplate<T> bounds = computeBounds(points);

		if (bounds.min().nonzero()) 
			this->points = normalizePoints(points, bounds.min());
		else 
			this->points = points;

		this->dimension = bounds.dimension();
	}

	void render(const Canvas& canvas, bool fill, const Color& color) const {
		std::vector<ImVec2> poly(this->points.size());

		for (std::size_t index = 0; index < this->points.size(); index++)
			poly[index] = canvas.toAbsoluteScreenSpace(this->points[index]).iv();

		if (fill)
			ImGui::GetWindowDrawList()->AddConvexPolyFilled(poly.data(), poly.size(), color.u32());
		else
			ImGui::GetWindowDrawList()->AddPolyline(poly.data(), poly.size(), color.u32(), true, 1.0);
	}

	const Vector<T, 2>& operator[](std::size_t index) const {
		return points[index];
	}

	Vector<T, 2>& operator[](std::size_t index) {
		return points[index];
	}

	std::size_t size() const {
		return points.size();
	}

	auto begin() {
		return points.begin();
	}

	auto end() {
		return points.end();
	}

	auto begin() const {
		return points.begin();
	}

	auto end() const {
		return points.end();
	}

	static BoundsTemplate<T> computeBounds(const std::vector<Vector<T, 2>>& points) {
		BoundsTemplate<T> bounds(points[0], 0);
		for (std::size_t index = 1; index < points.size(); index++)
			bounds.add(points[index]);

		return bounds;
	}

	static std::vector<Vector<T, 2>> normalizePoints(const std::vector<Vector<T, 2>>& points,
	                                                 const Vector<T, 2>& offset) {
		std::vector<Vector<T, 2>> result(points.size());
		for (std::size_t index = 0; index < points.size(); index++)
			result[index] = points[index] - offset;

		return result;
	}
};

typedef ShapeTemplate<double> Shape;

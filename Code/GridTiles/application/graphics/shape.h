#pragma once
#include "bounds.h"

template <typename T>
class ShapeTemplate {
public:
	BoundsTemplate<T> bounds;
	std::vector<Vector<T, 2>> points;

	ShapeTemplate() = default;

	ShapeTemplate(const std::vector<Vector<T, 2>>& points) {
		this->points = points;
		this->bounds = computeBounds(points);
	}

	const Vector<T, 2>& operator[](std::size_t index) const {
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
};

typedef ShapeTemplate<double> Shape;
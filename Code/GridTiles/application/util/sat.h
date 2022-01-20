#pragma once

//#include <algorithm>
#include <vector>
#include "math/vec.h"

// https://github.com/winstxnhdw/2d-separating-axis-theorem

struct Sat {

	// Linear transform to find the orthogonal vector of the edge
	static Vec2 calculate_normalised_projection_axis(const Vec2& current_point, const Vec2& next_point) {
		const double axis_x = -(next_point.y - current_point.y);
		const double axis_y = next_point.x - current_point.x;
		const double magnitude = hypot(axis_x, axis_y);

		Vec2 axis_normalised;
		axis_normalised.x = axis_x / magnitude;
		axis_normalised.y = axis_y / magnitude;

		return axis_normalised;
	}

	// Project the vertices of each polygon onto a axis
	static void compute_projections(const std::vector<Vec2>& bounds_a, const std::vector<Vec2>& bounds_b, const Vec2& axis_normalised, std::vector<double>& projections_a, std::vector<double>& projections_b) {
		projections_a.reserve(bounds_a.size());
		projections_b.reserve(bounds_b.size());

		for (size_t j = 0; j < bounds_a.size(); j++) {
			const double projection_a = dot(axis_normalised, bounds_a[j]);
			const double projection_b = dot(axis_normalised, bounds_b[j]);
			projections_a.push_back(projection_a);
			projections_b.push_back(projection_b);
		}
	}

	// Check if the projections of two polygons overlap
	static bool is_overlapping(const std::vector<double>& projections_a, const std::vector<double>& projections_b) {
		const double max_projection_a = *std::ranges::max_element(projections_a);
		const double min_projection_a = *std::ranges::min_element(projections_a);
		const double max_projection_b = *std::ranges::max_element(projections_b);
		const double min_projection_b = *std::ranges::min_element(projections_b);

		// Does not intersect
		if (max_projection_a < min_projection_b || max_projection_b < min_projection_a)
			return false;

		// Projection overlaps but may not necessarily be intersecting yet
		return true;
	}

	// Check if two convex polygons intersect
	static bool separating_axis_intersect(const std::vector<Vec2>& bounds_a, const std::vector<Vec2>& bounds_b) {
		for (size_t i = 0; i < bounds_a.size(); i++) {
			Vec2 current_point;
			current_point.x = bounds_a[i].x;
			current_point.y = bounds_a[i].y;

			Vec2 next_point;
			next_point.x = bounds_a[(i + 1) % bounds_a.size()].x;
			next_point.y = bounds_a[(i + 1) % bounds_a.size()].y;

			Vec2 axis_normalised = calculate_normalised_projection_axis(current_point, next_point);

			std::vector<double> projections_a;
			std::vector<double> projections_b;
			compute_projections(bounds_a, bounds_b, axis_normalised, projections_a, projections_b);

			if (!is_overlapping(projections_a, projections_b)) 
				return false;
		}

		for (size_t i = 0; i < bounds_b.size(); i++) {
			Vec2 current_point;
			current_point.x = bounds_b[i].x;
			current_point.y = bounds_b[i].y;

			Vec2 next_point;
			next_point.x = bounds_b[(i + 1) % bounds_b.size()].x;
			next_point.y = bounds_b[(i + 1) % bounds_b.size()].y;

			Vec2 axis_normalised = calculate_normalised_projection_axis(current_point, next_point);

			std::vector<double> projections_a;
			std::vector<double> projections_b;
			compute_projections(bounds_a, bounds_b, axis_normalised, projections_a, projections_b);

			if (!is_overlapping(projections_a, projections_b))
				return false;
		}

		// Intersects
		return true;
	}
};


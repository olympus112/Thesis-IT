#pragma once

#include <set>
#include "util/list.h"
#include "util/sat.h"
#include "graphics/patch.h"

template <std::size_t Rows, std::size_t Cols = Rows>
struct CollisionGridTemplate {
	enum class GridType {
		Source,
		Target
	};

	int width;
	int height;
	GridType type;

	List<Patch*> cells[Rows * Cols];

	CollisionGridTemplate(int width, int height, GridType type) {
		this->width = width;
		this->height = height;
		this->type = type;

		for (int row = 0; row < Rows; row++) {
			for (int col = 0; col < Cols; col++) {
				cells[index(row, col)] = List<Patch*>();
			}
		}
	}

	Vec2i cell(const Vec2& point) {
		return Vec2i(
			cvFloor(point.x / this->width * Cols),
			cvFloor(point.y / this->height * Rows)
		);
	}

	Vec4i region(const Bounds& bounds) {
		return Vec4i(cell(bounds.min()), cell(bounds.max()));
	}

	bool overlaps(Patch* patch) {
		Vec4i cellRegion = region(type == GridType::Source ? patch->sourceBounds() : patch->targetBounds());

		for (int row = cellRegion[1]; row < cellRegion[3]; ++row) {
			for (int col = cellRegion[0]; col < cellRegion[2]; ++col) {
				const List<Patch*>& patches = cells[index(row, col)];

				std::set checked = { patch };
				for (std::size_t i = 0; i < patches.size(); i++) {
					if (!checked.contains(patches[i])) {
						if (intersects(patches[i]->shape, patch->shape))
							return true;

						checked.insert(patches[i]);
					}
				}
			}
		}

		return false;
	}

	bool overlaps(Patch* a, Patch* b) {
		Vec4i cellRegion = region(type == GridType::Source ? a->sourceBounds() : a->targetBounds());

		for (int row = cellRegion[1]; row < cellRegion[3]; ++row) {
			for (int col = cellRegion[0]; col < cellRegion[2]; ++col) {
				const List<Patch*>& patches = cells[index(row, col)];
				if (patches.contains(b))
					return intersects(a.shape, b->shape);
			}
		}

		return false;
	}

	// https://gist.github.com/nyorain/dc5af42c6e83f7ac6d831a2cfd5fbece
	bool intersects(const Shape& a, const Shape& b) {
		return Sat::separating_axis_intersect(a.points, b.points);
	}

	constexpr std::size_t index(std::size_t row, std::size_t col) {
		return row * Cols + col;
	}
		
};
#pragma once

#include <set>
#include "util/list.h"
#include "util/sat.h"
#include "graphics/patch.h"

template <std::size_t Rows, std::size_t Cols = Rows>
struct CollisionGridTemplate {
	typedef Vec2i GridCell;

	struct GridRange {
		int start;
		int end;

		bool contains(int position) const {
			return Utils::contains(position, start, end);
		}

		bool overlaps(const GridRange& other) const {
			return contains(other.start) || contains(other.end);
		}

		GridRange operator+(const GridRange& other) const {
			return GridRange(
				Utils::min(start, other.start),
				Utils::max(end, other.end)
			);
		}
	};

	struct GridRegion {
		GridRange cols;
		GridRange rows;

		bool contains(const GridCell& gridCell) const {
			return cols.contains(gridCell.x) && rows.contains(gridCell.y);
		}

		bool overlaps(const GridRegion& other) const {
			return cols.overlap(other.cols) && rows.overlap(other.rows);
		}

		GridRegion operator+(const GridRegion& other) const {
			return GridRegion(
				cols + other.cols,
				rows + other.rows
			);
		}
	};

	enum class GridType {
		Source,
		Target
	};

	int width;
	int height;
	GridType type;

	std::set<Patch*> cells[Rows * Cols];

	CollisionGridTemplate(int width, int height, GridType type) {
		this->width = width;
		this->height = height;
		this->type = type;

		for (int row = 0; row < Rows; row++) {
			for (int col = 0; col < Cols; col++) {
				cells[index(row, col)] = std::set<Patch*>();
			}
		}
	}

	void erase(Patch* patch, const GridRegion& region) {
		for (int col = region.cols.start; col <= region.cols.end; ++col) {
			for (int row = region.rows.start; row <= region.rows.end; ++row) {
				cells[index(row, col)].erase(patch);
			}
		}
	}

	void insert(Patch* patch, const GridRegion& region) {
		for (int col = region.cols.start; col <= region.cols.end; ++col) {
			for (int row = region.rows.start; row <= region.rows.end; ++row) {
				cells[index(row, col)].insert(patch);
			}
		}
	}

	void swap(Patch* patch, const GridRegion& oldRegion, const GridRegion& newRegion) {
		GridRegion totalRegion = oldRegion + newRegion;

		for (int col = totalRegion.cols.start; col <= totalRegion.cols.end; ++col) {
			for (int row = totalRegion.rows.start; row <= totalRegion.rows.end; ++row) {
				std::set<Patch*>& cell = cells[index(row, col)];
				bool inOldRegion = oldRegion.contains(cell);
				bool inNewRegion = newRegion.contains(cell);

				if (inOldRegion && !inNewRegion)
					cell.erase(patch);

				if (inNewRegion && !inOldRegion)
					cell.insert(patch);
			}
		}
	}

	void update(Patch* patch, const Bounds& oldBounds, const Bounds& newBounds) {
		GridRegion oldRegion = region(oldBounds);
		GridRegion newRegion = region(newBounds);

		if (oldRegion.overlaps(newRegion)) {
			swap(patch, oldRegion, newRegion);
		} else {
			erase(patch, oldRegion);
			insert(patch, newRegion);
		}
		GridRegion totalRegion = oldRegion + newRegion;
	}

	GridCell cell(const Vec2& point) const {
		return GridCell(
			cvFloor(point.x / this->width * Cols),
			cvFloor(point.y / this->height * Rows)
		);
	}

	GridRegion region(const Bounds& bounds) const {
		GridCell min = cell(bounds.min());
		GridCell max = cell(bounds.max());

		return GridRegion(
			GridRange(min.x, max.x),
			GridRange(min.y, max.y)
		);
	}

	bool overlaps(Patch* patch) const {
		GridRegion cellRegion = region(type == GridType::Source ? patch->sourceBounds() : patch->targetBounds());

		for (int row = cellRegion.rows.start; row < cellRegion.rows.end; ++row) {
			for (int col = cellRegion.cols.start; col < cellRegion.cols.end; ++col) {
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

	bool overlaps(Patch* a, Patch* b) const {
		GridRegion cellRegion = region(type == GridType::Source ? a->sourceBounds() : a->targetBounds());

		for (int row = cellRegion.rows.start; row < cellRegion.rows.end; ++row) {
			for (int col = cellRegion.cols.start; col < cellRegion.cols.end; ++col) {
				const List<Patch*>& patches = cells[index(row, col)];
				if (patches.contains(b))
					return intersects(a->shape, b->shape);
			}
		}

		return false;
	}

	// https://gist.github.com/nyorain/dc5af42c6e83f7ac6d831a2cfd5fbece
	bool intersects(const Shape& a, const Shape& b) const {
		return Sat::separating_axis_intersect(a.points, b.points);
	}

	constexpr std::size_t index(std::size_t row, std::size_t col) const {
		return row * Cols + col;
	}
		
};
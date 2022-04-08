#pragma once

#include <set>
#include "util/list.h"
#include "util/sat.h"
#include "graphics/mondriaanPatch.h"

typedef int Type;
enum Type_ {
	Type_None = 0 << 0,
	Type_Source = 1 << 0,
	Type_Target = 1 << 1
};


template <std::size_t Rows, std::size_t Cols = Rows>
struct RegularGrid {
	typedef Vector<std::size_t, 2> GridCell;

	struct GridRange {
		std::size_t start;
		std::size_t end;

		GridRange() = default;
		GridRange(std::size_t start, std::size_t end) : start(start), end(end) {}

		bool contains(std::size_t position) const {
			return start <= position && position <= end;
		}

		bool contains(const GridRange& other) const {
			return contains(other.start) && contains(other.end);
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

		GridRegion() = default;
		GridRegion(const GridRange& cols, const GridRange& rows) : cols(cols), rows(rows) {}

		bool contains(const GridCell& gridCell) const {
			return cols.contains(gridCell.x) && rows.contains(gridCell.y);
		}

		bool overlaps(const GridRegion& other) const {
			return cols.overlaps(other.cols) && rows.overlaps(other.rows);
		}

		bool outside() const {
			return !GridRange(0, Cols - 1).contains(cols) || !GridRange(0, Rows - 1).contains(rows);
		}

		GridRegion operator+(const GridRegion& other) const {
			return GridRegion(
				cols + other.cols,
				rows + other.rows
			);
		}
	};

	struct GridConstriant {
		typedef int ConstraintType;
		enum ConstaintType_ {
			None,
			XMin,
			XMax,
			YMin,
			YMax,
		};

		ConstraintType type;
		std::size_t patchIndexA;
		std::size_t patchIndexB;

		GridConstriant(std::size_t patchIndexA, std::size_t patchIndexB, ConstraintType type) {
			this->patchIndexA = patchIndexA;
			this->patchIndexB = patchIndexB;
			this->type = type;
		}
	};

	std::size_t cols = Cols;
	std::size_t rows = Rows;

	Vec2i sourceDimension;
	Vec2i targetDimension;

	std::array<std::set<std::size_t>, Rows * Cols> targetCells;
	std::array<std::set<std::size_t>, Rows* Cols> sourceCells;

	std::vector<MondriaanPatch> patches;
	std::vector<GridConstriant> constraints;

public:
	RegularGrid() = default;

	RegularGrid(const Vec2i sourceDimension, const Vec2i targetDimension) {
		this->sourceDimension = sourceDimension;
		this->targetDimension = targetDimension;

		clear();
	}

	RegularGrid(const std::vector<MondriaanPatch>& patches, const Vec2i sourceDimension, const Vec2i targetDimension) : RegularGrid(sourceDimension, targetDimension) {
		for (const MondriaanPatch& patch : patches)
			add(patch);
	}

	RegularGrid(const RegularGrid& other) {
		this->patches = other.patches;

		this->sourceDimension = other.sourceDimension;
		this->targetDimension = other.targetDimension;

		this->constraints = other.constraints;

		this->sourceCells = other.sourceCells;
		this->targetCells = other.targetCells;
	}

	RegularGrid(RegularGrid&& other) noexcept {
		this->patches = std::move(other.patches);

		this->sourceDimension = std::move(other.sourceDimension);
		this->targetDimension = std::move(other.targetDimension);

		this->sourceCells = std::move(other.sourceCells);
		this->targetCells = std::move(other.targetCells);

		this->constraints = std::move(other.constraints);

		return *this;
	}

	RegularGrid& operator=(const RegularGrid& other) {
		this->patches = other.patches;

		this->sourceDimension = other.sourceDimension;
		this->targetDimension = other.targetDimension;

		this->sourceCells = other.sourceCells;
		this->targetCells = other.targetCells;

		this->constraints = other.constraints;

		return *this;
	}

	RegularGrid& operator=(RegularGrid&& other) {
		this->patches = std::move(other.patches);

		this->sourceDimension = std::move(other.sourceDimension);
		this->targetDimension = std::move(other.targetDimension);

		this->sourceCells = std::move(other.sourceCells);
		this->targetCells = std::move(other.targetCells);

		this->constraints = std::move(other.constraints);
		
		return *this;
	}

	// Returns the amount of patches in the regular grids
	std::size_t size() const {
		return patches.size();
	}

	// Returns the patch at index in the list of patches
	MondriaanPatch& operator[] (std::size_t index) {
		return patches[index];
	}

	// Returns the patch at index in the list of patches
	MondriaanPatch& operator() (std::size_t index) {
		return patches[index];
	}
	
	// Returns the set of patches in the regular grid cell of Type type at index
	std::set<std::size_t>& operator() (std::size_t index, Type type) {
		return (type == Type_Source ? sourceCells : targetCells)[index];
	}

	// Returns the set of patches in the regular grid cell of Type type at (col, row)
	std::set<std::size_t>& operator() (std::size_t row, std::size_t col, Type type) {
		return (type == Type_Source ? sourceCells : targetCells)[index(row, col)];
	}

	// Adds a constraint
	void constrain(std::size_t patchIndexA, std::size_t patchIndexB, GridConstriant::ConstraintType type) {
		this->constraints.emplace_back(patchIndexA, patchIndexB, type);
	}

	// Returns all neighbours in the grid tiles covered by the given patch's index
	std::set<std::size_t> neighbours(std::size_t patchIndex, Type type) {
		std::set<std::size_t> result;

		Bounds bounds = type == Type_Source ? patches[patchIndex].sourceRotatedBounds() : patches[patchIndex].targetBounds();
		GridRegion region = this->region(bounds, type);
		if (region.outside())
			return std::set<std::size_t>();

		for (std::size_t col = region.cols.start; col <= region.cols.end; ++col) {
			for (std::size_t row = region.rows.start; row <= region.rows.end; ++row) {
				std::size_t index = this->index(row, col);
				if (type & Type_Source)
					result.insert(sourceCells[index].begin(), sourceCells[index].end());
				if (type & Type_Target)
					result.insert(targetCells[index].begin(), targetCells[index].end());
			}
		}

		// Remove patchIndex
		auto iterator = result.find(patchIndex);
		if (iterator != result.end())
			result.erase(iterator);

		return result;
	}

	// Clears the regular grid of Type type
	void clear(Type type = Type_Source | Type_Target) {
		for (std::size_t row = 0; row < rows; row++) {
			for (std::size_t col = 0; col < cols; col++) {
				std::size_t index = this->index(row, col);
				if (type & Type_Target)
					targetCells[index] = std::set<std::size_t>();
				if (type & Type_Source)
					sourceCells[index] = std::set<std::size_t>();
			}
		}

		if (type & Type_Target)
			constraints.clear();

		if (type & (Type_Source | Type_Target))
			patches.clear();
	}

	// Emplaces a new patch in the patch list and inserts it into the regular grids
	template <typename... Args>
	void add(Args&&... args) {
		std::size_t index = this->patches.size();
		this->patches.emplace_back(std::forward<Args>(args)...);

		insert(index);
	}

	// Removes the patch at index from the patch list and the regular grid, this reinserts all patches 
	void erase(std::size_t index) {
		this->patches.erase(this->patches.begin() + index);
		clear();

		for (std::size_t patchIndex = 0; patchIndex < this->patches.size(); patchIndex++)
			insert(patchIndex);
	}

	// Removes a patch index from the regular grid region of Type type
	void erase(std::size_t patchIndex, const GridRegion& region, Type type) {
		for (std::size_t col = region.cols.start; col <= region.cols.end; ++col) {
			for (std::size_t row = region.rows.start; row <= region.rows.end; ++row) {
				std::size_t index = this->index(row, col);
				if (type & Type_Source)
					sourceCells[index].erase(patchIndex);
				if (type & Type_Target)
					targetCells[index].erase(patchIndex);
			}
		}
	}

	// Inserts a patch index into the regular grid region of Type type
	void insert(std::size_t patchIndex, const GridRegion& region, Type type) {
		if (region.outside()) {
			Log::debug("Region outside borders: (%d -> %d, %d -> %d)", region.cols.start, region.cols.end, region.rows.start, region.rows.end);
			return;
		}

		for (std::size_t col = region.cols.start; col <= region.cols.end; ++col) {
			for (std::size_t row = region.rows.start; row <= region.rows.end; ++row) {
				std::size_t index = this->index(row, col);
				if (type & Type_Source)
					sourceCells[index].insert(patchIndex);
				if (type & Type_Target)
					targetCells[index].insert(patchIndex);
			}
		}
	}

	// Inserts a patch index into the regular grid of both types
	void insert(std::size_t patchIndex) {
		GridRegion targetRegion = region(patches[patchIndex].targetBounds(), Type_Target);
		GridRegion sourceRegion = region(patches[patchIndex].sourceRotatedBounds(), Type_Source);

		insert(patchIndex, sourceRegion, Type_Source);
		insert(patchIndex, targetRegion, Type_Target);
	}

	// Updates the regular grid of Type type after a patch's regions change
	void swap(std::size_t patchIndex, const GridRegion& oldRegion, const GridRegion& newRegion, Type type) {
		GridRegion totalRegion = oldRegion + newRegion;

		for (std::size_t col = totalRegion.cols.start; col <= totalRegion.cols.end; ++col) {
			for (std::size_t row = totalRegion.rows.start; row <= totalRegion.rows.end; ++row) {
				GridCell currentCell(col, row);
				std::set<std::size_t>& patches = (type == Type_Source ? sourceCells : targetCells)[index(row, col)];

				bool inOldRegion = oldRegion.contains(currentCell);
				bool inNewRegion = newRegion.contains(currentCell);

				if (inOldRegion && !inNewRegion)
					patches.erase(patchIndex);

				if (inNewRegion && !inOldRegion)
					patches.insert(patchIndex);
			}
		}
	}

	// Updates the regular grid of Type type after a patch's regions change
	void update(std::size_t patchIndex, const Boundsi& oldBounds, const Boundsi& newBounds, Type type) {
		GridRegion oldRegion = region(oldBounds, type);
		GridRegion newRegion = region(newBounds, type);

		if (oldRegion.overlaps(newRegion)) {
			swap(patchIndex, oldRegion, newRegion, type);
		} else {
			erase(patchIndex, oldRegion, type);
			insert(patchIndex, newRegion, type);
		}
	}

	// Returns the grid cell of Type type that covers point
	GridCell cell(const Vec2i& point, Type type) const {
		return GridCell(
			cvFloor(static_cast<double>(point.x * cols) / static_cast<double>(type == Type_Source ? sourceDimension.x : targetDimension.x)),
			cvFloor(static_cast<double>(point.y * rows) / static_cast<double>(type == Type_Source ? sourceDimension.y : targetDimension.y))
		);
	}

	// Returns the grid region of Type type that is covered by bounds
	GridRegion region(const Boundsi& bounds, Type type) const {
		GridCell min = cell(bounds.min(), type);
		GridCell max = cell(bounds.imax(), type);

		return GridRegion(
			GridRange(Utils::max<int>(0, min.x), Utils::min<int>(max.x, cols - 1)),
			GridRange(Utils::max<int>(0, min.y), Utils::min<int>(max.y, rows - 1))
		);
	}

	// Converts a row and column index to a list index
	constexpr std::size_t index(std::size_t row, std::size_t col) const {
		return row * cols + col;
	}

	// Return whether bounds overlap any patch in the regular grid of Type type
	bool overlaps(std::size_t patchIndex, const std::size_t ignore, Type type) const {
		if (type & Type_Target) {
			if (overlaps(patches[patchIndex].targetBounds(), &this->patches[ignore], type))
				return true;
		}

		if (type & Type_Source) {
			
		}

		return false;
	}

	// Return whether teh patch overlaps any other patch in the regular grid of Type type
	bool overlaps(const MondriaanPatch& patch, Type type, const MondriaanPatch* ignore = nullptr) const {
		// Check for source overlap
		if (type & Type_Source) {
			std::vector<Vec2> patchSourcePoints = patch.sourceRotatedPoints();
			GridRegion cellRegion = region(patch.sourceRotatedBounds(), type);

			// Collect all unique patches
			std::set<std::size_t> uniquePatches;
			for (std::size_t row = cellRegion.rows.start; row <= cellRegion.rows.end; ++row) {
				for (std::size_t col = cellRegion.cols.start; col <= cellRegion.cols.end; ++col) {
					const std::set<std::size_t>& patches = sourceCells[index(row, col)];

					uniquePatches.insert(patches.begin(), patches.end());
				}
			}

			for (std::size_t patchIndex : uniquePatches) {
				const MondriaanPatch& currentPatch = this->patches[patchIndex];
				if (&currentPatch == &patch || &currentPatch == ignore)
					continue;

				std::vector<Vec2> currenPatchSourcePoints = currentPatch.sourceRotatedPoints();
				if (Sat::separating_axis_intersect(patchSourcePoints, currenPatchSourcePoints))
					return true;
			}
		}

		// Check for target overlap
		if (type & Type_Target) {
			Bounds patchTargetBounds = patch.targetBounds();
			GridRegion cellRegion = region(patch.sourceRotatedBounds(), type);
			
			// Collect all unique patches
			std::set<std::size_t> uniquePatches;
			for (std::size_t row = cellRegion.rows.start; row <= cellRegion.rows.end; ++row) {
				for (std::size_t col = cellRegion.cols.start; col <= cellRegion.cols.end; ++col) {
					const std::set<std::size_t>& patches = targetCells[index(row, col)];

					uniquePatches.insert(patches.begin(), patches.end());
				}
			}

			for (std::size_t patchIndex : uniquePatches) {
				const MondriaanPatch& currentPatch = this->patches[patchIndex];
				if (&currentPatch == &patch || &currentPatch == ignore)
					continue;

				Bounds currentPatchTargetBounds = currentPatch.targetBounds();
				if (patchTargetBounds.ioverlaps(currentPatchTargetBounds))
					return true;
			}
		}

		return false;
	}

	// Renders the regular grid
	void render(Canvas& canvas, Type type) {
		for (std::size_t col = 0; col < cols; ++col) {
			ImGui::GetWindowDrawList()->AddLine(
				canvas.offset + Utils::transform(Vec2(col, 0), Vec2(cols, rows), canvas.screenDimension()).iv(),
				canvas.offset + Utils::transform(Vec2(col, rows), Vec2(cols, rows), canvas.screenDimension()).iv(),
				Colors::WHITE.u32()
			);
		}

		for (std::size_t row = 0; row < rows; ++row) {
			ImGui::GetWindowDrawList()->AddLine(
				canvas.offset + Utils::transform(Vec2(0, row), Vec2(cols, rows), canvas.screenDimension()).iv(),
				canvas.offset + Utils::transform(Vec2(cols, row), Vec2(cols, rows), canvas.screenDimension()).iv(),
				Colors::WHITE.u32()
			);
		}

		for (std::size_t col = 0; col < cols; ++col) {
			for (std::size_t row = 0; row < rows; ++row) {
				Color color = Colors::RGB_R.withOpacity(0.5 * Utils::min(1.0, static_cast<double>((type == Type_Source ? sourceCells : targetCells)[index(row, col)].size()) / 4.0));

				ImGui::GetWindowDrawList()->AddRectFilled(
					canvas.offset + Utils::transform(Vec2(col, row), Vec2(cols, rows), canvas.screenDimension()).iv(),
					canvas.offset + Utils::transform(Vec2(col + 1, row + 1), Vec2(cols, rows), canvas.screenDimension()).iv(),
					color.u32()
				);
			}
		}

	}
};
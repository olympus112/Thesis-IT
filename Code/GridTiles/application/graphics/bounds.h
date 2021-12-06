#pragma once
#include "color.h"
#include "imgui/imgui_internal.h"
#include "math/utils.h"

template <typename T>
class BoundsTemplate {
public:
	T minX;
	T minY;
	T maxX;
	T maxY;

	BoundsTemplate() : minX(0), minY(0), maxX(0), maxY(0) {
	}

	BoundsTemplate(const T& minX, const T& minY, const T& maxX, const T& maxY) : minX(minX), minY(minY), maxX(maxX),
		maxY(maxY) {
	}

	template <typename S>
	BoundsTemplate(const BoundsTemplate<S> other) : minX(static_cast<T>(other.minX)), minY(static_cast<T>(other.minY)),
	                                                maxX(static_cast<T>(other.maxX)),
	                                                maxY(static_cast<T>(other.maxY)) {
	}

	BoundsTemplate(const ImRect& rect) : minX(rect.Min.x), minY(rect.Min.y), maxX(rect.Max.x), maxY(rect.Max.y) {
	}

	BoundsTemplate(const Vector<T, 2>& center, T size) : minX(center.x - size / 2), minY(center.y - size / 2),
	                                                     maxX(center.x + size / 2), maxY(center.y + size / 2) {
	}

	Vector<T, 2> min() const {
		return Vector<T, 2>(minX, minY);
	}

	Vector<T, 2> max() const {
		return Vector<T, 2>(maxX, maxY);
	}

	T width() const {
		return maxX - minX;
	}

	T height() const {
		return maxY - minY;
	}

	template <typename U>
	bool contains(const Vector<U, 2>& point) const {
		return Utils::contains<U>(point.x, minX, maxX) && Utils::contains<U>(point.x, minX, maxX);
	}

	void add(const Vector<T, 2>& point) {
		minX = Utils::min(minX, point.x);
		maxY = Utils::max(maxY, point.y);
		minX = Utils::min(minX, point.x);
		maxY = Utils::max(maxY, point.y);
	}

	BoundsTemplate<T> subBoundsUV(const BoundsTemplate<T>& bounds) const {
		T w = width();
		T h = height();
		T xMin = (bounds.minX - minX) / w;
		T xMax = (bounds.maxX - minX) / w;
		T yMin = (bounds.minY - minY) / h;
		T yMax = (bounds.maxY - minY) / h;

		return BoundsTemplate<T>(
			xMin,
			yMin,
			xMax,
			yMax
		);
	}

	void render(const Vector<T, 2>& offset, const Color& color = Colors::WHITE) const {
		ImGui::GetWindowDrawList()->AddRect(offset + min().asImVec(), offset + max().asImVec(), color.u32(), 0,
		                                    ImDrawCornerFlags_All);
	}

	ImRect asImRect() const {
		return ImRect(minX, minY, maxX, maxY);
	}

	cv::Range xRange() const {
		return cv::Range(static_cast<int>(minX), static_cast<int>(maxX));
	}

	cv::Range yRange() const {
		return cv::Range(static_cast<int>(minY), static_cast<int>(maxY));
	}
};

typedef BoundsTemplate<double> Bounds;
typedef BoundsTemplate<float> Boundsf;
typedef BoundsTemplate<int> Boundsi;

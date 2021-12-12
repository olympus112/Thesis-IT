#pragma once

#include "canvas.h"
#include "color.h"
#include "imgui/imgui_internal.h"
#include "math/utils.h"

template <typename T>
class BoundsTemplate {
public:
	constexpr static int TL = 0;
	constexpr static int TR = 1;
	constexpr static int BR = 2;
	constexpr static int BL = 3;

public:
	T minX;
	T minY;
	T maxX;
	T maxY;

	BoundsTemplate() : minX(0)
	                 , minY(0)
	                 , maxX(0)
	                 , maxY(0) { }

	BoundsTemplate(const T& size) : minX(-size / 2)
	                              , minY(-size / 2)
	                              , maxX(size / 2)
	                              , maxY(size / 2) { }

	BoundsTemplate(const T& width, const T& height) : minX(-width / 2)
	                                                , minY(-height / 2)
	                                                , maxX(width / 2)
	                                                , maxY(height / 2) { }

	BoundsTemplate(const T& minX, const T& minY, const T& maxX, const T& maxY) : minX(minX)
	                                                                           , minY(minY)
	                                                                           , maxX(maxX)
	                                                                           , maxY(maxY) { }

	template <typename S>
	BoundsTemplate(const BoundsTemplate<S> other) : minX(static_cast<T>(other.minX))
	                                              , minY(static_cast<T>(other.minY))
	                                              , maxX(static_cast<T>(other.maxX))
	                                              , maxY(static_cast<T>(other.maxY)) { }

	BoundsTemplate(const ImRect& rect) : minX(rect.Min.x)
	                                   , minY(rect.Min.y)
	                                   , maxX(rect.Max.x)
	                                   , maxY(rect.Max.y) { }

	BoundsTemplate(const Vector<T, 2>& center, const T& size) : minX(center.x - size / 2)
	                                                          , minY(center.y - size / 2)
	                                                          , maxX(center.x + size / 2)
	                                                          , maxY(center.y + size / 2) { }

	BoundsTemplate(const Vector<T, 2>& center, const T& width, const T& height) : minX(center.x - width / 2)
		, minY(center.y - height / 2)
		, maxX(center.x + width / 2)
		, maxY(center.y + height / 2) { }

	BoundsTemplate(const Vector<T, 2>& tl, const Vector<T, 2>& br) : minX(tl.x)
	                                                               , minY(tl.y)
	                                                               , maxX(br.x)
	                                                               , maxY(br.y) { }

	const Vector<T, 2>& operator[](int index) const {
		switch (index) {
		case TL:
			return tl();
		case TR:
			return tr();
		case BR:
			return br();
		case BL:
			return bl();
		}

		assert(false);
		return Vector<T, 2>();
	}

	constexpr Vector<T, 2> min() const {
		return tl();
	}

	constexpr Vector<T, 2> max() const {
		return br();
	}

	constexpr Vector<T, 2> tr() const {
		return Vector<T, 2>(maxX, minY);
	}

	constexpr Vector<T, 2> tl() const {
		return Vector<T, 2>(minX, minY);
	}

	constexpr Vector<T, 2> br() const {
		return Vector<T, 2>(maxX, maxY);
	}

	constexpr Vector<T, 2> bl() const {
		return Vector<T, 2>(minX, maxY);
	}

	constexpr T width() const {
		return maxX - minX;
	}

	constexpr T height() const {
		return maxY - minY;
	}

	constexpr Vector<T, 2> dimension() const {
		return Vector<T, 2>(width(), height());
	}

	template <typename U>
	constexpr bool contains(const Vector<U, 2>& point) const {
		return Utils::contains<T>(static_cast<T>(point.x), minX, maxX) && Utils::contains<T>(static_cast<T>(point.y), minY, maxY);
	}

	constexpr void move(const Vector<T, 2>& offset) {
		minX += offset.x;
		minY += offset.y;
		maxX += offset.x;
		maxY += offset.y;
	}

	constexpr void add(const Vector<T, 2>& point) {
		minX = Utils::min(minX, point.x);
		minY = Utils::min(minY, point.y);
		maxX = Utils::max(maxX, point.x);
		maxY = Utils::max(maxY, point.y);
	}

	constexpr BoundsTemplate<T> subBoundsUV(const BoundsTemplate<T>& bounds) const {
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

	void render(const Canvas& canvas, const Color& color = Colors::WHITE) const {
		ImGui::GetWindowDrawList()->AddRect(canvas.offset + min().iv(), canvas.offset + max().iv(), color.u32(), 0,
		                                    ImDrawCornerFlags_All);
	}

	constexpr ImRect ir() const {
		return ImRect(minX, minY, maxX, maxY);
	}

	constexpr cv::Range xRange() const {
		return cv::Range(static_cast<int>(minX), static_cast<int>(maxX));
	}

	constexpr cv::Range yRange() const {
		return cv::Range(static_cast<int>(minY), static_cast<int>(maxY));
	}
};

typedef BoundsTemplate<double> Bounds;
typedef BoundsTemplate<float> Boundsf;
typedef BoundsTemplate<int> Boundsi;

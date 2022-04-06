#pragma once

#include "color.h"
#include "imgui/imgui_internal.h"
#include "canvas.h"
#include "math/utils.h"

template <typename T>
class BoundsTemplate {
public:
	constexpr static int TL = 0;
	constexpr static int TR = 1;
	constexpr static int BR = 2;
	constexpr static int BL = 3;

public:
	T x;
	T y;
	T w;
	T h;

	BoundsTemplate()
		: x(0)
		, y(0)
		, w(0)
		, h(0) { }

	BoundsTemplate(const T& size)
		: x(-size / 2)
		, y(-size / 2)
		, w(size)
		, h(size) { }

	BoundsTemplate(const T& width, const T& height)
		: x(-width / 2)
		, y(-height / 2)
		, w(width)
		, h(height) { }

	BoundsTemplate(const T& x, const T& y, const T& width, const T& height)
		: x(x)
		, y(y)
		, w(width)
		, h(height) { }

	template <typename S>
	BoundsTemplate(const BoundsTemplate<S> other)
		: x(static_cast<T>(other.x))
		, y(static_cast<T>(other.y))
		, w(static_cast<T>(other.w))
		, h(static_cast<T>(other.h)) { }

	BoundsTemplate(const cv::Rect& rect)
		: x(rect.x)
		, y(rect.y)
		, w(rect.width)
		, h(rect.height) {}

	BoundsTemplate(const ImRect& rect)
		: x(rect.Min.x)
		, y(rect.Min.y)
		, w(rect.Max.x - rect.Min.x)
		, h(rect.Max.y - rect.Min.y) { }

	BoundsTemplate(const Vector<T, 2>& center, const T& size)
		: x(center.x - size / 2)
		, y(center.y - size / 2)
		, w(size)
		, h(size) { }

	BoundsTemplate(const Vector<T, 2>& tl, const T& width, const T& height)
		: x(tl.x)
		, y(tl.y)
		, w(width)
		, h(height) { }

	BoundsTemplate(const Vector<T, 2>& tl, const Vector<T, 2>& dimension)
		: x(tl.x)
		, y(tl.y)
		, w(dimension.x)
		, h(dimension.y) { }

	// Clamps a given bounds within a clamp region, inclusive
	static BoundsTemplate<T> iclamped(const BoundsTemplate<T>& bounds, const BoundsTemplate<T>& clamp) {
		T newX = Utils::clamp<T>(bounds.minX(), clamp.minX(), clamp.imaxX());
		T newY = Utils::clamp<T>(bounds.minY(), clamp.imaxY(), clamp.imaxY());
		T newW = Utils::clamp<T>(bounds.imaxX(), clamp.minX(), clamp.imaxX()) - newX;
		T newH = Utils::clamp<T>(bounds.imaxY(), clamp.minY(), clamp.imaxY()) - newY;

		return BoundsTemplate<T>(newX, newY, newW, newH);
	}

	// Clamps a given bounds within a clamp region, exclusive
	static BoundsTemplate<T> eclamped(const BoundsTemplate<T>& bounds, const BoundsTemplate<T>& clamp) {
		T newX = Utils::clamp<T>(bounds.minX(), clamp.minX(), clamp.emaxX());
		T newY = Utils::clamp<T>(bounds.minY(), clamp.emaxY(), clamp.emaxY());
		T newW = Utils::clamp<T>(bounds.emaxX(), clamp.minX(), clamp.emaxX()) - newX;
		T newH = Utils::clamp<T>(bounds.emaxY(), clamp.minY(), clamp.emaxY()) - newY;

		return BoundsTemplate<T>(newX, newY, newW, newH);
	}

	// Returns the vertex at the given index, clockwise starting at the top left, exclusive
	const Vector<T, 2>& operator[](int index) const {
		switch (index) {
		case TL:
			return tl();
		case TR:
			return etr();
		case BR:
			return ebr();
		case BL:
			return ebl();
		}

		assert(false);
	}

	// Returns the topleft point of the bounds, inclusive
	constexpr Vector<T, 2> min() const {
		return tl();
	}

	// Returns the bottom right point of the bounds, inclusive
	constexpr Vector<T, 2> imax() const {
		return ibr();
	}

	// Returns the bottom right point of the bounds, exclusive
	constexpr Vector<T, 2> emax() const {
		return ebr();
	}

	// Returns top right point of the bounds, inclusive
	constexpr Vector<T, 2> itr() const {
		return Vector<T, 2>(imaxX(), minY());
	}

	// Returns top right point of the bounds, exclusive
	constexpr Vector<T, 2> etr() const {
		return Vector<T, 2>(emaxX(), minY());
	}

	// Returns top left point of the bounds, inclusive
	constexpr Vector<T, 2> tl() const {
		return Vector<T, 2>(minX(), minY());
	}

	// Returns bottom right point of the bounds, inclusive
	constexpr Vector<T, 2> ibr() const {
		return Vector<T, 2>(imaxX(), imaxY());
	}

	// Returns bottom right point of the bounds, exclusive
	constexpr Vector<T, 2> ebr() const {
		return Vector<T, 2>(emaxX(), emaxY());
	}

	// Returns bottom left point of the bounds, inclusive
	constexpr Vector<T, 2> ibl() const {
		return Vector<T, 2>(minX(), imaxY());
	}

	// Returns bottom left point of the bounds, exclusive
	constexpr Vector<T, 2> ebl() const {
		return Vector<T, 2>(minX(), emaxY());
	}

	// Returns the leftmost coordinate of the bounds
	constexpr T minX() const {
		return x;
	}

	// Returns the topmost coordinate of the bounds
	constexpr T minY() const {
		return y;
	}

	// Returns the rightmost coordinate of the bounds, inclusive
	constexpr T imaxX() const {
		return minX() + Utils::max<T>(0, width() - 1);
	}

	// Returns the bottommost coordinate of the bounds, inclusive
	constexpr T imaxY() const {
		return minY() + Utils::max<T>(0, height() - 1);
	}

	// Returns the rightmost coordinate of the bounds, exclusive
	constexpr T emaxX() const {
		return minX() + width();
	}

	// Returns the bottommost coordinate of the bounds, exclusive
	constexpr T emaxY() const {
		return minY() + height();
	}

	// Returns the width of the bounds
	constexpr T width() const {
		return w;
	}

	// Returns the height of the bounds
	constexpr T height() const {
		return h;
	}

	// Returns the dimension of the bounds
	constexpr Vector<T, 2> dimension() const {
		return Vector<T, 2>(width(), height());
	}

	// Returns a vector containing the corner points of this bounding box, inclusive
	constexpr std::vector<Vector<T, 2>> ipoints() const {
		return std::vector {
			tl(),
			itr(),
			ibr(),
			ibl()
		};
	}

	// Returns a vector containing the corner points of this bounding box, exclusive
	constexpr std::vector<Vector<T, 2>> epoints() const {
		return std::vector {
			tl(),
			etr(),
			ebr(),
			ebl()
		};
	}

	// Returns whether the bounds contain the given point, inclusive
	template <typename U>
	constexpr bool icontains(const Vector<U, 2>& point) const {
		return Utils::contains<T>(static_cast<T>(point.x), minX(), imaxX()) && Utils::contains<T>(static_cast<T>(point.y), minY(), imaxY());
	}

	// Returns whether the bounds contain the given point, exclusive
	template <typename U>
	constexpr bool econtains(const Vector<U, 2>& point) const {
		return Utils::contains<T>(static_cast<T>(point.x), minX(), emaxX()) && Utils::contains<T>(static_cast<T>(point.y), minY(), emaxY());
	}

	// Returns whether these bounds overlap with the given bounds, inclusive
	constexpr bool ioverlaps(const BoundsTemplate<T>& other) const {
		return minX() <= other.imaxX() && imaxX() >= other.minX() && minY() <= other.imaxY() && imaxY() >= other.minY();
	}

	// Returns whether these bounds overlap with the given bounds, exclusive
	constexpr bool eoverlaps(const BoundsTemplate<T>& other) const {
		return minX() <= other.emaxX() && emaxX() >= other.minX() && minY() <= other.emaxY() && emaxY() >= other.minY();
	}

	// Returns whether these bounds contain the given bounds, inclusive
	constexpr bool icontains(const BoundsTemplate<T>& other) const {
		return other.minX() >= minX() && other.imaxX() <= imaxX() && other.minY() >= minY() && other.imaxY() <= imaxY();
	}

	// Returns whether these bounds contain the given bounds, exclusive
	constexpr bool econtains(const BoundsTemplate<T>& other) const {
		return other.minX() >= minX() && other.emaxX() <= emaxX() && other.minY() >= minY() && other.emaxY() <= emaxY();
	}

	constexpr bool distance(const BoundsTemplate<T>& other) const {
		Vector<T, 2> dc = center() - other.center();
		Vector<T, 2> de = dimension() + other.dimension();
		Vector<T, 2> result = dc.abs() - de / 2.0;
		return length(Vector<T, 2>(std::max(0, result.x), std::max(0, result.y)));
	}

	constexpr void move(const Vector<T, 2>& offset) {
		x += offset.x;
		y += offset.y;
	}

	constexpr void iadd(const Vector<T, 2>& point) {
		x = Utils::min<T>(minX(), point.x);
		y = Utils::min<T>(minY(), point.y);
		w = Utils::max<T>(imaxX(), point.x) - minX();
		h = Utils::max<T>(imaxY(), point.y) - minY();
	}

	constexpr void eadd(const Vector<T, 2>& point) {
		x = Utils::min<T>(minX(), point.x);
		y = Utils::min<T>(minY(), point.y);
		w = Utils::max<T>(emaxX(), point.x) - minX();
		h = Utils::max<T>(emaxY(), point.y) - minY();
	}

	constexpr BoundsTemplate<T> subBoundsUV(const BoundsTemplate<T>& bounds) const {
		T xMin = (bounds.minX() - minX()) / width();
		T xMax = (bounds.emaxX() - minX()) / width();
		T yMin = (bounds.minY() - minY()) / height();
		T yMax = (bounds.emaxY() - minY()) / height();

		return BoundsTemplate<T>(xMin, yMin, xMax - xMin, yMax - yMin);
	}

	// Render bounds in pixel space
	void render(const Canvas& canvas, const Color& color = Colors::WHITE) const {
		ImGui::GetWindowDrawList()->AddRect(canvas.toAbsoluteScreenSpace(min()).iv(),
		                                    canvas.toAbsoluteScreenSpace(emax()).iv(),
		                                    color.u32(),
		                                    0,
		                                    ImDrawCornerFlags_All);
	}

	constexpr ImRect ir() const {
		return ImRect(minX(), minY(), emaxX(), emaxY());
	}

	constexpr cv::Rect cv() const {
		return cv::Rect(minX(), minY(), width(), height());
	}

	// cv::Range is non inclusive on the right
	constexpr cv::Range xRange() const {
		return cv::Range(static_cast<int>(minX()), static_cast<int>(emaxX()));
	}

	// cv::Range is non inclusive on the right
	constexpr cv::Range yRange() const {
		return cv::Range(static_cast<int>(minY()), static_cast<int>(emaxY()));
	}

	Vector<T, 2> center() {
		return Vector<T, 2>(minX() + width() / 2.0, minY() + height() / 2.0);
	}
};

typedef BoundsTemplate<double> Bounds;
typedef BoundsTemplate<float> Boundsf;
typedef BoundsTemplate<int> Boundsi;

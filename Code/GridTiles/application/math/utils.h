#pragma once

#include <numeric>

#include "graphics/canvas.h"

namespace Utils {

	template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	constexpr Vector<T, 2> transform(const Vector<T, 2>& vector, const Vector<T, 2>& inputDimension,
	                                            const Vector<T, 2>& outputDimension) {
		return Vector<T, 2>(
			vector.x / inputDimension.x * outputDimension.x,
			vector.y / inputDimension.y * outputDimension.y
		);
	}

	template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	constexpr Vector<T, 2> transform(const Vector<T, 2>& vector, const Canvas& source, const Canvas& target) {
		return Vector<T, 2>(
			vector.x / source.textureDimension().x * target.textureDimension().x,
			vector.y / source.textureDimension().y * target.textureDimension().y
		);
	}

	template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	constexpr T max(const T& a, const T& b) {
		return a >= b ? a : b;
	}

	template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	constexpr T min(const T& a, const T& b) {
		return a <= b ? a : b;
	}

	template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	constexpr T clamp(const T& value, const T& a, const T& b) {
		return max(a, min(value, b));
	}

	template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	constexpr bool between(const T& value, const T& a, const T& b) {
		return a < value && value < b;
	}

	template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	constexpr bool contains(const T& value, const T& a, const T& b) {
		return a <= value && value <= b;
	}
}

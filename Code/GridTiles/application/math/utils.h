#pragma once

#include <numeric>

namespace Utils {

	template <typename T> requires std::integral<T> || std::floating_point<T>
	constexpr Vector<T, 2> screenToTextureSpace(const Vector<T, 2>& vector, const Vector<T, 2>& screenDimension,
	                                            const Vector<T, 2>& textureDimension) {
		return Vector<T, 2>(
			vector.x / screenDimension.x * textureDimension.x,
			vector.y / screenDimension.y * textureDimension.y
		);
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	constexpr Vector<T, 2> textureToScreenSpace(const Vector<T, 2>& vector, const Vector<T, 2>& textureDimension,
	                                            const Vector<T, 2>& screenDimension) {
		return Vector<T, 2>(
			vector.x / textureDimension.x * screenDimension.x,
			vector.y / textureDimension.y * screenDimension.y
		);
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	constexpr T max(const T& a, const T& b) {
		return a >= b ? a : b;
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	constexpr T min(const T& a, const T& b) {
		return a <= b ? a : b;
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	constexpr T clamp(const T& value, const T& a, const T& b) {
		return max(a, min(value, b));
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	constexpr bool between(const T& value, const T& a, const T& b) {
		return a < value && value < b;
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	constexpr bool contains(const T& value, const T& a, const T& b) {
		return a <= value && value <= b;
	}
}

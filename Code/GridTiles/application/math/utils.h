#pragma once

#include <numeric>

namespace Utils {
	template <typename T> requires std::integral<T> || std::floating_point<T>
	T max(const T& a, const T& b) {
		return a >= b ? a : b;
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	T min(const T& a, const T& b) {
		return a <= b ? a : b;
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	T clamp(const T& value, const T& a, const T& b) {
		return max(a, min(value, b));
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	bool between(const T& value, const T& a, const T& b) {
		return a < value && value < b;
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	bool contains(const T & value, const T & a, const T & b) {
		return a <= value && value <= b;
	}
}

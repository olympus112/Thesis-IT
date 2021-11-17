#pragma once

#include <numeric>

namespace Utils {
	template <typename T> requires std::integral<T> || std::floating_point<T>
	T max(T a, T b) {
		return a >= b ? a : b;
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	T min(T a, T b) {
		return a <= b ? a : b;
	}

	template <typename T> requires std::integral<T> || std::floating_point<T>
	T clamp(T a, T value, T b) {
		return max(a, min(value, b));
	}
}

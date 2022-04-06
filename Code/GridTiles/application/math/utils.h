#pragma once

#include <numeric>
#include <random>

namespace Utils {

	inline std::vector<std::size_t> nUniqueRandomSizeTypesInRange(std::mt19937& generator, std::size_t n, std::size_t start, std::size_t end) {
		assert(n <= end - start);

		std::vector<std::size_t> result(end - start);
		for (std::size_t i = start; i < end; i++)
			result[i] = i;

		std::ranges::shuffle(result, generator);

		return std::vector(result.begin(), result.begin() + n);
	}

	inline std::vector<int> nUniqueRandomIntegersInRange(std::mt19937& generator, int n, int start, int end) {
		assert(n <= end - start);
		assert(end >= 0 && start >=0 && n > 0);

		std::vector<int> result(end - start);
		for (int i = start; i < end; i++)
			result[i] = i;

		std::ranges::shuffle(result, generator);

		return std::vector(result.begin(), result.begin() + n);
	}

	inline bool randBool(std::mt19937& generator) {
		std::uniform_int_distribution distribution(0, 1);
		return distribution(generator) == 0;
	}

	inline float randomUnsignedFloatInRange(std::mt19937& generator, float start, float end) {
		std::uniform_real_distribution distribution(start, end);

		return distribution(generator);
	}

	inline float randomSignedFloatInRange(std::mt19937& generator, float start, float end, float sign) {
		float number = randomUnsignedFloatInRange(generator, start, end);
		bool negative = randomUnsignedFloatInRange(generator, 0.0f, 1.0f) < sign;

		return negative ? -number : number;
	}

	template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	constexpr Vector<T, 2> transform(const Vector<T, 2>& vector, const Vector<T, 2>& inputDimension,
	                                            const Vector<T, 2>& outputDimension) {
		return Vector<T, 2>(
			vector.x / inputDimension.x * outputDimension.x,
			vector.y / inputDimension.y * outputDimension.y
		);
	}

	//template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
	//constexpr Vector<T, 2> transform(const Vector<T, 2>& vector, const Canvas& source, const Canvas& target) {
	//	return Vector<T, 2>(
	//		vector.x / source.textureDimension().x * target.textureDimension().x,
	//		vector.y / source.textureDimension().y * target.textureDimension().y
	//	);
	//}

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

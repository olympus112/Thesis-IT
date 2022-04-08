#pragma once

#include <numeric>
#include <random>
#include <opencv2/core/mat.hpp>

namespace Utils {

void matchTemplate(const cv::Mat& image, const cv::Mat& templ, cv::Mat& result, int method, const cv::Mat& templMask, const cv::Mat& globalMask);

std::vector<std::size_t> nUniqueRandomSizeTypesInRange(std::mt19937& generator, std::size_t n, std::size_t start, std::size_t end);
std::vector<std::size_t> nUniqueSampledSizeTypesInRange(std::mt19937& generator, std::size_t n, std::size_t start, std::size_t end, float (*pdf)(float));

std::vector<int> nUniqueRandomIntegersInRange(std::mt19937& generator, int n, int start, int end);
std::vector<int> nUniqueSampledIntegersInRange(std::mt19937& generator, int n, int start, int end, float (*pdf)(float));


bool randomBool(std::mt19937& generator);

float randomUnsignedFloatInRange(std::mt19937& generator, float start, float end);

float randomSignedFloatInRange(std::mt19937& generator, float start, float end, float sign);

template <typename T> /*requires std::integral<T> || std::floating_point<T>*/
constexpr Vector<T, 2> transform(const Vector<T, 2>& vector, const Vector<T, 2>& inputDimension, const Vector<T, 2>& outputDimension) {
	return Vector<T, 2>(vector.x / inputDimension.x * outputDimension.x, vector.y / inputDimension.y * outputDimension.y);
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

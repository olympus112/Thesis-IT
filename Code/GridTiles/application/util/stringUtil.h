#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace Util {

	std::vector<std::string> split(const std::string& string, char splitter);

	bool startsWith(const std::string& string, const std::string& prefix);
	bool endsWith(const std::string& string, const std::string& suffix);

	std::string until(const std::string& string, char end);

	std::string ltrim(std::string string);
	std::string rtrim(std::string string);
	std::string trim(const std::string& string);

	template<typename T>
	std::string str(const T& value) {
		return value;
	}

	template<>
	inline std::string str(const Vec2f& value) {
		std::stringstream ss;
		ss << "{ " << value.x << ", " << value.y << " }";
		return ss.str();
	}
};

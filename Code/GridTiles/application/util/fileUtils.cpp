#include <core.h>
#include "fileUtils.h"

#include "log.h"
#include <fstream>
#include <sstream>


namespace Util {

	bool doesFileExist(const std::string& fileName) {
		struct stat buffer;
		if (stat(fileName.c_str(), &buffer) != -1) {
			return true;
		}
		return false;
	}

	void warnIfFileExists(const std::string& fileName) {
		if (doesFileExist(fileName)) {
			Log::warn("File already exists: %s", fileName.c_str());
		}
	}

	std::string parseFile(const std::string& path) {
		if (path.empty())
			return std::string();

		std::ifstream fileStream(path);

		Log::setDelimiter("");
		Log::info("Reading (%s) ... ", path.c_str());

		if (fileStream.fail() || !fileStream.is_open()) {
			Log::error("Fail");
			Log::setDelimiter("\n");

			return "";
		}

		Log::debug("Done");
		Log::setDelimiter("\n");

		std::string line;
		std::stringstream stringStream;
		while (getline(fileStream, line))
			stringStream << line << "\n";

		fileStream.close();

		return stringStream.str();
	}

};

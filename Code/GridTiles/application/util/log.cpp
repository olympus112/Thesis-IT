#include <core.h>

#include "log.h"

#include <iostream>
#include <cstdarg>

#include "terminalColor.h"

namespace Log {
	static Level logLevel = Level::INFO;
	static std::string delimiter = "\n";

	// the va_list must be recreated for both print and fprint
	#define va_logf(format, message) \
		va_list args; va_start(args, format); \
		vprintf(message.c_str(), args); va_end(args);

	void setLogLevel(Level logLevel) {
		Log::logLevel = logLevel;
	}

	Level getLogLevel() {
		return logLevel;
	}

	void setDelimiter(const std::string& delimiter) {
		Log::delimiter = delimiter;
	}

	void debug(std::string format, ...) {
		if (logLevel != Level::NONE) {
			std::string message = "[DEBUG]: " + format + delimiter;
			setColor(Color::DEBUG);
			va_logf(format, message);
			setColor(Color::NORMAL);
		}
	}

	void info(std::string format, ...) {
		if (logLevel <= Level::INFO) {
			std::string message = "[INFO]: " + format + delimiter;
			setColor(Color::INFO);
			va_logf(format, message);
			setColor(Color::NORMAL);
		}
	}

	void warn(std::string format, ...) {
		if (logLevel <= Level::WARNING) {
			std::string message = "[WARN]: " + format + delimiter;
			setColor(Color::WARNING);
			va_logf(format, message);
			setColor(Color::NORMAL);
		}
	}

	void error(std::string format, ...) {
		if (logLevel <= Level::ERROR) {
			std::string message = "[ERROR]: " + format + delimiter;
			setColor(Color::ERROR);
			va_logf(format, message);
			setColor(Color::NORMAL);
		}
	}

	void fatal(std::string format, ...) {
		if (logLevel <= Level::FATAL) {
			std::string message = "[FATAL]: " + format + delimiter;
			setColor(Color::FATAL);
			va_logf(format, message);
			setColor(Color::NORMAL);
		}
	}

	void print(std::string format,  ...) {
		setColor(Color::NORMAL);
		va_logf(format, format);
	}

	void print(TerminalColorPair color, std::string format, ...) {
		setColor(color);
		va_logf(format, format);
	}
}

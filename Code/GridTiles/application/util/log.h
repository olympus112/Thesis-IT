#pragma once

#include <string>

#include "terminalColor.h"

#define LOG_DEBUG(FMT, ...) Log::debug(std::string("[%s:%d] ") + FMT, __FUNCTION__, __LINE__, __VA_ARGS__);

namespace Log {
	
	namespace Color {
		constexpr TerminalColorPair DEBUG   { TerminalColor::GREEN  , TerminalColor::BLACK};
		constexpr TerminalColorPair INFO    { TerminalColor::CYAN   , TerminalColor::BLACK};
		constexpr TerminalColorPair WARNING { TerminalColor::YELLOW , TerminalColor::BLACK};
		constexpr TerminalColorPair ERROR   { TerminalColor::RED    , TerminalColor::BLACK};
		constexpr TerminalColorPair FATAL   { TerminalColor::BLACK  , TerminalColor::RED  };
		constexpr TerminalColorPair NORMAL  { TerminalColor::WHITE  , TerminalColor::BLACK};
		constexpr TerminalColorPair SUBJECT { TerminalColor::MAGENTA, TerminalColor::BLACK};
	}

	enum class Level : char {
		INFO    = 0,
		WARNING = 1,
		ERROR   = 2,
		FATAL   = 3,
		NONE    = 4
	};

	void setDelimiter(const std::string& delimiter);

	void debug(std::string format, ...);
	void info(std::string format, ...);
	void warn(std::string format, ...);
	void error(std::string format, ...);
	void fatal(std::string format, ...);
	void print(std::string format, ...);
	void print(TerminalColorPair color, std::string format, ...);

	void setLogLevel(Level logLevel);
	Level getLogLevel();
};

#include "Logger.h"

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;
#include <iomanip>
#include <ctime>
#include <sstream>

bsExp::Logger::Logger()
{
	const auto time = std::time(nullptr);
	const auto localTime = *std::localtime(&time);

	std::ostringstream newLogNameStream;
	newLogNameStream << "experimentData/";
	newLogNameStream << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S");
	newLogNameStream << ".log";
	pLogger_ = spdlog::basic_logger_mt("Logger", newLogNameStream.str());
}

void bsExp::Logger::LogDelimiter()
{
	pLogger_->info("========");
	std::cout << "========\n";
}

void bsExp::Logger::LogControllerPosition(const char* deviceName, const bs::CartesianCoord coord)
{
	pLogger_->info("Position of {0} is: ({1:03.2f};{2:03.2f};{3:03.2f})", deviceName, coord.x, coord.y, coord.y);

	std::string s = "Position of ";
	s += deviceName;
	s += " is: (";
	s += std::to_string(coord.x);
	s += ";";
	s += std::to_string(coord.y);
	s += ";";
	s += std::to_string(coord.z);
	s += ").\n";
	std::cout << s;
}

void bsExp::Logger::LogRendererChange(const char* rendererName)
{
	pLogger_->info("Selected {} renderer.", rendererName);

	std::string s = "Selected ";
	s += rendererName;
	s += " renderer.\n";
	std::cout << s;
}

void bsExp::Logger::LogNoiseToggle(const bool enabled)
{
	pLogger_->info("{} noise over headphones.", enabled ? "Playing" : "Stopped");

	std::string s = enabled ? "Playing" : "Stopped";
	s += " noise over headphones.\n";
	std::cout << s;
}

void bsExp::Logger::LogNewSoundPos(const bs::CartesianCoord coord)
{
	pLogger_->info("New position of sound is: ({0:03.2f};{1:03.2f};{2:03.2f})", coord.x, coord.y, coord.y);

	std::string s = "New position of sound is: (";
	s += std::to_string(coord.x);
	s += ";";
	s += std::to_string(coord.y);
	s += ";";
	s += std::to_string(coord.z);
	s += ").\n";
	std::cout << s;

}

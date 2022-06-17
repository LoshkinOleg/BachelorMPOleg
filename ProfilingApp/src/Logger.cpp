#include "Logger.h"

#include <filesystem>
#include <iostream>

bsExp::Logger::Logger()
{
	if (std::filesystem::exists("experimentData/0.txt"))
	{
		std::vector<std::string> paths;
		for (const auto& entry : std::filesystem::directory_iterator("experimentData"))
		{
			paths.push_back(entry.path().filename().string());
		}
		std::sort(paths.begin(), paths.end());
		const char latestLogName = paths.back().c_str()[0];
		assert(latestLogName <= '9', "More than 9 log files in folder, next symbol is : which will cause issues!"); // Oleg@self: implement >9 files handling...

		std::string newLogName = "experimentData/";
		newLogName += (latestLogName + 1);
		newLogName += ".txt";
		pLogger_ = spdlog::basic_logger_mt("Logger", newLogName);
	}
	else
	{
		pLogger_ = spdlog::basic_logger_mt("Logger", "experimentData/0.txt");
	}
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

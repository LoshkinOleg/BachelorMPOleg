#include "Logger.h"

#include <filesystem>

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

		std::string newLogName = "experimentData/";
		newLogName += (latestLogName + 1);
		newLogName += ".txt"; // Oleg@self: c'mon, surely there's a better way to do this...
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
}

void bsExp::Logger::LogControllerPosition(const char* deviceName, const bs::CartesianCoord coord)
{
	pLogger_->info("Position of {0} is: ({1:03.2f};{2:03.2f};{3:03.2f})", deviceName, coord.x, coord.y, coord.y);
}

void bsExp::Logger::LogRendererChange(const char* rendererName)
{
	pLogger_->info("Selected {} renderer.", rendererName);
}

void bsExp::Logger::LogNoiseToggle(const bool enabled)
{
	pLogger_->info("{} noise over headphones.", enabled ? "Playing" : "Stopped");
}

void bsExp::Logger::LogNewSoundPos(const bs::CartesianCoord coord)
{
	pLogger_->info("New position of sound is: ({1:03.2f};{2:03.2f};{3:03.2f})", coord.x, coord.y, coord.y);
}

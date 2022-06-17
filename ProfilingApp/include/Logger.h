#pragma once

#include <spdlog/sinks/basic_file_sink.h>

#include "BSCommon.h"

namespace bsExp
{
	class Logger
	{
	public:
		BS_NON_COPYABLE(Logger);
		BS_NON_MOVEABLE(Logger);

		Logger();

		void LogDelimiter();
		void LogControllerPosition(const char* deviceName, const bs::CartesianCoord coord);
		void LogRendererChange(const char* rendererName);
		void LogNoiseToggle(const bool enabled);
		void LogNewSoundPos(const bs::CartesianCoord coord);

	private:
		std::shared_ptr<spdlog::logger> pLogger_;
	};
}
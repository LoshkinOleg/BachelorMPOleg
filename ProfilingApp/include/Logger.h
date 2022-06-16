#pragma once

#include <spdlog/sinks/basic_file_sink.h>

#include "BSExpCommon.h"
#include "BSCommon.h"

namespace bsExp
{
	class Logger
	{
	public:
		BSEXP_NON_COPYABLE(Logger);
		BSEXP_NON_MOVEABLE(Logger);

		Logger();

		void LogDelimiter();
		void LogControllerPose(const char* deviceName, bs::CartesianCoord coord);
		void LogRendererChange(const char* rendererName);
		void LogNoiseToggle(const bool enabled);

	private:
		std::shared_ptr<spdlog::logger> pLogger_;
	};
}
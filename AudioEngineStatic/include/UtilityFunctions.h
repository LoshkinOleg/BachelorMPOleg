#pragma once

#include <vector>

namespace bs
{
	std::vector<float> LoadWavOLD(const char* path);
	std::vector<float> LoadWav(const char* path, const uint32_t nrOfChannels, const uint32_t sampleRate);
}
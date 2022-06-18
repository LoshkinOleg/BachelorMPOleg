#include "BSCommon.h"

#include <cassert>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;
#include <math.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

std::vector<float> bs::LoadWav(const char* path, const uint32_t desiredNrOfChanels, const uint32_t desiredSampleRate)
{
	unsigned int fileChannels = 0;
	unsigned int fileSampleRate = 0;
	drwav_uint64 sampleCount = 0;
	
	float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(path, &fileChannels, &fileSampleRate, &sampleCount, NULL);

	assert(fileChannels == desiredNrOfChanels, "Error reading wav file: desired vs actual nrOfChannels mismatch!");
	assert(fileSampleRate == desiredSampleRate, "Error reading wav file: desired vs actual sampleRate mismatch!");
	assert(pSampleData != NULL, "Error reading wav file: couldn't read wav data!");

	std::vector<float> returnVal;
	returnVal.reserve(sampleCount);
	for (size_t i = 0; i < sampleCount; i++)
	{
		returnVal.push_back(pSampleData[i]);
	}

	drwav_free(pSampleData, NULL);

	return returnVal;
}

std::array<float, 3> bs::ToStdArray(const CartesianCoord coord)
{
	return {coord.x, coord.y, coord.z};
}

std::array<float, 3> bs::ToStdArray(const SphericalCoord coord)
{
	return {coord.azimuth, coord.elevation, coord.radius};
}

bs::CartesianCoord bs::ToCartesian(const SphericalCoord coord)
{
	return {coord.radius * sinf(coord.elevation) * cosf(coord.azimuth),
			coord.radius * sinf(coord.elevation) * sinf(coord.azimuth),
			coord.radius * cosf(coord.elevation)};
}

float bs::RemapToRange(const float value, const float inMin, const float inMax, const float outMin, const float outMax)
{
	return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

void bs::Interlace(std::vector<float>& out, const std::vector<float>& left, const std::vector<float>& right)
{
	assert(out.size() == left.size() + right.size() && left.size() == right.size(), "Vectors passed to Interlace() are not size compatible!");
	const auto len = left.size();
	for (size_t i = 0; i < len; i++)
	{
		out[2 * i] = left[i];
		out[2 * i + 1] = right[i];
	}
}

void bs::SumSignals(std::vector<float>& out, const std::vector<float>& other)
{
	assert(out.size() == other.size(), "Vectors passed to SumSignals() are not size compatible!");
	const auto len = other.size();
	for (size_t i = 0; i < len; i++)
	{
		out[i] += other[i];
	}
}

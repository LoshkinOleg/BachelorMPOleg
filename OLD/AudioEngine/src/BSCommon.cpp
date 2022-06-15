#include "BSCommon.h"

#include <cassert>
#include <algorithm>

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

bs::SphericalCoord bs::ToSpherical(const CartesianCoord coord)
{
	return {atanf(coord.y / coord.x),
			atanf((sqrtf(coord.x * coord.x + coord.y * coord.y) / coord.z)),
			sqrtf(coord.x * coord.x + coord.y * coord.y + coord.z * coord.z)};
}

float bs::RemapToRange(const float value, const float inMin, const float inMax, const float outMin, const float outMax)
{
	return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

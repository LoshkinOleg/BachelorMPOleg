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

bool bs::Equivalent(const bs::CartesianCoord a, const bs::CartesianCoord b)
{
	constexpr const float epsilon = 0.001f;
	return std::fabsf(a.x - b.x) < epsilon && std::fabsf(a.y - b.y) && std::fabsf(a.z - b.z);
}

std::array<float, 3> bs::QuatToEuler(const std::array<float, 4> quat)
{
	// Taken from: https://steamcommunity.com/app/250820/discussions/0/1728711392744037419/

	std::array<float, 3> v;
	const float test = quat[0] * quat[1] + quat[2] * quat[3];
	constexpr const float pi = 3.14159f;
	if (test > 0.499f)
	{ // singularity at north pole
		v[0] = 2.0f * std::atan2f(quat[0], quat[3]); // heading
		v[1] = pi / 2.0f; // attitude
		v[2] = 0; // bank
		return v;
	}
	if (test < -0.499f)
	{ // singularity at south pole
		v[0] = -2.0f * std::atan2f(quat[0], quat[3]); // headingq
		v[1] = pi / 2.0f; // attitude
		v[2] = 0; // bank
		return v;
	}
	const float sqx = quat[0] * quat[0];
	const float sqy = quat[1] * quat[1];
	const float sqz = quat[2] * quat[2];
	v[0] = std::atan2f(2.0f * quat[1] * quat[3] - 2.0f * quat[0] * quat[2], 1.0f - 2.0f * sqy - 2.0f * sqz); // heading
	v[1] = std::asinf(2.0f * test); // attitude
	v[2] = std::atan2f(2.0f * quat[0] * quat[3] - 2 * quat[1] * quat[2], 1.0f - 2.0f * sqx - 2.0f * sqz); // bank
	return v;
}

float bs::CartesianCoord::Magnitude() const
{
	return std::sqrtf(x * x + y * y + z * z);
}

bs::CartesianCoord bs::CartesianCoord::Normalized() const
{
	const auto magnitude = Magnitude();
	return { x / magnitude, y / magnitude, z / magnitude };
}

bs::CartesianCoord bs::CartesianCoord::operator-(const CartesianCoord other) const
{
	return { x - other.x, y - other.y, z - other.z };
}

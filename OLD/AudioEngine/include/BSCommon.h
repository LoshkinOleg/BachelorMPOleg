#pragma once

#include <array>
#include <vector>

namespace bs
{
	enum class ClipWrapMode
	{
		ONE_SHOT,
		LOOP
	};

	using SoundMakerId = size_t;
	constexpr const size_t INVALID_ID = (size_t)-1;

	/*
		Units are in meters. Values are ]-inf;+inf[
	*/
	struct CartesianCoord
	{
		float x, y, z;
	};
	/*
		Units are in euler degrees for azimuth and elevation. Meters for radius.
		Value of azimuth is [-180;180] where values <0 are left relative to listener, >0 are right relative to the listener.
		Value of elevation is [-90;90] where values <0 are down relative to the listener, >0 are above relative to the listener.
		Value of radius is [0;+inf[ .
	*/
	struct SphericalCoord
	{
		float azimuth, elevation, radius;
	};

	std::vector<float> LoadWav(const char* path, const uint32_t nrOfChannels, const uint32_t sampleRate);

	std::array<float, 3> ToStdArray(const CartesianCoord coord); // Oleg@self: inline those
	std::array<float, 3> ToStdArray(const SphericalCoord coord);
	CartesianCoord ToCartesian(const SphericalCoord coord);
	SphericalCoord ToSpherical(const CartesianCoord coord);
	float RemapToRange(const float value, const float inMin, const float inMax, const float outMin, const float outMax);
}
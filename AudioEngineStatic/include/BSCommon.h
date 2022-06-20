#pragma once

#include <array>
#include <vector>
#include <string>

#define BS_NON_COPYABLE(type) type(const type&) = delete; type& operator=(const type&) = delete
#define BS_NON_MOVEABLE(type) type(type&&) = delete; type& operator=(type&&) = delete
#define BS_COPYABLE(type) type(const type&) = default; type& operator=(const type&) = default
#define BS_MOVEABLE(type) type(type&&) = default; type& operator=(type&&) = default

namespace bs
{
	/*
		Units are in meters. Values are ]-inf;+inf[
	*/
	struct CartesianCoord
	{
		float x{ 0 }, y{ 0 }, z{ 0 };

		float Magnitude() const;
		CartesianCoord Normalized() const;
		CartesianCoord operator-(const CartesianCoord other) const;
	};
	/*
		Units are in radians for azimuth and elevation. Meters for radius.
		Value of azimuth is [-pi;pi] where values <0 are left relative to listener, >0 are right relative to the listener.
		Value of elevation is [-pi/2;pi/2] where values <0 are down relative to the listener, >0 are above relative to the listener.
		Value of radius is [0;+inf[ .
	*/
	struct SphericalCoord
	{
		float azimuth{ 0 }, elevation{ 0 }, radius{ 0 };
	};

	std::vector<float> LoadWav(const char* path, const uint32_t nrOfChannels, const uint32_t sampleRate);

	std::array<float, 3> ToStdArray(const CartesianCoord coord); // Oleg@self: inline those
	std::array<float, 3> ToStdArray(const SphericalCoord coord);
	CartesianCoord ToCartesian(const SphericalCoord coord);

	float RemapToRange(const float value, const float inMin, const float inMax, const float outMin, const float outMax);

	void Interlace(std::vector<float>& out, const std::vector<float>& left, const std::vector<float>& right);
	void SumSignals(std::vector<float>& out, const std::vector<float>& other);

	constexpr inline float ToRadians(const float euler)
	{
		constexpr const float pi = 3.14159265359f;
		return euler * (pi / 180.0f);
	}
}
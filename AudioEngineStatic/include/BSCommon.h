#pragma once

#include <array>
#include <vector>
#include <string>

#define BS_NON_COPYABLE(type) type(const type&) = delete; type& operator=(const type&) = delete
#define BS_NON_MOVEABLE(type) type(type&&) = delete; type& operator=(type&&) = delete
#define BS_COPYABLE(type) type(const type&) = default; type& operator=(const type&) = default
#define BS_MOVEABLE(type) type(type&&) = default; type& operator=(type&&) = default

/*
	Front:	+X
	Back:	-X
	Left:	+Y
	Right:	-Y
	Up:		+Z
	Down:	-Z

	Roll left:	+X
	Roll right:	-X
	Pitch up:	+Y
	Pitch down:	-Y
	Yaw left:	+Z
	Yaw right:	-Z
*/

namespace bs
{
	struct CartesianCoord;
	struct SphericalCoord;
	struct Euler;
	struct Radians;
	struct Quaternion;
	struct Mat3x3;
	struct Mat3x4;

	/*
		Units are in meters. Values are ]-inf;+inf[
	*/
	struct CartesianCoord
	{
		float x{ 0 }, y{ 0 }, z{ 0 };

		CartesianCoord() = default;
		CartesianCoord(const float x, const float y, const float z): x(x), y(y), z(z) {}
		explicit CartesianCoord(const SphericalCoord& coord);
		CartesianCoord& operator=(const SphericalCoord& coord);

		float GetMagnitude() const;
		CartesianCoord GetNormalized() const;
		CartesianCoord operator-(const CartesianCoord other) const;
		bool operator==(const CartesianCoord other) const;
		bool operator!=(const CartesianCoord other) const;
	};
	/*
		Units are in radians for a and e. Meters for r.
		Value of a is [-pi;pi] where values <0 are left relative to listener, >0 are right relative to the listener.
		Value of e is [-pi/2;pi/2] where values <0 are down relative to the listener, >0 are above relative to the listener.
		Value of r is [0;+inf[ .
	*/
	struct SphericalCoord
	{
		float a{ 0 }, e{ 0 }, r{ 0 };

		SphericalCoord() = default;
		SphericalCoord(const float a, const float e, const float r): a(a), e(e), r(r) {}
		explicit SphericalCoord(const CartesianCoord& coord);
		SphericalCoord& operator=(const CartesianCoord& coord);
	};

	struct Euler
	{
		float r{ 0 }, p{ 0 }, y{ 0 };

		Euler() = default;
		Euler(const float r, const float p, const float y): r(r), p(p), y(y) {}
		explicit Euler(Radians& rad);
		Euler& operator=(Radians& rad);
	};

	struct Radians
	{
		float r{ 0 }, p{ 0 }, y{ 0 };

		Radians() = default;
		Radians(const float r, const float p, const float y): r(r), p(p), y(y) {}
		explicit Radians(Euler& deg);
		Radians& operator=(Euler& deg);
	};

	struct Quaternion
	{
		float w{ 1.0f }, i{ 0.0f }, j{ 0.0f }, k{ 0.0f };

		Quaternion GetInverse() const;
		Radians GetRadians() const;
		Euler GetEuler() const;
	};

	/*
		Values are stored in a row-major manner.
		This is the rotation and scale matrix (scale is usually 1.0f).
	*/
	struct Mat3x3
	{
		float	m00{ 1.0f }, m01{ 0.0f }, m02{ 0.0f },
				m10{ 0.0f }, m11{ 1.0f }, m12{ 0.0f },
				m20{ 0.0f }, m21{ 0.0f }, m22{ 1.0f };

		Mat3x3() = default;
		explicit Mat3x3(const Quaternion quat);
		Mat3x3(const float m00, const float m01, const float m02, 
			   const float m10, const float m11, const float m12,
			   const float m20, const float m21, const float m22):
			m00(m00), m01(m01), m02(m02),
			m10(m10), m11(m11), m12(m12),
			m20(m20), m21(m21), m22(m22){}

		Mat3x3 GetTranspose() const;
	};

	/*
		Values are stored in a row-major manner.
		Position is stored in m03 = x, m13 = y, m23 = z in cartesian coordinates.
		The rest is the rotation and scale matrix (scale is usually 1.0f).
	*/
	struct Mat3x4
	{
		float	m00{ 1.0f }, m01{ 0.0f }, m02{ 0.0f }, m03{ 0.0f },
				m10{ 0.0f }, m11{ 1.0f }, m12{ 0.0f }, m13{ 0.0f },
				m20{ 0.0f }, m21{ 0.0f }, m22{ 1.0f }, m23{ 0.0f };

		Mat3x3 GetRotationMatrix() const;
		CartesianCoord GetPosition() const;
		Quaternion GetQuaternion() const;
	};

	std::vector<float> LoadWav(const char* path, const uint32_t nrOfChannels, const uint32_t sampleRate);

	float RemapToRange(const float value, const float inMin, const float inMax, const float outMin, const float outMax);

	void Interlace(std::vector<float>& out, const std::vector<float>& left, const std::vector<float>& right);
	void SumSignals(std::vector<float>& out, const std::vector<float>& other);

	constexpr inline float ToRadians(const float euler)
	{
		constexpr const float pi = 3.14159265359f;
		return euler * (pi / 180.0f);
	}
	constexpr inline float ToEuler(const float radians)
	{
		constexpr const float pi = 3.14159265359f;
		return radians * (180.0f / pi);
	}
}
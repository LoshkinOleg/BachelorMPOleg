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

float bs::CartesianCoord::GetMagnitude() const
{
	return std::sqrtf(x * x + y * y + z * z);
}

bs::CartesianCoord bs::CartesianCoord::GetNormalized() const
{
	const auto magnitude = GetMagnitude();
	return { x / magnitude, y / magnitude, z / magnitude };
}

bs::CartesianCoord bs::CartesianCoord::operator-(const CartesianCoord other) const
{
	return { x - other.x, y - other.y, z - other.z };
}

bool bs::CartesianCoord::operator==(const CartesianCoord other) const
{
	constexpr const float epsilon = 0.001f;
	return std::fabsf(other.x - x) < epsilon && std::fabsf(other.y - y) < epsilon && std::fabsf(other.z - z) < epsilon;
}

bool bs::CartesianCoord::operator!=(const CartesianCoord other) const
{
	return !(*this == other);
}

bs::CartesianCoord::CartesianCoord(const SphericalCoord& coord):
	x(coord.r* sinf(coord.e)* cosf(coord.a)),
	y(coord.r* sinf(coord.e)* sinf(coord.a)),
	z(coord.r* cosf(coord.e)){}

bs::CartesianCoord& bs::CartesianCoord::operator=(const SphericalCoord& coord)
{
	x = coord.r * sinf(coord.e) * cosf(coord.a);
	y = coord.r * sinf(coord.e) * sinf(coord.a);
	z = coord.r * cosf(coord.e);
	return *this;
}

bs::SphericalCoord::SphericalCoord(const CartesianCoord& coord)
{
	assert(false && "Implement this.");
}

bs::SphericalCoord& bs::SphericalCoord::operator=(const CartesianCoord& coord)
{
	assert(false && "Implement this.");
	return *this;
}

bs::Quaternion bs::Quaternion::GetInverse() const
{
	assert(false && "Implement this.");
	return {};
}

bs::Radians bs::Quaternion::GetRadians() const
{
	// Taken from: https://steamcommunity.com/app/250820/discussions/0/1728711392744037419/

	std::array<float, 3> v;
	const float test = i * j + k * w;
	constexpr const float pi = 3.14159f;
	if (test > 0.499f)
	{ // singularity at north pole
		v[0] = 2.0f * std::atan2f(i, w); // heading
		v[1] = pi / 2.0f; // attitude
		v[2] = 0; // bank
		return { ToEuler(v[0]), ToEuler(v[1]), ToEuler(v[2]) };
	}
	if (test < -0.499f)
	{ // singularity at south pole
		v[0] = -2.0f * std::atan2f(i, w); // headingq
		v[1] = pi / 2.0f; // attitude
		v[2] = 0; // bank
		return { ToEuler(v[0]), ToEuler(v[1]), ToEuler(v[2]) };
	}
	const float sqx = i * i;
	const float sqy = j * j;
	const float sqz = k * k;
	v[0] = std::atan2f(2.0f * j * w - 2.0f * i * k, 1.0f - 2.0f * sqy - 2.0f * sqz); // heading
	v[1] = std::asinf(2.0f * test); // attitude
	v[2] = std::atan2f(2.0f * i * w - 2 * j * k, 1.0f - 2.0f * sqx - 2.0f * sqz); // bank
	return { v[0], v[1], v[2] };
}

bs::Euler bs::Quaternion::GetEuler() const
{
	return bs::Euler(GetRadians());
}

bs::Euler::Euler(Radians& rad): r(ToEuler(rad.r)), p(ToEuler(rad.p)), y(ToEuler(rad.y)) {}

bs::Euler& bs::Euler::operator=(Radians& rad)
{
	r = ToEuler(rad.r);
	p = ToEuler(rad.p);
	y = ToEuler(rad.y);
	return *this;
}

bs::Radians::Radians(Euler& deg): r(ToRadians(deg.r)), p(ToRadians(deg.p)), y(ToRadians(deg.y)) {}

bs::Radians& bs::Radians::operator=(Euler& deg)
{
	r = ToRadians(deg.r);
	p = ToRadians(deg.p);
	y = ToRadians(deg.y);
	return *this;
}

bs::Mat3x3::Mat3x3(const Quaternion quat):
	m00(2.0f * (quat.w * quat.w + quat.i * quat.i) - 1.0f),		m01(2.0f * (quat.i * quat.j - quat.w * quat.k)),			m02(2.0f * (quat.i * quat.k + quat.w * quat.j)),
	m10(2.0f * (quat.i * quat.j + quat.w * quat.k)),			m11(2.0f * (quat.w * quat.w + quat.j * quat.j) - 1.0f),		m12(2.0f * (quat.j * quat.k - quat.w * quat.i)),
	m20(2.0f * (quat.i * quat.k - quat.w * quat.j)),			m21(2.0f * (quat.j * quat.k + quat.w * quat.i)),			m22(2.0f * (quat.w * quat.w + quat.k * quat.k) - 1.0f)
{
	// Taken from: https://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/
}

bs::Mat3x3 bs::Mat3x3::GetTranspose() const
{
	return
	{
		m00, m10, m20,
		m01, m11, m21,
		m02, m12, m22
	};
}

bs::Mat3x3 bs::Mat3x4::GetRotationMatrix() const
{
	return
	{
		m00, m01, m02,
		m10, m11, m12,
		m20, m21, m22
	};
}

bs::CartesianCoord bs::Mat3x4::GetPosition() const
{
	return {m03, m13, m23};
}

bs::Quaternion bs::Mat3x4::GetQuaternion() const
{
	// Taken from: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

	const float w = std::sqrtf(1.0f + m00 + m11 + m22) * 0.5f;
	return
	{
		w,
		(m21 - m12) / (4.0f * w),
		(m02 - m20) / (4.0f * w),
		(m10 - m01) / (4.0f * w)
	};
}

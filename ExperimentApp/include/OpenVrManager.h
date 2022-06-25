#pragma once

#include <functional>
#include <map>
#include <cassert>

#include <openvr.h>

#include "BSCommon.h"

namespace bsExp
{
	class OpenVrManager
	{
	public:
		enum class Input: size_t
		{
			LeftTrigger = 0,
			LeftPad = 1,
			LeftGrip = 2,
			RightTrigger = 3,
			RightGrip = 4,
			RightPad = 5,
			LeftMenu = 6,
			RightMenu = 7
		};

		BS_NON_COPYABLE(OpenVrManager);
		BS_NON_MOVEABLE(OpenVrManager);

		OpenVrManager();
		~OpenVrManager();

		void RegisterCallback(Input input, std::function<void(void)> callback);
		void Update();

		bs::CartesianCoord GetLeftControllerPos() const;
		bs::CartesianCoord GetRightControllerPos() const;
		bs::Mat3x4 GetHeadsetMatrix() const;

#ifdef USE_DUMMY_INPUTS
		void SetHmdPos(const bs::CartesianCoord coord);
		void SetHmdRot(const bs::Quaternion quat);
		void SetLeftControllerPos(const bs::CartesianCoord coord);
		void SetRightControllerPos(const bs::CartesianCoord coord);
#endif // USE_DUMMY_INPUTS

		static bs::CartesianCoord ToStandardAxisSystem(const vr::HmdVector3_t coord)
		{
			return { -coord.v[2], -coord.v[0], coord.v[1] };
		}
		static vr::HmdVector3_t ToOpenVrAxisSystem(const bs::CartesianCoord coord)
		{
			return { -coord.y, coord.z, -coord.x };
		}
		static bs::CartesianCoord CartesianFromMatrix(const vr::HmdMatrix34_t matrix)
		{
			return {matrix.m[0][3], matrix.m[1][3], matrix.m[2][3]};
		};
		static bs::Quaternion QuaternionFromMatrix(const vr::HmdMatrix34_t matrix)
		{
			// Taken from: https://www.intel.com/content/dam/develop/external/us/en/documents/293748-142817.pdf

			const auto ReciprocalSqrt = [](const float x)->float
			{
				long i;
				float y, r;
				y = x * 0.5f;
				i = *(long*)(&x);
				i = 0x5f3759df - (i >> 1);
				r = *(float*)(&i);
				r = r * (1.5f - r * r * y);
				return r;
			};

			float q[4] = {0};
			const float m[12] =
			{
				matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
				matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
				matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3]
			};

			if (m[0 * 4 + 0] + m[1 * 4 + 1] + m[2 * 4 + 2] > 0.0f)
			{
				float t = +m[0 * 4 + 0] + m[1 * 4 + 1] + m[2 * 4 + 2] + 1.0f;
				float s = ReciprocalSqrt(t) * 0.5f;
				q[3] = s * t;
				q[2] = (m[0 * 4 + 1] - m[1 * 4 + 0]) * s;
				q[1] = (m[2 * 4 + 0] - m[0 * 4 + 2]) * s;
				q[0] = (m[1 * 4 + 2] - m[2 * 4 + 1]) * s;
			}
			else if (m[0 * 4 + 0] > m[1 * 4 + 1] && m[0 * 4 + 0] > m[2 * 4 + 2])
			{
				float t = +m[0 * 4 + 0] - m[1 * 4 + 1] - m[2 * 4 + 2] + 1.0f;
				float s = ReciprocalSqrt(t) * 0.5f;
				q[0] = s * t;
				q[1] = (m[0 * 4 + 1] + m[1 * 4 + 0]) * s;
				q[2] = (m[2 * 4 + 0] + m[0 * 4 + 2]) * s;
				q[3] = (m[1 * 4 + 2] - m[2 * 4 + 1]) * s;
			}
			else if (m[1 * 4 + 1] > m[2 * 4 + 2])
			{
				float t = -m[0 * 4 + 0] + m[1 * 4 + 1] - m[2 * 4 + 2] + 1.0f;
				float s = ReciprocalSqrt(t) * 0.5f;
				q[1] = s * t;
				q[0] = (m[0 * 4 + 1] + m[1 * 4 + 0]) * s;
				q[3] = (m[2 * 4 + 0] - m[0 * 4 + 2]) * s;
				q[2] = (m[1 * 4 + 2] + m[2 * 4 + 1]) * s;
			}
			else
			{
				float t = -m[0 * 4 + 0] - m[1 * 4 + 1] + m[2 * 4 + 2] + 1.0f;
				float s = ReciprocalSqrt(t) * 0.5f;
				q[2] = s * t;
				q[3] = (m[0 * 4 + 1] - m[1 * 4 + 0]) * s;
				q[0] = (m[2 * 4 + 0] + m[0 * 4 + 2]) * s;
				q[1] = (m[1 * 4 + 2] + m[2 * 4 + 1]) * s;
			}

			return {q[3], q[0], q[1], q[2]};
		}

	private:

		vr::IVRSystem* context_ = nullptr;
		vr::IVRInput* input_ = nullptr;
		vr::VREvent_t event_{};

		vr::TrackedDeviceIndex_t leftControllerId = vr::k_unTrackedDeviceIndexInvalid;
		vr::TrackedDeviceIndex_t rightControllerId = vr::k_unTrackedDeviceIndexInvalid;
		vr::HmdVector3_t leftControllerPos_{};
		vr::HmdVector3_t rightControllerPos_{};

		vr::HmdMatrix34_t hmdMat_{};

		std::map<Input, std::vector<std::function<void(void)>>> callbacks_;
	};
}
#pragma once

#include <functional>
#include <map>

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

		bs::CartesianCoord LeftControllerPos() const;
		bs::CartesianCoord RightControllerPos() const;
		vr::HmdMatrix34_t HmdMatrix();

		static bs::CartesianCoord ToStandardAxisSystem(const bs::CartesianCoord coord)
		{
			return { -coord.z, -coord.x, coord.y };
		}
		static bs::CartesianCoord CartesianFromMatrix(const vr::HmdMatrix34_t matrix)
		{
			return ToStandardAxisSystem({matrix.m[0][3], matrix.m[1][3], matrix.m[2][3]});
		};
		static std::array<float, 4> QuaternionFromMatrix(const vr::HmdMatrix34_t m)
		{
			vr::HmdQuaternionf_t q;
			q.w = std::sqrtf(1.0f + m.m[0][0] + m.m[1][1] + m.m[2][2]) / 2.0f; // Scalar
			q.x = (m.m[2][1] - m.m[1][2]) / (4.0f * q.w);
			q.y = (m.m[0][2] - m.m[2][0]) / (4.0f * q.w);
			q.z = (m.m[1][0] - m.m[0][1]) / (4.0f * q.w);
			return {q.x, q.y, q.z, q.w};
		}

	private:

		vr::IVRSystem* context_ = nullptr;
		vr::IVRInput* input_ = nullptr;
		vr::VREvent_t event_{};

		vr::TrackedDeviceIndex_t leftControllerId = vr::k_unTrackedDeviceIndexInvalid;
		vr::TrackedDeviceIndex_t rightControllerId = vr::k_unTrackedDeviceIndexInvalid;
		bs::CartesianCoord leftControllerPos_{};
		bs::CartesianCoord rightControllerPos_{};

		std::map<Input, std::vector<std::function<void(void)>>> callbacks_;
	};
}
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
			RightPad = 5
		};

		BS_NON_COPYABLE(OpenVrManager);
		BS_NON_MOVEABLE(OpenVrManager);

		OpenVrManager();
		~OpenVrManager();

		void RegisterCallback(Input input, std::function<void(void)> callback);
		void Update();

		bs::CartesianCoord LeftControllerPos() const;
		bs::CartesianCoord RightControllerPos() const;

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
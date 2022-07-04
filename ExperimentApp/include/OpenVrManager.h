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
			LeftGrip,
			LeftMenu,
			LeftPadRight,
			LeftPadUp,
			LeftPadLeft,
			LeftPadDown,

			RightTrigger,
			RightGrip,
			RightMenu,
			RightPadRight,
			RightPadUp,
			RightPadLeft,
			RightPadDown
		};

		BS_NON_COPYABLE(OpenVrManager);
		BS_NON_MOVEABLE(OpenVrManager);

		OpenVrManager();
		~OpenVrManager();

		void RegisterCallback(Input input, std::function<void(void)> callback);
		void Update();

		bs::Mat3x4 GetHeadsetMat() const;
		bs::Mat3x4 GetLeftControllerMat() const;
		bs::Mat3x4 GetRightControllerMat() const;
		bs::CartesianCoord GetHeadsetPos() const;
		bs::CartesianCoord GetLeftControllerPos() const;
		bs::CartesianCoord GetRightControllerPos() const;
		bs::Quaternion GetHeadsetRot() const;
		bs::Quaternion GetLeftControllerRot() const;
		bs::Quaternion GetRightControllerRot() const;

#ifdef USE_DUMMY_INPUTS
		void SetHeadsetPos(const bs::CartesianCoord coord);
		void SetLeftControllerPos(const bs::CartesianCoord coord);
		void SetRightControllerPos(const bs::CartesianCoord coord);
		void SetHeadsetRot(const bs::Quaternion quat);
		void SetLeftControllerRot(const bs::Quaternion quat);
		void SetRightControllerRot(const bs::Quaternion quat);
#endif // USE_DUMMY_INPUTS

	private:

		static bs::CartesianCoord ToStandardBasis_(const vr::HmdVector3_t coord);
		static vr::HmdVector3_t ToHmdBasis_(const bs::CartesianCoord coord);
		static bs::Quaternion ToStandardBasis_(const vr::HmdQuaternionf_t quat);
		static vr::HmdQuaternionf_t ToHmdBasis_(const bs::Quaternion quat);
		static bs::Mat3x4 ToStandardBasis_(const vr::HmdMatrix34_t mat);
		static vr::HmdMatrix34_t ToHmdBasis_(const bs::Mat3x4 mat);

		static vr::HmdVector3_t HmdPosFromHmdMatrix_(const vr::HmdMatrix34_t matrix);
		static vr::HmdQuaternionf_t HmdQuatFromHmdMatrix_(const vr::HmdMatrix34_t matrix);
		static vr::HmdMatrix33_t HmdRotMatFromHmdQuat_(const vr::HmdQuaternionf_t quat);
		static vr::HmdQuaternionf_t HmdQuatFromHmdRotMat(const vr::HmdMatrix33_t matrix);

		vr::IVRSystem* context_ = nullptr;
		vr::VREvent_t event_{};

		vr::TrackedDeviceIndex_t leftControllerId_ = vr::k_unTrackedDeviceIndexInvalid;
		vr::TrackedDeviceIndex_t rightControllerId_ = vr::k_unTrackedDeviceIndexInvalid;

		vr::HmdMatrix34_t headsetMat_{};
		vr::HmdMatrix34_t leftControllerMat_{};
		vr::HmdMatrix34_t rightControllerMat_{};

		std::map<Input, std::vector<std::function<void(void)>>> callbacks_;
	};
}
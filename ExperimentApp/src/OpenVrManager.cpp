#include <OpenVrManager.h>

#include <cassert>

#undef USE_DUMMY_INPUTS

bsExp::OpenVrManager::OpenVrManager()
{
#ifndef USE_DUMMY_INPUTS
	vr::EVRInitError err;
	context_ = vr::VR_Init(&err, vr::EVRApplicationType::VRApplication_Background);
	assert(err == vr::EVRInitError::VRInitError_None && "Failed to initialize openvr!");

	for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		const auto deviceClass = context_->GetTrackedDeviceClass(i);
		if (deviceClass == vr::TrackedDeviceClass::TrackedDeviceClass_Controller)
		{
			if (leftControllerId == vr::k_unTrackedDeviceIndexInvalid)
			{
				leftControllerId = i;
			}
			else
			{
				rightControllerId = i;
				break;
			}
		}
	}
#else

#endif // USE_DUMMY_INPUTS
}

bsExp::OpenVrManager::~OpenVrManager()
{
#ifndef USE_DUMMY_INPUTS
	vr::VR_Shutdown();
#else

#endif // USE_DUMMY_INPUTS
}

void bsExp::OpenVrManager::RegisterCallback(Input input, std::function<void(void)> callback)
{
	callbacks_[input].push_back(callback);
}

void bsExp::OpenVrManager::Update()
{
#ifndef USE_DUMMY_INPUTS
	vr::TrackedDevicePose_t pose;
	context_->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0.0f, &pose, 1);
	hmdMat_ = pose.mDeviceToAbsoluteTracking;

	while (context_->PollNextEvent(&event_, sizeof(vr::VREvent_t)))
	{
		if (event_.trackedDeviceIndex != leftControllerId && event_.trackedDeviceIndex != rightControllerId) continue;

		const bool left = event_.trackedDeviceIndex == leftControllerId ? true : false;

		switch (event_.eventType)
		{
			case vr::EVREventType::VREvent_ButtonPress:
			{
				vr::VRControllerState_t state;
				vr::TrackedDevicePose_t pose;
				auto result = context_->GetControllerStateWithPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, event_.trackedDeviceIndex, &state, sizeof(vr::VRControllerState_t), &pose);
				assert(result && "Failed to get controller state with pose!");

				if (left)
				{
					leftControllerPos_ = { pose.mDeviceToAbsoluteTracking.m[0][3], pose.mDeviceToAbsoluteTracking.m[1][3], pose.mDeviceToAbsoluteTracking.m[2][3] };
				}
				else
				{
					rightControllerPos_ = { pose.mDeviceToAbsoluteTracking.m[0][3], pose.mDeviceToAbsoluteTracking.m[1][3], pose.mDeviceToAbsoluteTracking.m[2][3] };
				}

				if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger))
				{
					const auto& callbacks = callbacks_[left ? Input::LeftTrigger : Input::RightTrigger];
					for (auto& callback : callbacks)
					{
						callback();
					}
				}
				else if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad))
				{
					const auto& callbacks = callbacks_[left ? Input::LeftPad : Input::RightPad];
					for (auto& callback : callbacks)
					{
						callback();
					}
				}
				else if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip))
				{
					const auto& callbacks = callbacks_[left ? Input::LeftGrip : Input::RightGrip];
					for (auto& callback : callbacks)
					{
						callback();
					}
				}
				else if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu))
				{
					const auto& callbacks = callbacks_[left ? Input::LeftMenu : Input::RightMenu];
					for (auto& callback : callbacks)
					{
						callback();
					}
				}
			}break;
			default:
			break;
		}
	}
#else

#endif // USE_DUMMY_INPUTS
}

vr::HmdVector3_t bsExp::OpenVrManager::LeftControllerPos() const
{
	return leftControllerPos_;
}

bs::CartesianCoord bsExp::OpenVrManager::RightControllerPos() const
{
	return rightControllerPos_;
}

vr::HmdMatrix34_t bsExp::OpenVrManager::HmdMatrix()
{
	return hmdMat_;
}

#ifdef USE_DUMMY_INPUTS
void bsExp::OpenVrManager::SetHmdPos(const bs::CartesianCoord coord)
{
	
}

void bsExp::OpenVrManager::SetHmdRot(const vr::HmdQuaternionf_t quat)
{
}

void bsExp::OpenVrManager::SetLeftControllerPos(const bs::CartesianCoord coord)
{
}

void bsExp::OpenVrManager::SetRightControllerPos(const bs::CartesianCoord coord)
{
}
#endif // USE_DUMMY_INPUTS

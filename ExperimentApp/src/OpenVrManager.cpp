#include <OpenVrManager.h>

#include <cassert>

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
			if (leftControllerId_ == vr::k_unTrackedDeviceIndexInvalid)
			{
				leftControllerId_ = i;
			}
			else
			{
				rightControllerId_ = i;
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
	headsetMat_ = pose.mDeviceToAbsoluteTracking;

	while (context_->PollNextEvent(&event_, sizeof(vr::VREvent_t)))
	{
		if (event_.trackedDeviceIndex != leftControllerId_ && event_.trackedDeviceIndex != rightControllerId_) continue;

		const bool left = event_.trackedDeviceIndex == leftControllerId_ ? true : false;

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
					leftControllerMat_ = pose.mDeviceToAbsoluteTracking;
				}
				else
				{
					rightControllerMat_ = pose.mDeviceToAbsoluteTracking;
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
				else if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_DPad_Right))
				{
					const auto& callbacks = callbacks_[left ? Input::LeftPadRight : Input::RightPadRight];
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

bs::Mat3x4 bsExp::OpenVrManager::GetHeadsetMat() const
{
	// TODO: shit's bugged
	const auto m = ToStandardBasis_(headsetMat_);
	// Rotate to get rid of wierd roll rotation and null out z coord to remove elevation since it's bugged.
	const auto q = bs::Quaternion(bs::Euler(0.0f, 0.0f, 90.0f)) * m.GetQuaternion();
	const auto rotMat = bs::Mat3x3(q);
	const auto pos = m.GetPosition();
	return
	{
		1.0f, 0.0f, 0.0f, pos.x,
		0.0f, 1.0f, 0.0f, pos.y,
		0.0f, 0.0f, 1.0f, pos.z
	};
}

bs::Mat3x4 bsExp::OpenVrManager::GetLeftControllerMat() const
{
	return ToStandardBasis_(leftControllerMat_);
}

bs::Mat3x4 bsExp::OpenVrManager::GetRightControllerMat() const
{
	return ToStandardBasis_(rightControllerMat_);
}

bs::CartesianCoord bsExp::OpenVrManager::GetHeadsetPos() const
{
	return ToStandardBasis_(HmdPosFromHmdMatrix_(headsetMat_));
}

bs::CartesianCoord bsExp::OpenVrManager::GetLeftControllerPos() const
{
	return ToStandardBasis_(HmdPosFromHmdMatrix_(leftControllerMat_));
}

bs::CartesianCoord bsExp::OpenVrManager::GetRightControllerPos() const
{
	return ToStandardBasis_(HmdPosFromHmdMatrix_(rightControllerMat_));
}

bs::Quaternion bsExp::OpenVrManager::GetHeadsetRot() const
{
	return ToStandardBasis_(HmdQuatFromHmdMatrix_(headsetMat_));
}

bs::Quaternion bsExp::OpenVrManager::GetLeftControllerRot() const
{
	return ToStandardBasis_(HmdQuatFromHmdMatrix_(leftControllerMat_));
}

bs::Quaternion bsExp::OpenVrManager::GetRightControllerRot() const
{
	return ToStandardBasis_(HmdQuatFromHmdMatrix_(rightControllerMat_));
}

#ifdef USE_DUMMY_INPUTS
void bsExp::OpenVrManager::SetHeadsetPos(const bs::CartesianCoord coord)
{
	const auto pos = ToHmdBasis_(coord);
	headsetMat_.m[0][3] = pos.v[0];
	headsetMat_.m[1][3] = pos.v[1];
	headsetMat_.m[2][3] = pos.v[2];
}

void bsExp::OpenVrManager::SetLeftControllerPos(const bs::CartesianCoord coord)
{
	const auto pos = ToHmdBasis_(coord);
	leftControllerMat_.m[0][3] = pos.v[0];
	leftControllerMat_.m[1][3] = pos.v[1];
	leftControllerMat_.m[2][3] = pos.v[2];
}

void bsExp::OpenVrManager::SetRightControllerPos(const bs::CartesianCoord coord)
{
	const auto pos = ToHmdBasis_(coord);
	rightControllerMat_.m[0][3] = pos.v[0];
	rightControllerMat_.m[1][3] = pos.v[1];
	rightControllerMat_.m[2][3] = pos.v[2];
}
void bsExp::OpenVrManager::SetHeadsetRot(const bs::Quaternion quat)
{
	const auto rotMat = HmdRotMatFromHmdQuat_(ToHmdBasis_(quat));
	headsetMat_.m[0][0] = rotMat.m[0][0]; headsetMat_.m[0][1] = rotMat.m[0][1]; headsetMat_.m[0][2] = rotMat.m[0][2];
	headsetMat_.m[1][0] = rotMat.m[1][0]; headsetMat_.m[1][1] = rotMat.m[1][1]; headsetMat_.m[1][2] = rotMat.m[1][2];
	headsetMat_.m[2][0] = rotMat.m[2][0]; headsetMat_.m[2][1] = rotMat.m[2][1]; headsetMat_.m[2][2] = rotMat.m[2][2];
}
void bsExp::OpenVrManager::SetLeftControllerRot(const bs::Quaternion quat)
{
	const auto rotMat = HmdRotMatFromHmdQuat_(ToHmdBasis_(quat));
	leftControllerMat_.m[0][0] = rotMat.m[0][0]; leftControllerMat_.m[0][1] = rotMat.m[0][1]; leftControllerMat_.m[0][2] = rotMat.m[0][2];
	leftControllerMat_.m[1][0] = rotMat.m[1][0]; leftControllerMat_.m[1][1] = rotMat.m[1][1]; leftControllerMat_.m[1][2] = rotMat.m[1][2];
	leftControllerMat_.m[2][0] = rotMat.m[2][0]; leftControllerMat_.m[2][1] = rotMat.m[2][1]; leftControllerMat_.m[2][2] = rotMat.m[2][2];
}
void bsExp::OpenVrManager::SetRightControllerRot(const bs::Quaternion quat)
{
	const auto rotMat = HmdRotMatFromHmdQuat_(ToHmdBasis_(quat));
	rightControllerMat_.m[0][0] = rotMat.m[0][0]; rightControllerMat_.m[0][1] = rotMat.m[0][1]; rightControllerMat_.m[0][2] = rotMat.m[0][2];
	rightControllerMat_.m[1][0] = rotMat.m[1][0]; rightControllerMat_.m[1][1] = rotMat.m[1][1]; rightControllerMat_.m[1][2] = rotMat.m[1][2];
	rightControllerMat_.m[2][0] = rotMat.m[2][0]; rightControllerMat_.m[2][1] = rotMat.m[2][1]; rightControllerMat_.m[2][2] = rotMat.m[2][2];
}
#endif // USE_DUMMY_INPUTS

bs::CartesianCoord bsExp::OpenVrManager::ToStandardBasis_(const vr::HmdVector3_t coord)
{
	return { -coord.v[2], -coord.v[0], coord.v[1] };
}
bs::Quaternion bsExp::OpenVrManager::ToStandardBasis_(const vr::HmdQuaternionf_t quat)
{
	// No operations needed? https://stackoverflow.com/questions/1274936/flipping-a-quaternion-from-right-to-left-handed-coordinates
	return {quat.w, quat.x, quat.y, quat.z};
}
vr::HmdVector3_t bsExp::OpenVrManager::ToHmdBasis_(const bs::CartesianCoord coord)
{
	return { -coord.y, coord.z, -coord.x };
}
vr::HmdQuaternionf_t bsExp::OpenVrManager::ToHmdBasis_(const bs::Quaternion quat)
{
	// No operations needed? https://stackoverflow.com/questions/1274936/flipping-a-quaternion-from-right-to-left-handed-coordinates
	return { quat.w, quat.i, quat.j, quat.k };
}
bs::Mat3x4 bsExp::OpenVrManager::ToStandardBasis_(const vr::HmdMatrix34_t mat)
{
	const auto stdPos = ToStandardBasis_(HmdPosFromHmdMatrix_(mat));
	const bs::Mat3x3 stdRotMat = bs::Mat3x3(ToStandardBasis_(HmdQuatFromHmdMatrix_(mat)));
	
	return
	{
		stdRotMat.m00, stdRotMat.m01, stdRotMat.m02, stdPos.x,
		stdRotMat.m10, stdRotMat.m11, stdRotMat.m12, stdPos.y,
		stdRotMat.m20, stdRotMat.m21, stdRotMat.m22, stdPos.z
	};
}
vr::HmdMatrix34_t bsExp::OpenVrManager::ToHmdBasis_(const bs::Mat3x4 mat)
{
	const auto hmdPos = ToHmdBasis_(mat.GetPosition());
	const auto hmdRotMat = HmdRotMatFromHmdQuat_(ToHmdBasis_(mat.GetQuaternion()));
	return
	{
		hmdRotMat.m[0][0], hmdRotMat.m[0][1], hmdRotMat.m[0][2], hmdPos.v[0],
		hmdRotMat.m[1][0], hmdRotMat.m[1][1], hmdRotMat.m[1][2], hmdPos.v[1],
		hmdRotMat.m[2][0], hmdRotMat.m[2][1], hmdRotMat.m[2][2], hmdPos.v[2]
	};
}
vr::HmdVector3_t bsExp::OpenVrManager::HmdPosFromHmdMatrix_(const vr::HmdMatrix34_t matrix)
{
	return { matrix.m[0][3], matrix.m[1][3], matrix.m[2][3] };
};
vr::HmdQuaternionf_t bsExp::OpenVrManager::HmdQuatFromHmdMatrix_(const vr::HmdMatrix34_t matrix)
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

	float q[4] = { 0 };
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

	return { q[3], q[0], q[1], q[2] };
}

vr::HmdMatrix33_t bsExp::OpenVrManager::HmdRotMatFromHmdQuat_(const vr::HmdQuaternionf_t quat)
{
	// Taken from: https://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/
	return
	{
		(2.0f * (quat.w * quat.w + quat.x * quat.x) - 1.0f),	(2.0f * (quat.x * quat.y - quat.w * quat.z)),			(2.0f * (quat.x * quat.z + quat.w * quat.y)),
		(2.0f * (quat.x * quat.y + quat.w * quat.z)),			(2.0f * (quat.w * quat.w + quat.y * quat.y) - 1.0f),	(2.0f * (quat.y * quat.z - quat.w * quat.x)),
		(2.0f * (quat.x * quat.z - quat.w * quat.y)),			(2.0f * (quat.y * quat.z + quat.w * quat.x)),			(2.0f * (quat.w * quat.w + quat.z * quat.z) - 1.0f)
	};
}

vr::HmdQuaternionf_t bsExp::OpenVrManager::HmdQuatFromHmdRotMat(const vr::HmdMatrix33_t matrix)
{
	// Taken from: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	const auto& m = matrix.m; // Just an alias.
	const float w = std::sqrtf(1.0f + m[0][0] + m[1][1] + m[2][2]) * 0.5f;
	return
	{
		w,
		(m[2][1] - m[1][2]) / (4.0f * w),
		(m[0][2] - m[2][0]) / (4.0f * w),
		(m[1][0] - m[0][1]) / (4.0f * w)
	};
}

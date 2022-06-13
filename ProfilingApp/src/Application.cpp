#include <Application.h>

#include <cassert>
#include <stdlib.h>
#include <time.h>
#include <iostream>

bsExp::Application::Application(const char* hrtfFile, const char* brirFile, const char* soundFile, bs::ClipWrapMode wrapMode, const size_t bufferSize, const size_t sampleRate)
{
	// Init bachelor renderers
	bool result = threeDTI_renderer_.Init(hrtfFile, brirFile, bufferSize, sampleRate);
	threeDTI_renderer_.SetIsActive(false);
	assert(result, "Failed to initialize ThreeDTI_AudioRenderer!");
	result = steamAudio_renderer_.Init(bufferSize, sampleRate);
	steamAudio_renderer_.SetIsActive(false);
	assert(result, "Failed to initialize SteamAudio_AudioRenderer!");
	result = fmod_renderer_.Init(bufferSize, sampleRate);
	assert(result, "Failed to initialize FMod_AudioRenderer!");

	auto soundId = threeDTI_renderer_.CreateSoundMaker(soundFile, wrapMode);
	assert(soundId != bs::INVALID_ID, "ThreeDTI_AudioRenderer failed to load wav file!");
	soundId = steamAudio_renderer_.CreateSoundMaker(soundFile, wrapMode);
	assert(soundId != bs::INVALID_ID, "SteamAudio_AudioRenderer failed to load wav file!");
	soundId = fmod_renderer_.CreateSoundMaker(soundFile, wrapMode);
	assert(soundId != bs::INVALID_ID, "Fmod_AudioRenderer failed to load wav file!");

	// Init randomness lib and pick random renderer
	seed_ = time(NULL);
	srand(seed_);
	selectedRenderer_ = static_cast<bsExp::AudioRendererType>(rand() % 3);
	switch (selectedRenderer_)
	{
		case bsExp::AudioRendererType::ThreeDTI:
		{
			threeDTI_renderer_.SetIsActive(true);
		}break;

		case bsExp::AudioRendererType::SteamAudio:
		{
			steamAudio_renderer_.SetIsActive(true);
		}break;

		default: break;
	}

	// Init appropriate application SDK
#ifdef USE_DUMMY_INPUTS
	// Init sdl
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "Couldn't initialize SDL!" << std::endl;
		throw;
	}
	sdlWindow_ = SDL_CreateWindow("MyWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 720, 720, SDL_WINDOW_SHOWN);
	assert(sdlWindow_, "Failed to create sdl window!");
#else
	// Init openvr
	vr::HmdError vrErr;
	pVrSystem_ = vr::VR_Init(&vrErr, vr::EVRApplicationType::VRApplication_Background);
	assert(vrErr == vr::EVRInitError::VRInitError_None, "Error initializing OpenVR!");

	// Retireve ids of vr controllers.
	for (size_t device = 0; device < vr::k_unMaxTrackedDeviceCount; device++)
	{
		if (pVrSystem_->IsTrackedDeviceConnected(device) && pVrSystem_->GetTrackedDeviceClass(device) == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller)
		{
			if (controller0Id_ == vr::k_unTrackedDeviceIndexInvalid)
			{
				controller0Id_ = device;
			}
			else
			{
				controller1Id_ = device;
				break;
			}
		}
	}
	assert(controller0Id_ != vr::k_unTrackedDeviceIndexInvalid && controller1Id_ != vr::k_unTrackedDeviceIndexInvalid, "Failed to get ids for two controllers!");
#endif // USE_DUMMY_INPUTS
}

bsExp::Application::~Application()
{
	// Shutdown bachelor renderers
	threeDTI_renderer_.Shutdown();
	steamAudio_renderer_.Shutdown();
	fmod_renderer_.Shutdown();

	// Shutdown application sdk
#ifdef USE_DUMMY_INPUTS
	SDL_Quit();
#else
	vr::VR_Shutdown();
#endif //! USE_DUMMY_INPUTS
}

int bsExp::Application::RunProgram()
{
	while (!shutdown_)
	{
		// Poll events
#ifdef USE_DUMMY_INPUTS
		UpdateTransforms_();

		while (SDL_PollEvent(&sdlEvent_))
		{
			switch (sdlEvent_.type)
			{
				case SDL_KEYDOWN:
				{
					switch (sdlEvent_.key.keysym.scancode)
					{
						case SDL_SCANCODE_ESCAPE:
						{
							shutdown_ = true;
						}break;

						case SDL_SCANCODE_1:
						{
							ProcessTriggerPullOnController0_();
						}break;

						case SDL_SCANCODE_2:
						{
							ProcessPadPushOnController0_();
						}break;

						case SDL_SCANCODE_3:
						{
							ProcessTriggerPullOnController1_();
						}break;

						default:
						break;
					}
				}break;

				case SDL_QUIT:
				{
					shutdown_ = true;
				}break;

				default:
				break;
			}
		}
		fmod_renderer_.Update(); // Update fmod's C api
#else
		// Update transforms.
		if (pVrSystem_->IsTrackedDeviceConnected(hmdId_) &&
			pVrSystem_->IsTrackedDeviceConnected(controller0Id_) &&
			pVrSystem_->IsTrackedDeviceConnected(controller1Id_))
		{
			UpdateTransforms_();
		}
		else
		{
			std::cerr << "Error: not tracking required devices!" << std::endl;
			shutdown_ = true;
		}

		// Process inputs.
		constexpr const auto SIZE_OF_VR_EVENT = sizeof(vr::VREvent_t);
		while (pVrSystem_->PollNextEvent(&vrEvent_, SIZE_OF_VR_EVENT))
		{
			switch (vrEvent_.eventType)
			{
				case vr::EVREventType::VREvent_Quit:
				{
					shutdown_ = true;
				}break;

				case vr::EVREventType::VREvent_ButtonPress:
				{
					if (pVrSystem_->IsTrackedDeviceConnected(vrEvent_.trackedDeviceIndex) &&
						pVrSystem_->GetTrackedDeviceClass(vrEvent_.trackedDeviceIndex) == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller)
					{
						ProcessControllerInput_(vrEvent_.trackedDeviceIndex);
					}
				}break;

				case vr::EVREventType::VREvent_TrackedDeviceDeactivated:
				{
					controller0Id_ = vrEvent_.trackedDeviceIndex == controller0Id_ ? vr::k_unTrackedDeviceIndexInvalid : controller0Id_;
					controller1Id_ = vrEvent_.trackedDeviceIndex == controller1Id_ ? vr::k_unTrackedDeviceIndexInvalid : controller1Id_;
				}break;

				case vr::EVREventType::VREvent_TrackedDeviceActivated:
				{
					if (controller0Id_ == vr::k_unTrackedDeviceIndexInvalid &&
						pVrSystem_->IsTrackedDeviceConnected(vrEvent_.trackedDeviceIndex) &&
						pVrSystem_->GetTrackedDeviceClass(vrEvent_.trackedDeviceIndex) == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller)
					{
						controller0Id_ = vrEvent_.trackedDeviceIndex;
						break;
					}

					if (controller1Id_ == vr::k_unTrackedDeviceIndexInvalid &&
						pVrSystem_->IsTrackedDeviceConnected(vrEvent_.trackedDeviceIndex) &&
						pVrSystem_->GetTrackedDeviceClass(vrEvent_.trackedDeviceIndex) == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller)
					{
						controller1Id_ = vrEvent_.trackedDeviceIndex;
						break;
					}
				}break;

				default:
				break;
			}
		}
		fmod_renderer_.Update();
#endif // USE_DUMMY_INPUTS
	}

	return 0;
}

void bsExp::Application::ProcessControllerInput_(const vr::TrackedDeviceIndex_t device)
{
	vr::VRControllerState_t state;

	constexpr const auto SIZE_OF_STATE = sizeof(vr::VRControllerState_t);
	if (pVrSystem_->GetControllerState(device, &state, SIZE_OF_STATE))
	{
		const auto buttonFlags = state.ulButtonPressed;
		const bool triggerDown = buttonFlags & ((uint64_t)1 << (uint64_t)vr::EVRButtonId::k_EButton_SteamVR_Trigger); // Set controller trigger bit in bitmask.

		if (triggerDown && vrEvent_.trackedDeviceIndex == controller0Id_)
		{
			ProcessTriggerPullOnController0_();
		}
		else if (triggerDown && vrEvent_.trackedDeviceIndex == controller1Id_)
		{
			ProcessTriggerPullOnController1_();
		}
	}
}

void bsExp::Application::ProcessTriggerPullOnController0_()
{
	std::cout << "Trigger on controller 0 press detected! Position of controller 0 is: " << controller0Transform_.m[0][3] << "; " << controller0Transform_.m[1][3] << "; " << controller0Transform_.m[2][3] << std::endl;
}

void bsExp::Application::ProcessTriggerPullOnController1_()
{
	std::cout << "Trigger on controller 1 press detected! Position of controller 1 is: " << controller1Transform_.m[0][3] << "; " << controller1Transform_.m[1][3] << "; " << controller1Transform_.m[2][3] << std::endl;
	std::cout << "Headset position is: " << hmdTransform_.m[0][3] << "; " << hmdTransform_.m[1][3] << "; " << hmdTransform_.m[2][3] << std::endl;
}

void bsExp::Application::ProcessPadPushOnController0_()
{
	selectedRenderer_ = static_cast<bsExp::AudioRendererType>(rand() % 3);
	currentSoundPos_ = RandomCartesianPos_();
	switch (selectedRenderer_)
	{
		case AudioRendererType::ThreeDTI:
		{
			threeDTI_renderer_.MoveSoundMaker(0, currentSoundPos_.x, currentSoundPos_.y, currentSoundPos_.z);
			// threeDTI_renderer_.ResetSoundMaker(0);
		}
		break;
		case AudioRendererType::SteamAudio:
		{
			steamAudio_renderer_.MoveSoundMaker(0, currentSoundPos_.x, currentSoundPos_.y, currentSoundPos_.z);
		}
		break;
		case AudioRendererType::FMod:
		{
			fmod_renderer_.MoveSoundMaker(0, currentSoundPos_.x, currentSoundPos_.y, currentSoundPos_.z);
			fmod_renderer_.PlaySound(0);
		}
		break;
		default:break;
	}
}

bs::CartesianCoord bsExp::Application::PositionFromMatrix_(const vr::HmdMatrix34_t matrix)
{
	return { matrix.m[0][3], matrix.m[1][3], matrix.m[2][3] };
}

bs::CartesianCoord bsExp::Application::RandomCartesianPos_(const float minRadius, const float maxRadius)
{
	bs::SphericalCoord s;
	float fraction = static_cast<float>(rand() % 100) / 100.0f; // Random value between 0.0f and 1.0f
	s.azimuth = bs::RemapToRange(fraction, 0.0f, 1.0f, -180.0f, 180.0f);
	fraction = static_cast<float>(rand() % 100) / 100.0f;
	s.elevation = bs::RemapToRange(fraction, 0.0f, 1.0f, -90.0f, 90.0f);
	fraction = static_cast<float>(rand() % 100) / 100.0f;
	s.radius = bs::RemapToRange(fraction, 0.0f, 1.0f, minRadius, maxRadius);
	return bs::ToCartesian(s);
}

void bsExp::Application::UpdateTransforms_()
{
#ifdef USE_DUMMY_INPUTS
	hmdTransform_.m[0][3] = (float)(rand() % 10 + 1);
	hmdTransform_.m[1][3] = (float)(rand() % 10 + 1);
	hmdTransform_.m[2][3] = (float)(rand() % 10 + 1);

	controller0Transform_.m[0][3] = (float)(rand() % 10 + 1);
	controller0Transform_.m[1][3] = (float)(rand() % 10 + 1);
	controller0Transform_.m[2][3] = (float)(rand() % 10 + 1);

	controller1Transform_.m[0][3] = (float)(rand() % 10 + 1);
	controller1Transform_.m[1][3] = (float)(rand() % 10 + 1);
	controller1Transform_.m[2][3] = (float)(rand() % 10 + 1);
#else
	vr::VRControllerState_t hmdState, controller0State, controller1State; // Not actually needed but there's no GetControllerPose() function so...
	vr::TrackedDevicePose_t hmdPose, controller0Pose, controller1Pose;

	constexpr const auto SIZE_OF_CONTROLLER_STATE = sizeof(vr::VRControllerState_t);
	bool result = pVrSystem_->GetControllerStateWithPose(vr::TrackingUniverseRawAndUncalibrated, hmdId_, &hmdState, SIZE_OF_CONTROLLER_STATE, &hmdPose); // Oleg@self: investigate vr::TrackingUniverseStanding
	assert(result && hmdPose.bPoseIsValid, "Failed to get hmd state!");
	result = pVrSystem_->GetControllerStateWithPose(vr::TrackingUniverseRawAndUncalibrated, controller0Id_, &controller0State, SIZE_OF_CONTROLLER_STATE, &controller0Pose);
	assert(result && controller0Pose.bPoseIsValid, "Failed to get controller0 state!");
	result = pVrSystem_->GetControllerStateWithPose(vr::TrackingUniverseRawAndUncalibrated, controller1Id_, &controller1State, SIZE_OF_CONTROLLER_STATE, &controller1Pose);
	assert(result && controller1Pose.bPoseIsValid, "Failed to get controller1 state!");

	hmdTransform_ = hmdPose.mDeviceToAbsoluteTracking;
	controller0Transform_ = controller0Pose.mDeviceToAbsoluteTracking;
	controller1Transform_ = controller1Pose.mDeviceToAbsoluteTracking;
#endif //! USE_DUMMY_INPUTS
}

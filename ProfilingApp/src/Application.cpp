#include <Application.h>

#include <cassert>

#include <spdlog/spdlog.h>

bsExp::Application::Application(const char* hrtfFile, const char* brirFile, const char* soundFile, const size_t randSeed)
{
	// Init log file.
	pLogger_ = spdlog::basic_logger_mt("DummyLogger", "experimentData/dummyLog.txt");
	pLogger_->info("Application starting up...!");

	// Init bachelor renderers
	// Oleg@self: configure room spaces for each engine if possible: 8x7x3, facing the 8 axis.
	constexpr const size_t BUFFER_SIZE = 1024;
	constexpr const size_t SAMPLE_RATE = 44100;
	bool result = threeDTI_renderer_.Init(hrtfFile, brirFile, BUFFER_SIZE, SAMPLE_RATE);
	threeDTI_renderer_.SetIsActive(false);
	assert(result, "Failed to initialize ThreeDTI_AudioRenderer!");
	result = steamAudio_renderer_.Init(BUFFER_SIZE, SAMPLE_RATE);
	steamAudio_renderer_.SetIsActive(false);
	assert(result, "Failed to initialize SteamAudio_AudioRenderer!");
	result = fmod_renderer_.Init(BUFFER_SIZE, SAMPLE_RATE);
	assert(result, "Failed to initialize FMod_AudioRenderer!");
	result = noise_renderer_.Init(BUFFER_SIZE, SAMPLE_RATE);
	assert(result, "Failed to initialize Noise_AudioRenderer!");

	auto soundId = threeDTI_renderer_.CreateSoundMaker(soundFile, bs::ClipWrapMode::ONE_SHOT);
	assert(soundId != bs::INVALID_ID, "ThreeDTI_AudioRenderer failed to load wav file!");
	soundId = steamAudio_renderer_.CreateSoundMaker(soundFile, bs::ClipWrapMode::ONE_SHOT);
	assert(soundId != bs::INVALID_ID, "SteamAudio_AudioRenderer failed to load wav file!");
	soundId = fmod_renderer_.CreateSoundMaker(soundFile, bs::ClipWrapMode::ONE_SHOT);
	assert(soundId != bs::INVALID_ID, "Fmod_AudioRenderer failed to load wav file!");
	soundId = noise_renderer_.CreateSoundMaker(soundFile, bs::ClipWrapMode::LOOP);
	assert(soundId != bs::INVALID_ID, "Noise_AudioRenderer failed to load wav file!");

	// Init randomness lib and pick random renderer
	constexpr const float MIN_ELEVATION = -15.0f; // In euler degrees. -15 instead of -90 since we don't want sound sources to spawn inside the ground, making it impossible for the participants to place the vr controller at the source of the sound.
	constexpr const float MAX_ELEVATION = 45.0f; // In euler degrees. 45 instead of 90 since we don't want sound sources to spawn too high up for the participants to place the vr controller at the source of the sound.
	constexpr const float MIN_RADIUS = 0.15f; // 15 cm away from listener at the closest to allow the experiment to take into account near-field effects.
	constexpr const float MAX_RADIUS = 2.5f; // 2.5 meters away from the listener at the furthest due to room dimensions.
	seed_ = randSeed;
	pLogger_->info("Seeding normal distributions with seed_: {0:d}", seed_);
	distrAzimuth_.SetSeed(seed_);
	distrElevation_.SetSeed(seed_);
	distrRadius_.SetSeed(seed_);
	distrMiddleware_.SetSeed(seed_);
	distrAzimuth_.SetDistributionRange(-180.0f, 180.0f);
	distrElevation_.SetDistributionRange(MIN_ELEVATION, MAX_ELEVATION);
	distrRadius_.SetDistributionRange(MIN_RADIUS, MAX_RADIUS);
	distrMiddleware_.SetDistributionRange(0, 2);
	selectedRenderer_ = AudioRendererType::Noise;

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
	pLogger_->info("SDL initialized.");
#else
	// Init openvr
	vr::HmdError vrErr;
	pVrSystem_ = vr::VR_Init(&vrErr, vr::EVRApplicationType::VRApplication_Background);
	assert(vrErr == vr::EVRInitError::VRInitError_None, "Error initializing OpenVR!");
	pLogger_->info("Openvr initialized.");

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
	pLogger_->info("Identified vr controllers. Controller0Id_ is %u, Controller1Id_ is %u", controller0Id_, controller1Id_);
#endif // USE_DUMMY_INPUTS
	pLogger_->info("Application has finished initialization.");
}

bsExp::Application::~Application()
{
	pLogger_->info("Shutting down Application.");
	// Shutdown bachelor renderers
	threeDTI_renderer_.Shutdown();
	steamAudio_renderer_.Shutdown();
	fmod_renderer_.Shutdown();
	noise_renderer_.Shutdown();

	// Shutdown application sdk
#ifdef USE_DUMMY_INPUTS
	SDL_Quit();
#else
	vr::VR_Shutdown();
#endif //! USE_DUMMY_INPUTS
	pLogger_->info("Application has shut down.");
}

int bsExp::Application::RunProgram()
{
	pLogger_->info("Application is now running.");

	while (!shutdown_)
	{
		// Poll events
#ifdef USE_DUMMY_INPUTS
		UpdateTransforms_();

		constexpr const auto PARTICIPANT_TRIGGER = SDL_SCANCODE_1;

		constexpr const auto SCIENTIST_TRIGGER = SDL_SCANCODE_2;
		constexpr const auto LOG_DELIMITER = SDL_SCANCODE_4;
		constexpr const auto TOGGLE_NOISE = SDL_SCANCODE_3;
		constexpr const auto CHANGE_MIDDLEWARE_AND_POS = SDL_SCANCODE_5;

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

						case PARTICIPANT_TRIGGER:
						{
							LogControllerPose_(controller1Id_);
						}break;

						case SCIENTIST_TRIGGER:
						{
							LogControllerPose_(controller0Id_);
						}break;

						case LOG_DELIMITER:
						{
							LogDelimiter_();
						}break;

						case TOGGLE_NOISE:
						{
							ToggleNoise_();
						}break;

						case CHANGE_MIDDLEWARE_AND_POS:
						{
							SetRandomRenderer_();
							SetRandomSourcePos_();
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
	pLogger_->info("Application is now stopping.");

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

		}
		else if (triggerDown && vrEvent_.trackedDeviceIndex == controller1Id_)
		{

		}
	}
}

bool bsExp::Application::ToggleNoise_()
{
	return false;
}

void bsExp::Application::LogControllerPose_(const vr::TrackedDeviceIndex_t device)
{
}

void bsExp::Application::SetRandomSourcePos_()
{
}

void bsExp::Application::SetRandomRenderer_()
{
}

void bsExp::Application::LogDelimiter_()
{
}

bs::CartesianCoord bsExp::Application::PositionFromMatrix_(const vr::HmdMatrix34_t matrix)
{
	return { matrix.m[0][3], matrix.m[1][3], matrix.m[2][3] };
}

void bsExp::Application::UpdateTransforms_()
{
#ifdef USE_DUMMY_INPUTS
	// Set transforms to random positions.

	bs::SphericalCoord s{ distrAzimuth_.Generate(), distrElevation_.Generate(), distrRadius_.Generate() };
	auto c = bs::ToCartesian(s);
	hmdTransform_.m[0][3] = c.x;
	hmdTransform_.m[1][3] = c.y;
	hmdTransform_.m[2][3] = c.z;

	s = { distrAzimuth_.Generate(), distrElevation_.Generate(), distrRadius_.Generate() };
	c = bs::ToCartesian(s);
	controller0Transform_.m[0][3] = c.x;
	controller0Transform_.m[1][3] = c.y;
	controller0Transform_.m[2][3] = c.z;

	s = { distrAzimuth_.Generate(), distrElevation_.Generate(), distrRadius_.Generate() };
	c = bs::ToCartesian(s);
	controller1Transform_.m[0][3] = c.x;
	controller1Transform_.m[1][3] = c.y;
	controller1Transform_.m[2][3] = c.z;
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

size_t bsExp::UniDistrUintGen::seed_ = (size_t)-1;

void bsExp::UniDistrUintGen::SetSeed(const size_t seed)
{
	seed_ = seed;
	e_ = std::default_random_engine(seed_);
}

void bsExp::UniDistrUintGen::SetDistributionRange(const size_t min, const size_t max)
{
	d_ = std::uniform_int_distribution(min, max);
}

size_t bsExp::UniDistrUintGen::Generate()
{
	return d_(e_);
}

size_t bsExp::NormDistrFloatGen::seed_ = (size_t)-1;

void bsExp::NormDistrFloatGen::SetSeed(const size_t seed)
{
	seed_ = seed;
	e_ = std::default_random_engine(seed_);
}

void bsExp::NormDistrFloatGen::SetDistributionRange(const float min, const float max)
{
	d_ = std::normal_distribution(min, max);
}

float bsExp::NormDistrFloatGen::Generate()
{
	return d_(e_);
}
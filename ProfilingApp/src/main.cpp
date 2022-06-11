#include <iostream>
#include <array>
#include <cassert>
#include <stdlib.h>
#include <time.h>

#include <openvr.h>

#include <SteamAudio_AudioRenderer.h>
#include "Fmod_AudioRenderer.h"
#include <ThreeDTI_AudioRenderer.h>

namespace bs
{
	enum class EAudioRendererType: unsigned int
	{
		ThreeDTI = 0,
		SteamAudio = 1,
		FMod = 2
	};

	using EngineVector = std::array<float, 3>;

	class Application
	{
	public:
		Application(const char* hrtfFile, const char* brirFile, const char* soundFile, bs::ClipWrapMode wrapMode, const size_t bufferSize, const size_t sampleRate)
		{
			bool result = threeDTI_renderer_.Init(hrtfFile, brirFile, bufferSize, sampleRate);
			assert(result, "Failed to initialize ThreeDTI_AudioRenderer!");
			result = steamAudio_renderer_.Init(bufferSize, sampleRate);
			assert(result, "Failed to initialize SteamAudio_AudioRenderer!");
			result = fmod_renderer_.Init(bufferSize, sampleRate);
			assert(result, "Failed to initialize FMod_AudioRenderer!");

			auto soundId = threeDTI_renderer_.CreateSoundMaker(soundFile, wrapMode);
			assert(soundId != bs::INVALID_ID, "ThreeDTI_AudioRenderer failed to load wav file!");
			soundId = steamAudio_renderer_.CreateSoundMaker(soundFile, wrapMode);
			assert(soundId != bs::INVALID_ID, "SteamAudio_AudioRenderer failed to load wav file!");
			soundId = fmod_renderer_.CreateSoundMaker(soundFile, wrapMode);
			assert(soundId != bs::INVALID_ID, "Fmod_AudioRenderer failed to load wav file!");

			vr::HmdError vrErr;
			pVrSystem_ = vr::VR_Init(&vrErr, vr::EVRApplicationType::VRApplication_Background);
			assert(vrErr == vr::EVRInitError::VRInitError_None, "Error initializing OpenVR!");

			// Retireve ids of controllers.
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

			seed_ = time(NULL);
			srand(seed_);
			selectedRenderer_ = static_cast<bs::EAudioRendererType>(rand() % 2);
		}
		~Application()
		{
			threeDTI_renderer_.Shutdown();
			steamAudio_renderer_.Shutdown();
			fmod_renderer_.Shutdown();
			vr::VR_Shutdown();
		}

		Application() = delete;
		Application(const Application&) = delete;
		Application(const Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		int RunProgram()
		{
			while (!shutdown_)
			{
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
			}
		}

	private:
		void ChangeSourcePosition_()
		{
			static size_t currentPosition = 0;
			constexpr const size_t NR_OF_POSITIONS = 6;
			constexpr const std::array<std::array<float, 3>, NR_OF_POSITIONS> positions =
			{ {
				{1.0f, 0.0f, 0.0f},
				{-1.0f, 0.0f, 0.0f},

				{0.0f, 1.0f, 0.0f},
				{0.0f, -1.0f, 0.0f},

				{0.0f, 0.0f, 1.0f},
				{0.0f, 0.0f, -1.0f}
			} };

			if (++currentPosition >= NR_OF_POSITIONS) currentPosition = 0;

			switch (selectedRenderer_)
			{
			case EAudioRendererType::ThreeDTI: {
				threeDTI_renderer_.MoveSoundMaker(0, positions[currentPosition][0], positions[currentPosition][1], positions[currentPosition][2]);
				// threeDTI_renderer_.ResetSoundMaker(0);
			}
				break;
			case EAudioRendererType::SteamAudio: {
				steamAudio_renderer_.MoveSoundMaker(0, positions[currentPosition][0], positions[currentPosition][1], positions[currentPosition][2]);
			}
				break;
			case EAudioRendererType::FMod: {
				fmod_renderer_.MoveSoundMaker(0, positions[currentPosition][0], positions[currentPosition][1], positions[currentPosition][2]);
				fmod_renderer_.PlaySound(0);
			}
				break;
			default:
				break;
			}
		}
		void ProcessControllerInput_(const vr::TrackedDeviceIndex_t device)
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
		void ProcessTriggerPullOnController0_()
		{
			std::cout << "Trigger on controller 0 press detected!" << std::endl;
		}
		void ProcessTriggerPullOnController1_()
		{
			std::cout << "Trigger on controller 1 press detected!" << std::endl;
		}
		void ProcessPadPushOnController0_()
		{
			std::cout << "Pad was pushed on controller 0!" << std::endl;
			selectedRenderer_ = static_cast<bs::EAudioRendererType>(rand() % 2);
			ChangeSourcePosition_();
		}
		void UpdateTransforms_()
		{
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
		}
		inline static EngineVector PositionFromMatrix(const vr::HmdMatrix34_t matrix)
		{
			return { matrix.m[0][3], matrix.m[1][3], matrix.m[2][3] };
		}

		bs::ThreeDTI_AudioRenderer threeDTI_renderer_{};
		bs::SteamAudio_AudioRenderer steamAudio_renderer_{};
		bs::Fmod_AudioRenderer fmod_renderer_{};
		EAudioRendererType selectedRenderer_ = EAudioRendererType::ThreeDTI;

		vr::IVRSystem* pVrSystem_ = nullptr;
		vr::VREvent_t vrEvent_{};
		vr::TrackedDeviceIndex_t controller0Id_ = vr::k_unTrackedDeviceIndexInvalid; // Warning: I don't think that reconnecting a previously disconnected devices will yield the same device index!
		vr::TrackedDeviceIndex_t controller1Id_ = vr::k_unTrackedDeviceIndexInvalid;
		constexpr const static vr::TrackedDeviceIndex_t hmdId_ = vr::k_unTrackedDeviceIndex_Hmd;
		vr::HmdMatrix34_t hmdTransform_{ 0.0f };
		vr::HmdMatrix34_t controller0Transform_{ 0.0f };
		vr::HmdMatrix34_t controller1Transform_{ 0.0f };

		bool shutdown_ = false;
		unsigned int seed_ = 0;
	};
}


int main()
{
	auto app = bs::Application("../../resources/HRTF/SOFA/3DTI_HRTF_IRC1008_128s_44100Hz.sofa", "../../resources/BRIR/SOFA/3DTI_BRIR_medium_44100Hz.sofa", "../../resources/AudioSamples/AnechoicSpeech44100.wav", bs::ClipWrapMode::LOOP, 1024, 44100);
	return app.RunProgram();
}
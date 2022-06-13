#pragma once

#include <array>
#include <fstream>

#include <openvr.h>
#ifdef USE_DUMMY_INPUTS
#include <SDL.h>
#undef main
#endif //! USE_DUMMY_INPUTS

#include "ThreeDTI_AudioRenderer.h"
#include "SteamAudio_AudioRenderer.h"
#include "Fmod_AudioRenderer.h"

namespace bsExp
{
	enum class AudioRendererType : unsigned int // unsigned int to ensure we can do simple arithmetic with it.
	{
		ThreeDTI = 0,
		SteamAudio = 1,
		FMod = 2
	};

	class Application
	{
	public:
		Application(const char* hrtfFile, const char* brirFile, const char* soundFile, bs::ClipWrapMode wrapMode, const size_t bufferSize, const size_t sampleRate);
		~Application();
		Application() = delete;
		Application(const Application&) = delete;
		Application(const Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		int RunProgram();

	private:
		void UpdateTransforms_();

		void ProcessControllerInput_(const vr::TrackedDeviceIndex_t device);
		void ProcessTriggerPullOnController0_();
		void ProcessTriggerPullOnController1_();
		void ProcessPadPushOnController0_();

		static bs::CartesianCoord PositionFromMatrix_(const vr::HmdMatrix34_t matrix);
		static bs::CartesianCoord RandomCartesianPos_(const float minRadius = 0.0f, const float maxRadius = 10.0f);

	private:
		// Renderers
		bs::ThreeDTI_AudioRenderer threeDTI_renderer_{};
		bs::SteamAudio_AudioRenderer steamAudio_renderer_{};
		bs::Fmod_AudioRenderer fmod_renderer_{};
		AudioRendererType selectedRenderer_ = AudioRendererType::ThreeDTI;
		bs::CartesianCoord currentSoundPos_{};

		// OpenVR stuff
		vr::IVRSystem* pVrSystem_ = nullptr;
		vr::VREvent_t vrEvent_{};
		vr::TrackedDeviceIndex_t controller0Id_ = vr::k_unTrackedDeviceIndexInvalid; // Warning: I don't think that reconnecting a previously disconnected devices will yield the same device index!
		vr::TrackedDeviceIndex_t controller1Id_ = vr::k_unTrackedDeviceIndexInvalid;
		constexpr const static vr::TrackedDeviceIndex_t hmdId_ = vr::k_unTrackedDeviceIndex_Hmd;
		vr::HmdMatrix34_t hmdTransform_{ 0.0f };
		vr::HmdMatrix34_t controller0Transform_{ 0.0f };
		vr::HmdMatrix34_t controller1Transform_{ 0.0f };

		// App specific stuff
		bool shutdown_ = false;
		unsigned int seed_ = 0;

		// SDL stuff if used
#ifdef USE_DUMMY_INPUTS
		SDL_Event sdlEvent_{};
		SDL_Window* sdlWindow_ = nullptr;
#endif // USE_DUMMY_INPUTS

		// Logging stuff
		std::ofstream logStream_;
	};
}
#pragma once

#include <array>
#include <random>

#include <openvr.h>
#ifdef USE_DUMMY_INPUTS
#include <SDL.h>
#undef main
#endif //! USE_DUMMY_INPUTS
#include <spdlog/sinks/basic_file_sink.h>

#include "ThreeDTI_AudioRenderer.h"
#include "SteamAudio_AudioRenderer.h"
#include "Fmod_AudioRenderer.h"
#include "Noise_AudioRenderer.h"

namespace bsExp
{
	enum class AudioRendererType : size_t // size_t to ensure we can do simple arithmetic with it when generating random numbers.
	{
		ThreeDTI = 0,
		SteamAudio = 1,
		FMod = 2,
		Noise = 3
	};

	// Oleg@self: make a template class out of this
	class NormDistrFloatGen
	{
	public:
		void SetSeed(const size_t seed);
		void SetDistributionRange(const float min, const float max);
		float Generate();

	private:
		std::default_random_engine e_;
		std::normal_distribution<float> d_;

		static size_t seed_;
	};

	class UniDistrUintGen
	{
	public:
		void SetSeed(const size_t seed);
		void SetDistributionRange(const size_t min, const size_t max);
		size_t Generate();

	private:
		std::default_random_engine e_;
		std::uniform_int_distribution<size_t> d_;

		static size_t seed_;
	};

	class Application
	{
	public:
		Application(const char* hrtfFile, const char* brirFile, const char* soundFile, const size_t randSeed);
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
		bool ToggleNoise_();
		void LogControllerPose_(const vr::TrackedDeviceIndex_t device);
		void SetRandomSourcePos_();
		void SetRandomRenderer_();
		void LogDelimiter_();

		static bs::CartesianCoord PositionFromMatrix_(const vr::HmdMatrix34_t matrix);

	private:
		// Renderers
		bs::ThreeDTI_AudioRenderer threeDTI_renderer_{};
		bs::SteamAudio_AudioRenderer steamAudio_renderer_{};
		bs::Fmod_AudioRenderer fmod_renderer_{};
		bsExp::Noise_AudioRenderer noise_renderer_{};
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
		NormDistrFloatGen distrAzimuth_, distrElevation_, distrRadius_;
		UniDistrUintGen distrMiddleware_;

		// SDL stuff if used
#ifdef USE_DUMMY_INPUTS
		SDL_Event sdlEvent_{};
		SDL_Window* sdlWindow_ = nullptr;
#endif // USE_DUMMY_INPUTS

		// Logging stuff
		std::shared_ptr<spdlog::logger> pLogger_;
	};
}
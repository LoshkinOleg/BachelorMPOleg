#pragma once

#include "portaudio.h"

#include "ThreeDTI_AudioRenderer.h"
#include "SteamAudio_AudioRenderer.h"

namespace bsExp
{
	class RendererManager
	{
	public:
		enum class AudioRendererType: size_t
		{
			ThreeDTI = 0,
			SteamAudio = 1,

			MAX = SteamAudio
		};

		BS_NON_COPYABLE(RendererManager);
		BS_NON_MOVEABLE(RendererManager);

		RendererManager();
		~RendererManager();

		void CallImplementationsUpdates();

		void PlaySound(const char* soundName);
		void PauseSound(const char* soundName);
		bool IsPaused(const char* soundName);
		void StopSound(const char* soundName);
		void StopAll();

		void MoveSound(const char* soundName, const bs::CartesianCoord coord);
		void MoveAllSounds(const bs::CartesianCoord coord);

		void SetSelectedRenderer(const AudioRendererType type);
		AudioRendererType GetSelectedRenderer() const;

		constexpr static float const MIN_SOUND_AZIMUTH = bs::ToRadians(-180.0f);
		constexpr static float const MAX_SOUND_AZIMUTH = bs::ToRadians(180.0f);
		constexpr static float const MIN_SOUND_DISTANCE = 0.33f;
		constexpr static float const MAX_SOUND_DISTANCE = 3.0f;
		constexpr static float const HEAD_ALTITUDE = 1.3f; // Oleg@self: find a way to pass this as argument to program. Move to application maybe since that's where it's used mostly?
		constexpr static float const MIN_SOUND_ELEVATION = bs::ToRadians(50.0f);
		constexpr static float const MAX_SOUND_ELEVATION = bs::ToRadians(120.0f);
		constexpr static char const* const HRTF_PATH = "../resources/HRTF/SOFA/3DTI_HRTF_IRC1008_128s_44100Hz.sofa";
		constexpr static char const* const BRIR_PATH = "../resources/BRIR/SOFA/3DTI_BRIR_medium_44100Hz.sofa";
		constexpr static char const* const WAV_PATH_SPEECH = "../resources/AudioSamples/AnechoicSpeech44100.wav";
		constexpr static char const* const WAV_PATH_BROWN_NOISE = "../resources/AudioSamples/brownNoise_44100Hz_32f_5sec.wav";
		constexpr static char const* const WAV_PATH_SWEEP = "../resources/AudioSamples/sweep20to16000_44100Hz_32f_10sec.wav";
		constexpr static size_t const BUFFER_SIZE = 1024;
		constexpr static size_t const SAMPLE_RATE = 44100;
		constexpr static float const ROOM_SIZE_X = 7.0f;
		constexpr static float const ROOM_SIZE_Y = 8.0f;
		constexpr static float const ROOM_SIZE_Z = 3.0f;
		constexpr static size_t const MIN_RENDERER_INDEX = 0;
		constexpr static size_t const MAX_RENDERER_INDEX = (size_t)AudioRendererType::MAX;

	private:
		static int ServiceAudio_
		(
			const void* unused, void* outputBuffer,
			unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void* userData
		);

		AudioRendererType selectedRenderer_ = AudioRendererType::ThreeDTI;
		bs::ThreeDTI_AudioRenderer threeDTI_renderer_;
		std::map<std::string, size_t> threeDTI_soundIds_;
		bs::SteamAudio_AudioRenderer steamAudio_renderer_;
		std::map<std::string, size_t> steamAudio_soundIds_;

		PaStream* pStream_ = nullptr;
		std::vector<float> renderResult_;
	};
}
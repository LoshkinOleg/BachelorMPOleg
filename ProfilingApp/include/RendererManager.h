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

		RendererManager() = delete;
		RendererManager(const char* hrtfFile, const char* brirFile);
		~RendererManager();

		bool ToggleNoise();

		void SetRandomRenderer();
		void SetSelectedRenderer(const AudioRendererType type);
		AudioRendererType GetSelectedRenderer() const;

		void StartRendering();
		void StopRendering();
		void PauseRendering();

		constexpr static float const MIN_SOUND_AZIMUTH = -180.0f;
		constexpr static float const MIN_SOUND_AZIMUTH = 180.0f;
		constexpr static float const MIN_SOUND_DISTANCE = 0.15f;
		constexpr static float const MAX_SOUND_DISTANCE = 2.5f;
		constexpr static float const MIN_SOUND_ELEVATION = -15.0f;
		constexpr static float const MAX_SOUND_ELEVATION = 30.0f;
		constexpr static char const* const HRTF_PATH = "../resources/HRTF/SOFA/3DTI_HRTF_IRC1008_128s_44100Hz.sofa";
		constexpr static char const* const BRIR_PATH = "../resources/BRIR/SOFA/3DTI_BRIR_medium_44100Hz.sofa";
		constexpr static char const* const WAV_PATH = "../resources/AudioSamples/AnechoicSpeech44100.wav";
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

		bs::ThreeDTI_AudioRenderer threeDTI_renderer_{};
		bs::SteamAudio_AudioRenderer steamAudio_renderer_{};
		AudioRendererType selectedRenderer_ = AudioRendererType::ThreeDTI;

		PaStream* pStream_ = nullptr;
	};
}
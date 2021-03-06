#pragma once

#include "portaudio.h"

#include "ThreeDTI_AudioRenderer.h"
#include "Fmod_AudioRenderer.h"

namespace bsExp
{
	class RendererManager
	{
	public:
		enum class AudioRendererType: size_t
		{
			ThreeDTI = 0,
			Fmod = 1,

			MAX = Fmod
		};

		struct RendererParams
		{
			AudioRendererType rendererType = AudioRendererType::ThreeDTI;
			
			// Params for 3dti
			bool ILDEnabled = true;
		};

		struct SoundParams
		{
			bool anechoicEnabled = true;
			bool distanceBasedAttenuationAnechoic = true;

			bool reverbEnabled = true;
			bool distanceBasedAttenuationReverb = true;

			bool highQualitySimulation = true;
			bool atmosphericFiltering = true;
			bool nearFieldEffects = true;
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
		bool AnyPlaying();

		void MoveSound(const char* soundName, const bs::CartesianCoord coord);
		void MoveAllSounds(const bs::CartesianCoord coord);
		void SetListenerTransform(const bs::Mat3x4& mat);

		void UpdateRendererParams(const RendererParams p);
		void UpdateSoundParams(const char* soundName, const SoundParams p);
		
		RendererParams GetRendererParams() const;
		SoundParams GetSoundParams(const char* soundName);

		void SetSelectedRenderer(const AudioRendererType type);
		AudioRendererType GetSelectedRenderer() const;

		constexpr static float const MIN_SOUND_AZIMUTH = bs::ToRadians(-180.0f);
		constexpr static float const MAX_SOUND_AZIMUTH = bs::ToRadians(180.0f);
		constexpr static float const MIN_SOUND_DISTANCE = 0.33f;
		constexpr static float const MAX_SOUND_DISTANCE = 1.9f;
		constexpr static float const HEAD_ALTITUDE = 1.2f; // Oleg@self: find a way to pass this as argument to program. Move to application maybe since that's where it's used mostly?
		// constexpr static float const HEAD_ALTITUDE = 0.0f; // Oleg@self: find a way to pass this as argument to program. Move to application maybe since that's where it's used mostly?
		constexpr static float const MIN_SOUND_ELEVATION = bs::ToRadians(50.0f); // Values chosen to land at ~(2,0,2) in cartesian relative to ground. 70.0f
		constexpr static float const MAX_SOUND_ELEVATION = bs::ToRadians(123.0f); // Values chosen to land at ~(2,0,0) in cartesian relative to ground. 123.0f
		constexpr static char const* const HRTF_PATH = "../resources/HRTF/3DTI_HRTF_IRC1008_128s_44100Hz.sofa";
		constexpr static char const* const BRIR_PATH = "../resources/BRIR/3DTI_BRIR_small_44100Hz.sofa";
		constexpr static char const* const WAV_PATH_SPEECH = "../resources/AudioSamples/olegSpeech_44100Hz_32f.wav";
		constexpr static char const* const WAV_PATH_BROWN_NOISE = "../resources/AudioSamples/brownNoise_44100Hz_32f_5sec.wav";
		constexpr static char const* const WAV_PATH_SQUEAK = "../resources/AudioSamples/squeak_44100Hz_32f.wav";
		
#ifdef _DEBUG
		constexpr static size_t const BUFFER_SIZE = 2048;
#else
		constexpr static size_t const BUFFER_SIZE = 512;
#endif // _DEBUG

		
		constexpr static size_t const SAMPLE_RATE = 44100;
		constexpr static float const ROOM_SIZE_X = 7.0f; // Unused
		constexpr static float const ROOM_SIZE_Y = 8.0f; // Unused
		constexpr static float const ROOM_SIZE_Z = 3.0f; // Unused
		constexpr static size_t const MIN_RENDERER_INDEX = 0;
		constexpr static size_t const MAX_RENDERER_INDEX = (size_t)AudioRendererType::MAX;
		constexpr static float const FMOD_DEFAULT_REVERB_PARAMS[12] = FMOD_PRESET_ROOM;

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
		bs::Fmod_AudioRenderer fmod_renderer_;
		std::map<std::string, size_t> fmod_soundIds_;

		PaStream* pStream_ = nullptr;
		std::vector<float> renderResult_;
	};
}
#pragma once

#include <vector>

#include <portaudio.h>
#include <phonon.h>
#include <cmath>

#include "BSCommon.h"

// Oleg@self: make an abstraction out of this.

namespace bs
{
	class SteamAudio_AudioRenderer;

	class SteamAudio_SoundMaker
	{
	public:
		SteamAudio_SoundMaker() = default;
		SteamAudio_SoundMaker(const SteamAudio_SoundMaker&) = delete; // Owns a resource it's responsible for, so no copying.
		SteamAudio_SoundMaker(SteamAudio_SoundMaker&&) = default; // There's no reason for moving this class around.
		SteamAudio_SoundMaker& operator=(const SteamAudio_SoundMaker&) = delete; // Owns a resource it's responsible for, so no copy assignement.
		SteamAudio_SoundMaker& operator=(SteamAudio_SoundMaker&&) = default; // There's no need for move assignment for this class.

		// Oleg@self: those really should be replaced with constructors / destructors.
		bool Init(PaStreamCallback* serviceAudioCallback, bs::SteamAudio_AudioRenderer* engine, const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::ONE_SHOT);
		void Shutdown();

		void SetPosition(const float globalX, const float globalY, const float globalZ);

		// Oleg@self: maybe make a singleton out of this class instead of passing references of it in every method?...
		void ProcessAudio(std::vector<float>& outBuff, SteamAudio_AudioRenderer& engine);
		void Reset(SteamAudio_AudioRenderer& engine);

		inline PaStream* const GetStream() const { return pStream_; };
		inline ClipWrapMode GetWrapMode() const { return wrapMode_; };
		inline PaError const GetError() const { return err_; };
		inline uint32_t const GetCurrentBegin() const { return currentBegin_; };
		inline uint32_t const GetCurrentEnd() const { return currentEnd_; };
		inline const std::vector<float> GetSoundData() const { return soundData_; }; // Oleg@self: is this method useful?...
		
	private:
		// Phonon stuff
		// Oleg@self: these really should be part of the renderer
		IPLContext context_; // I think this is a ptr to a struct?... not sure, it's intentionally obfuscated. Treat as a hex variable.
		IPLHRTF hrtf_; // idem
		IPLBinauralEffect effect_; // idem
		IPLAudioBuffer iplOutBuffer_; // buffer used to output by phonon to output spatialized sound.
		IPLBinauralEffectParams spatializationParams_
		{
			IPLVector3{ 0.0f, 0.0f, 0.0f },
			IPL_HRTFINTERPOLATION_NEAREST,
			1.0f,
			hrtf_
		};

		// portaudio stuff
		PaStream* pStream_ = nullptr; // portaudio stream to playback device
		PaError err_;

		// SoundMaker specific stuff
		std::vector<float> soundData_; // Very large, contains whole wav file.
		uint32_t currentBegin_ = 0;
		uint32_t currentEnd_ = 0;
		ClipWrapMode wrapMode_ = ClipWrapMode::ONE_SHOT;
	};
}
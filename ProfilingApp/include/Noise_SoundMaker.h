#pragma once

#include <vector>

#include <portaudio.h>

#include "BSCommon.h"

namespace bsExp
{
	class Noise_AudioRenderer;

	class Noise_SoundMaker
	{
	public:
		Noise_SoundMaker() = default;
		Noise_SoundMaker(const Noise_SoundMaker&) = delete; // Owns a resource it's responsible for, so no copying.
		Noise_SoundMaker(Noise_SoundMaker&&) = default; // There's no reason for moving this class around.
		Noise_SoundMaker& operator=(const Noise_SoundMaker&) = delete; // Owns a resource it's responsible for, so no copy assignement.
		Noise_SoundMaker& operator=(Noise_SoundMaker&&) = default; // There's no need for move assignment for this class.

		// Oleg@self: those really should be replaced with constructors / destructors.
		bool Init(PaStreamCallback* serviceAudioCallback, bsExp::Noise_AudioRenderer* engine, const char* wavFileName);
		void Shutdown();

		void ProcessAudio(std::vector<float>& outBuff, Noise_AudioRenderer& engine);

		inline PaStream* const GetStream() const { return pStream_; };
		inline bs::ClipWrapMode GetWrapMode() const { return wrapMode_; };
		inline PaError const GetError() const { return err_; };
		inline uint32_t const GetCurrentBegin() const { return currentBegin_; };
		inline uint32_t const GetCurrentEnd() const { return currentEnd_; };
		inline const std::vector<float> GetSoundData() const { return soundData_; }; // Oleg@self: is this method useful?...

	private:
		PaStream* pStream_ = nullptr; // portaudio stream to playback device
		PaError err_;

		std::vector<float> soundData_; // Very large, contains whole wav file.
		uint32_t currentBegin_ = 0;
		uint32_t currentEnd_ = 0;
		bs::ClipWrapMode wrapMode_ = bs::ClipWrapMode::LOOP;
	};
}//! bsExp
#pragma once

#include "BSCommon.h"
#include "Noise_SoundMaker.h"

// Oleg@self: for the love of all that is holy, make an abstraction out of this kind of sheite

namespace bsExp
{
	class Noise_AudioRenderer
	{
	public:
		// Oleg@self: replace with constructor / destructor
		bool Init(size_t BUFFER_SIZE = 1024, size_t SAMPLE_RATE = 44100);
		void Update();
		void Shutdown();

		bs::SoundMakerId CreateSoundMaker(const char* wavFileName, const bs::ClipWrapMode wrapMode = bs::ClipWrapMode::ONE_SHOT);
		void MoveSoundMaker(bs::SoundMakerId id, const float globalX, const float globalY, const float globalZ);
		void ResetSoundMaker(bs::SoundMakerId id);
		void PlaySound(bs::SoundMakerId id);
		void StopSound(bs::SoundMakerId id);

		inline const std::vector<Noise_SoundMaker>& GetSounds() const { return sounds_; };
		void SetIsActive(const bool isActive);

		static size_t GetBufferSize() { return BUFFER_SIZE_; };
		static size_t GetSampleRate() { return SAMPLE_RATE_; };

	private:
		static int ServiceAudio_
		(
			const void* unused, void* outputBuffer,
			unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void* userData
		);

		// Renderer specific stuff
		std::vector<Noise_SoundMaker> sounds_;

		static size_t BUFFER_SIZE_;
		static size_t SAMPLE_RATE_;

		bool isActive_ = true;
	};
}//! bsExp
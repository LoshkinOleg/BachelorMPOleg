#pragma once

#include "SteamAudio_SoundMaker.h"

// Oleg@self: make an abstraction out of this.

namespace bs
{
	class SteamAudio_AudioRenderer
	{
	public:
		// Oleg@self: replace with constructor / destructor
		bool Init(size_t BUFFER_SIZE = 1024, size_t SAMPLE_RATE = 44100);
		void Shutdown();

		SoundMakerId CreateSoundMaker(const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::ONE_SHOT, const bool spatialize = true);
		void MoveSoundMaker(SoundMakerId id, const float globalX, const float globalY, const float globalZ);
		void ResetSoundMaker(SoundMakerId id);

		inline const std::vector<SteamAudio_SoundMaker>& GetSounds() const { return sounds_; };

		void SetIsActive(const bool isActive);
		void SetSelectedSound(const size_t soundId);
		size_t GetSelectedSound() const;

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
		std::vector<SteamAudio_SoundMaker> sounds_;
		size_t selectedSound_ = 0;

		static size_t BUFFER_SIZE_;
		static size_t SAMPLE_RATE_;

		bool isActive_ = true;
	};
}
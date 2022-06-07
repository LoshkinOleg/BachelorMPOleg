#pragma once

#include "SteamAudio_SoundMaker.h"

namespace bs
{
	using SoundMakerId = size_t; // Oleg@self: move to common file
	constexpr const size_t INVALID_ID = (size_t)-1;

	class SteamAudio_AudioRenderer
	{
	public:
		// Oleg@self: replace with constructor / destructor
		bool Init(size_t BUFFER_SIZE = 1024, size_t SAMPLE_RATE = 44100);
		void Shutdown();

		SoundMakerId CreateSoundMaker(const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::ONE_SHOT);
		void MoveSoundMaker(SoundMakerId id, const float globalX, const float globalY, const float globalZ);
		void ResetSoundMaker(SoundMakerId id);

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

		static size_t BUFFER_SIZE_;
		static size_t SAMPLE_RATE_;
	};
}
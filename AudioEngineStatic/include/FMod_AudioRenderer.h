#pragma once

#include <fmod.hpp>

#include "IAudioRenderer.h"
#include "FMod_SoundMaker.h"

namespace bs {
	class FMod_AudioRenderer : public IAudioRenderer
	{
	public:
		// IAudioRenderer implementation.
		bool Init(const char* hrtfFileName, const char* brirFileName, size_t BUFFER_SIZE = 512, size_t SAMPLE_RATE = 44100) override;
		void Run() override;
		void Shutdown() override;

		SoundMakerId CreateSoundMaker(const char* wavFileName) override;
		void MoveSoundMaker(SoundMakerId id, float globalX, float globalY, float globalZ) override;
		//! IAudioRenderer implementation.

	private:
		static int ServiceAudio_
		(
			const void* unused, void* outputBuffer,
			unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void* userData
		);

		std::vector<FMod_SoundMaker> sounds_;

		// Fmod stuff.
		FMOD::System* fmodSystem_ = nullptr;
	};
}
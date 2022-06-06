#pragma once

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

#include "IAudioRenderer.h"
#include "ThreeDTI_SoundMaker.h"

namespace Binaural
{
	class CCore;
}

namespace bs
{
	using Listener = std::shared_ptr<Binaural::CListener>;
	using Environment = std::shared_ptr<Binaural::CEnvironment>;

	class ThreeDTI_AudioRenderer: public IAudioRenderer
	{
	public:
		// IAudioRenderer implementation.
		bool Init(const char* hrtfFileName, const char* brirFileName, size_t BUFFER_SIZE = 512, size_t SAMPLE_RATE = 44100) override;
		void Run() override;
		void Shutdown() override;

		SoundMakerId CreateSoundMaker(const char* wavFileName) override;
		void MoveSoundMaker(SoundMakerId id, float globalX, float globalY, float globalZ) override;
		//! IAudioRenderer implementation.

		void ResetSoundMaker(SoundMakerId id);
		void ResetEnvironment();

		Binaural::CCore& GetCore();

	private:
		static int ServiceAudio_
		(
			const void* unused, void* outputBuffer,
			unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void* userData
		);

		Binaural::CCore core_;
		Listener listener_;
		Environment environment_;

		std::vector<ThreeDTI_SoundMaker> sounds_;

		static constexpr size_t HRTF_RESAMPLING_ANGLE = 15;
	};
}
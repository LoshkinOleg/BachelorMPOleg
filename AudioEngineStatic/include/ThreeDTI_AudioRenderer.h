#pragma once

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

#include "ThreeDTI_SoundMaker.h"

// Oleg@self: make an abstraction out of this.

namespace bs
{
	using Listener = std::shared_ptr<Binaural::CListener>;
	using Environment = std::shared_ptr<Binaural::CEnvironment>;

	// Oleg@self: make an abstraction out of this.

	class ThreeDTI_AudioRenderer
	{
	public:
		bool Init(const char* hrtfFileName, const char* brirFileName, size_t BUFFER_SIZE = 1024, size_t SAMPLE_RATE = 44100);
		void Shutdown();

		SoundMakerId CreateSoundMaker(const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::ONE_SHOT);
		void MoveSoundMaker(SoundMakerId id, const float globalX, const float globalY, const float globalZ);
		void ResetSoundMaker(SoundMakerId id);

		inline const std::vector<ThreeDTI_SoundMaker>& GetSounds() const { return sounds_; };

		void SetIsActive(const bool isActive);

		void ResetEnvironment();
		Environment GetEnvironment();
		Binaural::CCore& GetCore();

		static size_t GetBufferSize() { return BUFFER_SIZE_; };
		static size_t GetSampleRate() { return SAMPLE_RATE_; };

	private:
		static int ServiceAudio_
		(
			const void* unused, void* outputBuffer,
			unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void* userData
		);

		// 3dti stuff
		Binaural::CCore core_;
		Listener listener_;
		Environment environment_;
		static constexpr size_t HRTF_RESAMPLING_ANGLE = 15;

		// Renderer specific stuff
		std::vector<ThreeDTI_SoundMaker> sounds_;

		static size_t BUFFER_SIZE_;
		static size_t SAMPLE_RATE_;

		bool isActive_ = true;
	};
}
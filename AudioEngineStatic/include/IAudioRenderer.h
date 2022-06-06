#pragma once

#include <vector>

#include "ISoundMaker.h"

namespace bs
{
	using SoundMakerId = size_t;
	constexpr const size_t INVALID_ID = (size_t)-1;

	class IAudioRenderer {
	public:
		virtual bool Init(const char* hrtfFileName, const char* brirFileName, const size_t BUFFER_SIZE = 512, const size_t SAMPLE_RATE = 44100) = 0;
		virtual void Run() = 0;
		virtual void Shutdown() = 0;

		virtual SoundMakerId CreateSoundMaker(const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::CLAMP) = 0;
		virtual void MoveSoundMaker(SoundMakerId id, float globalX, float globalY, float globalZ) = 0;

		static size_t GetBufferSize() { return BUFFER_SIZE_; };
		static size_t GetSampleRate() { return SAMPLE_RATE_; };
	protected:

		static size_t BUFFER_SIZE_;
		static size_t SAMPLE_RATE_;
	};
}//! bs
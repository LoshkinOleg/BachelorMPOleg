#pragma once

#include <vector>

#include "portaudio.h"

namespace bs
{
	class IAudioRenderer;

	enum class ClipWrapMode {
		CLAMP,
		LOOP
	};

	class ISoundMaker {
	public:
		virtual bool Init(PaStreamCallback* serviceAudioCallback, bs::IAudioRenderer* engine, const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::CLAMP) = 0;
		virtual void Update() = 0;
		virtual void Run() = 0;
		virtual void Shutdown() = 0;

		virtual void SetPosition(float globalX, float globalY, float globalZ) = 0;
		virtual void AddPosition(float globalX, float globalY, float globalZ) = 0;
		virtual void SetRotation(float radX, float radY, float radZ) = 0;
		virtual void AddRotation(float radX, float radY, float radZ) = 0;

		inline PaStream* const GetStream() const { return pStream_; };
		inline ClipWrapMode GetWrapMode() const { return wrapMode_; };
		inline PaError const GetError() const { return err_; };
		inline uint32_t const GetCurrentBegin() const { return currentBegin_; };
		inline uint32_t const GetCurrentEnd() const { return currentEnd_; };
		inline const std::vector<float> GetSoundData() const { return soundData_; };

	protected:
		std::vector<float> soundData_; // Very large, contains whole wav file.
		uint32_t currentBegin_ = 0;
		uint32_t currentEnd_ = 0; // Points to subset of soundData that's currently in ears_.
		PaStream* pStream_ = nullptr; // portaudio stream ptr. Oleg@self: what's the difference between pStream and soundData?
		ClipWrapMode wrapMode_ = ClipWrapMode::LOOP;
		PaError err_;
	};
}//! bs
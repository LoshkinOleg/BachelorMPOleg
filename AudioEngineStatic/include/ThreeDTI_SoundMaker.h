#pragma once

#include <memory>
#include <vector>

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>
#include <portaudio.h>

namespace bs
{
	using Environment = std::shared_ptr<Binaural::CEnvironment>;
	class ThreeDTI_AudioRenderer;

	enum class ClipWrapMode {
		ONE_SHOT,
		LOOP
	};

	class ThreeDTI_SoundMaker
	{
	public:
		bool Init(PaStreamCallback* serviceAudioCallback, bs::ThreeDTI_AudioRenderer* engine, const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::ONE_SHOT);
		void Shutdown();

		void SetPosition(const float globalX, const float globalY, const float globalZ);

		void ProcessAudio(CStereoBuffer<float>& outBuff, ThreeDTI_AudioRenderer& engine);
		void Reset(ThreeDTI_AudioRenderer& engine);

		inline PaStream* const GetStream() const { return pStream_; };
		inline ClipWrapMode GetWrapMode() const { return wrapMode_; };
		inline PaError const GetError() const { return err_; };
		inline uint32_t const GetCurrentBegin() const { return currentBegin_; };
		inline uint32_t const GetCurrentEnd() const { return currentEnd_; };
		inline const std::vector<float> GetSoundData() const { return soundData_; };
		
	private:
		// 3dti stuff
		std::shared_ptr<Binaural::CSingleSourceDSP> source_; // 3dti class that manages spatialization.

		// portaudio stuff
		PaStream* pStream_ = nullptr; // portaudio stream ptr. Oleg@self: what's the difference between pStream and soundData?
		PaError err_;

		// SoundMaker specific stuff
		std::vector<float> soundData_; // Very large, contains whole wav file.
		uint32_t currentBegin_ = 0;
		uint32_t currentEnd_ = 0; // Points to subset of soundData that's currently in ears_.
		ClipWrapMode wrapMode_ = ClipWrapMode::ONE_SHOT;
	};
}
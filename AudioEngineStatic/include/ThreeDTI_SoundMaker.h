#pragma once

#include <memory>
#include <vector>

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

#include "ISoundMaker.h"

namespace bs
{
	using Environment = std::shared_ptr<Binaural::CEnvironment>;
	class ThreeDTI_AudioRenderer;

	class ThreeDTI_SoundMaker: public ISoundMaker
	{
	public:
		// ISoundMaker implementation.
		bool Init(PaStreamCallback* serviceAudioCallback, bs::IAudioRenderer* engine, const char* wavFileName) override;
		void Update() override;
		void Run() override;
		void Shutdown() override;

		void SetPosition(float globalX, float globalY, float globalZ) override;
		void AddPosition(float globalX, float globalY, float globalZ) override;
		void SetRotation(float radX, float radY, float radZ) override;
		void AddRotation(float radX, float radY, float radZ) override;
		//! ISoundMaker implementation.

		void ProcessAudio(CStereoBuffer<float>& outBuff, const bs::Environment& environment);
		void Reset(ThreeDTI_AudioRenderer& renderer);
		
	private:
		std::shared_ptr<Binaural::CSingleSourceDSP> source_; // 3dti class that manages spatialization.
	};
}
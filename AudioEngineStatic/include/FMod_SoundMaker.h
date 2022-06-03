#pragma once

#include <vector>

#include "ISoundMaker.h"

namespace bs
{
	class FMod_SoundMaker : public ISoundMaker
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

		void ProcessAudio();

	private:

	};
}
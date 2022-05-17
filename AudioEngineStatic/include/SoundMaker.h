#pragma once

#include <memory>
#include <vector>

#include <portaudio.h>
#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

namespace bs
{
	class AudioEngine;

	enum class ClipWrapMode {
		CLAMP,
		LOOP
	};

	class SoundMaker
	{
	public:

		bool Init(PaStreamCallback* serviceAudioCallback, bs::AudioEngine* engine, const char* wavFileName);
		void Update();
		void Shutdown();

		void SetPosition(float globalX, float globalY, float globalZ);
		void AddPosition(float globalX, float globalY, float globalZ);
		void SetRotation(float radX, float radY, float radZ);
		void AddRotation(float radX, float radY, float radZ);
	private:
		friend class AudioEngine; // Oleg@self: get rid of this, this smells.

		PaStream* pStream_ = nullptr; // portaudio stream ptr. Oleg@self: what's the difference between pStream and soundData?
		uint32_t currentBegin_, currentEnd_; // Points to subset of soundData that's currently in ears_.
		std::shared_ptr<Binaural::CSingleSourceDSP> source_; // 3dti class that manages spatialization.
		std::vector<float> soundData_; // Very large, contains whole wav file. Oleg@self: if a resource manager is ever implemented, move this to reserved memory.
		ClipWrapMode wrapMode_ = ClipWrapMode::LOOP;
		PaError err_;
	};
}
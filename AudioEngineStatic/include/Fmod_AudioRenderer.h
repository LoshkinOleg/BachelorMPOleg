#pragma once

#include <vector>

#include <fmod.hpp>

#include "BSCommon.h"

namespace bs
{
	class Fmod_AudioRenderer
	{
	public:

		BS_NON_COPYABLE(Fmod_AudioRenderer);
		BS_NON_MOVEABLE(Fmod_AudioRenderer);

		Fmod_AudioRenderer() = delete;
		Fmod_AudioRenderer(const float headAltitude, const size_t bufferSize, const size_t sampleRate,
						   const float DecayTime, const float EarlyDelay, const float LateDelay, const float HFReference,
						   const float HFDecayRatio, const float Diffusion, const float Density, const float LowShelfFrequency,
						   const float LowShelfGain, const float HighCut, const float EarlyLateMix, const float WetLevel);
		~Fmod_AudioRenderer();

		void GetRendererParams(float& outHeadAltitude) const;
		void UpdateRendererParams(const float headAltitude);

		size_t CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize);
		FMOD::Sound& GetSound(const size_t soundId);  // Oleg@self: return an abstraction of a sound instead.

		void PlaySound(const size_t soundId);
		void PauseSound(const size_t soundId);
		void StopSound(const size_t soundId);
		bool IsPaused(const size_t soundId);
		void GetSoundParams(const size_t soundId);
		void UpdateSoundParams(const size_t soundId);
		void MoveSound(const size_t soundId, const bs::CartesianCoord coord);

		void MoveListener(const bs::Mat3x4& mat);

		void Update();
		void StopAllSounds();

		const size_t bufferSize;
		const size_t sampleRate;

	private:
		FMOD::System* context_ = nullptr;
		FMOD::Reverb3D* reverb_ = nullptr;

		std::vector<std::pair<FMOD::Sound*, FMOD::Channel*>> sounds_;
	};
}
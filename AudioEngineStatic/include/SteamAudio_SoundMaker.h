#pragma once

#include <phonon.h>

#include "BSCommon.h"

namespace bs
{
	class SteamAudio_SoundMaker
	{
	public:
		BS_NON_COPYABLE(SteamAudio_SoundMaker);
		BS_MOVEABLE(SteamAudio_SoundMaker);

		SteamAudio_SoundMaker() = delete;

		void SetPosition(const bs::CartesianCoord coord);
		void SetPosition(const bs::SphericalCoord coord);

		void Play();
		void Pause();
		void Stop();

		bool IsPaused() const;
		bool IsPlaying() const;

		const bool looping;
		const bool spatialized;
		const size_t bufferSize;

	private:
		friend class SteamAudio_AudioRenderer;
		SteamAudio_SoundMaker(const std::vector<float>& data, const bool loop, const bool spatialize, const size_t bufferSize, const IPLContext& context, const IPLHRTF& hrtf, const IPLBinauralEffect& effect); // Called only by SteamAudio_AudioRenderer.
		void ProcessAudio_(std::vector<float>& interlacedStereoOut, const bs::CartesianCoord listenerPos); // Called by SteamAudio_AudioRenderer.

		const std::vector<float>& soundData_;
		std::vector<float> soundDataSubset_;
		size_t currentBegin_ = 0;
		size_t currentEnd_ = 0;
		bool paused_ = false;

		IPLAudioBuffer stereoSpatializedData_{0,0,nullptr};
		// IPLAudioBuffer spatializationTail_{0,0,nullptr};
		IPLBinauralEffectParams spatializationParams_;
		CartesianCoord pos_{};
		
		const IPLContext& context_;
		const IPLHRTF& hrtf_;
		const IPLBinauralEffect& effect_;
	};
}
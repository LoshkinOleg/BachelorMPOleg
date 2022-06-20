#pragma once

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

#include "BSCommon.h"

namespace bs
{
	struct ThreeDTI_SoundParams
	{
		bool anechoicEnabled;
		bool distanceBasedAttenuationAnechoic;

		bool reverbEnabled;
		bool distanceBasedAttenuationReverb;

		bool highQualitySimulation;
		bool atmosphericFiltering;
		bool nearFieldEffects;
	};

	class ThreeDTI_SoundMaker
	{
	public:

		BS_NON_COPYABLE(ThreeDTI_SoundMaker);
		BS_MOVEABLE(ThreeDTI_SoundMaker);

		ThreeDTI_SoundMaker() = delete;

		void SetPosition(const bs::CartesianCoord coord);
		void SetPosition(const bs::SphericalCoord coord);

		void Play();
		void Pause();
		void Stop();

		bool IsPaused() const;
		bool IsPlaying() const;

		ThreeDTI_SoundParams GetSoundParams() const;
		void UpdateSpatializationParams(const ThreeDTI_SoundParams p);
		
		const bool looping;
		const bool spatialized;
		const size_t bufferSize;

	private:
		friend class ThreeDTI_AudioRenderer;
		ThreeDTI_SoundMaker(const std::vector<float>& data, Binaural::CCore& core, const bool loop, const bool spatialize, const size_t bufferSize, const ThreeDTI_SoundParams p);
		void ProcessAudio_(std::vector<float>& interlacedStereoOut); // Called by ThreeDTI_AudioRenderer.

		std::shared_ptr<Binaural::CSingleSourceDSP> source_;
		Common::CEarPair<CMonoBuffer<float>> anechoic_;

		const std::vector<float>& soundData_;
		CMonoBuffer<float> soundDataSubset_;
		size_t currentBegin_ = 0;
		size_t currentEnd_ = 0;
		bool paused_ = false;
	};
}
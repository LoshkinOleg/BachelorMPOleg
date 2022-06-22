#pragma once

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

#include "BSCommon.h"

namespace bs
{
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

		void GetSoundParams(bool& anechoicEnabled, bool& distanceBasedAttenuationAnechoic,
							bool& reverbEnabled, bool& distanceBasedAttenuationReverb,
							bool& highQualitySimulation, bool& atmosphericFiltering, bool& nearFieldEffects) const;
		void UpdateSpatializationParams(const bool anechoicEnabled, const bool distanceBasedAttenuationAnechoic,
										const bool reverbEnabled, const bool distanceBasedAttenuationReverb,
										const bool highQualitySimulation, const bool atmosphericFiltering, const bool nearFieldEffects);
		
		const bool looping;
		const bool spatialized;
		const size_t bufferSize;

	private:
		friend class ThreeDTI_AudioRenderer;
		ThreeDTI_SoundMaker(const std::vector<float>& data, Binaural::CCore& core, const bool loop, const bool spatialize, const size_t bufferSize,
							const bool anechoicEnabled, const bool distanceBasedAttenuationAnechoic,
							const bool reverbEnabled, const bool distanceBasedAttenuationReverb,
							const bool highQualitySimulation, const bool atmosphericFiltering, const bool nearFieldEffects);
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
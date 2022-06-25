#pragma once

#include <map>
#include <string>

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

#include "BSCommon.h"
#include "ThreeDTI_SoundMaker.h"

namespace bs
{
	class ThreeDTI_AudioRenderer
	{
	public:

		BS_NON_COPYABLE(ThreeDTI_AudioRenderer);
		BS_NON_MOVEABLE(ThreeDTI_AudioRenderer);

		ThreeDTI_AudioRenderer() = delete;
		ThreeDTI_AudioRenderer(const char* hrtfPath, const char* brirPath, const size_t bufferSize, const size_t sampleRate, const float headAltitude, const bool ILDEnabled);

		void GetRendererParams(float& outHeadAltitude, bool& outILDEnabled) const;
		void UpdateRendererParams(const float headAltitude, const bool ILDEnabled);

		size_t CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize,
								const bool anechoicEnabled, const bool distanceBasedAttenuationAnechoic,
								const bool reverbEnabled, const bool distanceBasedAttenuationReverb,
								const bool highQualitySimulation, const bool atmosphericFiltering, const bool nearFieldEffects);
		ThreeDTI_SoundMaker& GetSound(const size_t soundId);

		void StopAllSounds();

		void ProcessAudio(std::vector<float>& interleavedStereoOut);

		void MoveListener(const bs::CartesianCoord pos, const bs::Quaternion quat);

		const size_t bufferSize;
		const size_t sampleRate;
		constexpr static size_t const HRTF_RESAMPLING_STEP = 15;

	private:
		Binaural::CCore core_;
		std::shared_ptr<Binaural::CListener> listener_;
		std::shared_ptr<Binaural::CEnvironment> environment_;

		std::vector<float> interlacedReverb_;
		std::vector<float> currentlyProcessedSignal_;
		Common::CEarPair<CMonoBuffer<float>> reverb_;

		std::vector<ThreeDTI_SoundMaker> sounds_;
		std::map<size_t, std::vector<float>> assets_;

		std::hash<std::string> hasher_{};
	};
}
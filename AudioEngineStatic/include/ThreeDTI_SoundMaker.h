#pragma once

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

#include "BSCommon.h"

namespace bs
{
	class ThreeDTI_SoundMaker
	{
	public:
		BS_NON_COPYABLE(ThreeDTI_SoundMaker);

		ThreeDTI_SoundMaker() = delete;
		ThreeDTI_SoundMaker(const std::vector<float>& data, Binaural::CCore& core, const bool loop, const bool spatialize, const size_t bufferSize);

		void SetPosition(const bs::CartesianCoord coord);
		void SetPosition(const bs::SphericalCoord coord);

		void Play();
		void Pause();
		void Stop();

		bool GetPaused() const;
		
		const bool looping;
		const bool spatialized;
		const size_t bufferSize;

	private:
		friend class ThreeDTI_AudioRenderer;
		void ProcessAudio_(CStereoBuffer<float>& outBuff);

		std::shared_ptr<Binaural::CSingleSourceDSP> source_;
		CMonoBuffer<float> window_;
		Common::CEarPair<CMonoBuffer<float>> anechoic_;

		const std::vector<float>& soundData_;
		uint32_t currentBegin_ = 0;
		uint32_t currentEnd_ = 0;
		bool paused_ = false;
	};
}
#pragma once

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
		ThreeDTI_AudioRenderer(const char* hrtfFileName, const char* brirFileName, const size_t bufferSize, const size_t sampleRate);
		~ThreeDTI_AudioRenderer();

		ThreeDTI_SoundMaker& CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize);

		void ProcessAudio();

		const size_t bufferSize;
		const size_t sampleRate;

	private:
		Binaural::CCore core_;
		std::shared_ptr<Binaural::CListener> listener_;
		std::shared_ptr<Binaural::CEnvironment> environment_;

		std::vector<ThreeDTI_SoundMaker> sounds_;
	};
}
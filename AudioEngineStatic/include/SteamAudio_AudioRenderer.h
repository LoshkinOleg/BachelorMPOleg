#pragma once

#include <map>

#include "SteamAudio_SoundMaker.h"

namespace bs
{
	class SteamAudio_AudioRenderer
	{
	public:
		BS_NON_COPYABLE(SteamAudio_AudioRenderer);
		BS_NON_MOVEABLE(SteamAudio_AudioRenderer);

		SteamAudio_AudioRenderer() = delete;
		SteamAudio_AudioRenderer(const size_t bufferSize, const size_t sampleRate, const float headAltitude);
		~SteamAudio_AudioRenderer();

		size_t CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize);
		SteamAudio_SoundMaker& GetSound(const size_t soundId);

		void ProcessAudio(std::vector<float>& interleavedStereoOut);

		const size_t bufferSize;
		const size_t sampleRate;
		constexpr static size_t const HRTF_RESAMPLING_STEP = 15;

	private:
		std::vector<SteamAudio_SoundMaker> sounds_;
		std::map<size_t, std::vector<float>> assets_;

		IPLContext context_; // I think this is a ptr to a struct?... not sure, it's intentionally obfuscated. Treat as a hex variable.
		IPLHRTF hrtf_;
		IPLBinauralEffect effect_;
		std::vector<float> currentlyProcessedSignal_;

		std::hash<std::string> hasher_{};
	};
}
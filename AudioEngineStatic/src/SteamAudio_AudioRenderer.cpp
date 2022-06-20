#include <SteamAudio_AudioRenderer.h>

#include <cassert>

#include "SteamAudio_SoundMaker.h"

bs::SteamAudio_AudioRenderer::SteamAudio_AudioRenderer(const size_t bufferSize, const size_t sampleRate, const float headAltitude):
	bufferSize(bufferSize), sampleRate(sampleRate), listenerPos_{0.0f, 0.0f, headAltitude}
{
	// Phonon stuff
	IPLAudioSettings audioSettings{ sampleRate, bufferSize };
	IPLContextSettings contextSettings{};
	IPLBinauralEffectSettings effectSettings;
	IPLHRTFSettings hrtfSettings;
	IPLAmbisonicsEncodeEffectSettings ambiEncodingSettings{1};
	IPLAmbisonicsDecodeEffectSettings ambiDecodingSettings{0 , hrtf_, 1};

	contextSettings.version = STEAMAUDIO_VERSION;
	hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;

	auto result = iplContextCreate(&contextSettings, &context_);
	assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to create a phonon context!");

	result = iplHRTFCreate(context_, &audioSettings, &hrtfSettings, &hrtf_);
	assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to create a phonon hrtf!");
	effectSettings.hrtf = hrtf_;

	result = iplBinauralEffectCreate(context_, &audioSettings, &effectSettings, &binauralEffect_);
	assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to create a phonon binaural effect!");

	result = iplAmbisonicsEncodeEffectCreate(context_, &audioSettings, &ambiEncodingSettings, &ambiEncodeEffect_);
	assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to create a phonon ambisonic encoding effect!");

	result = iplAmbisonicsDecodeEffectCreate(context_, &audioSettings, &ambiDecodingSettings, &ambiDecodeEffect_);
	assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to create a phonon ambisonic decoding effect!");

	// TODO: init simulation and listener and move listener to headAltitude. https://valvesoftware.github.io/steam-audio/doc/capi/guide.html#sources

	currentlyProcessedSignal_.resize(2 * bufferSize, 0.0f);
}

bs::SteamAudio_AudioRenderer::~SteamAudio_AudioRenderer()
{
	iplBinauralEffectRelease(&binauralEffect_);
	iplHRTFRelease(&hrtf_);
	iplContextRelease(&context_);
}

size_t bs::SteamAudio_AudioRenderer::CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize)
{
	const auto hash = hasher_(wavFileName);

	if (assets_.find(hash) == assets_.end()) // Sound not loaded.
	{
		assets_.emplace(hash, bs::LoadWav(wavFileName, 1, sampleRate));
	}
	sounds_.emplace_back(SteamAudio_SoundMaker(assets_[hash], loop, spatialize, bufferSize, context_, hrtf_, binauralEffect_));

	return sounds_.size() - 1;
}

bs::SteamAudio_SoundMaker& bs::SteamAudio_AudioRenderer::GetSound(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to GetSound()!");
	return sounds_[soundId];
}

void bs::SteamAudio_AudioRenderer::MoveListener(const bs::CartesianCoord coord)
{
	listenerPos_ = coord;
}

void bs::SteamAudio_AudioRenderer::MoveListener(const bs::SphericalCoord coord)
{
	listenerPos_ = bs::ToCartesian(coord);
}

void bs::SteamAudio_AudioRenderer::ProcessAudio(std::vector<float>& interleavedStereoOut)
{
	// Process anechoic.
	for (auto& sound : sounds_)
	{
		std::fill(currentlyProcessedSignal_.begin(), currentlyProcessedSignal_.end(), 0.0f);
		sound.ProcessAudio_(currentlyProcessedSignal_, listenerPos_);
		bs::SumSignals(interleavedStereoOut, currentlyProcessedSignal_);
	}

	// Process reverb.
	IPLAudioBuffer inReverb; // mono buffer
	auto result = iplAudioBufferAllocate(context_, 1, bufferSize, &inReverb);
	assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to allocate a phonon buffer!");
	IPLAudioBuffer outReverb; // B-format buffer
	auto result = iplAudioBufferAllocate(context_, 4, bufferSize, &outReverb);
	assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to allocate a phonon buffer!");

	
}

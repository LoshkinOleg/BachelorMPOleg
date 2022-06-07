#include <SteamAudio_AudioRenderer.h>

#include <cassert>

#ifndef BUILD_WITH_EASY_PROFILER
#define BUILD_WITH_EASY_PROFILER
#endif
#include <easy/profiler.h>

#include "SteamAudio_SoundMaker.h"

size_t bs::SteamAudio_AudioRenderer::BUFFER_SIZE_ = 0;
size_t bs::SteamAudio_AudioRenderer::SAMPLE_RATE_ = 0;

bool bs::SteamAudio_AudioRenderer::Init(size_t BUFFER_SIZE, size_t SAMPLE_RATE)
{
	BUFFER_SIZE_ = BUFFER_SIZE;
	SAMPLE_RATE_ = SAMPLE_RATE;

	// Init portaudio.
	auto err = Pa_Initialize();
	if (err != paNoError) throw;

#ifdef USE_EASY_PROFILER
	// Enable writing a profiling file by easy_profiler.
	EASY_PROFILER_ENABLE;
#endif //! USE_EASY_PROFILER

	return true; // Oleg@self: use int instead. Most libs use int as return val and this would allow to have a single local result var to check for errors of all libs.
}
void bs::SteamAudio_AudioRenderer::Shutdown()
{
#ifdef USE_EASY_PROFILER
	// Oleg@self: parse directors and look how to name the next file without overwriting the existing one.
	// Write profiling data to file.
	const auto result = profiler::dumpBlocksToFile("profilingData/profilingData0.prof");
	assert(result, "Couldn't write .prof file!");
#endif //! USE_EASY_PROFILER

	for (auto& sound : sounds_)
	{
		sound.Shutdown();
	}
}

bs::SoundMakerId bs::SteamAudio_AudioRenderer::CreateSoundMaker(const char* wavFileName, const ClipWrapMode wrapMode)
{
	sounds_.emplace_back(SteamAudio_SoundMaker());
	if (!sounds_.back().Init(&ServiceAudio_, this, wavFileName, wrapMode))
	{
		assert(false, "Problem initializing the new SoundMaker!");
		sounds_.pop_back();
		return INVALID_ID;
	}
	return sounds_.size() - 1;
}

void bs::SteamAudio_AudioRenderer::MoveSoundMaker(bs::SoundMakerId id, float globalX, float globalY, float globalZ)
{
	assert(id != (size_t)-1, "Invalid sound maker id recieved in MoveSoundMaker()!");
	sounds_[id].SetPosition(globalX, globalY, globalZ);
}

void bs::SteamAudio_AudioRenderer::ResetSoundMaker(SoundMakerId id)
{
	/*
		WARNING: calling this function seems to do some wierd shit... Some data used for reverb processing gets invalidated?...
	*/
	sounds_[id].Reset(*this);
}

int bs::SteamAudio_AudioRenderer::ServiceAudio_
(
	const void* unused, void* outputBuffer,
	unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, void* userData
)
{
	auto* engine = (SteamAudio_AudioRenderer*)userData; // Annoying hack to have a non static servicing method.
	if (engine->sounds_.size() <= 0) return paContinue;
	auto* sound = dynamic_cast<SteamAudio_SoundMaker*>(&engine->sounds_[0]);
	static std::vector<float> processedFrame(BUFFER_SIZE_ * 2);
	auto* outBuff = static_cast<float*>(outputBuffer); // Cast output buffer to float buffer.

	sound->ProcessAudio(processedFrame, *engine); // Oleg@self: make a virtual method out of this.

	// Oleg@self: use memcpy?
	for (auto it = processedFrame.begin(); it != processedFrame.end(); it++)
	{
		*(outBuff++) = *it;
	}

	return paContinue;
}
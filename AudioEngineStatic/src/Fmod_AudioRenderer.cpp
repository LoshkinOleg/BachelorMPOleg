#include <Fmod_AudioRenderer.h>

#include <cassert>

#ifndef BUILD_WITH_EASY_PROFILER
#define BUILD_WITH_EASY_PROFILER
#endif
#include <easy/profiler.h>
#include <fmod.hpp>

#include "Fmod_SoundMaker.h"

size_t bs::Fmod_AudioRenderer::BUFFER_SIZE_ = 0;
size_t bs::Fmod_AudioRenderer::SAMPLE_RATE_ = 0;

bool bs::Fmod_AudioRenderer::Init(size_t BUFFER_SIZE, size_t SAMPLE_RATE)
{
	BUFFER_SIZE_ = BUFFER_SIZE;
	SAMPLE_RATE_ = SAMPLE_RATE;

#ifdef USE_EASY_PROFILER
	// Enable writing a profiling file by easy_profiler.
	EASY_PROFILER_ENABLE;
#endif //! USE_EASY_PROFILER

	fmodErr_ = FMOD::System_Create(&fmodSystem_);
	assert(fmodErr_ == FMOD_RESULT::FMOD_OK, "Fmod failed to create a system!");
	
	constexpr const int MAX_CHANNELS = 32;
	fmodErr_ = fmodSystem_->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
	assert(fmodErr_ == FMOD_RESULT::FMOD_OK, "Fmod failed to initialize its system!");

	return true; // Oleg@self: use int instead. Most libs use int as return val and this would allow to have a single local result var to check for errors of all libs.
}
void bs::Fmod_AudioRenderer::Update()
{
	fmodErr_ = fmodSystem_->update();
	assert(fmodErr_ == FMOD_OK, "Error updating fmod!");
}
void bs::Fmod_AudioRenderer::Shutdown()
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
	fmodSystem_->release();
}

bs::SoundMakerId bs::Fmod_AudioRenderer::CreateSoundMaker(const char* wavFileName, const ClipWrapMode wrapMode)
{
	sounds_.emplace_back(Fmod_SoundMaker());
	if (!sounds_.back().Init(this, wavFileName, fmodSystem_, wrapMode))
	{
		assert(false, "Problem initializing the new SoundMaker!");
		sounds_.pop_back();
		return INVALID_ID;
	}
	return sounds_.size() - 1;
}

void bs::Fmod_AudioRenderer::MoveSoundMaker(bs::SoundMakerId id, float globalX, float globalY, float globalZ)
{
	assert(id != (size_t)-1, "Invalid sound maker id recieved in MoveSoundMaker()!");
	sounds_[id].SetPosition(globalX, globalY, globalZ);
}

void bs::Fmod_AudioRenderer::ResetSoundMaker(SoundMakerId id)
{
	/*
		WARNING: calling this function seems to do some wierd shit... Some data used for reverb processing gets invalidated?...
	*/
	sounds_[id].Reset(*this);
}

void bs::Fmod_AudioRenderer::PlaySound(SoundMakerId id)
{
	assert(id != INVALID_ID, "Trying to play a sound with an invalid id!");
	sounds_[id].Play(fmodSystem_);
}

void bs::Fmod_AudioRenderer::StopSound(SoundMakerId id)
{
	assert(id != INVALID_ID, "Trying to stop a sound with an invalid id!");
	sounds_[id].Stop();
}

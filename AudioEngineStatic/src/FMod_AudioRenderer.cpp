#include "FMod_AudioRenderer.h"

#include <cassert>

#include <easy/profiler.h>

bool bs::FMod_AudioRenderer::Init(const char* hrtfFileName, const char* brirFileName, size_t BUFFER_SIZE, size_t SAMPLE_RATE)
{
    BUFFER_SIZE_ = BUFFER_SIZE;
    SAMPLE_RATE_ = SAMPLE_RATE;

    auto result = FMOD::System_Create(&fmodSystem_);
    assert(result == FMOD_OK, "Failed to create an fmod system!");

    int driverCount = 0;
    fmodSystem_->getNumDrivers(&driverCount);
    assert(driverCount, "Fmod couldn't find any suitable drivers!");

    result = fmodSystem_->init(
        32, // Nr of channels
        FMOD_INIT_NORMAL, // Initialization flags
        NULL // Additional driver data
    );
    assert(result, "Couldn't initialize fmod!");

    // Init portaudio.
    auto paErr_ = Pa_Initialize();
    assert(paErr_ == paNoError, "PortAudio failed to initialize!");

    // Enable writing a profiling file by easy_profiler.
#ifdef USE_EASY_PROFILER
    EASY_PROFILER_ENABLE;
#endif //! USE_EASY_PROFILER

    return true;
}

void bs::FMod_AudioRenderer::Run()
{
}

void bs::FMod_AudioRenderer::Shutdown()
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

bs::SoundMakerId bs::FMod_AudioRenderer::CreateSoundMaker(const char* wavFileName)
{
    sounds_.push_back(FMod_SoundMaker());
    // fmodSystem_->createSound();
    if (sounds_.back().Init(&bs::FMod_AudioRenderer::ServiceAudio_, this, wavFileName)) {
        assert(false, "Problem initializing the new SoundMaker!");
        sounds_.pop_back();
        return sounds_.size() - 1;
    }
    return INVALID_ID;
}

void bs::FMod_AudioRenderer::MoveSoundMaker(SoundMakerId id, float globalX, float globalY, float globalZ)
{
}

int bs::FMod_AudioRenderer::ServiceAudio_(const void* unused, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
    return 0;
}

#include "SteamAudio_AudioRenderer.h"

#include <cassert>
#include <iterator>

#ifndef BUILD_WITH_EASY_PROFILER
#define BUILD_WITH_EASY_PROFILER
#endif
#include <easy/profiler.h>

bool bs::SteamAudio_AudioRenderer::Init(const char* hrtfFileName, const char* brirFileName, size_t BUFFER_SIZE, size_t SAMPLE_RATE)
{
    BUFFER_SIZE_ = BUFFER_SIZE;
    SAMPLE_RATE_ = SAMPLE_RATE;

    IPLContextSettings contextSettings{};
    IPLAudioSettings audioSettings{SAMPLE_RATE_, BUFFER_SIZE_};
    IPLHRTFSettings hrtfSettings{ IPL_HRTFTYPE_DEFAULT , nullptr};
    IPLBinauralEffectSettings binauralSettings{hrtf_};

    contextSettings.version = STEAMAUDIO_VERSION;

    iplErr_ = iplContextCreate(&contextSettings, &context_);
    assert(iplErr_ == IPLerror::IPL_STATUS_SUCCESS, "SteamAudio failed to initialize a context!");

    iplErr_ = iplHRTFCreate(context_, &audioSettings, &hrtfSettings, &hrtf_);
    assert(iplErr_ == IPLerror::IPL_STATUS_SUCCESS, "SteamAudio failed to initialize the hrtf!");
    
    iplErr_ = iplBinauralEffectCreate(context_, &audioSettings, &binauralSettings, &binauralEffect_);
    assert(iplErr_ == IPLerror::IPL_STATUS_SUCCESS, "SteamAudio failed to initialize the binaural effect!");

    iplErr_ = iplAudioBufferAllocate(context_, 2, BUFFER_SIZE_, &iplFrame_);
    assert(iplErr_ == IPLerror::IPL_STATUS_SUCCESS, "SteamAudio failed to allocate a buffer!");

    // Init portaudio.
    auto paErr_ = Pa_Initialize();
    assert(paErr_ == paNoError, "PortAudio failed to initialize!");

    // Enable writing a profiling file by easy_profiler.
#ifdef USE_EASY_PROFILER
    EASY_PROFILER_ENABLE;
#endif //! USE_EASY_PROFILER

    return true;
}

void bs::SteamAudio_AudioRenderer::Run()
{
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

    iplAudioBufferFree(context_, &iplFrame_);
    iplBinauralEffectRelease(&binauralEffect_);
    iplHRTFRelease(&hrtf_);
    iplContextRelease(&context_);
}

bs::SoundMakerId bs::SteamAudio_AudioRenderer::CreateSoundMaker(const char* wavFileName)
{
    sounds_.push_back(SteamAudio_SoundMaker());
    if (sounds_.back().Init(&bs::SteamAudio_AudioRenderer::ServiceAudio_, this, wavFileName)) {
        assert(false, "Problem initializing the new SteamAudio_SoundMaker!");
        sounds_.pop_back();
        return sounds_.size() - 1;
    }
    return INVALID_ID;
}

void bs::SteamAudio_AudioRenderer::MoveSoundMaker(SoundMakerId id, float globalX, float globalY, float globalZ)
{
    assert(id != (size_t)-1, "Invalid sound maker id recieved in MoveSoundMaker()!");
    sounds_[id].SetPosition(globalX, globalY, globalZ);
}

int bs::SteamAudio_AudioRenderer::ServiceAudio_(const void* unused, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
    auto* engine = (SteamAudio_AudioRenderer*)userData; // Annoying hack to have a non static servicing method.
    if (engine->sounds_.size() <= 0) return paContinue;
    auto* sound = dynamic_cast<SteamAudio_SoundMaker*>(&engine->sounds_[0]);
    auto* outBuff = static_cast<float*>(outputBuffer);
    static std::vector<float> processedFrame;

    // Oleg@self: implement non looping clips.
    assert(sound->GetWrapMode() != ClipWrapMode::LOOP, "Non looping clips not yet implemented!");

    IPLBinauralEffectParams params;
    params.direction = IPLVector3{1.0f, 1.0f, 1.0f};
    params.interpolation = IPL_HRTFINTERPOLATION_NEAREST;
    params.spatialBlend = 1.0f; // Oleg@self: investigate
    params.hrtf = engine->hrtf_;
    sound->ProcessAudio(processedFrame, engine->iplFrame_, params, engine->binauralEffect_);

    for (auto it = processedFrame.begin(); it != processedFrame.end(); it++)
    {
        *(outBuff++) = *it;
    }

    return paContinue;
}

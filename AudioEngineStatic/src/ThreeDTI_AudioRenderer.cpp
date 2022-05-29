#include <ThreeDTI_AudioRenderer.h>

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>
#include <HRTF/HRTFFactory.h>
#include <BRIR/BRIRFactory.h>

#ifndef BUILD_WITH_EASY_PROFILER
#define BUILD_WITH_EASY_PROFILER
#endif
#include <easy/profiler.h>

#include "ThreeDTI_SoundMaker.h"

size_t bs::ThreeDTI_AudioRenderer::BUFFER_SIZE_ = 0;
size_t bs::ThreeDTI_AudioRenderer::SAMPLE_RATE_ = 0;

bool bs::ThreeDTI_AudioRenderer::Init(const char* hrtfFileName, const char* brirFileName, size_t BUFFER_SIZE, size_t SAMPLE_RATE)
{
	BUFFER_SIZE_ = BUFFER_SIZE;
	SAMPLE_RATE_ = SAMPLE_RATE;

	// Init 3dti core.
	core_.SetAudioState({ (int)SAMPLE_RATE, (int)BUFFER_SIZE });
	core_.SetHRTFResamplingStep(HRTF_RESAMPLING_ANGLE);

	// Init 3dti listener.
	// Oleg@self: handle splitscreen multiplayer.
	listener_ = core_.CreateListener();
	listener_->DisableCustomizedITD(); // Oleg@self: investigate
	bool unused; // Oleg@self: investigate
	if (!HRTF::CreateFromSofa(hrtfFileName, listener_, unused)) return false;

	// Init 3dti environment.
	environment_ = core_.CreateEnvironment();
	environment_->SetReverberationOrder(TReverberationOrder::BIDIMENSIONAL); // Oleg@self:investigate
	if (!BRIR::CreateFromSofa(brirFileName, environment_)) return false;

	// Init portaudio.
	auto err = Pa_Initialize();
	if (err != paNoError) throw;

#ifdef USE_EASY_PROFILER
	// Enable writing a profiling file by easy_profiler.
	EASY_PROFILER_ENABLE;
#endif //! USE_EASY_PROFILER

	return true;
}
void bs::ThreeDTI_AudioRenderer::Run()
{
	for (auto& sound : sounds_)
	{
		sound.Run();
	}

	Pa_Sleep(5000); // Oleg@self: design an updatable engine?
}
void bs::ThreeDTI_AudioRenderer::Shutdown()
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

bs::SoundMakerId bs::ThreeDTI_AudioRenderer::CreateSoundMaker(const char* wavFileName)
{
	sounds_.push_back(ThreeDTI_SoundMaker());
	if (!sounds_.back().Init(&ServiceAudio_, this, wavFileName))
	{
		assert(false, "Problem initializing the new ThreeDTI_SoundMaker!");
		sounds_.pop_back();
		return INVALID_ID;
	}
	return sounds_.size() - 1;
}

void bs::ThreeDTI_AudioRenderer::MoveSoundMaker(bs::SoundMakerId id, float globalX, float globalY, float globalZ)
{
	assert(id != (size_t)-1, "Invalid sound maker id recieved in MoveSoundMaker()!");
	sounds_[id].SetPosition(globalX, globalY, globalZ);
}

Binaural::CCore& bs::ThreeDTI_AudioRenderer::GetCore()
{
	return core_;
}

int bs::ThreeDTI_AudioRenderer::ServiceAudio_
(
	const void* unused, void* outputBuffer,
	unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, void* userData
)
{
	auto* engine = (ThreeDTI_AudioRenderer*)userData; // Annoying hack to have a non static servicing method.
	if (engine->sounds_.size() <= 0) return paContinue;
	auto* sound = dynamic_cast<ThreeDTI_SoundMaker*>(&engine->sounds_[0]);
	static CStereoBuffer<float> processedFrame;
	auto* outBuff = static_cast<float*>(outputBuffer); // Cast output buffer to float buffer.
	
	// Oleg@self: implement non looping clips.
	assert(sound->GetWrapMode() != ClipWrapMode::LOOP, "Non looping clips not yet implemented!");

	sound->ProcessAudio(processedFrame, engine->environment_); // Oleg@self: make a virtual method out of this.

	// Oleg@self: use memcpy?
	for (auto it = processedFrame.begin(); it != processedFrame.end(); it++)
	{
		*(outBuff++) = *it;
	}

	return paContinue;
}
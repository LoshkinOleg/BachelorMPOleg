#include <SteamAudio_AudioRenderer.h>

#include <cassert>
#include <iostream>

#ifndef BUILD_WITH_EASY_PROFILER
#define BUILD_WITH_EASY_PROFILER
#endif
#include <easy/profiler.h>

#include "SteamAudio_SoundMaker.h"

//size_t bs::SteamAudio_AudioRenderer::BUFFER_SIZE_ = 0;
//size_t bs::SteamAudio_AudioRenderer::SAMPLE_RATE_ = 0;
//
//bool bs::SteamAudio_AudioRenderer::Init(size_t BUFFER_SIZE, size_t SAMPLE_RATE)
//{
//	BUFFER_SIZE_ = BUFFER_SIZE;
//	SAMPLE_RATE_ = SAMPLE_RATE;
//
//	// Init portaudio.
//	auto err = Pa_Initialize();
//	if (err != paNoError) throw;
//
//	// Portaudio stuff
//	PaStreamParameters outputParams{
//		Pa_GetDefaultOutputDevice(), // Oleg@self: handle this properly.
//		2,
//		paFloat32, // Oleg@self: check this properly!
//		0.050, // Oleg@self: magic number. Investigate.
//		NULL
//	};
//
//	err_ = Pa_OpenStream(
//		&pStream_,
//		NULL,
//		&outputParams,
//		(double)SteamAudio_AudioRenderer::GetSampleRate(),
//		(unsigned long)SteamAudio_AudioRenderer::GetBufferSize(),
//		paClipOff, // Oleg@self: investigate
//		ServiceAudio_,
//		this
//	);
//
//	if (err_ != paNoError)
//	{
//		std::cerr << "Error opening stream: " << Pa_GetErrorText(err_) << std::endl;
//		return false;
//	}
//
//	err_ = Pa_StartStream(pStream_); // Oleg@self: I think this should be part of Renderer instead.
//	assert(!err_, "Sound maker reported error starting a stream.");
//
//#ifdef USE_EASY_PROFILER
//	// Enable writing a profiling file by easy_profiler.
//	EASY_PROFILER_ENABLE;
//#endif //! USE_EASY_PROFILER
//
//	return true; // Oleg@self: use int instead. Most libs use int as return val and this would allow to have a single local result var to check for errors of all libs.
//}
//void bs::SteamAudio_AudioRenderer::Shutdown()
//{
//#ifdef USE_EASY_PROFILER
//	// Oleg@self: parse directors and look how to name the next file without overwriting the existing one.
//	// Write profiling data to file.
//	const auto result = profiler::dumpBlocksToFile("profilingData/profilingData0.prof");
//	assert(result, "Couldn't write .prof file!");
//#endif //! USE_EASY_PROFILER
//
//	for (auto& sound : sounds_)
//	{
//		sound.Shutdown();
//	}
//
//	// Portaudio stuff
//	err_ = Pa_StopStream(pStream_); // Oleg@self: it's important to do this first! Otherwise ServiceAudio_() might be called when the iplBuffer has been freed!
//	assert(!err_, "Sound maker reported error stopping a stream.");
//	err_ = Pa_CloseStream(pStream_);
//	assert(!err_, "Sound maker reported error closing a stream.");
//}
//
//bs::SoundMakerId bs::SteamAudio_AudioRenderer::CreateSoundMaker(const char* wavFileName, const ClipWrapMode wrapMode, const bool spatialize)
//{
//	sounds_.emplace_back(SteamAudio_SoundMaker());
//	if (!sounds_.back().Init(this, wavFileName, wrapMode, spatialize))
//	{
//		assert(false, "Problem initializing the new SoundMaker!");
//		sounds_.pop_back();
//		return INVALID_ID;
//	}
//	return sounds_.size() - 1;
//}
//
//void bs::SteamAudio_AudioRenderer::MoveSoundMaker(bs::SoundMakerId id, float globalX, float globalY, float globalZ)
//{
//	assert(id != (size_t)-1, "Invalid sound maker id recieved in MoveSoundMaker()!");
//	sounds_[id].SetPosition(globalX, globalY, globalZ);
//}
//
//void bs::SteamAudio_AudioRenderer::ResetSoundMaker(SoundMakerId id)
//{
//	/*
//		WARNING: calling this function seems to do some wierd shit... Some data used for reverb processing gets invalidated?...
//	*/
//	sounds_[id].Reset(*this);
//}
//
//void bs::SteamAudio_AudioRenderer::SetIsActive(const bool isActive)
//{
//	isActive_ = isActive;
//	if (isActive_ && sounds_.size())
//	{
//		sounds_[selectedSound_].Reset(*this);
//	}
//}
//
//void bs::SteamAudio_AudioRenderer::SetSelectedSound(const size_t soundId)
//{
//	assert(soundId < sounds_.size(), "Invalid soundId passed to SetSelectedSound()!");
//	sounds_[selectedSound_].SetPaused(true);
//	selectedSound_ = soundId;
//	sounds_[selectedSound_].SetPaused(false);
//}
//
//size_t bs::SteamAudio_AudioRenderer::GetSelectedSound() const
//{
//	return selectedSound_;
//}
//
//#include <iostream>
//
//int bs::SteamAudio_AudioRenderer::ServiceAudio_
//(
//	const void* unused, void* outputBuffer,
//	unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
//	PaStreamCallbackFlags statusFlags, void* userData
//)
//{
//
//	auto* engine = (SteamAudio_AudioRenderer*)userData; // Annoying hack to have a non static servicing method.
//	if (engine->sounds_.size() <= 0) return paContinue;
//	auto* sound = dynamic_cast<SteamAudio_SoundMaker*>(&engine->sounds_[engine->selectedSound_]);
//	auto* outBuff = static_cast<float*>(outputBuffer); // Cast output buffer to float buffer.
//	// static std::vector<float> processedFrame(BUFFER_SIZE_ * 2);
//	std::vector<float> processedFrame(BUFFER_SIZE_ * 2);
//
//	if (engine->isActive_)
//	{
//		sound->ProcessAudio(processedFrame, *engine);
//		std::cout << "steamaudio renderer servicing audio.\n";
//	}
//	else
//	{
//		std::fill(processedFrame.begin(), processedFrame.end(), 0.0f);
//	}
//
//	// Oleg@self: use memcpy?
//	for (auto it = processedFrame.begin(); it != processedFrame.end(); it++)
//	{
//		*(outBuff++) = *it;
//	}
//
//	return paContinue;
//}
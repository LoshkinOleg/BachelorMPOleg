#include "Noise_AudioRenderer.h"

#include <cassert>

#include <portaudio.h>

size_t bsExp::Noise_AudioRenderer::BUFFER_SIZE_ = 0;
size_t bsExp::Noise_AudioRenderer::SAMPLE_RATE_ = 0;

bool bsExp::Noise_AudioRenderer::Init(size_t BUFFER_SIZE, size_t SAMPLE_RATE)
{
	BUFFER_SIZE_ = BUFFER_SIZE;
	SAMPLE_RATE_ = SAMPLE_RATE;

	// Init portaudio.
	auto err = Pa_Initialize();
	if (err != paNoError) throw;

	return true;
}

void bsExp::Noise_AudioRenderer::Shutdown()
{
	for (auto& sound : sounds_)
	{
		sound.Shutdown();
	}
}

bs::SoundMakerId bsExp::Noise_AudioRenderer::CreateSoundMaker(const char* wavFileName, const bs::ClipWrapMode wrapMode)
{
	sounds_.emplace_back(bsExp::Noise_SoundMaker());
	if (!sounds_.back().Init(&ServiceAudio_, this, wavFileName))
	{
		assert(false, "Problem initializing the new Noise_SoundMaker!");
		sounds_.pop_back();
		return bs::INVALID_ID;
	}
	return sounds_.size() - 1;
}

void bsExp::Noise_AudioRenderer::SetIsActive(const bool isActive)
{
	isActive_ = isActive;
}

#include <iostream>

int bsExp::Noise_AudioRenderer::ServiceAudio_(const void* unused, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	auto* engine = (Noise_AudioRenderer*)userData; // Annoying hack to have a non static servicing method.
	if (engine->sounds_.size() <= 0) return paContinue;
	static std::vector<float> processedFrame(engine->BUFFER_SIZE_ * 2);
	auto* sound = dynamic_cast<Noise_SoundMaker*>(&engine->sounds_[0]);
	auto* outBuff = static_cast<float*>(outputBuffer); // Cast output buffer to float buffer.

	if (engine->isActive_)
	{
		sound->ProcessAudio(processedFrame, *engine);
		std::cout << "Noise renderer servicing audio.\n";
	}
	else
	{
		std::fill(processedFrame.begin(), processedFrame.end(), 0.0f);
	}

	// Oleg@self: use memcpy?
	for (auto it = processedFrame.begin(); it != processedFrame.end(); it++)
	{
		*(outBuff++) = *it;
	}

	return paContinue;
}

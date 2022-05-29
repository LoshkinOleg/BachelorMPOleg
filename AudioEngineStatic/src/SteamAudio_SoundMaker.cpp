#include "SteamAudio_SoundMaker.h"

#include <cstdint>
#include <cstdio>
#include <iostream>

#include "IAudioRenderer.h"
#include "UtilityFunctions.h"

bool bs::SteamAudio_SoundMaker::Init(PaStreamCallback* serviceAudioCallback, bs::IAudioRenderer* engine, const char* wavFileName)
{
	currentBegin_ = 0;
	currentEnd_ = 0;

	soundData_ = LoadWav(wavFileName);

	PaStreamParameters outputParams{
		Pa_GetDefaultOutputDevice(), // Oleg@self: handle this properly.
		2,
		paFloat32, // Oleg@self: check this properly!
		0.050, // Oleg@self: magic number. Investigate.
		NULL
	};

	err_ = Pa_OpenStream(
		&pStream_,
		NULL,
		&outputParams,
		(double)IAudioRenderer::GetSampleRate(),
		(unsigned long)IAudioRenderer::GetBufferSize(),
		paClipOff, // Oleg@self: investigate
		serviceAudioCallback,
		engine
	);

	if (err_ != paNoError) {
		std::cerr << "Error opening stream: " << Pa_GetErrorText(err_) << std::endl;
		return false;
	}

	return true;
}

void bs::SteamAudio_SoundMaker::Update()
{
}

void bs::SteamAudio_SoundMaker::Run()
{
}

void bs::SteamAudio_SoundMaker::Shutdown()
{
}

void bs::SteamAudio_SoundMaker::SetPosition(float globalX, float globalY, float globalZ)
{
}

void bs::SteamAudio_SoundMaker::AddPosition(float globalX, float globalY, float globalZ)
{
}

void bs::SteamAudio_SoundMaker::SetRotation(float radX, float radY, float radZ)
{
}

void bs::SteamAudio_SoundMaker::AddRotation(float radX, float radY, float radZ)
{
}

void bs::SteamAudio_SoundMaker::ProcessAudio(std::vector<float>& output, IPLAudioBuffer& iplFrame, IPLBinauralEffectParams params, IPLBinauralEffect effect)
{
	// Oleg@self: implement this once a consistent audio processing policy has been designed.

	const auto bufferSize = IAudioRenderer::GetBufferSize();
	// static IPLAudioBuffer inBuff{1, bufferSize, };

	for (size_t i = 0; i < bufferSize; i++)
	{
		iplBinauralEffectApply(effect, &params, );
	}
}

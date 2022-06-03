#include "FMod_SoundMaker.h"

#include <iostream>

#include "UtilityFunctions.h"
#include "IAudioRenderer.h"

bool bs::FMod_SoundMaker::Init(PaStreamCallback* serviceAudioCallback, bs::IAudioRenderer* engine, const char* wavFileName)
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

void bs::FMod_SoundMaker::Update()
{
}

void bs::FMod_SoundMaker::Run()
{
}

void bs::FMod_SoundMaker::Shutdown()
{
}

void bs::FMod_SoundMaker::SetPosition(float globalX, float globalY, float globalZ)
{
}

void bs::FMod_SoundMaker::AddPosition(float globalX, float globalY, float globalZ)
{
}

void bs::FMod_SoundMaker::SetRotation(float radX, float radY, float radZ)
{
}

void bs::FMod_SoundMaker::AddRotation(float radX, float radY, float radZ)
{
}

void bs::FMod_SoundMaker::ProcessAudio()
{
}

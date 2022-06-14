#include "Noise_SoundMaker.h"

#include <cassert>
#include <algorithm>

#include "Noise_AudioRenderer.h"

bool bsExp::Noise_SoundMaker::Init(PaStreamCallback* serviceAudioCallback, bsExp::Noise_AudioRenderer* engine, const char* wavFileName)
{
	currentBegin_ = 0;
	currentEnd_ = 0;

	soundData_ = bs::LoadWav(wavFileName, 1, engine->GetSampleRate());
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
		(double)Noise_AudioRenderer::GetSampleRate(),
		(unsigned long)Noise_AudioRenderer::GetBufferSize(),
		paClipOff, // Oleg@self: investigate
		serviceAudioCallback,
		engine
	);
	assert(err_ == paNoError, "Error opening stream!");
	
	err_ = Pa_StartStream(pStream_); // Oleg@self: I think this should be part of Renderer instead.
	assert(!err_, "Sound maker reported error starting a stream.");

	return true;
}

void bsExp::Noise_SoundMaker::Shutdown()
{
	err_ = Pa_StopStream(pStream_);
	assert(!err_, "Sound maker reported error stopping a stream.");
	err_ = Pa_CloseStream(pStream_);
	assert(!err_, "Sound maker reported error closing a stream.");
}

void bsExp::Noise_SoundMaker::ProcessAudio(std::vector<float>& outBuff, Noise_AudioRenderer& engine)
{
	const auto bufferSize = engine.GetBufferSize();
	const auto wavSize = soundData_.size();
	static std::vector<float> frame(bufferSize);

	// Fill all buffers with silence.
	std::fill(outBuff.begin(), outBuff.end(), 0.0f);
	std::fill(frame.begin(), frame.end(), 0.0f);

	// Oleg@self: fix this fuckery, I'm getting confused by the indices / frame sizes, sample rates, etc...

	// Advance frame indices.
	// Oleg@self: will create popping I think since we're not looping properly...
	assert(wrapMode_ == bs::ClipWrapMode::LOOP);
	currentBegin_ = currentEnd_ + 1;
	if (currentBegin_ + engine.GetBufferSize() - 1 > wavSize) currentBegin_ = 0;
	currentEnd_ = currentBegin_ + engine.GetBufferSize() - 1;

	// Load subset of audio data into frame.
	for (size_t i = 0; i < bufferSize; i++)
	{
		// Oleg@self: use memcpy?
		if ((currentBegin_ + i) < wavSize) // If we're not overruning the clip data, copy data.
		{
			frame[i] = soundData_[currentBegin_ + i];
		}
	}

	// Manually interleave
	for (size_t i = 0; i < bufferSize; i++)
	{
		outBuff[i * 2] = frame[i];
		outBuff[i * 2 + 1] = frame[i];
	}
}

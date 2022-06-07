#include <Fmod_SoundMaker.h>

#include <iostream>
#include <cassert>

#include <Fmod_AudioRenderer.h>
#include <UtilityFunctions.h>

bool bs::Fmod_SoundMaker::Init(PaStreamCallback* serviceAudioCallback, bs::Fmod_AudioRenderer* engine, const char* wavFileName, const ClipWrapMode wrapMode)
{
	currentBegin_ = 0;
	currentEnd_ = 0;
	wrapMode_ = wrapMode;

	soundData_ = LoadWav(wavFileName, 1, engine->GetSampleRate());

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
		(double)Fmod_AudioRenderer::GetSampleRate(),
		(unsigned long)Fmod_AudioRenderer::GetBufferSize(),
		paClipOff, // Oleg@self: investigate
		serviceAudioCallback,
		engine
	);

	if (err_ != paNoError) {
		std::cerr << "Error opening stream: " << Pa_GetErrorText(err_) << std::endl;
		return false;
	}

	err_ = Pa_StartStream(pStream_); // Oleg@self: I think this should be part of Renderer instead.
	assert(!err_, "Sound maker reported error starting a stream.");

	return true;
}
void bs::Fmod_SoundMaker::Shutdown()
{
	err_ = Pa_StopStream(pStream_);
	assert(!err_, "Sound maker reported error stopping a stream.");
	err_ = Pa_CloseStream(pStream_);
	assert(!err_, "Sound maker reported error closing a stream.");
}
void bs::Fmod_SoundMaker::SetPosition(float globalX, float globalY, float globalZ)
{
	assert(false, "SetPosition() isn't implemented!");
}

void bs::Fmod_SoundMaker::ProcessAudio(std::vector<float>& outBuff, Fmod_AudioRenderer& engine)
{
	const auto bufferSize = engine.GetBufferSize();
	const auto wavSize = soundData_.size();
	static std::vector<float> frame(bufferSize);

	// Fill all buffers with silence.
	std::fill(frame.begin(), frame.end(), 0.0f);
	std::fill(outBuff.begin(), outBuff.end(), 0.0f);

	// Oleg@self: fix this fuckery, I'm getting confused by the indices / frame sizes, sample rates, etc...

	// Advance frame indices.
	currentBegin_ = currentEnd_ + 1;
	if (wrapMode_ == ClipWrapMode::ONE_SHOT)
	{
		if (currentBegin_ < wavSize) // Not overruning wav data.
		{
			currentEnd_ = currentBegin_ + engine.GetBufferSize() - 1;
		}
		else
		{
			currentEnd_ = wavSize - 1;
		}
	}
	else
	{
		if (currentBegin_ + engine.GetBufferSize() - 1 > wavSize) currentBegin_ = 0;
		currentEnd_ = currentBegin_ + engine.GetBufferSize() - 1;
	}

	// Load subset of audio data into frame.
	for (size_t i = 0; i < bufferSize; i++)
	{
		// Oleg@self: use memcpy?
		if ((currentBegin_ + i) < wavSize) // If we're not overruning the clip data, copy data.
		{
			frame[i] = soundData_[currentBegin_ + i];
		}
	}

	// Manually interleave.
	for (size_t i = 0; i < bufferSize; i++)
	{
		outBuff[2 * i] = frame[i];
		outBuff[2 * i + 1] = frame[i];
	}
}

void bs::Fmod_SoundMaker::Reset(Fmod_AudioRenderer& renderer)
{
	currentBegin_ = 0;
	currentEnd_ = 0;
}
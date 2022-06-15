#include <SteamAudio_SoundMaker.h>

#include <iostream>
#include <cassert>
#include <algorithm>

#include <SteamAudio_AudioRenderer.h>
#include <UtilityFunctions.h>

bool bs::SteamAudio_SoundMaker::Init(PaStreamCallback* serviceAudioCallback, bs::SteamAudio_AudioRenderer* engine, const char* wavFileName, const ClipWrapMode wrapMode, const bool spatialize)
{
	currentBegin_ = 0;
	currentEnd_ = 0;
	wrapMode_ = wrapMode;
	spatialized_ = spatialize;

	soundData_ = LoadWav(wavFileName, 1, engine->GetSampleRate());

	// Portaudio stuff
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
		(double)SteamAudio_AudioRenderer::GetSampleRate(),
		(unsigned long)SteamAudio_AudioRenderer::GetBufferSize(),
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

	if (spatialized_)
	{
		// Phonon stuff
		IPLAudioSettings audioSettings{ engine->GetSampleRate(), engine->GetBufferSize() };
		IPLContextSettings contextSettings{};
		IPLBinauralEffectSettings effectSettings;
		IPLHRTFSettings hrtfSettings;

		contextSettings.version = STEAMAUDIO_VERSION;
		hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;

		auto result = iplContextCreate(&contextSettings, &context_);
		assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to create a phonon context!");

		result = iplHRTFCreate(context_, &audioSettings, &hrtfSettings, &hrtf_);
		assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to create a phonon hrtf!");
		effectSettings.hrtf = hrtf_; // Oleg@self: not sure this assignement has to be made after iplHRTFCreate() or not. I think it does since hrtf_ is a ptr and not a reference?
		spatializationParams_.hrtf = hrtf_;

		result = iplBinauralEffectCreate(context_, &audioSettings, &effectSettings, &effect_);
		assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to create a phonon binaural effect!");

		constexpr const IPLint32 NR_OF_CHANNELS = 2;
		result = iplAudioBufferAllocate(context_, NR_OF_CHANNELS, engine->GetBufferSize(), &iplOutBuffer_);
		assert(result == IPLerror::IPL_STATUS_SUCCESS, "Failed to allocate a phonon buffer!");
	}

	return true;
}
void bs::SteamAudio_SoundMaker::Shutdown()
{
	// Portaudio stuff
	err_ = Pa_StopStream(pStream_); // Oleg@self: it's important to do this first! Otherwise ServiceAudio_() might be called when the iplBuffer has been freed!
	assert(!err_, "Sound maker reported error stopping a stream.");
	err_ = Pa_CloseStream(pStream_);
	assert(!err_, "Sound maker reported error closing a stream.");

	// Phonon stuff
	if (spatialized_)
	{
		iplAudioBufferFree(context_, &iplOutBuffer_);
		iplBinauralEffectRelease(&effect_);
		iplHRTFRelease(&hrtf_);
		iplContextRelease(&context_);
	}
}
void bs::SteamAudio_SoundMaker::SetPosition(float globalX, float globalY, float globalZ)
{
	// Oleg@self: this is completely wrong. Fine for testing purposes but fix it for actual use.
	if (spatialized_)
	{
		spatializationParams_.direction = IPLVector3{globalX, globalY, globalZ};
	}
}

void bs::SteamAudio_SoundMaker::ProcessAudio(std::vector<float>& outBuff, SteamAudio_AudioRenderer& engine)
{
	const auto bufferSize = engine.GetBufferSize();
	const auto wavSize = soundData_.size();
	// static std::vector<float> frame(bufferSize);
	std::vector<float> frame(bufferSize);

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

	if (spatialized_)
	{
		// Oleg@self: clean this sheit
		float* rawFrame[] = { frame.data() };
		IPLAudioBuffer iplInBuffer{ 1, engine.GetBufferSize(), rawFrame };

		// Oleg@self: find a way to carry over the tail of the spatialized signal.
		auto remainingSamples = iplBinauralEffectApply(effect_, &spatializationParams_, &iplInBuffer, &iplOutBuffer_);
		// assert(remainingSamples == IPLAudioEffectState::IPL_AUDIOEFFECTSTATE_TAILCOMPLETE, "Couldn't write all spatialized sound into output buffer!");
		iplAudioBufferInterleave(context_, &iplOutBuffer_, outBuff.data());
	}
	else
	{
		for (size_t i = 0; i < bufferSize; i++)
		{
			outBuff[i * 2] = frame[i];
			outBuff[i * 2 + 1] = frame[i];
		}
	}
}

void bs::SteamAudio_SoundMaker::Reset(SteamAudio_AudioRenderer& renderer)
{
	currentBegin_ = 0;
	currentEnd_ = 0;
}
#include <ThreeDTI_SoundMaker.h>

#include <ThreeDTI_AudioRenderer.h>
#include <UtilityFunctions.h>

bool bs::ThreeDTI_SoundMaker::Init(PaStreamCallback* serviceAudioCallback, bs::IAudioRenderer* engine, const char* wavFileName)
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

	err_ = Pa_StartStream(pStream_); // Oleg@self: I think this should be part of Renderer instead.
	assert(!err_, "Sound maker reported error starting a stream.");

	source_ = static_cast<ThreeDTI_AudioRenderer*>(engine)->GetCore().CreateSingleSourceDSP();
	source_->SetSpatializationMode(Binaural::TSpatializationMode::HighQuality);
	source_->DisableNearFieldEffect();
	source_->EnableAnechoicProcess();
	source_->EnableDistanceAttenuationAnechoic();
	source_->EnableDistanceAttenuationReverb(); // Oleg@self: investigate

	return true;
}
void bs::ThreeDTI_SoundMaker::Update()
{

}
void bs::ThreeDTI_SoundMaker::Run()
{
	
}
void bs::ThreeDTI_SoundMaker::Shutdown()
{
	err_ = Pa_StopStream(pStream_);
	assert(!err_, "Sound maker reported error stopping a stream.");
	err_ = Pa_CloseStream(pStream_);
	assert(!err_, "Sound maker reported error closing a stream.");
}
void bs::ThreeDTI_SoundMaker::SetPosition(float globalX, float globalY, float globalZ)
{
	Common::CTransform t = source_->GetSourceTransform(); // Copy transform.
	t.SetPosition(Common::CVector3(globalX, globalY, globalZ)); // Modify it.
	source_->SetSourceTransform(t); // And set it.
}
void bs::ThreeDTI_SoundMaker::AddPosition(float globalX, float globalY, float globalZ)
{
	// Oleg@self todo
}
void bs::ThreeDTI_SoundMaker::SetRotation(float radX, float radY, float radZ)
{
	// Oleg@self todo
}
void bs::ThreeDTI_SoundMaker::AddRotation(float radX, float radY, float radZ)
{
	// Oleg@self todo
}

void bs::ThreeDTI_SoundMaker::ProcessAudio(CStereoBuffer<float>& outBuff, const bs::Environment& environment)
{
	const auto bufferSize = IAudioRenderer::GetBufferSize();
	const auto wavSize = soundData_.size();
	static CMonoBuffer<float> frame(bufferSize);
	static Common::CEarPair<CMonoBuffer<float>> anechoic{ CMonoBuffer<float>(bufferSize) , CMonoBuffer<float>(bufferSize) };
	static Common::CEarPair<CMonoBuffer<float>> reverb{ CMonoBuffer<float>(bufferSize) , CMonoBuffer<float>(bufferSize) };

	// Fill all buffers with silence.
	outBuff.Fill(bufferSize * 2, 0.0f); // Output is interleaved stereo, hence the *2
	frame.Fill(bufferSize, 0.0f);
	anechoic.left.Fill(bufferSize, 0.0f);
	anechoic.right.Fill(bufferSize, 0.0f);
	reverb.left.Fill(bufferSize, 0.0f);
	reverb.right.Fill(bufferSize, 0.0f);

	// Oleg@self: implement non looping clips.
	// Advance frame indices.
	currentBegin_ = currentEnd_ + 1;
	if (currentBegin_ < wavSize) // Not overruning wav data.
	{
		currentEnd_ = currentBegin_ + IAudioRenderer::GetBufferSize() - 1;

		// Load subset of audio data into frame.
		for (size_t i = 0; i < bufferSize; i++)
		{
			// Oleg@self: use memcpy?
			if ((currentBegin_ + i) < wavSize) // If we're not overruning the clip data, copy data.
			{
				frame[i] = soundData_[currentBegin_ + i];
			}
		}

		// Process anechoic.
		source_->SetBuffer(frame); // Set source buffer to read from. Makes the next lines use the window buffer as source of sound data.
		source_->ProcessAnechoic(anechoic.left, anechoic.right); // Write anechoic component of the sound to anechoic buffer.
		// Oleg@self: investigate, does this need to be called for every sound maker? Or does it need to be done only once per servicing?
		// Process reverb.
		environment->ProcessVirtualAmbisonicReverb(reverb.left, reverb.right); // Write reverb component of the sound to reverb buffer.
		// Combine anechoic and reverb then interlace.
		anechoic.left += reverb.left;
		anechoic.right += reverb.right;
		outBuff.Interlace(anechoic.left, anechoic.right);
	}
}

void bs::ThreeDTI_SoundMaker::Reset(ThreeDTI_AudioRenderer& renderer)
{
	currentBegin_ = 0;
	currentEnd_ = 0;
	source_->ResetSourceBuffers();
	renderer.ResetEnvironment();
}

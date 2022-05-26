#include <ThreeDTI_SoundMaker.h>

#include <ThreeDTI_AudioRenderer.h>

bool bs::ThreeDTI_SoundMaker::Init(PaStreamCallback* serviceAudioCallback, bs::IAudioRenderer* engine, const char* wavFileName)
{
	currentBegin_ = 0;
	currentEnd_ = 0;

	// Oleg@self: investigate reading of wav.
	struct WavHeader								 // Local declaration of wav header struct type (more info in http://soundfile.sapp.org/doc/WaveFormat/)
	{												 // We only need the number of samples, so the rest will be unused assuming file is mono, 16-bit depth and 44.1kHz sampling rate
		char		  fill[40];
		uint32_t	bytesCount;
	} wavHeader;

	FILE* wavFile = fopen(wavFileName, "rb");
	if (wavFile == NULL)
	{
		std::cerr << "Couldn't open file!\n";
		return false;
	}
	fread(&wavHeader, sizeof(wavHeader), 1, wavFile); // Reading of the 44 bytes of header to get the number of samples of the file
	fseek(wavFile, sizeof(wavHeader), SEEK_SET); // Moving of the file pointer to the start of the audio samples

	// Oleg@self: check invariants: wav file, 16-bit depth for samples
	unsigned int samplesCount = wavHeader.bytesCount / 2; // Getting number of samples by dividing number of bytes by 2 because we are reading 16-bit samples
	// Oleg@self: point of this?
	int16_t* sample; sample = new int16_t[samplesCount]; // Declaration and initialization of 16-bit signed integer pointer
	memset(sample, 0, sizeof(int16_t) * samplesCount); // Setting its size

	uint8_t* byteSample; byteSample = new uint8_t[2 * samplesCount]; // Declaration and initialization of 8-bit unsigned integer pointer
	memset(byteSample, 0, sizeof(uint8_t) * 2 * samplesCount); // Setting its size

	fread(byteSample, 1, 2 * samplesCount, wavFile); // Reading the whole file byte per byte, needed for endian-independent wav parsing

	for (int i = 0; i < samplesCount; i++)
		sample[i] = int16_t(byteSample[2 * i] | byteSample[2 * i + 1] << 8); // Conversion from two 8-bit unsigned integer to a 16-bit signed integer

	soundData_.reserve(samplesCount); // Reserving memory for samples vector

	for (int i = 0; i < samplesCount; i++)
		soundData_.push_back((float)sample[i] / (float)INT16_MAX); // Converting samples to float to push them in samples vector

	fclose(wavFile); // Oleg@self: handle exceptions?

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
	err_ = Pa_StartStream(pStream_);
	assert(!err_, "Sound maker reported error starting a stream.");
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
	frame.Fill(bufferSize, 0.0f);
	anechoic.left.Fill(bufferSize, 0.0f);
	anechoic.right.Fill(bufferSize, 0.0f);
	reverb.left.Fill(bufferSize, 0.0f);
	reverb.right.Fill(bufferSize, 0.0f);

	// Oleg@self: implement non looping clips.
	// Advance frame indices.
	currentBegin_ = currentEnd_ + 1;
	if (currentBegin_ >= wavSize) currentBegin_ = 0; // Wrap around if reached end of sound data.
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

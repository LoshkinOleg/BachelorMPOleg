#include <AudioEngine.h>
#include <SoundMaker.h>

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>
#include <HRTF/HRTFFactory.h>
#include <BRIR/BRIRFactory.h>

#ifndef BUILD_WITH_EASY_PROFILER
#define BUILD_WITH_EASY_PROFILER
#endif

#include <easy/profiler.h>

bool bs::AudioEngine::Init(const char* hrtfFileName, const char* brirFileName, size_t BUFFER_SIZE, size_t SAMPLE_RATE)
{
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

	// Enable writing a profiling file by easy_profiler.
	EASY_PROFILER_ENABLE;

	return true;
}
void bs::AudioEngine::Run()
{
	// Oleg@self: handle exceptions properly.
	for (auto sound : sounds_)
	{
		sound.err_ = Pa_StartStream(sound.pStream_); // Oleg@self: make a method out of this.
		if (sound.err_ != paNoError) throw; // Oleg@self: make a method out of this kind of thing.
	}

	Pa_Sleep(5000); // Oleg@self: design an updatable engine?
}
void bs::AudioEngine::Shutdown()
{
	// Write profiling data to file.
	if (!profiler::dumpBlocksToFile("profilingData.prof")) {
		std::cerr << "Couldn't write profilingData.prof!" << std::endl;
	}

	for (auto sound : sounds_)
	{
		sound.err_ = Pa_StopStream(sound.pStream_);
		if (sound.err_ != paNoError) throw; // Oleg@self: handle exceptions properly.
		sound.err_ = Pa_CloseStream(sound.pStream_);
		if (sound.err_ != paNoError) throw;
	}
}

bs::SoundMakerId bs::AudioEngine::CreateSoundMaker(const char* wavFileName)
{
	sounds_.push_back(SoundMaker());
	if (!sounds_.back().Init(&ServiceAudio_, this, wavFileName))
	{
		sounds_.pop_back();
		return INVALID_ID;
	}
	return sounds_.size() - 1;
}

void bs::AudioEngine::MoveSoundMaker(bs::SoundMakerId id, float globalX, float globalY, float globalZ)
{
	// Oleg@self: add invariant check
	sounds_[id].SetPosition(globalX, globalY, globalZ);
}

Binaural::CCore& bs::AudioEngine::GetCore()
{
	return core_;
}

int bs::AudioEngine::ServiceAudio_
(
	const void* unused, void* outputBuffer,
	unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, void* userData
)
{
	EASY_FUNCTION(profiler::colors::Magenta);

	EASY_BLOCK("ServiceAudio_ initialization.");
	auto* engine = (AudioEngine*)userData; // Annoying hack to have a non static servicing method.

	// For now only handling looping sounds for now. Oleg@self: implement non looping clips.
	if (engine->sounds_[0].wrapMode_ != ClipWrapMode::LOOP) return paAbort;

	// Oleg@self: handle more than one sound being played cases.
	// Move sliding window to next chunk of data.
	auto& windowBegin = engine->sounds_[0].currentBegin_;
	auto& windowEnd = engine->sounds_[0].currentEnd_;
	auto& soundData = engine->sounds_[0].soundData_;
	const auto wavSize = soundData.size();
	windowBegin = windowEnd + 1;
	// Oleg@self: implement non looping clips.
	if (windowBegin >= wavSize) windowBegin = 0; // Wrap around if reached end of sound data.
	windowEnd = windowBegin + BUFFER_SIZE - 1;
	EASY_END_BLOCK; //! ServiceAudio_ initialization.

	EASY_BLOCK("Reading into window buffer.");
	// Read audio data from wav buffer into the window vector.
	static CMonoBuffer<float> window(BUFFER_SIZE); // Stores subset of wav data that will be feed to ears.
	window.Fill(BUFFER_SIZE, 0.0f); // Fill window with silence.
	for (size_t i = 0; i < BUFFER_SIZE; i++)
	{
		if ((windowBegin + i) < wavSize) // If we're not overruning the clip data, copy data.
		{
			window[i] = soundData[windowBegin + i];
		}
	}
	EASY_END_BLOCK; //! Reading into window buffer.

	EASY_BLOCK("Initializing spatialization buffers.");
	// Oleg@self: this should be a method of SoundMaker
	static Common::CEarPair<CMonoBuffer<float>> anechoic; // Anechoic part of the sound.
	static Common::CEarPair<CMonoBuffer<float>> reverb; // Reverb part of the sound.
	static Common::CEarPair<CMonoBuffer<float>> ears; // anechoic + reverb
	anechoic.left.clear();
	anechoic.right.clear();
	reverb.left.clear();
	reverb.right.clear();
	ears.left.clear();
	ears.right.clear();
	// anechoic.left.resize(BUFFER_SIZE); // Oleg@self: see if this is even necessary...
	// anechoic.left.Fill(BUFFER_SIZE, (AudioDataFormat)0); // Oleg@self: see if this is even necessary...
	// anechoic.right.resize(BUFFER_SIZE);
	// anechoic.right.Fill(BUFFER_SIZE, (AudioDataFormat)0);
	// reverb.left.resize(BUFFER_SIZE);
	// reverb.left.Fill(BUFFER_SIZE, (AudioDataFormat)0);
	// reverb.right.resize(BUFFER_SIZE);
	// reverb.right.Fill(BUFFER_SIZE, (AudioDataFormat)0);
	ears.left.resize(BUFFER_SIZE);
	ears.left.Fill(BUFFER_SIZE, 0.0f); // Fill it with all 0 in case some of the data doesn't get written to so we get silence instead of noise.
	ears.right.resize(BUFFER_SIZE);
	ears.left.Fill(BUFFER_SIZE, 0.0f);
	engine->sounds_[0].source_->SetBuffer(window); // Set source buffer to read from. Makes the next lines use the window buffer as source of sound data.
	EASY_END_BLOCK; //! Initializing spatialization buffers.

	EASY_BLOCK("Processing anechoic.");
	engine->sounds_[0].source_->ProcessAnechoic(anechoic.left, anechoic.right); // Write anechoic component of the sound to anechoic buffer.
	EASY_END_BLOCK; //! Processing anechoic.

	EASY_BLOCK("Processing reverb.");
	engine->environment_->ProcessVirtualAmbisonicReverb(reverb.left, reverb.right); // Write reverb component of the sound to reverb buffer.
	EASY_END_BLOCK; //! Processing reverb.

	EASY_BLOCK("Interlacing left and right buffers.");
	ears.left += anechoic.left + reverb.left; // Mix the anechoic and reverb part of the spatialized sound.
	ears.right += anechoic.right + reverb.right; // Oleg@self: inspect the + operator overloads.
	// Write interlaced audio data to hardware buffer.
	// Oleg@self: again, avoid creating vectors on every call...
	static CStereoBuffer<float> output;
	output.Interlace(ears.left, ears.right);
	EASY_END_BLOCK; //! Interlacing left and right buffers.

	EASY_BLOCK("Writing to out buffer.");
	float* outBuff = (float*)outputBuffer; // Cast output buffer to float buffer.
	for (auto it = output.begin(); it != output.end(); it++)
	{
		*(outBuff++) = *it; // Oleg@self: surely there's a way to just pass outBuff to the Interlace method?...
	}
	EASY_END_BLOCK; //! Writing to out buffer.

	return paContinue;
}
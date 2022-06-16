#include "RendererManager.h"

bsExp::RendererManager::RendererManager(const char* hrtfFile, const char* brirFile)
{
	// Init portaudio.
	auto err = Pa_Initialize(); // Oleg@self: isn't it a problem that we're calling it for each renderer?
	if (err != paNoError) throw;

	PaStreamParameters outputParams{
		Pa_GetDefaultOutputDevice(), // Oleg@self: handle this properly.
		2,
		paFloat32,
		0.050, // Oleg@self: magic number. Investigate.
		NULL
	};

	err = Pa_OpenStream(
		&pStream_,
		NULL,
		&outputParams,
		(double)SAMPLE_RATE,
		(unsigned long)BUFFER_SIZE,
		paClipOff, // Oleg@self: investigate
		ServiceAudio_,
		this
	);

	if (err != paNoError)
	{
		std::cerr << "Error opening stream: " << Pa_GetErrorText(err_) << std::endl;
		return false;
	}

	err_ = Pa_StartStream(pStream_); // Oleg@self: I think this should be part of Renderer instead.
	assert(!err_, "Sound maker reported error starting a stream.");
}

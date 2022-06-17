#include "RendererManager.h"

bsExp::RendererManager::RendererManager():
	threeDTI_renderer_(bs::ThreeDTI_AudioRenderer(HRTF_PATH, BRIR_PATH, BUFFER_SIZE, SAMPLE_RATE))
{
	// Init portaudio.
	auto err = Pa_Initialize();
	assert(err == paNoError, "Portaudio failed to initialize!");

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
	assert(err == paNoError, "Failed to open a portaudio stream!");

	err = Pa_StartStream(pStream_); // Oleg@self: I think this should be part of Renderer instead.
	assert(err == paNoError, "Failed to open portaudio stream!");

	renderResult_.resize(2 * BUFFER_SIZE, 0.0f);

	threeDTI_soundIds_.emplace("speech", threeDTI_renderer_.CreateSoundMaker(WAV_PATH_SPEECH, false, true));
	threeDTI_soundIds_.emplace("noise", threeDTI_renderer_.CreateSoundMaker(WAV_PATH_BROWN_NOISE, true, false));
	threeDTI_soundIds_.emplace("sweep", threeDTI_renderer_.CreateSoundMaker(WAV_PATH_SWEEP, false, true));
}

bsExp::RendererManager::~RendererManager()
{
	auto err = Pa_AbortStream(pStream_);
	assert(err == paNoError, "Failed to stop portaudio stream!");
	err = Pa_Terminate();
	assert(err == paNoError, "Portaudio failed to terminate properly!");
}

void bsExp::RendererManager::CallImplementationsUpdates()
{
	// fmod update call goes here
}

void bsExp::RendererManager::PlaySound(const char* soundName)
{
	switch (selectedRenderer_)
	{
		case bsExp::RendererManager::AudioRendererType::ThreeDTI:
		{
			if (threeDTI_soundIds_.find(soundName) != threeDTI_soundIds_.end())
			{
				threeDTI_renderer_.GetSound(threeDTI_soundIds_[soundName]).Play();
			}
			else
			{
				assert(false, "Sound isn't loaded: " << soundName);
			}
		}
		break;
		default:
		break;
	}
}

void bsExp::RendererManager::PauseSound(const char* soundName)
{
	switch (selectedRenderer_)
	{
		case bsExp::RendererManager::AudioRendererType::ThreeDTI:
		{
			if (threeDTI_soundIds_.find(soundName) != threeDTI_soundIds_.end())
			{
				threeDTI_renderer_.GetSound(threeDTI_soundIds_[soundName]).Pause();
			}
			else
			{
				assert(false, "Sound isn't loaded: " << soundName);
			}
		}
		break;
		default:
		break;
	}
}

bool bsExp::RendererManager::IsPaused(const char* soundName)
{
	switch (selectedRenderer_)
	{
		case bsExp::RendererManager::AudioRendererType::ThreeDTI:
		{
			if (threeDTI_soundIds_.find(soundName) != threeDTI_soundIds_.end())
			{
				return threeDTI_renderer_.GetSound(threeDTI_soundIds_[soundName]).IsPaused();
			}
			else
			{
				assert(false, "Sound isn't loaded: " << soundName);
			}
		}
		break;
		default:
		break;
	}

	return false;
}

void bsExp::RendererManager::StopSound(const char* soundName)
{
	switch (selectedRenderer_)
	{
		case bsExp::RendererManager::AudioRendererType::ThreeDTI:
		{
			if (threeDTI_soundIds_.find(soundName) != threeDTI_soundIds_.end())
			{
				threeDTI_renderer_.GetSound(threeDTI_soundIds_[soundName]).Stop();
			}
			else
			{
				assert(false, "Sound isn't loaded: " << soundName);
			}
		}
		break;
		default:
		break;
	}
}

void bsExp::RendererManager::StopAll()
{
	for (auto& pair : threeDTI_soundIds_)
	{
		threeDTI_renderer_.GetSound(pair.second).Stop();
	}
}

void bsExp::RendererManager::MoveSound(const char* soundName, const bs::CartesianCoord coord)
{
	if (threeDTI_soundIds_.find(soundName) != threeDTI_soundIds_.end())
	{
		threeDTI_renderer_.GetSound(threeDTI_soundIds_[soundName]).SetPosition(coord);
	}
	// Oleg@self: Call other implementations too
}

void bsExp::RendererManager::MoveAllSounds(const bs::CartesianCoord coord)
{
	for (auto& pair : threeDTI_soundIds_)
	{
		threeDTI_renderer_.GetSound(pair.second).SetPosition(coord);
	}
	// Oleg@self: Call other implementations too
}

void bsExp::RendererManager::SetSelectedRenderer(const AudioRendererType type)
{
	selectedRenderer_ = type;
}

bsExp::RendererManager::AudioRendererType bsExp::RendererManager::GetSelectedRenderer() const
{
	return selectedRenderer_;
}

int bsExp::RendererManager::ServiceAudio_(const void* unused, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	RendererManager* self = (RendererManager*)(userData);
	float* out = (float*)outputBuffer;

	std::fill(self->renderResult_.begin(), self->renderResult_.end(), 0.0f);

	switch (self->selectedRenderer_)
	{
		case bsExp::RendererManager::AudioRendererType::ThreeDTI:
		{
			self->threeDTI_renderer_.ProcessAudio(self->renderResult_);
		}
		break;

		default:
		{
			assert(false, "Invalid renderer selected!");
		}
		break;
	}

	for (auto it = self->renderResult_.begin(); it != self->renderResult_.end(); it++)
	{
		*out++ = *it;
	}

	return paContinue;
}

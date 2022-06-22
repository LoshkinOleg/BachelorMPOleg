#include "RendererManager.h"

bsExp::RendererManager::RendererManager():
	threeDTI_renderer_(bs::ThreeDTI_AudioRenderer(HRTF_PATH, BRIR_PATH, BUFFER_SIZE, SAMPLE_RATE, HEAD_ALTITUDE, true)), fmod_renderer_(bs::Fmod_AudioRenderer(HEAD_ALTITUDE, BUFFER_SIZE, SAMPLE_RATE))
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

	threeDTI_soundIds_.emplace("speech", threeDTI_renderer_.CreateSoundMaker(WAV_PATH_SPEECH, false, true, true, true, true, true, true, true, true));
	threeDTI_soundIds_.emplace("noise", threeDTI_renderer_.CreateSoundMaker(WAV_PATH_BROWN_NOISE, true, false, true, true, true, true, true, true, true));
	threeDTI_soundIds_.emplace("sweep", threeDTI_renderer_.CreateSoundMaker(WAV_PATH_SWEEP, false, true, true, true, true, true, true, true, true));

	fmod_soundIds_.emplace("speech", fmod_renderer_.CreateSoundMaker(WAV_PATH_SPEECH, false, true));
	fmod_soundIds_.emplace("noise", fmod_renderer_.CreateSoundMaker(WAV_PATH_BROWN_NOISE, true, false));
	fmod_soundIds_.emplace("sweep", fmod_renderer_.CreateSoundMaker(WAV_PATH_SWEEP, false, true));
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
	fmod_renderer_.Update();
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
		case bsExp::RendererManager::AudioRendererType::Fmod:
		{
			if (fmod_soundIds_.find(soundName) != fmod_soundIds_.end())
			{
				fmod_renderer_.PlaySound(fmod_soundIds_[soundName]);
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
		case bsExp::RendererManager::AudioRendererType::Fmod:
		{
			if (fmod_soundIds_.find(soundName) != fmod_soundIds_.end())
			{
				fmod_renderer_.PauseSound(fmod_soundIds_[soundName]);
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
		case bsExp::RendererManager::AudioRendererType::Fmod:
		{
			if (fmod_soundIds_.find(soundName) != fmod_soundIds_.end())
			{
				return fmod_renderer_.IsPaused(fmod_soundIds_[soundName]);
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
		case bsExp::RendererManager::AudioRendererType::Fmod:
		{
			if (fmod_soundIds_.find(soundName) != fmod_soundIds_.end())
			{
				fmod_renderer_.StopSound(fmod_soundIds_[soundName]);
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
	for (size_t i = 0; i < fmod_soundIds_.size(); i++)
	{
		fmod_renderer_.StopSound(i);
	}
}

void bsExp::RendererManager::MoveSound(const char* soundName, const bs::CartesianCoord coord)
{
	if (threeDTI_soundIds_.find(soundName) != threeDTI_soundIds_.end())
	{
		threeDTI_renderer_.GetSound(threeDTI_soundIds_[soundName]).SetPosition(coord);
	}
	if (fmod_soundIds_.find(soundName) != fmod_soundIds_.end())
	{
		fmod_renderer_.MoveSound(threeDTI_soundIds_[soundName], coord);
	}
}

void bsExp::RendererManager::MoveAllSounds(const bs::CartesianCoord coord)
{
	for (auto& pair : threeDTI_soundIds_)
	{
		threeDTI_renderer_.GetSound(pair.second).SetPosition(coord);
	}
	for (auto& pair : threeDTI_soundIds_)
	{
		fmod_renderer_.MoveSound(pair.second, coord);
	}
}

void bsExp::RendererManager::UpdateRendererParams(const RendererParams p)
{
	threeDTI_renderer_.UpdateRendererParams(p.headAltitude, p.ILDEnabled);
	fmod_renderer_.UpdateRendererParams(p.headAltitude);
}

bsExp::RendererManager::RendererParams bsExp::RendererManager::GetRendererParams() const
{
	RendererParams p; // Oleg@self: implement possibility of loaded sounds mismatch.
	threeDTI_renderer_.GetRendererParams(p.headAltitude, p.ILDEnabled);
	return p;
}

void bsExp::RendererManager::UpdateSoundParams(const char* soundName, const SoundParams p)
{
	if (threeDTI_soundIds_.find(soundName) != threeDTI_soundIds_.end())
	{
		assert(fmod_soundIds_.find(soundName) != fmod_soundIds_.end(), "Fmod and ThreeDTI renderers don't have the same sounds!"); // Oleg@self: implement possibility of loaded sounds mismatch.
		threeDTI_renderer_.GetSound(threeDTI_soundIds_[soundName]).UpdateSpatializationParams(p.anechoicEnabled, p.distanceBasedAttenuationAnechoic,
																							  p.reverbEnabled, p.distanceBasedAttenuationReverb,
																							  p.highQualitySimulation, p.atmosphericFiltering, p.nearFieldEffects);
		fmod_renderer_.UpdateSoundParams(threeDTI_soundIds_[soundName]);
	}
}

bsExp::RendererManager::SoundParams bsExp::RendererManager::GetSoundParams(const char* soundName)
{
	SoundParams p{};
	if (threeDTI_soundIds_.find(soundName) != threeDTI_soundIds_.end())
	{
		assert(fmod_soundIds_.find(soundName) != fmod_soundIds_.end(), "Fmod and ThreeDTI renderers don't have the same sounds!"); // Oleg@self: implement possibility of loaded sounds mismatch.
		threeDTI_renderer_.GetSound(threeDTI_soundIds_[soundName]).GetSoundParams(p.anechoicEnabled, p.distanceBasedAttenuationAnechoic,
																				  p.reverbEnabled, p.distanceBasedAttenuationReverb,
																				  p.highQualitySimulation, p.atmosphericFiltering, p.nearFieldEffects);
	}
	return p;
}

void bsExp::RendererManager::SetSelectedRenderer(const AudioRendererType type)
{
	assert((size_t)type < ((size_t)AudioRendererType::MAX) + 1);
	threeDTI_renderer_.StopAllSounds();
	fmod_renderer_.StopAllSounds();
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

		case bsExp::RendererManager::AudioRendererType::Fmod:
		{
			// No servicing audio ourselves.
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

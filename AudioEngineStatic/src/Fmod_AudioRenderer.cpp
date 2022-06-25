#include "Fmod_AudioRenderer.h"

#include <cassert>

/*
	+X is right
	-X is left
	+Y is up
	-Y is down
	+Z is front
	-Z is back
*/

bs::Fmod_AudioRenderer::Fmod_AudioRenderer(const float headAltitude, const size_t bufferSize, const size_t sampleRate,
										   const float DecayTime, const float EarlyDelay, const float LateDelay, const float HFReference,
										   const float HFDecayRatio, const float Diffusion, const float Density, const float LowShelfFrequency,
										   const float LowShelfGain, const float HighCut, const float EarlyLateMix, const float WetLevel):
	bufferSize(bufferSize), sampleRate(sampleRate)
{
	FMOD_RESULT result = FMOD::System_Create(&context_);      // Create the main system object.
	assert(result == FMOD_OK, "Couldn't create Fmod system!");

	result = context_->init(32, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	assert(result == FMOD_OK, "Couldn't initialize Fmod system!");

	result = context_->createReverb3D(&reverb_);
	assert(result == FMOD_OK, "Couldn't create fmod reverb object!");
	FMOD_REVERB_PROPERTIES p = {DecayTime, EarlyDelay, LateDelay, HFReference, HFDecayRatio, Diffusion, Density, LowShelfFrequency, LowShelfGain, HighCut, EarlyLateMix, WetLevel};
	result = reverb_->setProperties(&p);
	assert(result == FMOD_OK, "Couldn't set fmod reverb properties!");
	FMOD_VECTOR pos{0.0f, headAltitude, 0.0f};
	result = context_->set3DListenerAttributes(0, &pos, 0, 0, 0);
	assert(result == FMOD_OK, "Couldn't set listener position!");
	result = reverb_->set3DAttributes(&pos, 0.0f, 10.0f); // Center reverb area on listener. Start attenuating at 0.0 meters away from listener and up to 10.0 meters.
	assert(result == FMOD_OK, "Couldn't set reverb area position!");
}
bs::Fmod_AudioRenderer::~Fmod_AudioRenderer()
{
	for (auto& sound : sounds_)
	{
		auto result = sound.first->release();
		assert(result == FMOD_OK, "Failed to release fmod sound!");
	}
	auto result = reverb_->release();
	assert(result == FMOD_OK, "Failed to release fmod reverb!");
	result = context_->release();
	assert(result == FMOD_OK, "Failed to release fmod context!");
}

void bs::Fmod_AudioRenderer::GetRendererParams(float& outHeadAltitude) const
{
	FMOD_VECTOR pos;
	FMOD_RESULT result = context_->get3DListenerAttributes(0, &pos, nullptr, nullptr, nullptr);
	assert(result == FMOD_OK, "Couldn't retrieve listener attributes!");
	outHeadAltitude = pos.y; // y is up in fmod
}
void bs::Fmod_AudioRenderer::UpdateRendererParams(const float headAltitude)
{
	FMOD_VECTOR pos, vel, forward, up;
	FMOD_RESULT result = context_->get3DListenerAttributes(0, &pos, &vel, &forward, &up);
	assert(result == FMOD_OK, "Couldn't retrieve listener attributes!");
	pos.y = headAltitude;
	result = context_->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
	assert(result == FMOD_OK, "Couldn't set listener attributes!");
}

size_t bs::Fmod_AudioRenderer::CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize)
{
	sounds_.push_back({ nullptr, nullptr });

	FMOD_MODE m = 0;
	m |= loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	if (spatialize)
	{
		m |= FMOD_3D;
		m |= FMOD_3D_WORLDRELATIVE;
		m |= FMOD_3D_INVERSEROLLOFF;
		m |= FMOD_CREATESAMPLE;
		m |= FMOD_UNIQUE;
		m |= FMOD_3D_IGNOREGEOMETRY;
	}
	else
	{
		m |= FMOD_2D;
		m |= FMOD_CREATESAMPLE;
		m |= FMOD_UNIQUE;
	}
	FMOD_RESULT result = context_->createSound(wavFileName, m, 0, &sounds_.back().first);
	assert(result == FMOD_OK, "Couldn't create fmod sound!");

	return sounds_.size() - 1;
}
FMOD::Sound& bs::Fmod_AudioRenderer::GetSound(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to GetSound()!");
	return *sounds_[soundId].first;
}

void bs::Fmod_AudioRenderer::PlaySound(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to PlaySound()!");

	FMOD_RESULT result;
	if (sounds_[soundId].second) // If sound handle is valid.
	{
		if (IsPaused(soundId)) // If sound handle is valid, that means logically that the sound is paused since the handle is set to nullptr at end of playing.
		{
			result = sounds_[soundId].second->setPaused(false);
		}
		else
		{
			assert(true, "This shouldn't be reachable...");
			result = context_->playSound(sounds_[soundId].first, nullptr, false, &sounds_[soundId].second);
		}
		assert(result == FMOD_OK, "Failed to play fmod sound!");
	}
	else // Sound handle is invalid, meaning the sound is not paused and not playing.
	{
		result = context_->playSound(sounds_[soundId].first, nullptr, true, &sounds_[soundId].second);
		assert(result == FMOD_OK, "Failed to play fmod sound!");

		FMOD_MODE m;
		result = sounds_[soundId].first->getMode(&m);
		assert(result == FMOD_OK, "Failed to retrieve fmod's sound mode!");
		if (m & FMOD_2D) // Disable reverb for 2d sounds.
		{
			sounds_[soundId].second->setReverbProperties(0, 0.0f); // Note: 0 is the default reverb object that doesn't apply any reverberations.
			assert(result == FMOD_OK, "Failed to set fmod's sound's reverb properties!");
		}
		result = sounds_[soundId].second->setPaused(false);
		assert(result == FMOD_OK, "Failed to unpause fmod sound!");
	}
}

void bs::Fmod_AudioRenderer::PauseSound(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to PauseSound()!");
	if (sounds_[soundId].second)
	{
		if (!IsPaused(soundId))
		{
			FMOD_RESULT result = sounds_[soundId].second->setPaused(true);
			assert(result == FMOD_OK, "Failed to pause fmod sound!");
		}
	}
}

void bs::Fmod_AudioRenderer::StopSound(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to StopSound()!");
	if (sounds_[soundId].second)
	{
		FMOD_RESULT result = sounds_[soundId].second->stop();
		assert(result == FMOD_OK, "Failed to stop fmod sound!");
		sounds_[soundId].second = nullptr;
	}
}

bool bs::Fmod_AudioRenderer::IsPaused(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to IsPaused()!");
	if (sounds_[soundId].second)
	{
		bool paused;
		assert(sounds_[soundId].second != nullptr, "Sound is not playing, cannot call IsPaused()."); // Oleg@self: this shouldn't be throwing, handle the case where we're calling IsPaused() without playing the sound first properly!
		FMOD_RESULT result = sounds_[soundId].second->getPaused(&paused);
		assert(result == FMOD_OK, "Failed to retieve paused bool of fmod sound!");
		return paused;
	}
	else
	{
		return false;
	}
}

void bs::Fmod_AudioRenderer::Update()
{
	for (auto& sound : sounds_)
	{
		if (sound.second)
		{
			bool playing;
			auto result = sound.second->isPlaying(&playing);
			assert(result == FMOD_OK, "Failed to retireve sound's isPlaying bool.");
			if (!playing) sound.second = nullptr;
		}
	}
	context_->update();
}

void bs::Fmod_AudioRenderer::UpdateSoundParams(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to UpdateSoundParams()!");
	return;
}

void bs::Fmod_AudioRenderer::MoveSound(const size_t soundId, const bs::CartesianCoord coord)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to MoveSound()!");
	if (sounds_[soundId].second)
	{
		FMOD_MODE m;
		auto result = sounds_[soundId].first->getMode(&m);
		assert(result == FMOD_OK, "Failed to get mode of fmod sound.");
		if (m & FMOD_3D)
		{
			FMOD_VECTOR pos, vel;
			FMOD_RESULT result = sounds_[soundId].second->get3DAttributes(&pos, &vel);
			assert(result == FMOD_OK, "Failed to retrieve fmod sound position.");
			pos.z = coord.x; // Oleg@self: check that this is correct.
			pos.y = coord.z;
			pos.x = -coord.y;
			sounds_[soundId].second->set3DAttributes(&pos, &vel);
			assert(result == FMOD_OK, "Failed to set fmod sound position.");
		}
	}
}

void bs::Fmod_AudioRenderer::MoveListener(const bs::CartesianCoord pos, const std::array<float, 4> quat)
{
}

void bs::Fmod_AudioRenderer::GetSoundParams(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to GetSoundParams()!");
}

void bs::Fmod_AudioRenderer::StopAllSounds()
{
	for (size_t i = 0; i < sounds_.size(); i++)
	{
		StopSound(i);
	}
}
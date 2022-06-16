#include <Fmod_SoundMaker.h>

#include <iostream>
#include <cassert>

#include <Fmod_AudioRenderer.h>
#include <UtilityFunctions.h>

bool bs::Fmod_SoundMaker::Init(bs::Fmod_AudioRenderer* engine, const char* wavFileName, FMOD::System* fmodSystem, const ClipWrapMode wrapMode, const bool spatialize)
{
	wrapMode_ = wrapMode;
	spatialized_ = spatialize;

	if (spatialized_)
	{
		const FMOD_MODE soundMode =
			FMOD_3D_IGNOREGEOMETRY | // We only want to test the middleware's panning ability
			FMOD_3D_INVERSEROLLOFF |
			FMOD_3D_WORLDRELATIVE |
			FMOD_UNIQUE | // Not gonna have multiple instances of sound
			FMOD_CREATESAMPLE | // No streaming the sound, just load the whole thing
			FMOD_3D |
			(wrapMode == ClipWrapMode::LOOP ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		auto result = fmodSystem->createSound(wavFileName, soundMode, NULL, &fmodSound_);
		assert(result == FMOD_OK, "Fmod failed to create a sound!");
	}
	else
	{
		const FMOD_MODE soundMode =
			FMOD_3D_IGNOREGEOMETRY | // We only want to test the middleware's panning ability
			FMOD_UNIQUE | // Not gonna have multiple instances of sound
			FMOD_CREATESAMPLE | // No streaming the sound, just load the whole thing
			FMOD_2D |
			(wrapMode == ClipWrapMode::LOOP ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		auto result = fmodSystem->createSound(wavFileName, soundMode, NULL, &fmodSound_);
		assert(result == FMOD_OK, "Fmod failed to create a sound!");
	}

	return true;
}
void bs::Fmod_SoundMaker::Play(FMOD::System* fmodSystem)
{
	auto result = fmodSystem->playSound(fmodSound_, NULL, true, &fmodChannel_); // Oleg@self: apparently the position of a sound is linked to it's channel?...
	assert(result == FMOD_OK, "Fmod couldn't play sound!");
	if (spatialized_)
	{
		result = fmodChannel_->set3DAttributes(&pos_, &linearVel_);
		assert(result == FMOD_OK, "Fmod couldn't set position and velocity of sound!");
	}
	result = fmodChannel_->setPaused(false);
	assert(result == FMOD_OK, "Fmod couldn't unpause the sound!");
	// Oleg@self: once done playing, the channel handle will become invalid.
}
void bs::Fmod_SoundMaker::Stop()
{
	if (fmodChannel_)
	{
		auto result = fmodChannel_->stop();
		fmodChannel_ = nullptr;
		assert(result == FMOD_OK, "Failed to stop sound!");
	}
}
void bs::Fmod_SoundMaker::Shutdown()
{
	auto result = fmodSound_->release();
	assert(result == FMOD_OK, "Error releasing an fmod sound!");
	fmodSound_ = NULL;
}

bool bs::Fmod_SoundMaker::GetPaused()
{
	if (!fmodChannel_->getPaused(&paused_))
	{
		return paused_;
	}
	else
	{
		return true;
	}
}
void bs::Fmod_SoundMaker::SetPosition(float globalX, float globalY, float globalZ)
{
	// Oleg@self: check coordinate system
	pos_.x = globalX;
	pos_.y = globalY;
	pos_.z = globalZ;
}

void bs::Fmod_SoundMaker::Reset(Fmod_AudioRenderer& renderer)
{

}
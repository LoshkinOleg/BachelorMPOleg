#include <Fmod_SoundMaker.h>

#include <iostream>
#include <cassert>

#include <Fmod_AudioRenderer.h>
#include <UtilityFunctions.h>

bool bs::Fmod_SoundMaker::Init(bs::Fmod_AudioRenderer* engine, const char* wavFileName, FMOD::System* fmodSystem, const ClipWrapMode wrapMode)
{
	wrapMode_ = wrapMode;

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

	return true;
}
void bs::Fmod_SoundMaker::Play(FMOD::System* fmodSystem)
{
	FMOD::Channel* fmodChannel;
	auto result = fmodSystem->playSound(fmodSound_, NULL, false, &fmodChannel); // Oleg@self: apparently the position of a sound is linked to it's channel?...
	// Oleg@self: once done playing, the channel handle will become invalid.
	assert(result == FMOD_OK, "Fmod couldn't play sound!");
}
void bs::Fmod_SoundMaker::Shutdown()
{
	auto result = fmodSound_->release();
	assert(result == FMOD_OK, "Error releasing an fmod sound!");
	fmodSound_ = NULL;
}
void bs::Fmod_SoundMaker::SetPosition(float globalX, float globalY, float globalZ)
{
	assert(false, "SetPosition() isn't implemented!");
}

void bs::Fmod_SoundMaker::Reset(Fmod_AudioRenderer& renderer)
{

}
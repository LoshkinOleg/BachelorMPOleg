#pragma once

#include <vector>

#include <portaudio.h>
#include <fmod.hpp>

// Oleg@self: make an abstraction out of this.

namespace bs
{
	class Fmod_AudioRenderer;

	class Fmod_SoundMaker
	{
	public:
		Fmod_SoundMaker() = default;
		Fmod_SoundMaker(const Fmod_SoundMaker&) = delete; // Owns a resource it's responsible for, so no copying.
		Fmod_SoundMaker(Fmod_SoundMaker&&) = default; // There's no reason for moving this class around.
		Fmod_SoundMaker& operator=(const Fmod_SoundMaker&) = delete; // Owns a resource it's responsible for, so no copy assignement.
		Fmod_SoundMaker& operator=(Fmod_SoundMaker&&) = default; // There's no need for move assignment for this class.

		// Oleg@self: those really should be replaced with constructors / destructors.
		bool Init(bs::Fmod_AudioRenderer* engine, const char* wavFileName, FMOD::System* fmodSystem, const ClipWrapMode wrapMode = ClipWrapMode::ONE_SHOT);
		void Play(FMOD::System* fmodSystem);
		void Shutdown();

		void SetPosition(const float globalX, const float globalY, const float globalZ);

		// Oleg@self: maybe make a singleton out of this class instead of passing references of it in every method?...
		void Reset(Fmod_AudioRenderer& engine);

	private:
		// Fmod stuff
		FMOD::Sound* fmodSound_;
		FMOD_VECTOR pos_ = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR linearVel_ = { 0.0f, 0.0f, 0.0f };
		

		// SoundMaker specific stuff
		ClipWrapMode wrapMode_ = ClipWrapMode::ONE_SHOT;
	};
}
#pragma once

#include <fmod.hpp>

#include "Fmod_SoundMaker.h"
#include "BSCommon.h"

//namespace bs
//{
//	class Fmod_AudioRenderer
//	{
//	public:
//		// Oleg@self: replace with constructor / destructor
//		bool Init(size_t BUFFER_SIZE = 1024, size_t SAMPLE_RATE = 44100);
//		void Update();
//		void Shutdown();
//
//		SoundMakerId CreateSoundMaker(const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::ONE_SHOT, const bool spatialize = true);
//		void MoveSoundMaker(SoundMakerId id, const float globalX, const float globalY, const float globalZ);
//		void ResetSoundMaker(SoundMakerId id);
//		void PlaySound(SoundMakerId id);
//		void StopSound(SoundMakerId id);
//
//		void SetSelectedSound(const size_t soundId);
//		bool GetSelectedSound() const;
//
//		inline const std::vector<Fmod_SoundMaker>& GetSounds() const { return sounds_; };
//
//		static size_t GetBufferSize() { return BUFFER_SIZE_; };
//		static size_t GetSampleRate() { return SAMPLE_RATE_; };
//
//	private:
//		// Fmod stuff
//		FMOD::System* fmodSystem_ = NULL;
//		FMOD_RESULT fmodErr_ = FMOD_RESULT::FMOD_OK;
//
//		// Renderer specific stuff
//		std::vector<Fmod_SoundMaker> sounds_;
//		size_t selectedSound_ = 0;
//
//		static size_t BUFFER_SIZE_;
//		static size_t SAMPLE_RATE_;
//	};
//}
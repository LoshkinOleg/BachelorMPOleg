#pragma once

#include <vector>
#include <cassert>

#include <fmod.hpp>

#include "BSCommon.h"
// #include "Fmod_SoundMaker.h"

// Oleg@self: issue: sound not playing, specified right device?

namespace bs
{
	class Fmod_AudioRenderer
	{
	public:

		BS_NON_COPYABLE(Fmod_AudioRenderer);
		BS_NON_MOVEABLE(Fmod_AudioRenderer);

		Fmod_AudioRenderer() = delete;
		Fmod_AudioRenderer(const float headAltitude):
			bufferSize(bufferSize), sampleRate(sampleRate)
		{
			FMOD_RESULT result = FMOD::System_Create(&context_);      // Create the main system object.
			assert(result == FMOD_OK, "Couldn't create Fmod system!");

			result = context_->init(32, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
			assert(result == FMOD_OK, "Couldn't initialize Fmod system!");
		}

		void GetRendererParams(float& outHeadAltitude) const
		{
			FMOD_VECTOR pos;
			FMOD_RESULT result = context_->get3DListenerAttributes(0, &pos, nullptr, nullptr, nullptr);
			assert(result == FMOD_OK, "Couldn't retrieve listener attributes!");
			outHeadAltitude = pos.y; // y is up in fmod
		}
		void UpdateRendererParams(const float headAltitude)
		{
			FMOD_VECTOR pos, vel, forward, up;
			FMOD_RESULT result = context_->get3DListenerAttributes(0, &pos, &vel, &forward, &up);
			assert(result == FMOD_OK, "Couldn't retrieve listener attributes!");
			pos.y = headAltitude;
			result = context_->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
			assert(result == FMOD_OK, "Couldn't set listener attributes!");
		}

		size_t CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize)
		{
			sounds_.push_back({nullptr, nullptr});

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
		FMOD::Sound& GetSound(const size_t soundId)
		{
			assert(soundId < sounds_.size(), "Invalid soundId passed to GetSound()!");
			return *sounds_[soundId].first;
		}

		void PlaySound(const size_t soundId)
		{
			assert(soundId < sounds_.size(), "Invalid soundId passed to PlaySound()!");
			FMOD_RESULT result;
			if (sounds_[soundId].second)
			{
				if (IsPaused(soundId))
				{
					result = sounds_[soundId].second->setPaused(false);
				}
				else
				{
					result = context_->playSound(sounds_[soundId].first, nullptr, false, &sounds_[soundId].second);
				}
				assert(result == FMOD_OK, "Failed to play fmod sound!");
			}
		}

		void PauseSound(const size_t soundId)
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

		void StopSound(const size_t soundId)
		{
			assert(soundId < sounds_.size(), "Invalid soundId passed to StopSound()!");
			if (sounds_[soundId].second)
			{
				FMOD_RESULT result = sounds_[soundId].second->stop();
				assert(result == FMOD_OK, "Failed to stop fmod sound!");
				sounds_[soundId].second = nullptr;
			}
		}

		bool IsPaused(const size_t soundId)
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

		void Update()
		{
			context_->update();
		}

		void UpdateSoundParams(const size_t soundId)
		{
			assert(soundId < sounds_.size(), "Invalid soundId passed to UpdateSoundParams()!");
			return;
		}

		void MoveSound(const size_t soundId, const bs::CartesianCoord coord)
		{
			assert(soundId < sounds_.size(), "Invalid soundId passed to MoveSound()!");
			if (sounds_[soundId].second)
			{
				FMOD_VECTOR pos, vel;
				FMOD_RESULT result = sounds_[soundId].second->get3DAttributes(&pos, &vel);
				assert(result == FMOD_OK, "Failed to retrieve fmod sound position.");
				pos.x = coord.x; // Oleg@self: check that this is correct.
				pos.y = coord.z;
				pos.z = coord.y;
				sounds_[soundId].second->set3DAttributes(&pos, &vel);
				assert(result == FMOD_OK, "Failed to set fmod sound position.");
			}
		}

		void GetSoundParams(const size_t soundId)
		{
			assert(soundId < sounds_.size(), "Invalid soundId passed to GetSoundParams()!");
		}

		const size_t bufferSize;
		const size_t sampleRate;
		constexpr static size_t const HRTF_RESAMPLING_STEP = 15;

	private:
		FMOD::System* context_ = nullptr;

		std::vector<std::pair<FMOD::Sound*, FMOD::Channel*>> sounds_;
	};
}
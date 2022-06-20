#include <SteamAudio_SoundMaker.h>

#include <cassert>

bs::SteamAudio_SoundMaker::SteamAudio_SoundMaker(const std::vector<float>& data, const bool loop, const bool spatialize, const size_t bufferSize, const IPLContext& context, const IPLHRTF& hrtf):
	soundData_(data), looping(loop), spatialized(spatialize), bufferSize(bufferSize), context_(context)
{
	if (iplAudioBufferAllocate(context, 2, bufferSize, &stereoSpatializedData_))
	{
		assert(true, "Phonon failed to allocate a buffer for stereoSpatializedData_!");
	}
	// if (iplAudioBufferAllocate(context, 2, bufferSize, &spatializationTail_))
	// {
	// 	assert(true, "Phonon failed to allocate a buffer for spatializationTail_!");
	// }

	spatializationParams_ =
	{
		IPLVector3{ 0.0f, 0.0f, 0.0f },
		IPL_HRTFINTERPOLATION_NEAREST,
		1.0f,
		hrtf
	};

	soundDataSubset_.resize(bufferSize, 0.0f);
	assert(soundData_.size() >= bufferSize, "The buffer size is greater than the wav data size! Something's wierd's going on, check your code.");
	currentBegin_ = soundData_.size();
	currentEnd_ = soundData_.size();

	// TODO: init source
}

bs::SteamAudio_SoundMaker::~SteamAudio_SoundMaker()
{
	iplAudioBufferFree(context_, &stereoSpatializedData_); // Oleg@self: this should only be called on object shutdown, not destruction since this object can be moved!
}

void bs::SteamAudio_SoundMaker::SetPosition(const bs::CartesianCoord coord)
{
	spatializationParams_.direction = IPLVector3{ coord.x, coord.y, coord.z }; // Oleg@self: take into account coordinate system differences. And you're given a position, not a direction...
}

void bs::SteamAudio_SoundMaker::SetPosition(const bs::SphericalCoord coord)
{
	SetPosition(bs::ToCartesian(coord));
}

void bs::SteamAudio_SoundMaker::Play()
{
	// If this sound was paused, just unpause it.
	if (paused_)
	{
		paused_ = false;
	}
	if (!IsPlaying())
	{
		// Reset wav data indices back to start of the data.
		currentBegin_ = 0;
		currentEnd_ = bufferSize - 1;
	}
}

void bs::SteamAudio_SoundMaker::Pause()
{
	paused_ = true;
}

void bs::SteamAudio_SoundMaker::Stop()
{
	currentBegin_ = soundData_.size();
}

bool bs::SteamAudio_SoundMaker::IsPaused() const
{
	return paused_;
}

bool bs::SteamAudio_SoundMaker::IsPlaying() const
{
	return currentBegin_ != soundData_.size();
}

void bs::SteamAudio_SoundMaker::ProcessAudio_(std::vector<float>& interlacedStereoOut, const IPLBinauralEffect& effect)
{
	const auto wavSize = soundData_.size();

	// Fill buffers with silence.
	std::fill(interlacedStereoOut.begin(), interlacedStereoOut.end(), 0.0f);
	std::fill(soundDataSubset_.begin(), soundDataSubset_.end(), 0.0f);

	// Read wav subset.
	if (!paused_ && currentBegin_ != wavSize) // Not paused and haven't reached end of the wav data if this is a non looping sound.
	{
		if (currentBegin_ > currentEnd_) // Wrapping around wav data.
		{
			for (size_t i = currentBegin_; i < wavSize; i++) // Finish reading end of the wav.
			{
				soundDataSubset_[i - currentBegin_] = soundData_[i];
			}
			for (size_t i = 0; i < currentEnd_ + 1; i++) // Read the start of the wav into the remaining not yet updated part of the soundDataSubset_.
			{
				soundDataSubset_[wavSize - currentBegin_ + i] = soundData_[i];
			}
		}
		else // Not wrapping around wav data, just copy soundData into subset continuously.
		{
			for (size_t i = currentBegin_; i < currentEnd_ + 1; i++)
			{
				soundDataSubset_[i - currentBegin_] = soundData_[i];
			}
		}

		// Spatialize sound and write to output buffer.
		if (spatialized)
		{
			float* in = soundDataSubset_.data();
			IPLAudioBuffer iplIn{ 1, bufferSize, &in };
			auto remainingSamples = iplBinauralEffectApply(effect, &spatializationParams_, &iplIn, &stereoSpatializedData_);
			if (remainingSamples == IPLAudioEffectState::IPL_AUDIOEFFECTSTATE_TAILREMAINING)
			{
				// TODO: implement carrying over trailing samples.
			}
			iplAudioBufferInterleave(context_, &stereoSpatializedData_, interlacedStereoOut.data());
		}
		else
		{
			bs::Interlace(interlacedStereoOut, soundDataSubset_, soundDataSubset_);
		}

		// Update indices.
		if (looping) // currentBegin_ can never reach wavSize.
		{
			// Update currentBegin_
			assert(currentEnd_ + 1 <= wavSize, "currentEnd_ somehow incremented past wavSize! Last iteration must have had currentEnd_ = wavSize, which shouldn't be possible. Check your code.");
			if (currentEnd_ + 1 == wavSize) // If wavSize % bufferSize = 0, this can happen, wrap back to 0.
			{
				currentBegin_ = 0;
			}
			else // Just advance to next subset with no issues.
			{
				currentBegin_ = currentEnd_ + 1;
			}

			// Update currentEnd_
			if (currentBegin_ + bufferSize - 1 >= wavSize) // If overruning wav data, wrap around.
			{
				currentEnd_ = bufferSize - 1 - (wavSize - currentBegin_);
			}
			else // Not overruning wav data, just update currentEnd_ with no issues.
			{
				currentEnd_ = currentBegin_ + bufferSize - 1;
			}
		}
		else // currentBegin_ can reach wavSize.
		{
			if (currentEnd_ + 1 == wavSize) // Happens when clip reached the last window of the wav data.
			{
				currentBegin_ = wavSize;
				currentEnd_ = wavSize;
			}
			else
			{
				assert(currentEnd_ + 1 <= wavSize, "Somehow currentEnd_ is greater than wavSize, this shouldn't be possible, check your code!");
				currentBegin_ = currentEnd_ + 1;

				if (currentBegin_ + bufferSize - 1 >= wavSize) // Reached end of the wav.
				{
					currentEnd_ = wavSize - 1;
				}
				else
				{
					currentEnd_ = currentBegin_ + bufferSize - 1;
				}
			}
		}
	}
}

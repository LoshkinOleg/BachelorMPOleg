#include <ThreeDTI_SoundMaker.h>

#include <algorithm>

#include <ThreeDTI_AudioRenderer.h>
#include <BSCommon.h>

bs::ThreeDTI_SoundMaker::ThreeDTI_SoundMaker(const std::vector<float>& data, Binaural::CCore& core, const bool loop, const bool spatialize, const size_t bufferSize):
	soundData_(data), looping(loop), spatialized(spatialize), bufferSize(bufferSize)
{
	source_ = core.CreateSingleSourceDSP();
	if (spatialized)
	{
		source_->SetSpatializationMode(Binaural::TSpatializationMode::HighQuality);
		source_->EnableNearFieldEffect();
		source_->EnableAnechoicProcess();
		source_->EnableDistanceAttenuationAnechoic();
		source_->EnableDistanceAttenuationReverb();
	}
	else
	{
		source_->SetSpatializationMode(Binaural::TSpatializationMode::NoSpatialization);
		source_->DisableNearFieldEffect();
		source_->DisableAnechoicProcess();
		source_->DisableDistanceAttenuationAnechoic();
		source_->DisableDistanceAttenuationReverb();
	}

	soundDataSubset_.resize(bufferSize);
	anechoic_.left.resize(bufferSize);
	anechoic_.right.resize(bufferSize);

	source_->SetBuffer(soundDataSubset_);

	assert(soundData_.size() >= bufferSize, "The buffer size is greater than the wav data size! Something's wierd's going on, check your code.");
	currentBegin_ = soundData_.size();
	currentEnd_ = soundData_.size();
}

void bs::ThreeDTI_SoundMaker::SetPosition(const bs::CartesianCoord coord)
{
	if (spatialized)
	{
		Common::CTransform t = source_->GetSourceTransform(); // Have to copy it to preserve rotation.
		t.SetPosition(Common::CVector3(coord.x, coord.y, coord.z)); // Oleg@self: account for axis system difference.
		source_->SetSourceTransform(t);
	}
}

void bs::ThreeDTI_SoundMaker::SetPosition(const bs::SphericalCoord coord)
{
	const auto cartCoord = bs::ToCartesian(coord);
	if (spatialized)
	{
		Common::CTransform t = source_->GetSourceTransform(); // Have to copy it to preserve rotation.
		t.SetPosition(Common::CVector3(cartCoord.x, cartCoord.y, cartCoord.z)); // Oleg@self: account for axis system difference.
		source_->SetSourceTransform(t);
	}
}

void bs::ThreeDTI_SoundMaker::Play()
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

void bs::ThreeDTI_SoundMaker::Pause()
{
	paused_ = true;
}

void bs::ThreeDTI_SoundMaker::Stop()
{
	currentBegin_ = soundData_.size();
}

bool bs::ThreeDTI_SoundMaker::IsPaused() const
{
	return paused_;
}

bool bs::ThreeDTI_SoundMaker::IsPlaying() const
{
	return currentBegin_ != soundData_.size();
}

void bs::ThreeDTI_SoundMaker::ProcessAudio_(std::vector<float>& interlacedStereoOut)
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

		// Spatialize sound if needed and interlace it into the output buffer.
		if (spatialized)
		{
			source_->ProcessAnechoic(anechoic_.left, anechoic_.right); // Ideally, this should NOT be done upon portaudio's servicing callback as it may introduce audio stuttering due to computational time.
		}
		else
		{
			anechoic_.left.insert(anechoic_.left.begin(), soundDataSubset_.begin(), soundDataSubset_.end());
			anechoic_.right.insert(anechoic_.left.begin(), soundDataSubset_.begin(), soundDataSubset_.end());
		}
		bs::Interlace(interlacedStereoOut, anechoic_.left, anechoic_.right);

		// Update indices.
		if (looping) // currentBegin_ can never reach wavSize.
		{
			// Update currentBegin_
			assert(currentEnd_ + 1 > wavSize, "currentEnd_ somehow incremented past wavSize! Last iteration must have had currentEnd_ = wavSize, which shouldn't be possible. Check your code.");
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
			if (currentEnd_ + 1 == wavSize) // If wavSize % bufferSize = 0, this can happen.
			{
				currentBegin_ = wavSize;
				currentEnd_ = wavSize;
			}
			else
			{
				assert(currentEnd_ + 1 > wavSize, "Somehow currentEnd_ is greater than wavSize, this shouldn't be possible, check your code!");
				currentBegin_ = currentEnd_ + 1;

				if (currentBegin_ + bufferSize - 1 >= wavSize) // Reached end of the wav.
				{
					currentEnd_ = wavSize;
				}
				else
				{
					currentEnd_ = currentBegin_ + bufferSize - 1;
				}
			}
		}
	}
}
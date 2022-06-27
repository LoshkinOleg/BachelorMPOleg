#include <ThreeDTI_SoundMaker.h>

#include <algorithm>

#include <ThreeDTI_AudioRenderer.h>
#include <BSCommon.h>

/*
	+X is front
	-X is back
	+Y is left
	-Y is right
	+Z is up
	-Z is down
*/

void bs::ThreeDTI_SoundMaker::GetSoundParams(bool& anechoicEnabled, bool& distanceBasedAttenuationAnechoic, bool& reverbEnabled, bool& distanceBasedAttenuationReverb, bool& highQualitySimulation, bool& atmosphericFiltering, bool& nearFieldEffects) const
{
	anechoicEnabled = source_->IsAnechoicProcessEnabled();
	distanceBasedAttenuationAnechoic = source_->IsDistanceAttenuationEnabledAnechoic();
	reverbEnabled = source_->IsReverbProcessEnabled();
	distanceBasedAttenuationReverb = source_->IsDistanceAttenuationEnabledReverb();
	highQualitySimulation = source_->GetSpatializationMode() == Binaural::TSpatializationMode::HighQuality ? true : false;
	nearFieldEffects = source_->IsNearFieldEffectEnabled();
	atmosphericFiltering = source_->IsFarDistanceEffectEnabled();
}

void bs::ThreeDTI_SoundMaker::UpdateSpatializationParams(const bool anechoicEnabled, const bool distanceBasedAttenuationAnechoic, const bool reverbEnabled, const bool distanceBasedAttenuationReverb, const bool highQualitySimulation, const bool atmosphericFiltering, const bool nearFieldEffects)
{
	if (spatialized)
	{
		if (anechoicEnabled) source_->EnableAnechoicProcess(); else source_->DisableAnechoicProcess();
		if (distanceBasedAttenuationAnechoic) source_->EnableDistanceAttenuationAnechoic(); else source_->DisableDistanceAttenuationAnechoic();
		if (reverbEnabled) source_->EnableReverbProcess(); else source_->DisableReverbProcess();
		if (distanceBasedAttenuationReverb) source_->EnableDistanceAttenuationReverb(); else source_->DisableDistanceAttenuationReverb();
		if (highQualitySimulation) source_->SetSpatializationMode(Binaural::TSpatializationMode::HighQuality); else source_->SetSpatializationMode(Binaural::TSpatializationMode::HighPerformance);
		if (nearFieldEffects) source_->EnableNearFieldEffect(); else source_->DisableNearFieldEffect();
		if (atmosphericFiltering) source_->EnableFarDistanceEffect(); else source_->DisableFarDistanceEffect();
		source_->ResetSourceBuffers();
	}
}

bs::ThreeDTI_SoundMaker::ThreeDTI_SoundMaker(const std::vector<float>& data, Binaural::CCore& core, const bool loop, const bool spatialize, const size_t bufferSize, const bool anechoicEnabled, const bool distanceBasedAttenuationAnechoic, const bool reverbEnabled, const bool distanceBasedAttenuationReverb, const bool highQualitySimulation, const bool atmosphericFiltering, const bool nearFieldEffects):
	soundData_(data), looping(loop), spatialized(spatialize), bufferSize(bufferSize)
{
	source_ = core.CreateSingleSourceDSP();
	UpdateSpatializationParams(anechoicEnabled, distanceBasedAttenuationAnechoic, reverbEnabled, distanceBasedAttenuationReverb, highQualitySimulation, atmosphericFiltering, nearFieldEffects);

	soundDataSubset_.resize(bufferSize, 0.0f);
	anechoic_.left.resize(bufferSize, 0.0f);
	anechoic_.right.resize(bufferSize, 0.0f);

	assert(soundData_.size() >= bufferSize, "The buffer size is greater than the wav data size! Something's wierd's going on, check your code.");
	currentBegin_ = soundData_.size();
	currentEnd_ = soundData_.size();
}

void bs::ThreeDTI_SoundMaker::SetPosition(const bs::CartesianCoord coord)
{
	if (spatialized)
	{
		Common::CTransform t = source_->GetSourceTransform(); // Have to copy it to preserve rotation.
		t.SetPosition(Common::CVector3(coord.x, coord.y, coord.z));
		source_->SetSourceTransform(t);
	}
}

void bs::ThreeDTI_SoundMaker::SetPosition(const bs::SphericalCoord coord)
{
	const auto cartCoord = bs::CartesianCoord(coord);
	SetPosition(cartCoord);
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
			source_->SetBuffer(soundDataSubset_);
			source_->ProcessAnechoic(anechoic_.left, anechoic_.right); // Ideally, this should NOT be done upon portaudio's servicing callback as it may introduce audio stuttering due to computational time.
		}
		else
		{
			for (size_t i = 0; i < bufferSize; i++)
			{
				anechoic_.left[i] = soundDataSubset_[i];
				anechoic_.right[i] = soundDataSubset_[i];
			}
		}
		bs::Interlace(interlacedStereoOut, anechoic_.left, anechoic_.right);

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
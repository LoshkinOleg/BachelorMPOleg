#include <ThreeDTI_SoundMaker.h>

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

	window_.resize(bufferSize);
	anechoic_.left.resize(bufferSize);
	anechoic_.right.resize(bufferSize);
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

bool bs::ThreeDTI_SoundMaker::GetPaused() const
{
	return paused_;
}

void bs::ThreeDTI_SoundMaker::ProcessAudio_(CStereoBuffer<float>& outBuff)
{
	const auto wavSize = soundData_.size();

	outBuff.Fill(2 * bufferSize, 0.0f);

	// Advance frame indices.
	// Oleg@self: check this, I think it's wrong.
	if (!paused_)
	{
		currentBegin_ = currentEnd_;
		if (currentBegin_ + bufferSize <= wavSize) // Not overruning wav data.
		{
			currentEnd_ = currentBegin_ + bufferSize;
		}
		else
		{
			if (looping)
			{
				currentEnd_ = bufferSize - (wavSize - currentBegin_);
			}
			else
			{
				currentEnd_ = wavSize;
			}
		}
	}

	if (!paused_)
	{
		// Load subset of audio data into window.
		for (uint32_t i = currentBegin_; i < currentEnd_; i++)
		{
			window_[i - currentBegin_] = soundData_[i];
		}
	}

	if (spatialized)
	{
		source_->SetBuffer(window_); // Set source buffer to read from. Makes the next lines use the window buffer as source of sound data.
		source_->ProcessAnechoic(anechoic_.left, anechoic_.right); // Write anechoic component of the sound to anechoic buffer.
		outBuff.Interlace(anechoic_.left, anechoic_.right);
	}
	else
	{
		for (size_t i = 0; i < bufferSize; i++)
		{
			outBuff[i * 2] = window_[i];
			outBuff[i * 2 + 1] = window_[i];
		}
	}
}
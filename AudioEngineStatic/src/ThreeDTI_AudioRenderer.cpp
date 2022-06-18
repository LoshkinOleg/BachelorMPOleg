#include <ThreeDTI_AudioRenderer.h>

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>
#include <HRTF/HRTFFactory.h>
#include <BRIR/BRIRFactory.h>

#include "ThreeDTI_SoundMaker.h"

size_t bs::ThreeDTI_AudioRenderer::CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize)
{
	const auto hash = hasher_(wavFileName);

	if (assets_.find(hash) == assets_.end()) // Sound not loaded.
	{
		assets_.emplace(hash, bs::LoadWav(wavFileName, 1, sampleRate));
	}

	sounds_.emplace_back(ThreeDTI_SoundMaker(assets_[hash], core_, loop, spatialize, bufferSize));

	return sounds_.size() - 1;
}

bs::ThreeDTI_SoundMaker& bs::ThreeDTI_AudioRenderer::GetSound(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to GetSound()!");
	return sounds_[soundId];
}

bs::ThreeDTI_AudioRenderer::ThreeDTI_AudioRenderer(const char* hrtfFileName, const char* brirFileName, const size_t bufferSize, const size_t sampleRate):
	bufferSize(bufferSize), sampleRate(sampleRate)
{
	// Init 3dti core.
	core_.SetAudioState({ (int)sampleRate, (int)bufferSize });
	core_.SetHRTFResamplingStep(HRTF_RESAMPLING_STEP);

	// Init 3dti listener.
	listener_ = core_.CreateListener();
	listener_->DisableCustomizedITD();
	bool unused;
	if (!HRTF::CreateFromSofa(hrtfFileName, listener_, unused)) assert(false, "Failed to load HRTF sofa file!");

	// Init 3dti environment.
	environment_ = core_.CreateEnvironment();
	environment_->SetReverberationOrder(TReverberationOrder::BIDIMENSIONAL);
	if (!BRIR::CreateFromSofa(brirFileName, environment_)) assert(false, "Failed to load BRIR sofa file!");

	interlacedReverb_.resize(2 * bufferSize, 0.0f);
	currentlyProcessedSignal_.resize(2 * bufferSize, 0.0f);
}

void bs::ThreeDTI_AudioRenderer::ProcessAudio(std::vector<float>& interleavedStereoOut)
{
	// Process anechoic.
	for (auto& sound : sounds_)
	{
		std::fill(currentlyProcessedSignal_.begin(), currentlyProcessedSignal_.end(), 0.0f);
		sound.ProcessAudio_(currentlyProcessedSignal_);
		bs::SumSignals(interleavedStereoOut, currentlyProcessedSignal_);
	}

	// Process reverb_.
	environment_->ProcessVirtualAmbisonicReverb(reverb_.left, reverb_.right);
	
	if (reverb_.left.size()) // 3dti sets size to 0 if there's no reverb_ to process.
	{
		bs::Interlace(interlacedReverb_, reverb_.left, reverb_.right);
		bs::SumSignals(interleavedStereoOut, interlacedReverb_);
	}
}

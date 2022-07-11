#include <ThreeDTI_AudioRenderer.h>

#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>
#include <Common/ErrorHandler.h>
#include <HRTF/HRTFFactory.h>
#include <BRIR/BRIRFactory.h>
#include <easy/profiler.h>

#include "ThreeDTI_SoundMaker.h"

/*
	+X is front
	-X is back
	+Y is left
	-Y is right
	+Z is up
	-Z is down
*/

size_t bs::ThreeDTI_AudioRenderer::CreateSoundMaker(const char* wavFileName, const bool loop, const bool spatialize, const bool anechoicEnabled, const bool distanceBasedAttenuationAnechoic, const bool reverbEnabled, const bool distanceBasedAttenuationReverb, const bool highQualitySimulation, const bool atmosphericFiltering, const bool nearFieldEffects)
{
	const auto hash = hasher_(wavFileName);

	if (assets_.find(hash) == assets_.end()) // Sound not loaded.
	{
		assets_.emplace(hash, bs::LoadWav(wavFileName, 1, (uint32_t)sampleRate));
	}

	sounds_.emplace_back(ThreeDTI_SoundMaker(assets_[hash], core_, loop, spatialize, bufferSize, anechoicEnabled, distanceBasedAttenuationAnechoic, reverbEnabled, distanceBasedAttenuationReverb, highQualitySimulation, atmosphericFiltering, nearFieldEffects));

	return sounds_.size() - 1;
}

bs::ThreeDTI_SoundMaker& bs::ThreeDTI_AudioRenderer::GetSound(const size_t soundId)
{
	assert(soundId < sounds_.size(), "Invalid soundId passed to GetSound()!");
	return sounds_[soundId];
}

void bs::ThreeDTI_AudioRenderer::StopAllSounds()
{
	for (size_t i = 0; i < sounds_.size(); i++)
	{
		sounds_[i].Stop();
	}
}

bool bs::ThreeDTI_AudioRenderer::AnyPlaying()
{
	for (auto& sound : sounds_)
	{
		if (sound.currentBegin_ < sound.soundData_.size() && !sound.paused_) return true;
	}
	return false;
}

bs::ThreeDTI_AudioRenderer::ThreeDTI_AudioRenderer(const char* hrtfPath, const char* brirPath, const size_t bufferSize, const size_t sampleRate, const bool ILDEnabled):
	bufferSize(bufferSize), sampleRate(sampleRate)
{
	// Init 3dti core.
	core_.SetAudioState({ (int)sampleRate, (int)bufferSize });
	core_.SetHRTFResamplingStep(HRTF_RESAMPLING_STEP);

	// Init 3dti listener.
	listener_ = core_.CreateListener();
	listener_->DisableCustomizedITD();
	UpdateRendererParams(ILDEnabled);
	bool unused; // Oleg@self: investigate
	if (!HRTF::CreateFromSofa(hrtfPath, listener_, unused)) assert(false, "Failed to load HRTF sofa file!");

	// Init 3dti environment.
	environment_ = core_.CreateEnvironment();
	environment_->SetReverberationOrder(TReverberationOrder::BIDIMENSIONAL); // Oleg@self: investigate, I think this has to do with nr or ambisonic channels?
	if (!BRIR::CreateFromSofa(brirPath, environment_)) assert(false, "Failed to load BRIR sofa file!");

	interlacedReverb_.resize(2 * bufferSize, 0.0f);
	currentlyProcessedSignal_.resize(2 * bufferSize, 0.0f);
}

void bs::ThreeDTI_AudioRenderer::GetRendererParams(bool& outILDEnabled) const
{
	outILDEnabled = std::fabsf(listener_->GetILDAttenutaion()) < 0.001f ? false : true;
}

void bs::ThreeDTI_AudioRenderer::UpdateRendererParams(const bool ILDEnabled)
{
	listener_->SetILDAttenutaion(ILDEnabled ? -6.0f : 0.0f); // Oleg@self: check if this is the right values.
}

void bs::ThreeDTI_AudioRenderer::ProcessAudio(std::vector<float>& interleavedStereoOut)
{
	EASY_FUNCTION("ThreeDTI_AudioRenderer::ProcessAudio");

	// Process anechoic.
	for (auto& sound : sounds_)
	{
		std::fill(currentlyProcessedSignal_.begin(), currentlyProcessedSignal_.end(), 0.0f);
		sound.ProcessAudio_(currentlyProcessedSignal_);
		bs::SumSignals(interleavedStereoOut, currentlyProcessedSignal_);
	}

#ifdef BUILD_WITH_REVERB
	// Process reverb_.
	EASY_BLOCK("Reverb");
	environment_->ProcessVirtualAmbisonicReverb(reverb_.left, reverb_.right);
	EASY_END_BLOCK;
	auto result = GET_LAST_RESULT_STRUCT();

	if (reverb_.left.size()) // 3dti sets size to 0 if there's no reverb_ to process.
	{
		bs::Interlace(interlacedReverb_, reverb_.left, reverb_.right);
		bs::SumSignals(interleavedStereoOut, interlacedReverb_);
	}
#endif
	
}

void bs::ThreeDTI_AudioRenderer::MoveListener(const bs::Mat3x4& mat)
{
	const auto pos = mat.GetPosition();
	const auto quat = mat.GetQuaternion();
	auto t = listener_->GetListenerTransform();
	t.SetPosition({pos.x, pos.y, pos.z});
	// t.SetOrientation({quat.w, quat.i, quat.j, quat.k});
	listener_->SetListenerTransform(t);
}

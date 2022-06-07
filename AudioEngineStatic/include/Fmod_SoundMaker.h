#pragma once

#include <vector>

#include <portaudio.h>

namespace bs
{
	class Fmod_AudioRenderer;

	enum class ClipWrapMode // Oleg@self: move somewhere common for all audio renderer implementations
	{
		ONE_SHOT,
		LOOP
	};

	class Fmod_SoundMaker
	{
	public:
		Fmod_SoundMaker() = default;
		Fmod_SoundMaker(const Fmod_SoundMaker&) = delete; // Owns a resource it's responsible for, so no copying.
		Fmod_SoundMaker(Fmod_SoundMaker&&) = default; // There's no reason for moving this class around.
		Fmod_SoundMaker& operator=(const Fmod_SoundMaker&) = delete; // Owns a resource it's responsible for, so no copy assignement.
		Fmod_SoundMaker& operator=(Fmod_SoundMaker&&) = default; // There's no need for move assignment for this class.

		// Oleg@self: those really should be replaced with constructors / destructors.
		bool Init(PaStreamCallback* serviceAudioCallback, bs::Fmod_AudioRenderer* engine, const char* wavFileName, const ClipWrapMode wrapMode = ClipWrapMode::ONE_SHOT);
		void Shutdown();

		void SetPosition(const float globalX, const float globalY, const float globalZ);

		// Oleg@self: maybe make a singleton out of this class instead of passing references of it in every method?...
		void ProcessAudio(std::vector<float>& outBuff, Fmod_AudioRenderer& engine);
		void Reset(Fmod_AudioRenderer& engine);

		inline PaStream* const GetStream() const { return pStream_; };
		inline ClipWrapMode GetWrapMode() const { return wrapMode_; };
		inline PaError const GetError() const { return err_; };
		inline uint32_t const GetCurrentBegin() const { return currentBegin_; };
		inline uint32_t const GetCurrentEnd() const { return currentEnd_; };
		inline const std::vector<float> GetSoundData() const { return soundData_; }; // Oleg@self: is this method useful?...

	private:

		// portaudio stuff
		PaStream* pStream_ = nullptr; // portaudio stream to playback device
		PaError err_;

		// SoundMaker specific stuff
		std::vector<float> soundData_; // Very large, contains whole wav file.
		uint32_t currentBegin_ = 0;
		uint32_t currentEnd_ = 0;
		ClipWrapMode wrapMode_ = ClipWrapMode::ONE_SHOT;
	};
}
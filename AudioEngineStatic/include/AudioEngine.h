#pragma once

#include <portaudio.h>
#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>

#include <SoundMaker.h>

namespace Binaural
{
	class CCore;
}

namespace bs
{
	using Listener = std::shared_ptr<Binaural::CListener>;
	using Environment = std::shared_ptr<Binaural::CEnvironment>;
	using SoundMakerId = size_t;
	constexpr const size_t INVALID_ID = (size_t)-1;

	class AudioEngine
	{
	public:
		bool Init(const char* hrtfFileName, const char* brirFileName, size_t BUFFER_SIZE = 2048, size_t SAMPLE_RATE = 441000);
		void Run();
		void Shutdown();

		SoundMakerId CreateSoundMaker(const char* wavFileName);
		void MoveSoundMaker(SoundMakerId id, float globalX, float globalY, float globalZ);

		Binaural::CCore& GetCore();

	private:
		static int ServiceAudio_
		(
			const void* unused, void* outputBuffer,
			unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void* userData
		);

		// World and it's objects.
		Environment environment_;
		Listener listener_;
		std::vector<SoundMaker> sounds_;

		// Internal stuff.
		Binaural::CCore core_;

	public:
		static const size_t BUFFER_SIZE = 2048; // Oleg@self: make this configurable
		static const size_t SAMPLE_RATE = 44100;
		static const size_t HRTF_RESAMPLING_ANGLE = 15;
	};
}
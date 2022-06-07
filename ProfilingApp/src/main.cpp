#include <iostream>
#include <array>
#include <cassert>

#include <SDL.h>
#undef main

#include <SteamAudio_AudioRenderer.h>

// ThreeDTI app implementation
// #include <ThreeDTI_AudioRenderer.h>
/*static void Update(bs::ThreeDTI_AudioRenderer& audioRenderer, bs::SoundMakerId soundId)
{
	static size_t currentPosition = 0;
	constexpr const size_t NR_OF_POSITIONS = 6;
	constexpr const std::array<std::array<float, 3>, NR_OF_POSITIONS> positions =
	{{
		{1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},

		{0.0f, 1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},

		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, -1.0f}
	}};

	if (++currentPosition >= NR_OF_POSITIONS) currentPosition = 0;

	audioRenderer.MoveSoundMaker(soundId, positions[currentPosition][0], positions[currentPosition][1], positions[currentPosition][2]);
	// audioRenderer.ResetSoundMaker(soundId);
}

static int RunProgram()
{
	{// Scoped sucess check.
		auto result = SDL_Init(SDL_INIT_EVERYTHING);
		assert(result == 0, "SDL failed to initialize!");
	}

	auto* window = SDL_CreateWindow("ProfilingApp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 720, 0);
	assert(window, "SDL failed to create a window!");

	bs::ThreeDTI_AudioRenderer audioRenderer;
	{// Scoped success chack.
		auto initSuccess = audioRenderer.Init("../resources/HRTF/SOFA/hrtf.sofa", "../resources/BRIR/SOFA/brir.sofa", 2048);
		assert(initSuccess, "Failed to initialize audio renderer!");
	}
	auto sound = audioRenderer.CreateSoundMaker("../resources/AudioSamples/brownNoise_44100Hz_32f_5sec.wav", bs::ClipWrapMode::LOOP);
	assert(sound != bs::INVALID_ID, "Failed to load wav file!");

	bool shutdown = false;
	while (!shutdown)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
				{
					shutdown = true;

				}break;
				
				case SDL_KEYDOWN:
				{
					switch (e.key.keysym.scancode)
					{
						case SDL_SCANCODE_ESCAPE:
						{
							shutdown = true;
						}break;
						
						case SDL_SCANCODE_SPACE:
						{
							Update(audioRenderer, sound); // Update audio renderer
						}break;
					default:break;}
				}break;
			default:break;}
		}
	}
	audioRenderer.Shutdown();
	SDL_Quit();

	return 0;
}*/

static int TestSteamAudio()
{
	bs::SteamAudio_AudioRenderer renderer;
	{
		auto result = renderer.Init();
		assert(result, "SteamAudio_AudioRenderer failed to initialize!");
	}
	auto sound = renderer.CreateSoundMaker("../resources/AudioSamples/brownNoise_44100Hz_32f_5sec.wav", bs::ClipWrapMode::LOOP);
	assert(sound != bs::INVALID_ID, "Failed to create a sound maker!");

	std::cin.ignore();

	return 0;
}

// SteamAudio app implementation
static void Update(bs::SteamAudio_AudioRenderer& audioRenderer, bs::SoundMakerId soundId)
{
	static size_t currentPosition = 0;
	constexpr const size_t NR_OF_POSITIONS = 6;
	constexpr const std::array<std::array<float, 3>, NR_OF_POSITIONS> positions =
	{{
		{1.0f, 0.0f, 0.0f}, /* Oleg@self: move all this to a json file or something */
		{-1.0f, 0.0f, 0.0f},

		{0.0f, 1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},

		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, -1.0f}
	}};

	if (++currentPosition >= NR_OF_POSITIONS) currentPosition = 0;

	audioRenderer.MoveSoundMaker(soundId, positions[currentPosition][0], positions[currentPosition][1], positions[currentPosition][2]);
}

static int RunProgram()
{
	{// Scoped sucess check.
		auto result = SDL_Init(SDL_INIT_EVERYTHING);
		assert(result == 0, "SDL failed to initialize!");
	}

	auto* window = SDL_CreateWindow("ProfilingApp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 720, 0);
	assert(window, "SDL failed to create a window!");

	bs::SteamAudio_AudioRenderer audioRenderer;
	{// Scoped success chack.
		auto initSuccess = audioRenderer.Init(2048);
		assert(initSuccess, "Failed to initialize audio renderer!");
	}
	auto sound = audioRenderer.CreateSoundMaker("../resources/AudioSamples/brownNoise_44100Hz_32f_5sec.wav", bs::ClipWrapMode::LOOP);
	assert(sound != bs::INVALID_ID, "Failed to load wav file!");

	bool shutdown = false;
	while (!shutdown)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
				{
					shutdown = true;

				}break;

				case SDL_KEYDOWN:
				{
					switch (e.key.keysym.scancode)
					{
						case SDL_SCANCODE_ESCAPE:
						{
							shutdown = true;
						}break;

						case SDL_SCANCODE_SPACE:
						{
							Update(audioRenderer, sound); // Update audio renderer
						}break;
					default:break;}
				}break;
			default:break;}
		}
	}
	audioRenderer.Shutdown();
	SDL_Quit();

	return 0;
}

int main()
{
	return RunProgram();
}
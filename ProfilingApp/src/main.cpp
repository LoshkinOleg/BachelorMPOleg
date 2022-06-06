#include <iostream>

#include <SDL.h>
#undef main

#include <ThreeDTI_AudioRenderer.h>

int TestThreeDTI()
{
	constexpr const float pi = 3.14f;
	constexpr const float thirdOfTwoPi = (2.0f * pi) / 3.0f;
	const float firstPos[3] = {std::sinf(thirdOfTwoPi), std::cosf(thirdOfTwoPi) , 0.0f};
	const float secondPos[3] = {std::sinf(thirdOfTwoPi * 2.0f), std::cosf(thirdOfTwoPi * 2.0f) , 0.0f};
	const float thirdPos[3] = {std::sinf(thirdOfTwoPi * 3.0f), std::cosf(thirdOfTwoPi * 3.0f) , 0.0f};

	bs::ThreeDTI_AudioRenderer e;
	e.Init("../resources/HRTF/SOFA/hrtf.sofa", "../resources/BRIR/SOFA/brir.sofa", 2048);
	auto sound = e.CreateSoundMaker("../resources/AudioSamples/brownNoise_44100Hz_32f_5sec.wav", bs::ClipWrapMode::LOOP);
	if (sound != bs::INVALID_ID)
	{
		float theta = 0.0f;
		while (true)
		{
			e.MoveSoundMaker(sound, std::sinf(theta), std::cosf(theta), 0.0f);
			theta += 0.05f;
			Pa_Sleep(40);
		}

		/*e.MoveSoundMaker(sound, firstPos[0], firstPos[1], firstPos[2]);
		std::cin.ignore();

		e.ResetSoundMaker(sound);
		e.MoveSoundMaker(sound, secondPos[0], secondPos[1], secondPos[2]);
		std::cin.ignore();

		e.ResetSoundMaker(sound);
		e.MoveSoundMaker(sound, thirdPos[0], thirdPos[1], thirdPos[2]);
		std::cin.ignore();*/
	}
	e.Shutdown();

	return (sound == bs::INVALID_ID);
}

int TestKeyboardInputs()
{
	auto result = SDL_Init(SDL_INIT_EVERYTHING);
	assert(result == 0, "SDL failed to initialize!");

	auto* window = SDL_CreateWindow("ProfilingApp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 720, 0);
	assert(window, "SDL failed to create a window!");

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
					default:break;}
				}break;
			default:break;}
		}
	}
	SDL_Quit();

	return 0;
}

int main()
{
	return TestKeyboardInputs();
}
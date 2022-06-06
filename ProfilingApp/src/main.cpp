#include <ThreeDTI_AudioRenderer.h>

int main()
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
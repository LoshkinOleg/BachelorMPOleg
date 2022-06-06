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
	auto sound = e.CreateSoundMaker("../resources/AudioSamples/pure441_32float_1sec.wav", bs::ClipWrapMode::LOOP);
	if (sound != bs::INVALID_ID)
	{
		// e.MoveSoundMaker(sound, firstPos[0], firstPos[1], firstPos[2]);
		Pa_Sleep(2000);
	}
	e.Shutdown();

	return (sound == bs::INVALID_ID);
}
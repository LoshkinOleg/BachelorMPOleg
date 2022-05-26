#include <ThreeDTI_AudioRenderer.h>

int main()
{
	bs::ThreeDTI_AudioRenderer e;
	e.Init("../resources/HRTF/SOFA/hrtf.sofa", "../resources/BRIR/SOFA/brir.sofa");
	auto sound = e.CreateSoundMaker("../resources/AudioSamples/pure440.wav");
	if (sound != bs::INVALID_ID)
	{
		e.Run();
	}
	e.Shutdown();

	return (sound == bs::INVALID_ID);
}
#include <AudioEngine.h>

int main()
{
	bs::AudioEngine e;
	e.Init("hrtf.sofa", "brir.sofa");
	auto sound = e.CreateSoundMaker("pure440.wav");
	if (sound != bs::INVALID_ID)
	{
		e.Run();
	}
	e.Shutdown();

	return (sound == bs::INVALID_ID);
}
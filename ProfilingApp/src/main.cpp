#include "Application.h"

int main()
{
	auto app = bsExp::Application(HRTF_PATH, BRIR_PATH, WAV_PATH, 1337);
	return app.RunProgram();
}
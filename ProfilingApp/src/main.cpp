#include "Application.h"

constexpr const char* HRTF_PATH = "../resources/HRTF/SOFA/3DTI_HRTF_IRC1008_128s_44100Hz.sofa";
constexpr const char* BRIR_PATH = "../resources/BRIR/SOFA/3DTI_BRIR_medium_44100Hz.sofa";
constexpr const char* WAV_PATH = "../resources/AudioSamples/AnechoicSpeech44100.wav";
constexpr const size_t BUFFER_SIZE = 1024;
constexpr const size_t SAMPLE_RATE = 44100;

int main()
{
	auto app = bsExp::Application(HRTF_PATH, BRIR_PATH, WAV_PATH, bs::ClipWrapMode::LOOP, BUFFER_SIZE, SAMPLE_RATE);
	return app.RunProgram();
}
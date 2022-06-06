#include "UtilityFunctions.h"

#include <cassert>
#include <algorithm>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

std::vector<float> bs::LoadWavOLD(const char* path)
{
	// Oleg@self: replace this with c++ implementation.

	std::vector<float> returnVal;

	// Oleg@self: investigate reading of wav.
	struct WavHeader								 // Local declaration of wav header struct type (more info in http://soundfile.sapp.org/doc/WaveFormat/)
	{												 // We only need the number of samples, so the rest will be unused assuming file is mono, 16-bit depth and 44.1kHz sampling rate
		char		  fill[40];
		uint32_t	bytesCount;
	} wavHeader;

	FILE* wavFile = fopen(path, "rb");
	assert(wavFile, "Failed to read wav file!");

	auto result = fread(&wavHeader, sizeof(wavHeader), 1, wavFile); // Reading of the 44 bytes of header to get the number of samples of the file
	assert(result == 1, "Encountered an error reading from wav file!");
	result = fseek(wavFile, sizeof(wavHeader), SEEK_SET); // Moving of the file pointer to the start of the audio samples
	assert(!result, "Failed to seek to start of audio samples!");

	// Oleg@self: check invariants: wav file, 16-bit depth for samples
	unsigned int samplesCount = wavHeader.bytesCount / 2; // Getting number of samples by dividing number of bytes by 2 because we are reading 16-bit samples
	// Oleg@self: point of this?
	int16_t* sample; sample = new int16_t[samplesCount]; // Declaration and initialization of 16-bit signed integer pointer
	memset(sample, 0, sizeof(int16_t) * samplesCount); // Setting its size

	uint8_t* byteSample; byteSample = new uint8_t[2 * samplesCount]; // Declaration and initialization of 8-bit unsigned integer pointer
	memset(byteSample, 0, sizeof(uint8_t) * 2 * samplesCount); // Setting its size

	fread(byteSample, 1, 2 * samplesCount, wavFile); // Reading the whole file byte per byte, needed for endian-independent wav parsing

	for (int i = 0; i < samplesCount; i++)
		sample[i] = int16_t(byteSample[2 * i] | byteSample[2 * i + 1] << 8); // Conversion from two 8-bit unsigned integer to a 16-bit signed integer

	returnVal.reserve(samplesCount); // Reserving memory for samples vector

	for (int i = 0; i < samplesCount; i++)
		returnVal.push_back((float)sample[i] / (float)INT16_MAX); // Converting samples to float to push them in samples vector

	result = fclose(wavFile); // Oleg@self: handle exceptions?
	assert(!result, "Failed to close file!");

	return returnVal;
}

std::vector<float> bs::LoadWav(const char* path, const uint32_t desiredNrOfChanels, const uint32_t desiredSampleRate)
{
	unsigned int fileChannels;
	unsigned int fileSampleRate;
	drwav_uint64 totalPCMFrameCount;
	
	float* pSampleData = drwav_open_file_and_read_pcm_frames_f32("my_song.wav", &fileChannels, &fileSampleRate, &totalPCMFrameCount, NULL);

	assert(fileChannels == desiredNrOfChanels, "Error reading wav file: desired vs actual nrOfChannels mismatch!");
	assert(fileSampleRate == desiredSampleRate, "Error reading wav file: desired vs actual sampleRate mismatch!");
	assert(pSampleData != NULL, "Error reading wav file: couldn't read wav data!");

	std::vector<float> returnVal((size_t)fileSampleRate * (size_t)fileChannels);
	memcpy(&returnVal[0], pSampleData, sizeof(float) * (size_t)fileSampleRate * (size_t)fileChannels);
	assert(memcmp(returnVal.begin()._Ptr, pSampleData, sizeof(float) * (size_t)fileSampleRate * (size_t)fileChannels), "Error reading wav file: there's some mistake in the wav data copying code!");

	drwav_free(pSampleData, NULL);

	return returnVal;
}

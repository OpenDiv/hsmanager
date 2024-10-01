#ifndef SOUNDUTILS_H
#define SOUNDUTILS_H

#include <cstdint>
#include <fstream>

#include <cstring>
#include <cmath>
#include <filesystem>
#include <vector>
#include <windows.h>
#include <QDebug>

#include "wavHeader.h"

namespace fs = std::filesystem;

class soundUtils
{
public:
    soundUtils();

    static bool getIsTfFormat(fs::path filePath);
    static bool getIsWav(fs::path filePath);

    static bool setBitsPerSample(fs::path filePath, int requiedBitsPerSample);
    static bool setNumChannelsToMono(fs::path filePath);
    static bool setSampleRate(fs::path filePath, uint32_t newSampleRate);

    static bool setVolume(fs::path filePath, double newVolumeParam);
private:
    static bool readSamples(std::ifstream &readingStream, std::vector<uint8_t> &samples);
    static void convertSamplesInto16Bit(std::ifstream &writingStream, std::vector<uint16_t> &samplesNew, size_t sampleCount, int bitsPerSample);
};

#endif // SOUNDUTILS_H

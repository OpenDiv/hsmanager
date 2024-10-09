#include "soundutils.h"

#include <unistd.h>

soundUtils::soundUtils() {}

bool soundUtils::readSamples(std::ifstream &readingStream, std::vector<uint8_t> &samples)
{
    qDebug()<<"step #reading samples";
    uint32_t subchunk2Size;
    readingStream.seekg(40);
    readingStream.read(reinterpret_cast<char*>(&subchunk2Size), sizeof(uint32_t));
    // size = bitsPerSample/8 * subchunk2Size

    samples.clear();
    samples.shrink_to_fit();
    samples.resize(subchunk2Size);

    readingStream.read(reinterpret_cast<char*>(samples.data()), subchunk2Size);

    return true;
}
// fix to 24
bool soundUtils::setBitsPerSample(fs::path filePath, int requiedBitsPerSample)
{
    qDebug()<<"step #bitsPerSample";
    std::ifstream wavFileReadingStream(filePath, std::ios::binary);
    if (!wavFileReadingStream.is_open())
        { return false; }

    wavHeader wavHead;
    wavFileReadingStream.read(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader));

    if (wavHead.bitsPerSample == requiedBitsPerSample)
        { return true; }

    std::vector<unsigned int> samplesNew;
    std::vector<uint8_t> samplesOld;

    readSamples(wavFileReadingStream, samplesOld);

    wavFileReadingStream.close();

    std::ofstream wavFileWritingStream(filePath, std::ios::binary | std::ios::trunc);
    if(!wavFileWritingStream.is_open())
        { return false; }

    int oldBitsPerSample = wavHead.bitsPerSample;
    int oldSubchunk2Size = wavHead.subchunk2Size;

    wavHead.bitsPerSample = requiedBitsPerSample;
    wavHead.byteRate = wavHead.sampleRate * wavHead.numChannels * (wavHead.bitsPerSample / 8);
    wavHead.blockAlign = wavHead.numChannels * (wavHead.bitsPerSample / 8);
    wavHead.subchunk2Size = (samplesOld.size() / (oldBitsPerSample / 8)) * wavHead.numChannels * (wavHead.bitsPerSample / 8);
    wavHead.chunkSize = 36 + wavHead.subchunk2Size;

    wavFileWritingStream.write(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader));

    if(requiedBitsPerSample != 24)
    {
        for (int i = 0; i < oldSubchunk2Size; i += (oldBitsPerSample / 8))
        {
            unsigned fixedSample = 0;
            for (int k = i + (oldBitsPerSample / 8) - 1; k >= i; k--)
            {
                fixedSample = fixedSample << 8 | samplesOld[k];
            }
            samplesNew.push_back(fixedSample);
        }

        unsigned int oldSampleMaxValue = (pow(2, oldBitsPerSample) - 1);
        unsigned int newSampleMaxValue = (pow(2, requiedBitsPerSample) - 1);
        float normalizedSample;

        switch (requiedBitsPerSample)
        {
            case 8:
                uint8_t fixedSample8Bit;
                for(auto newSample : samplesNew)
                    {
                        normalizedSample = static_cast<float>(newSample) / static_cast<float>(oldSampleMaxValue);
                        fixedSample8Bit = normalizedSample * newSampleMaxValue;
                        wavFileWritingStream.write(reinterpret_cast<char*>(&fixedSample8Bit), sizeof(uint8_t));
                    }
                return true;
            case 16:
                uint16_t fixedSample16Bit;
                for(auto newSample : samplesNew)
                {
                    normalizedSample = static_cast<float>(newSample) / static_cast<float>(oldSampleMaxValue);
                    fixedSample16Bit = normalizedSample * newSampleMaxValue;
                    wavFileWritingStream.write(reinterpret_cast<char*>(&fixedSample16Bit), sizeof(uint16_t));
                }
                return true;
            case 32:
                uint32_t fixedSample32Bit;
                for(auto newSample : samplesNew)
                {
                    normalizedSample = static_cast<float>(newSample) / static_cast<float>(oldSampleMaxValue);
                    fixedSample32Bit = normalizedSample * newSampleMaxValue;
                    wavFileWritingStream.write(reinterpret_cast<char*>(&fixedSample32Bit), sizeof(uint32_t));
                }
                return true;
            default:
                return false;
        }
    }
    else
        { return false; }
}

bool soundUtils::setNumChannelsToMono(fs::path filePath)
{
    std::ifstream wavFileReadingStream(filePath, std::ios::binary);
    if (!wavFileReadingStream.is_open())
        return false;

    wavHeader wavHead;
    wavFileReadingStream.read(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader));

    if (wavHead.numChannels == 1) { return true; }

    std::vector<uint8_t> samples;
    readSamples(wavFileReadingStream, samples);
    std::vector<int64_t> mergedSamples;

    wavFileReadingStream.close();

    int bytesPerSample = wavHead.bitsPerSample / 8;

    for (size_t i = 0; i < samples.size(); i += bytesPerSample * wavHead.numChannels)
    {
        int64_t monoSample = 0;

        for (size_t channel = 0; channel < wavHead.numChannels; channel++)
        {
            int64_t restoredSample = 0;
            for (int byte = bytesPerSample - 1; byte >= 0; byte--)
            {
                restoredSample = restoredSample << 8 | samples[i + byte + (channel * bytesPerSample)];
            }

            if (wavHead.bitsPerSample == 16) {
                restoredSample = static_cast<int16_t>(restoredSample);
            } else if (wavHead.bitsPerSample == 32) {
                restoredSample = static_cast<int32_t>(restoredSample);
            }

            monoSample += restoredSample;
        }

        monoSample /= wavHead.numChannels;

        if (wavHead.bitsPerSample == 16)
        {
            monoSample = std::clamp(monoSample, (int64_t)std::numeric_limits<int16_t>::min(), (int64_t)std::numeric_limits<int16_t>::max());
        } else if (wavHead.bitsPerSample == 32)
        {
            monoSample = std::clamp(monoSample, (int64_t)std::numeric_limits<int32_t>::min(), (int64_t)std::numeric_limits<int32_t>::max());
        }

        mergedSamples.push_back(monoSample);
    }

    wavHead.numChannels = 1;
    wavHead.byteRate = wavHead.sampleRate * bytesPerSample;
    wavHead.blockAlign = bytesPerSample;
    wavHead.subchunk2Size = mergedSamples.size() * bytesPerSample;
    wavHead.chunkSize = 36 + wavHead.subchunk2Size;

    std::ofstream wavFileWritingStream(filePath, std::ios::binary | std::ios::trunc);
    if (!wavFileWritingStream.is_open())
        return false;

    wavFileWritingStream.write(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader));

    switch (wavHead.bitsPerSample)
    {
    case 8:
        for (auto sample : mergedSamples)
        {
            uint8_t sample8Bit = static_cast<uint8_t>(sample);
            wavFileWritingStream.write(reinterpret_cast<char*>(&sample8Bit), sizeof(uint8_t));
        }
        break;
    case 16:
        for (auto sample : mergedSamples)
        {
            int16_t sample16Bit = static_cast<int16_t>(sample);
            wavFileWritingStream.write(reinterpret_cast<char*>(&sample16Bit), sizeof(int16_t));
        }
        break;
    case 32:
        for (auto sample : mergedSamples)
        {
            int32_t sample32Bit = static_cast<int32_t>(sample);
            wavFileWritingStream.write(reinterpret_cast<char*>(&sample32Bit), sizeof(int32_t));
        }
        break;
    default:
        return false;
    }

    return true;
}


bool soundUtils::setSampleRate(fs::path filePath, uint32_t newSampleRate)
{
    std::ifstream wavFileReadingStream(filePath, std::ios::binary);
    if(!wavFileReadingStream.is_open())
        return false;

    wavHeader wavHead;
    wavFileReadingStream.read(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader));

    wavFileReadingStream.close();

    std::ofstream wavFileWritingStream(filePath, std::ios::binary | std::ios::in | std::ios::out);
    if(!wavFileWritingStream.is_open())
        return false;

    wavHead.sampleRate = newSampleRate;
    wavHead.byteRate = newSampleRate * wavHead.numChannels * wavHead.bitsPerSample / 8;

    wavFileWritingStream.write(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader));

    return true;
}

bool soundUtils::getIsTfFormat(fs::path filePath)
{
    std::ifstream wavFileReadingStream(filePath, std::ios::binary);
    if(!wavFileReadingStream.is_open())
        { return false; }

    uint16_t audioFormat;   // 20/2
    uint16_t numChannels;   // 22/2
    uint32_t sampleRate;    // 24/4
    uint16_t bitsPerSample; // 34/2

    wavFileReadingStream.seekg(20);
    wavFileReadingStream.read(reinterpret_cast<char*>(&audioFormat), sizeof(uint16_t));
    if (audioFormat != 1) { return false; }

    wavFileReadingStream.seekg(22);
    wavFileReadingStream.read(reinterpret_cast<char*>(&numChannels), sizeof(uint16_t));
    if (numChannels != 1) { return false; }

    wavFileReadingStream.seekg(24);
    wavFileReadingStream.read(reinterpret_cast<char*>(&sampleRate), sizeof(uint32_t));
    if (sampleRate != 44100 && sampleRate != 22050 && sampleRate != 11025) { return false; }

    wavFileReadingStream.seekg(34);
    wavFileReadingStream.read(reinterpret_cast<char*>(&bitsPerSample), sizeof(uint16_t));
    if (bitsPerSample != 16) { return false; }

    return true;
}

bool soundUtils::getIsWav(fs::path filePath)
{
    std::ifstream wavFileReadingStream(filePath, std::ios::binary);
    if(!wavFileReadingStream.is_open())
        { return false; }
        char header[12];
        wavFileReadingStream.read(header, sizeof(header));
        wavFileReadingStream.close();
        return (std::memcmp(header, "RIFF", 4) == 0 && std::memcmp(header+8, "WAVE", 4) == 0);
}

bool soundUtils::setVolume(fs::path filePath, double newVolumeParam)
{
    std::ifstream wavFileReadingStream(filePath, std::ios::binary);
    if (!wavFileReadingStream.is_open())
    {
        return false;
    }

    wavHeader wavHead;
    wavFileReadingStream.read(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader));

    std::vector<uint8_t> samples;
    readSamples(wavFileReadingStream, samples);

    wavFileReadingStream.close();

    int32_t maxSampleValue, minSampleValue;

    if (wavHead.bitsPerSample == 8)
    {
        maxSampleValue = (pow(2, wavHead.bitsPerSample))-1;
        minSampleValue = 0;
    } else if (wavHead.bitsPerSample == 16 || wavHead.bitsPerSample == 32)
    {
        maxSampleValue = (pow(2, wavHead.bitsPerSample) - 1);
        minSampleValue = -maxSampleValue - 1;
    } else { return false; }

    for (int i = 0; i < wavHead.subchunk2Size; i += (wavHead.bitsPerSample / 8))
    {
        int32_t sampleValue = 0;

        if (wavHead.bitsPerSample == 8)
        {
            sampleValue = samples[i];
        }
        else if (wavHead.bitsPerSample == 16)
        {
            int16_t* sample16 = reinterpret_cast<int16_t*>(&samples[i]);
            sampleValue = *sample16;
        }
        else if (wavHead.bitsPerSample == 32)
        {
            int32_t* sample32 = reinterpret_cast<int32_t*>(&samples[i]);
            sampleValue = *sample32;
        }

        sampleValue = static_cast<int32_t>(sampleValue * newVolumeParam);
        sampleValue = std::clamp(sampleValue, minSampleValue, maxSampleValue);

        if (wavHead.bitsPerSample == 8)
        {
            samples[i] = static_cast<uint8_t>(sampleValue);
        }
        else if (wavHead.bitsPerSample == 16)
        {
            int16_t* sample16 = reinterpret_cast<int16_t*>(&samples[i]);
            *sample16 = static_cast<int16_t>(sampleValue);
        }
        else if (wavHead.bitsPerSample == 32)
        {
            int32_t* sample32 = reinterpret_cast<int32_t*>(&samples[i]);
            *sample32 = static_cast<int32_t>(sampleValue);
        }
    }

    std::ofstream wavFileWritingStream("C:\\Users\\Sema\\Desktop\\volumeTester.wav", std::ios::binary);
    if (!wavFileWritingStream.is_open())
    {
        return false;
    }

    wavFileWritingStream.write(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader));
    wavFileWritingStream.write(reinterpret_cast<char*>(samples.data()), samples.size());

    return true;
}



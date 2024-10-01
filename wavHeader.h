#ifndef WAVHEADER_H
#define WAVHEADER_H
#include <cstdint>

struct wavHeader{
    uint32_t    chunkID         = 0; // [0]  "RIFF"
    uint32_t    chunkSize       = 0; // [4]  36 + subchunk2Size
    uint32_t    format          = 0; // [8]  "WAVE"
    uint32_t    subchunk1ID     = 0; // [12] "fmt "
    uint32_t    subchunk1Size   = 0; // [16] 16 for PCM
    uint16_t    audioFormat     = 0; // [20] PCM = 1 (i.e. Linear quantization)
    uint16_t    numChannels     = 0; // [22] Mono = 1, Stereo = 2, etc.
    uint32_t    sampleRate      = 0; // [24] 8000, 44100, etc.
    uint32_t    byteRate        = 0; // [28] == sampleRate * numChannels * bitsPerSample / 8
    uint16_t    blockAlign      = 0; // [32] == numChannels * bitsPerSample / 8
    uint16_t    bitsPerSample   = 0; // [34] 8 bits = 8, 16 bits = 16, etc.
    uint32_t     subchunk2ID    = 0; // [36] "data"
    uint32_t    subchunk2Size   = 0; // [40] == numSamples * numChannels * bitsPerSample / 8
};

#endif // WAVHEADER_H

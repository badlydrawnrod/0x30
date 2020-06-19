#pragma once

#include <cstdint>
#include <cstdio>

struct WaveStream
{
    FILE* fp;               // The underlying file pointer.
    long startOfData;       // The start of the data in the underlying file.
    uint32_t fileSize;      // The size of the underlying file according to the RIFF header.
    uint32_t dataSize;      // Length of data, in bytes.
    uint16_t channels;      // Number of channels.
    uint32_t sampleRate;    // Samples per second.
    uint16_t blockSize;     // Block (frame) size (bytes).
    uint16_t bitsPerSample; // Bits per sample (eg, 8, 16).
};

int OpenWaveStream(const char* filename, WaveStream* stream);
int CloseWaveStream(WaveStream* stream);
int ReadWaveStream(WaveStream* stream, void* buf, size_t size, size_t count, size_t* itemsRead);

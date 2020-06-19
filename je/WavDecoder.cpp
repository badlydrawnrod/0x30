// My C .wav loader from je_poc_invaders and hacked into place here.
// Description of .wav format comes from: http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

#include "WavDecoder.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#define WAVE_FORMAT_PCM 1

typedef char FourCC[4];

static int32_t Int32FromLE(const uint8_t* buf)
{
    const auto i = static_cast<int32_t>(buf[0] | (buf[1] << 8u) | (buf[2] << 16u) | (buf[3] << 24u));
    return i;
}

static uint32_t Uint32FromLE(const uint8_t* buf)
{
    const auto i = static_cast<uint32_t>(buf[0] | (buf[1] << 8u) | (buf[2] << 16u) | (buf[3] << 24u));
    return i;
}

static uint16_t Uint16FromLE(const uint8_t* buf)
{
    const auto i = static_cast<uint16_t>(buf[0] | (buf[1] << 8u));
    return i;
}

static bool Uint16FromLE(FILE* fp, uint16_t* out)
{
    uint8_t buf2[2];
    size_t itemsRead = fread(buf2, sizeof(buf2), 1, fp);

    if (itemsRead != 1)
    {
        return false;
    }
    uint16_t i = Uint16FromLE(buf2);
    *out = i;

    return true;
}

static bool Int32FromLE(FILE* fp, int32_t* out)
{
    uint8_t buf4[4];
    size_t itemsRead = fread(buf4, sizeof(buf4), 1, fp);

    if (itemsRead != 1)
    {
        return false;
    }
    int32_t i = Int32FromLE(buf4);
    *out = i;

    return true;
}

static bool Uint32FromLE(FILE* fp, uint32_t* out)
{
    uint8_t buf4[4];
    size_t itemsRead = fread(buf4, sizeof(buf4), 1, fp);

    if (itemsRead != 1)
    {
        return false;
    }
    uint32_t i = Uint32FromLE(buf4);
    *out = i;

    return true;
}

static int FindChunk(FILE* fp, FourCC id)
{
    int rc = 0;
    FourCC buf;

    while (rc == 0)// TODO: give this an upper bound.
    {
        size_t itemsRead = fread(&buf, sizeof(buf), 1, fp);
        if (itemsRead != 1)
        {
            fprintf(stderr, "Failed to read id from stream.\n");
            rc = 1;
            continue;
        }

        // Is it the chunk that we're looking for?
        if (strncmp(id, buf, sizeof(buf)) == 0)
        {
            break;
        }

        // No, so skip to the next chunk.

        // Find out how far we need to skip.
        int32_t skip;
        if (!Int32FromLE(fp, &skip))
        {
            fprintf(stderr, "Failed to read skip from stream.\n");
            rc = 1;
            continue;
        }

        // Skip.
        // TODO: what if some miserable toad asks us to skip backwards?
        int result = fseek(fp, (long)skip, SEEK_CUR);
        if (result != 0)
        {
            fprintf(stderr, "Failed to seek in stream.\n");
            rc = 1;
            continue;
        }
    }

    return rc;
}

static int ReadRiffHeader(WaveStream* stream)
{
    int rc = 0;

    // Check that the 'RIFF' chunk id is present.
    FourCC riffId;
    size_t itemsRead = fread(&riffId, sizeof(riffId), 1, stream->fp);
    if (itemsRead != 1 || strncmp("RIFF", riffId, 4) != 0)
    {
        fprintf(stderr, "No RIFF header found - not a wave file.\n");
        rc = 1;
    }

    // Read the size of the entire RIFF chunk.
    uint32_t size = 0;
    if (rc == 0)
    {
        if (!Uint32FromLE(stream->fp, &size))
        {
            fprintf(stderr, "Failed to read size from stream.\n");
            rc = 1;
        }
    }

    // Check that the 'WAVE' id is present.
    if (rc == 0)
    {
        FourCC waveId;
        itemsRead = fread(&waveId, sizeof(waveId), 1, stream->fp);
        if (itemsRead != 1 || strncmp("WAVE", waveId, 4) != 0)
        {
            fprintf(stderr, "No WAVE id found - not a wave file.\n");
            rc = 1;
        }
    }

    if (rc == 0)
    {
        stream->fileSize = sizeof(riffId) + sizeof(stream->fileSize) + size;
    }

    return rc;
}

static int ReadFmtChunk(WaveStream* stream)
{
    static char chunkId[] = "fmt ";

    // Look for the 'fmt ' chunk.
    int rc = FindChunk(stream->fp, chunkId);

    // Read the size.
    uint32_t size = 0;
    if (rc == 0 && !Uint32FromLE(stream->fp, &size))
    {
        fprintf(stderr, "Failed to read fmt size from stream.\n");
        rc = 1;
    }

    // TODO: guard against backwards.
    long nextChunk = ftell(stream->fp) + (long)size;

    // Read the format code.
    uint16_t formatCode;
    if (rc == 0 && !Uint16FromLE(stream->fp, &formatCode))
    {
        fprintf(stderr, "Failed to read format code.\n");
        rc = 1;
    }

    if (rc == 0 && formatCode != WAVE_FORMAT_PCM)
    {
        fprintf(stderr, "Not PCM. %hd\n", formatCode);
    }

    // Read the number of channels.
    if (rc == 0 && !Uint16FromLE(stream->fp, &stream->channels))
    {
        fprintf(stderr, "Failed to read channels.\n");
        rc = 1;
    }

    // Read the sample rate (samples per second).
    if (rc == 0 && !Uint32FromLE(stream->fp, &stream->sampleRate))
    {
        fprintf(stderr, "Failed to read sample rate.\n");
        rc = 1;
    }

    // Read the average data rate (bytes per second).
    uint32_t dataRate;
    if (rc == 0 && !Uint32FromLE(stream->fp, &dataRate))
    {
        fprintf(stderr, "Failed to read data rate (average bytes per second).\n");
        rc = 1;
    }

    // Read the data block size in bytes.
    if (rc == 0 && !Uint16FromLE(stream->fp, &stream->blockSize))
    {
        fprintf(stderr, "Failed to read block size.\n");
        rc = 1;
    }

    // Read the bits per sample.
    if (rc == 0 && !Uint16FromLE(stream->fp, &stream->bitsPerSample))
    {
        fprintf(stderr, "Failed to read bits per sample.\n");
        rc = 1;
    }

    // Calculate the sample size.
    if (rc == 0)
    {
        uint32_t bytesPerSample = stream->bitsPerSample / 8u;
        if (bytesPerSample != 1 && bytesPerSample != 2)
        {
            fprintf(stderr, "Unexpected sample size: %u\n", bytesPerSample);
            rc = 1;
        }
    }

    // We're done with the 'fmt ' chunk (we don't care if it's an extended fmt chunk), so skip over it to the next one.
    int result = fseek(stream->fp, nextChunk, SEEK_SET);
    if (result != 0)
    {
        fprintf(stderr, "Failed to seek in stream.\n");
        rc = 1;
    }

    return rc;
}

static int ReadDataHeader(WaveStream* stream)
{
    static char dataChunkId[] = "data";

    // Look for the 'data' chunk.
    int rc = FindChunk(stream->fp, dataChunkId);

    // Read the data size.
    if (rc == 0 && !Uint32FromLE(stream->fp, &stream->dataSize))
    {
        fprintf(stderr, "Failed to read data size from stream.\n");
        rc = 1;
    }

    return rc;
}

int OpenWaveStream(const char* filename, WaveStream* stream)
{
    int rc = 0;

    if (stream == nullptr)
    {
        rc = 1;
    }

    if (rc == 0)
    {
        stream->fp = fopen(filename, "rb");
        if (stream->fp == nullptr)
        {
            fprintf(stderr, "Couldn't open %s.\n", filename);
            rc = 1;
        }
    }

    if (rc == 0)
    {
        rc = ReadRiffHeader(stream);
    }

    if (rc == 0)
    {
        rc = ReadFmtChunk(stream);
    }

    if (rc == 0)
    {
        rc = ReadDataHeader(stream);
    }

    if (rc == 0)
    {
        stream->startOfData = ftell(stream->fp);
    }

    if (rc != 0)
    {
        if (stream->fp)
        {
            fclose(stream->fp);
        }
    }

    return rc;
}

int CloseWaveStream(WaveStream* stream)
{
    int rc = 0;

    if (stream == nullptr || stream->fp == nullptr)
    {
        rc = 1;
    }

    if (rc == 0)
    {
        fclose(stream->fp);
    }

    return rc;
}

int ReadWaveStream(WaveStream* stream, void* buf, size_t size, size_t count, size_t* itemsRead)
{
    if (stream == nullptr || buf == nullptr || itemsRead == nullptr)
    {
        return 1;
    }

    *itemsRead = fread(buf, size, count, stream->fp);

    return 0;
}

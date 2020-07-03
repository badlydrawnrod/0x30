#include "WavLoader.h"

#include "Logger.h"
#include "WavDecoder.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <memory>
#include <string>

namespace je
{
    ALuint LoadWav(const std::string& filename)
    {
        ALuint buffer = 0;

        // Attempt to open the file as a .wav.
        WaveStream stream{};
        int rc = OpenWaveStream(filename.c_str(), &stream);
        if (rc != 0)
        {
            return buffer;
        }

        std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(stream.dataSize);

        // Read the stream data.
        size_t itemsRead = 0;
        rc = ReadWaveStream(&stream, data.get(), sizeof(uint8_t), stream.dataSize, &itemsRead);
        CloseWaveStream(&stream);

        if (rc != 0)
        {
            LOG("Failed to read .wav from " << filename);
            return buffer;
        }

        // Determine the OpenAL buffer format from the stream data.
        ALenum format = AL_FORMAT_MONO8;
        LOG("  Start of data: " << stream.startOfData);
        LOG("      File size: " << stream.fileSize);
        LOG("      Data size: " << stream.dataSize);
        LOG("       Channels: " << stream.channels);
        LOG("    Sample rate: " << stream.sampleRate);
        LOG("     Block size: " << stream.blockSize);
        LOG("Bits per sample: " << stream.bitsPerSample);
        if (stream.bitsPerSample == 8)
        {
            format = stream.channels == 1 ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
        }
        else if (stream.bitsPerSample == 16)
        {
            format = stream.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        }

        alGenBuffers(1, &buffer);

        auto numBytes = static_cast<ALsizei>(stream.dataSize);
        alBufferData(buffer, format, data.get(), numBytes, stream.sampleRate);

        return buffer;
    }
} // namespace je

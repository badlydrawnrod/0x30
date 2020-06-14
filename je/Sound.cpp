#include "Sound.h"

#include "Logger.h"

#include "stb_vorbis.c"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// My .wav loader from je_poc_invaders and hacked into place here.

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

// Description of .wav format comes from: http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

#define WAVE_FORMAT_PCM 1

typedef char fourcc[4];

static int32_t Int32FromLE(const uint8_t* buf)
{
    int32_t i = (int32_t) (buf[0] | (buf[1] << 8u) | (buf[2] << 16u) | (buf[3] << 24u));
    return i;
}

static uint32_t Uint32FromLE(const uint8_t* buf)
{
    uint32_t i = (uint32_t) (buf[0] | (buf[1] << 8u) | (buf[2] << 16u) | (buf[3] << 24u));
    return i;
}

static uint16_t Uint16FromLE(const uint8_t* buf)
{
    uint16_t i = (uint16_t) (buf[0] | (buf[1] << 8u));
    return i;
}

static bool Uint16FromLE(FILE* fp, uint16_t* out)
{
    uint8_t buf2[2];
    size_t items_read = fread(buf2, sizeof(buf2), 1, fp);

    if (items_read != 1)
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
    size_t items_read = fread(buf4, sizeof(buf4), 1, fp);

    if (items_read != 1)
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
    size_t items_read = fread(buf4, sizeof(buf4), 1, fp);

    if (items_read != 1)
    {
        return false;
    }
    uint32_t i = Uint32FromLE(buf4);
    *out = i;

    return true;
}

static int FindChunk(FILE* fp, fourcc id)
{
    int rc = 0;
    fourcc buf;

    while (rc == 0) // TODO: give this an upper bound.
    {
        size_t items_read = fread(&buf, sizeof(buf), 1, fp);
        if (items_read != 1)
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
        int result = fseek(fp, (long) skip, SEEK_CUR);
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

    size_t items_read = 0;

    // Check that the 'RIFF' chunk id is present.
    fourcc riff_id;
    items_read = fread(&riff_id, sizeof(riff_id), 1, stream->fp);
    if (items_read != 1 || strncmp("RIFF", riff_id, 4) != 0)
    {
        fprintf(stderr, "No RIFF header found - not a wave file.\n");
        rc = 1;
    }

    // Read the size of the entire RIFF chunk.
    uint32_t chunk_size = 0;
    if (rc == 0)
    {
        if (!Uint32FromLE(stream->fp, &chunk_size))
        {
            fprintf(stderr, "Failed to read size from stream.\n");
            rc = 1;
        }
    }

    // Check that the 'WAVE' id is present.
    if (rc == 0)
    {
        fourcc wave_id;
        items_read = fread(&wave_id, sizeof(wave_id), 1, stream->fp);
        if (items_read != 1 || strncmp("WAVE", wave_id, 4) != 0)
        {
            fprintf(stderr, "No WAVE id found - not a wave file.\n");
            rc = 1;
        }
    }

    if (rc == 0)
    {
        stream->fileSize = sizeof(riff_id) + sizeof(stream->fileSize) + chunk_size;
    }

    return rc;
}

static int ReadFmtChunk(WaveStream* stream)
{
    static char fmtChunkId[] = "fmt ";

    // Look for the 'fmt ' chunk.
    int rc = 0;

    rc = FindChunk(stream->fp, fmtChunkId);

    // Read the size.
    uint32_t fmt_size = 0;
    if (rc == 0 && !Uint32FromLE(stream->fp, &fmt_size))
    {
        fprintf(stderr, "Failed to read fmt size from stream.\n");
        rc = 1;
    }

    // TODO: guard against backwards.
    long next_chunk = ftell(stream->fp) + (long) fmt_size;

    // Read the format code.
    uint16_t format_code;
    if (rc == 0 && !Uint16FromLE(stream->fp, &format_code))
    {
        fprintf(stderr, "Failed to read format code.\n");
        rc = 1;
    }

    if (rc == 0 && format_code != WAVE_FORMAT_PCM)
    {
        fprintf(stderr, "Not PCM. %hd\n", format_code);
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
    uint32_t data_rate;
    if (rc == 0 && !Uint32FromLE(stream->fp, &data_rate))
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
        uint32_t bytes_per_sample = stream->bitsPerSample / 8u;
        if (bytes_per_sample != 1 && bytes_per_sample != 2)
        {
            fprintf(stderr, "Unexpected sample size: %u\n", bytes_per_sample);
            rc = 1;
        }
    }

    // We're done with the 'fmt ' chunk (we don't care if it's an extended fmt chunk), so skip over it to the next one.
    int result = fseek(stream->fp, next_chunk, SEEK_SET);
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
    int rc = 0;

    // Look for the 'data' chunk.
    rc = FindChunk(stream->fp, dataChunkId);

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

int ReadWaveStream(WaveStream* stream, void* buf, size_t size, size_t count, size_t* items_read)
{
    if (stream == nullptr || buf == nullptr || items_read == nullptr)
    {
        return 1;
    }

    *items_read = fread(buf, size, count, stream->fp);

    return 0;
}

namespace je
{
    namespace
    {
        ALCdevice* device = nullptr;
        ALCcontext* context = nullptr;
        bool isInitialised = false;

        // This is more or less cribbed from C some proof-of-concept code that I wrote and put on Bitbucket.
        void InitOpenAl()
        {
            if (isInitialised)
            {
                LOG("OpenAL is already initialised");
                return;
            }

            device = alcOpenDevice(nullptr);
            if (!device)
            {
                LOG("alcOpenDevice() failed");
                return;
            }

            context = alcCreateContext(device, nullptr);
            if (!context)
            {
                LOG("alcCreateContext() failed");
                alcCloseDevice(device);
                return;
            }

            alcMakeContextCurrent(context);
            if (auto error = alGetError(); error != AL_NO_ERROR)
            {
                LOG("alcMakeContextCurrent() failed with OpenAL Error " << alGetString(error));
                alcDestroyContext(context);
                alcCloseDevice(device);
                return;
            }

            isInitialised = true;

            int major;
            int minor;
            alcGetIntegerv(nullptr, ALC_MAJOR_VERSION, 1, &major);
            alcGetIntegerv(nullptr, ALC_MINOR_VERSION, 1, &minor);

            LOG("ALC version: " << major << "." << minor);
            LOG("Default device: " << alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));

            LOG("OpenAL version: " << alGetString(AL_VERSION));
            LOG("OpenAL vendor: " << alGetString(AL_VENDOR));
            LOG("OpenAL renderer: " << alGetString(AL_RENDERER));

            // Create a listener.
            ALfloat listenerPos[] = {0.0, 0.0, 1.0};
            ALfloat listenerVel[] = {0.0, 0.0, 0.0};
            ALfloat listenerOri[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};

            alListenerfv(AL_POSITION, listenerPos);
            if (auto error = alGetError(); error != AL_NO_ERROR)
            {
                LOG("alListenerfv(AL_POSITION) failed with OpenAL Error " << alGetString(error));
                alcDestroyContext(context);
                alcCloseDevice(device);
            }

            alListenerfv(AL_VELOCITY, listenerVel);
            if (auto error = alGetError(); error != AL_NO_ERROR)
            {
                LOG("alListenerfv(AL_VELOCITY) failed with OpenAL Error " << alGetString(error));
                alcDestroyContext(context);
                alcCloseDevice(device);
            }

            alListenerfv(AL_ORIENTATION, listenerOri);
            if (auto error = alGetError(); error != AL_NO_ERROR)
            {
                LOG("alListenerfv(AL_ORIENTATION) failed with OpenAL Error " << alGetString(error));
                alcDestroyContext(context);
                alcCloseDevice(device);
            }

            // Set the global gain (volume).
            alListenerf(AL_GAIN, 1.0); // reset gain (volume) to default
            if (auto error = alGetError(); error != AL_NO_ERROR)
            {
                LOG("alListenerf(AL_GAIN) failed with OpenAL Error " << alGetString(error));
                alcDestroyContext(context);
                alcCloseDevice(device);
            }

            ALfloat volume;
            alGetListenerf(AL_GAIN, &volume);
            LOG("Volume is " << volume);
        }

        void TearDownOpenAl()
        {
            if (!isInitialised)
            {
                return;
            }

            alGetError();

            // Remove the context.
            device = alcGetContextsDevice(context);
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
            alcCloseDevice(device);
        }

        ALuint MakeSource()
        {
            ALuint source = 0;
            alGenSources(1, &source);
            return source;
        }

        ALuint MakeBuffer()
        {
            ALuint buffer{0};
            alGenBuffers(1, &buffer);
            return buffer;
        }
    }

    SoundSystem::SoundSystem()
    {
        InitOpenAl();
    }

    SoundSystem::~SoundSystem()
    {
        TearDownOpenAl();
    }

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

        // TODO: check that this is its size in bytes, not frames.
        std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(stream.dataSize);

        // Read the stream data.
        size_t items_read = 0;
        rc = ReadWaveStream(&stream, data.get(), sizeof(uint8_t), stream.dataSize, &items_read);
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

    ALubyte LoadVorbis(const std::string& filename)
    {
        ALuint buffer = 0;

        short* decoded;
        int channels;
        int sampleRate;
        int len = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &decoded);
        if (len <= 0)
        {
            LOG("vorbis decode failed on " << filename);
            return buffer;
        }

        LOG("vorbis decoded " << len << " bytes for " << filename);
        LOG("   channels: " << channels);
        LOG("sample rate: " << sampleRate);
        alGenBuffers(1, &buffer);
        ALenum format = channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        ALsizei numBytes = len;
        alBufferData(buffer, format, decoded, numBytes, sampleRate);
        if (auto error = alGetError(); error != AL_NO_ERROR)
        {
            LOG("alBufferData failed with OpenAL Error " << alGetString(error));
        }

        if (decoded)
        {
            free(decoded);
        }

        return buffer;
    }

    ALuint LoadSound(const std::string& filename)
    {
        LOG("Loading " << filename);

        if (auto buffer = LoadWav(filename); buffer != 0)
        {
            return buffer;
        }

        if (auto buffer = LoadVorbis(filename); buffer != 0)
        {
            return buffer;
        }

        return 0;
    }

    SoundSource::SoundSource()
            :source_{MakeSource()}
    {
    }

    SoundSource::SoundSource(ALuint source)
            :source_{source}
    {
    }

    SoundSource::~SoundSource()
    {
        alDeleteSources(1, &source_);
        source_ = 0;
    }

    void SoundSource::Play(SoundBuffer& buffer)
    {
        Stop();
        alSourcei(source_, AL_BUFFER, (ALint) buffer.Get());
        alSourcePlay(source_);
    }

    void SoundSource::Pause()
    {
        alSourcePause(source_);
    }

    void SoundSource::Resume()
    {
        alSourcePlay(source_);
    }

    void SoundSource::Stop()
    {
        alSourceStop(source_);
    }

    bool SoundSource::IsStopped() const
    {
        ALint value = 0;
        alGetSourcei(source_, AL_SOURCE_STATE, &value);
        return value == AL_STOPPED;
    }

    bool SoundSource::IsPaused() const
    {
        ALint value = 0;
        alGetSourcei(source_, AL_SOURCE_STATE, &value);
        return value == AL_PAUSED;
    }

    bool SoundSource::IsPlaying() const
    {
        ALint value = 0;
        alGetSourcei(source_, AL_SOURCE_STATE, &value);
        return value == AL_PLAYING;
    }

    SoundBuffer::SoundBuffer()
            :buffer_{MakeBuffer()}
    {
    }

    SoundBuffer::SoundBuffer(ALuint buffer)
            :buffer_{buffer}
    {
    }

    SoundBuffer::~SoundBuffer()
    {
        alDeleteBuffers(1, &buffer_);
        buffer_ = 0;
    }

    void SoundBuffer::TakeOwnership(ALuint buffer)
    {
        alDeleteBuffers(1, &buffer_);
        buffer_ = buffer;
    }
}

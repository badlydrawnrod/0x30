#include "Sound.h"

#include "Logger.h"

#include <string>

#include "stb_vorbis.c"

// My .wav loader from je_poc_invaders and hacked into place here.

typedef struct
{
    FILE* fp;                   // The underlying file pointer.
    long start_of_data;         // The start of the data in the underlying file.
    uint32_t file_size;         // The size of the underlying file according to the RIFF header.
    uint32_t data_size;         // Length of data, in bytes.
    uint16_t channels;          // Number of channels.
    uint32_t sample_rate;       // Samples per second.
    uint16_t block_size;        // Block (frame) size (bytes).
    uint16_t bits_per_sample;   // Bits per sample (eg, 8, 16).
} wav_stream;

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Description of .wav format comes from: http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

#define WAVE_FORMAT_PCM 1

typedef char fourcc[4];

static int32_t int32_from_le_buf(const uint8_t* buf)
{
    int32_t i = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    return i;
}

static uint32_t uint32_from_le_buf(const uint8_t* buf)
{
    uint32_t i = (uint32_t) (buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
    return i;
}

static uint16_t uint16_from_le_buf(const uint8_t* buf)
{
    uint16_t i = (uint16_t) (buf[0] | (buf[1] << 8));
    return i;
}

static bool uint16_from_le_fp(FILE* fp, uint16_t* out)
{
    uint8_t buf2[2];
    size_t items_read = fread(buf2, sizeof(buf2), 1, fp);

    if (items_read != 1)
    {
        return false;
    }
    uint16_t i = uint16_from_le_buf(buf2);
    *out = i;

    return true;
}

static bool int32_from_le_fp(FILE* fp, int32_t* out)
{
    uint8_t buf4[4];
    size_t items_read = fread(buf4, sizeof(buf4), 1, fp);

    if (items_read != 1)
    {
        return false;
    }
    int32_t i = int32_from_le_buf(buf4);
    *out = i;

    return true;
}

static bool uint32_from_le_fp(FILE* fp, uint32_t* out)
{
    uint8_t buf4[4];
    size_t items_read = fread(buf4, sizeof(buf4), 1, fp);

    if (items_read != 1)
    {
        return false;
    }
    uint32_t i = uint32_from_le_buf(buf4);
    *out = i;

    return true;
}

static int find_chunk(FILE* fp, fourcc id)
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
        if (!int32_from_le_fp(fp, &skip))
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

static int read_riff_header(wav_stream* stream)
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
        if (!uint32_from_le_fp(stream->fp, &chunk_size))
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
        stream->file_size = sizeof(riff_id) + sizeof(stream->file_size) + chunk_size;
    }

    return rc;
}

static int read_fmt_chunk(wav_stream* stream)
{
    // Look for the 'fmt ' chunk.
    int rc = 0;

    rc = find_chunk(stream->fp, "fmt ");    // TODO: not legal C++.

    // Read the size.
    uint32_t fmt_size = 0;
    if (rc == 0 && !uint32_from_le_fp(stream->fp, &fmt_size))
    {
        fprintf(stderr, "Failed to read fmt size from stream.\n");
        rc = 1;
    }

    // TODO: guard against backwards.
    long next_chunk = ftell(stream->fp) + (long) fmt_size;

    // Read the format code.
    uint16_t format_code;
    if (rc == 0 && !uint16_from_le_fp(stream->fp, &format_code))
    {
        fprintf(stderr, "Failed to read format code.\n");
        rc = 1;
    }

    if (rc == 0 && format_code != WAVE_FORMAT_PCM)
    {
        fprintf(stderr, "Not PCM. %hd\n", format_code);
    }

    // Read the number of channels.
    if (rc == 0 && !uint16_from_le_fp(stream->fp, &stream->channels))
    {
        fprintf(stderr, "Failed to read channels.\n");
        rc = 1;
    }

    // Read the sample rate (samples per second).
    if (rc == 0 && !uint32_from_le_fp(stream->fp, &stream->sample_rate))
    {
        fprintf(stderr, "Failed to read sample rate.\n");
        rc = 1;
    }

    // Read the average data rate (bytes per second).
    uint32_t data_rate;
    if (rc == 0 && !uint32_from_le_fp(stream->fp, &data_rate))
    {
        fprintf(stderr, "Failed to read data rate (average bytes per second).\n");
        rc = 1;
    }

    // Read the data block size in bytes.
    if (rc == 0 && !uint16_from_le_fp(stream->fp, &stream->block_size))
    {
        fprintf(stderr, "Failed to read block size.\n");
        rc = 1;
    }

    // Read the bits per sample.
    if (rc == 0 && !uint16_from_le_fp(stream->fp, &stream->bits_per_sample))
    {
        fprintf(stderr, "Failed to read bits per sample.\n");
        rc = 1;
    }

    // Calculate the sample size.
    if (rc == 0)
    {
        uint32_t bytes_per_sample = stream->bits_per_sample / 8u;
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

static int read_data_header(wav_stream* stream)
{
    int rc = 0;

    // Look for the 'data' chunk.
    rc = find_chunk(stream->fp, "data");    // TODO: not legal C++.

    // Read the data size.
    if (rc == 0 && !uint32_from_le_fp(stream->fp, &stream->data_size))
    {
        fprintf(stderr, "Failed to read data size from stream.\n");
        rc = 1;
    }

    return rc;
}

int open_wav_stream(const char* filename, wav_stream* stream)
{
    int rc = 0;

    if (stream == NULL)
    {
        rc = 1;
    }

    if (rc == 0)
    {
        stream->fp = fopen(filename, "rb");
        if (stream->fp == NULL)
        {
            fprintf(stderr, "Couldn't open %s.\n", filename);
            rc = 1;
        }
    }

    if (rc == 0)
    {
        rc = read_riff_header(stream);
    }

    if (rc == 0)
    {
        rc = read_fmt_chunk(stream);
    }

    if (rc == 0)
    {
        rc = read_data_header(stream);
    }

    if (rc == 0)
    {
        stream->start_of_data = ftell(stream->fp);
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

int close_wav_stream(wav_stream* stream)
{
    int rc = 0;

    if (stream == NULL || stream->fp == NULL)
    {
        rc = 1;
    }

    if (rc == 0)
    {
        fclose(stream->fp);
    }

    return rc;
}

int read_wav_stream(wav_stream* stream, void* buf, size_t size, size_t count, size_t* items_read)
{
    if (stream == NULL || buf == NULL || items_read == NULL)
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

    ALuint LoadSound(const std::string& filename)
    {
        LOG("Loading " << filename);

        wav_stream stream{};
        int rc = open_wav_stream(filename.c_str(), &stream);

        if (rc == 0)
        {
            // Having opened the stream, we know its reported size (in bytes?).
            // TODO: check that this is its size in bytes, not frames.
            std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(stream.data_size);
            if (!data)
            {
                rc = 1;
            }

            // Read the stream data.
            size_t items_read = 0;
            if (rc == 0)
            {
                rc = read_wav_stream(&stream, data.get(), sizeof(uint8_t), stream.data_size, &items_read);
                if (rc != 0)
                {
                    fprintf(stderr, "Failed to read %s\n", filename.c_str());
                }
            }

            // Determine the OpenAL buffer format from the stream data.
            ALenum format = AL_FORMAT_MONO8;
            if (rc == 0)
            {
                LOG("  Start of data: " << stream.start_of_data);
                LOG("      File size: " << stream.file_size);
                LOG("      Data size: " << stream.data_size);
                LOG("       Channels: " << stream.channels);
                LOG("    Sample rate: " << stream.sample_rate);
                LOG("     Block size: " << stream.block_size);
                LOG("Bits per sample: " << stream.bits_per_sample);
                if (stream.bits_per_sample == 8)
                {
                    if (stream.channels == 1)
                    {
                        format = AL_FORMAT_MONO8;
                    }
                    else if (stream.channels == 2)
                    {
                        format = AL_FORMAT_STEREO8;
                    }
                }
                else if (stream.bits_per_sample == 16)
                {
                    if (stream.channels == 1)
                    {
                        format = AL_FORMAT_MONO16;
                    }
                    else if (stream.channels == 2)
                    {
                        format = AL_FORMAT_STEREO16;
                    }
                }
            }

            ALuint buffer = 0;
            if (rc == 0)
            {
                alGenBuffers(1, &buffer);
            }

            if (rc == 0)
            {
                ALsizei numBytes = static_cast<ALsizei>(stream.data_size);
                alBufferData(buffer, format, data.get(), numBytes, stream.sample_rate);
            }

            if (rc == 0)
            {
                close_wav_stream(&stream);
            }

            LOG("Finished loading " << filename << ", buffer handle = " << buffer);
            return buffer;
        }

        // It isn't a .wav file, but maybe it's a .ogg file.
        short* decoded;
        int channels;
        int sampleRate;
        int len = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &decoded);
        if (len == 0)
        {
            LOG("vorbis decode failed on " << filename);
        }
        LOG("vorbis decoded " << len << " bytes for " << filename);
        LOG("   channels: " << channels);
        LOG("sample rate: " << sampleRate);
        ALuint buffer = 0;
        alGenBuffers(1, &buffer);
        ALenum format = AL_FORMAT_MONO16;
        if (channels == 1)
        {
            format = AL_FORMAT_MONO16;
        }
        else if (channels == 2)
        {
            format = AL_FORMAT_STEREO16;
        }
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

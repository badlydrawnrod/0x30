#include "Sound.h"

#include "Logger.h"

#include "sndfile.hh"   // Apparently "A lightweight C++ wrapper for the libsndfile API"

#include <string>


static ALCdevice* device = nullptr;
static ALCcontext* context = nullptr;
static ALenum error;
static bool isInitialised = false;


namespace je
{
    // TODO: Make this more C++ like, or at least add some RAII.
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
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            LOG("alcMakeContextCurrent() failed");
            alcDestroyContext(context);
            alcCloseDevice(device);
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

        ALfloat listenerPos[] = { 0.0, 0.0, 1.0 };
        ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
        ALfloat listenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };

        alListenerfv(AL_POSITION, listenerPos);
        alListenerfv(AL_VELOCITY, listenerVel);
        alListenerfv(AL_ORIENTATION, listenerOri);

        // Set the global gain (volume).
        alListenerf(AL_GAIN, 1.0); // reset gain (volume) to default
        ALfloat volume;
        alGetListenerf(AL_GAIN, &volume);
        LOG("Volume is " << volume);
    }

    
    ALuint LoadSound(const std::string& filename)
    {
        SndfileHandle sndFile(filename);
        if (sndFile.error() != 0)
        {
            LOG("Failed to open sound file: " << filename);
            return 0;
        }

        ALenum format;
        if (sndFile.channels() == 1)
        {
            format = AL_FORMAT_MONO16;
        }
        else if (sndFile.channels() == 2)
        {
            format = AL_FORMAT_STEREO16;
        }
        else
        {
            LOG("Unsupported channel count: " << sndFile.channels());
            return 0;
        }

        // Decode the file to a memory buffer.
        std::unique_ptr<short[]> memBuf = std::make_unique<short[]>(sndFile.frames() * sndFile.channels());
        sf_count_t numFrames = sndFile.readf(memBuf.get(), sndFile.frames());
        if (numFrames < 1)
        {
            LOG("Failed to read samples");
            return 0;
        }

        // Put the audio data into an OpenAL buffer object.
        ALuint buffer = 0;
        alGenBuffers(1, &buffer);
        ALsizei numBytes = static_cast<ALsizei>(numFrames * sndFile.channels()) * static_cast<ALsizei>(sizeof(short));
        alBufferData(buffer, format, memBuf.get(), numBytes, sndFile.samplerate());

        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            LOG("Failed to load " << filename << " with OpenAL Error " << alGetString(error));
            if (buffer != 0 && alIsBuffer(buffer))
            {
                alDeleteBuffers(1, &buffer);
            }
            return 0;
        }

        return buffer;
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
}

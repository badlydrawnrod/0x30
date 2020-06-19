#include "Sound.h"

#include "Logger.h"
#include "VorbisLoader.h"
#include "WavLoader.h"

#include <AL/al.h>
#include <AL/alc.h>

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
    } // namespace

    SoundSystem::SoundSystem()
    {
        InitOpenAl();
    }

    SoundSystem::~SoundSystem()
    {
        TearDownOpenAl();
    }

    SoundSource::SoundSource()
        : source_{MakeSource()}
    {
    }

    SoundSource::SoundSource(ALuint source)
        : source_{source}
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
        alSourcei(source_, AL_BUFFER, (ALint)buffer.Get());
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
        : buffer_{MakeBuffer()}
    {
    }

    SoundBuffer::SoundBuffer(ALuint buffer)
        : buffer_{buffer}
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
} // namespace je

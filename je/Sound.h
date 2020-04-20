#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <string>


namespace je
{
    ALuint LoadSound(const std::string& filename);

    class SoundSystem
    {
    public:
        SoundSystem();
        ~SoundSystem();
    };


    class SoundBuffer;

    class SoundSource
    {
    public:
        SoundSource();
        SoundSource(ALuint source);
        ~SoundSource();

        ALuint Get() const { return source_; }
        void Play(SoundBuffer& buffer);
        void Pause();
        void Resume();
        void Stop();

    private:
        ALuint source_{ 0 };
    };

    class SoundBuffer
    {
    public:
        SoundBuffer();
        SoundBuffer(ALuint buffer);
        ~SoundBuffer();

        void TakeOwnership(ALuint buffer);
        ALuint Get() const { return buffer_; }

    private:
        ALuint buffer_{ 0 };
    };
}

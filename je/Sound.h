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


    class SoundSource
    {
    public:
        SoundSource();
        SoundSource(ALuint source);
        ~SoundSource();

        ALuint Get() const { return source_; }

    private:
        ALuint source_{ 0 };
    };

    class SoundBuffer
    {
    public:
        SoundBuffer();
        SoundBuffer(ALuint buffer);
        ~SoundBuffer();

        ALuint Get() const { return buffer_; }

    private:
        ALuint buffer_{ 0 };
    };

    void Play(SoundBuffer& buffer, SoundSource& source);
}

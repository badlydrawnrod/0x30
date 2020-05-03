#pragma once

#include "je/Sound.h"

#include <future>


struct Sounds
{
    void Load();
    bool IsLoaded();

    je::SoundBuffer blocksSwapping;
    je::SoundBuffer blocksLanding;
    je::SoundBuffer blocksPopping;
    je::SoundBuffer menuSelect;

    je::SoundBuffer musicMinuteWaltz;
    je::SoundBuffer musicLAdieu;
    je::SoundBuffer musicHallelujah;
    je::SoundBuffer musicGymnopedie;

private:
    void LoaderTask();

    std::future<void> loader_;
};

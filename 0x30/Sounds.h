#pragma once

#include "je/Sound.h"


struct Sounds
{
    Sounds();

    je::SoundBuffer blocksSwapping;
    je::SoundBuffer blocksLanding;
    je::SoundBuffer blocksPopping;
    je::SoundBuffer menuSelect;

    je::SoundBuffer musicMinuteWaltz;
    je::SoundBuffer musicLAdieu;
    je::SoundBuffer musicHallelujah;
};

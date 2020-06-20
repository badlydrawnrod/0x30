#pragma once

#include "AsyncLoader.h"
#include "je/Logger.h"
#include "je/Sound.h"

class Sounds : public je::AsyncLoader<Sounds>
{
public:
    void BeginLoad();
    void OnLoaded();

    je::SoundBuffer blocksSwapping;
    je::SoundBuffer blocksLanding;
    je::SoundBuffer blocksPopping;
    je::SoundBuffer menuSelect;

    je::SoundBuffer musicMinuteWaltz;
    je::SoundBuffer musicLAdieu;
    je::SoundBuffer musicHallelujah;
    je::SoundBuffer musicGymnopedie;
};

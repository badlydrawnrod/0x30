#include "Sounds.h"

#include <chrono>
#include <future>


void Sounds::Load()
{
    loader_ = std::async(&Sounds::LoaderTask, this);
}


bool Sounds::IsLoaded()
{
    return loader_.wait_for(std::chrono::microseconds(1)) == std::future_status::ready;
}


void Sounds::LoaderTask()
{
    blocksSwapping = std::move(je::SoundBuffer(je::LoadSound("assets/sounds/swap.wav")));
    blocksLanding = std::move(je::SoundBuffer(je::LoadSound("assets/sounds/marble_click.wav")));
    blocksPopping = std::move(je::SoundBuffer(je::LoadSound("assets/sounds/pop.wav")));
    menuSelect = std::move(je::SoundBuffer(je::LoadSound("assets/sounds/swap.wav")));
    musicMinuteWaltz = std::move(je::SoundBuffer(je::LoadSound("assets/music/minute.ogg")));
    musicLAdieu = std::move(je::SoundBuffer(je::LoadSound("assets/music/adieu.ogg")));
    musicHallelujah = std::move(je::SoundBuffer(je::LoadSound("assets/music/hallelujah.ogg")));
}

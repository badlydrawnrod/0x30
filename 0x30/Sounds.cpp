#include "Sounds.h"

#include <chrono>
#include <future>


void Sounds::Load()
{
#if !defined(__EMSCRIPTEN__)
    loader_ = std::async(&Sounds::LoaderTask, this);
#endif
}


bool Sounds::IsLoaded()
{
#if !defined(__EMSCRIPTEN__)
    return loader_.wait_for(std::chrono::microseconds(1)) == std::future_status::ready;
#else
    return true;
#endif
}


void Sounds::LoaderTask()
{
    blocksSwapping.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
    blocksLanding.TakeOwnership(je::LoadSound("assets/sounds/marble_click.wav"));
    blocksPopping.TakeOwnership(je::LoadSound("assets/sounds/pop.wav"));
    menuSelect.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
#if !defined(__EMSCRIPTEN__)
    musicMinuteWaltz.TakeOwnership(je::LoadSound("assets/music/minute.ogg"));
    musicLAdieu.TakeOwnership(je::LoadSound("assets/music/adieu.ogg"));
    musicHallelujah.TakeOwnership(je::LoadSound("assets/music/hallelujah.ogg"));
    musicGymnopedie.TakeOwnership(je::LoadSound("assets/music/gymnopedie1.ogg"));
#endif
}

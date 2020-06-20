#include "Sounds.h"

#include "je/SoundLoader.h"

void Sounds::OnLoaderStarted()
{
    Download("sounds/swap.wav", "assets/sounds/swap.wav");
    Download("sounds/marble_click.wav", "assets/sounds/marble_click.wav");
    Download("sounds/pop.wav", "assets/sounds/pop.wav");
    Download("sounds/swap.wav", "assets/sounds/swap.wav");
    Download("music/minute.ogg", "assets/music/minute.ogg");
    Download("music/adieu.ogg", "assets/music/adieu.ogg");
    Download("music/hallelujah.ogg", "assets/music/hallelujah.ogg");
    Download("music/gymnopedie1.ogg", "assets/music/gymnopedie1.ogg");
}

void Sounds::OnLoaderFinished()
{
    blocksSwapping.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
    blocksLanding.TakeOwnership(je::LoadSound("assets/sounds/marble_click.wav"));
    blocksPopping.TakeOwnership(je::LoadSound("assets/sounds/pop.wav"));
    menuSelect.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
    musicMinuteWaltz.TakeOwnership(je::LoadSound("assets/music/minute.ogg"));
    musicLAdieu.TakeOwnership(je::LoadSound("assets/music/adieu.ogg"));
    musicHallelujah.TakeOwnership(je::LoadSound("assets/music/hallelujah.ogg"));
    musicGymnopedie.TakeOwnership(je::LoadSound("assets/music/gymnopedie1.ogg"));
}

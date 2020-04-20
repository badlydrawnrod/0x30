#include "Sounds.h"


Sounds::Sounds() :
    blocksSwapping(je::LoadSound("assets/sounds/swap.wav")),
    blocksLanding(je::LoadSound("assets/sounds/marble_click.wav")),
    blocksPopping(je::LoadSound("assets/sounds/pop.wav")),
    menuSelect(je::LoadSound("assets/sounds/swap.wav")),
    musicMinuteWaltz(je::LoadSound("assets/music/minute.ogg")),
    musicLAdieu(je::LoadSound("assets/music/adieu.ogg")),
    musicHallelujah(je::LoadSound("assets/music/hallelujah.ogg"))
{
}

#include "Sounds.h"


Sounds::Sounds() :
    blocksSwapping(je::LoadSound("assets/sounds/swap.wav")),
    blocksLanding(je::LoadSound("assets/sounds/marble_click.wav")),
    blocksPopping(je::LoadSound("assets/sounds/pop.wav")),
    menuSelect(je::LoadSound("assets/sounds/swap.wav"))
{
}

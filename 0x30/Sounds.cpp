#include "Sounds.h"


Sounds::Sounds() :
    blocksSwapping(je::LoadSound("../assets/sounds/bloop.wav")),
    blocksLanding(je::LoadSound("../assets/sounds/bloop.wav")),
    blocksPopping(je::LoadSound("../assets/sounds/bloop.wav")),
    menuSelect(je::LoadSound("../assets/sounds/bloop.wav"))
{
}

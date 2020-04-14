#pragma once

#include "Sounds.h"
#include "TextRenderer.h"
#include "Textures.h"
#include "Types.h"

#include "je/Batch.h"


class Dedication
{
public:
    Dedication(je::Batch& batch, Textures& textures, Sounds& sounds);

    Screens Update(double t, double dt);
    void Draw(double t);

private:
    je::Batch& batch_;
    Textures& textures_;
    Sounds& sounds_;
    je::SoundSource source_;

    TextRenderer textRenderer_;
    double startTime_;
};

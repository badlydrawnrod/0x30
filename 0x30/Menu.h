#pragma once

#include "Constants.h"  // TODO: (re)move Screens.
#include "TextRenderer.h"
#include "Textures.h"

#include "je/Batch.h"


class Menu
{
public:
    Menu(je::Batch& batch, Textures& textures);

    Screens Update(double t, double dt);
    void Draw();

private:
    je::Batch& batch_;
    Textures& textures_;
    TextRenderer textRenderer_;
};

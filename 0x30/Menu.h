#pragma once

#include "Assets.h"
#include "TextRenderer.h"

#include "je/Batch.h"


class Menu
{
public:
    Menu(je::Batch& batch, Textures& textures);

    void Update();
    void Draw();

private:
    je::Batch& batch;
    Textures& textures;
    TextRenderer textRenderer;
};

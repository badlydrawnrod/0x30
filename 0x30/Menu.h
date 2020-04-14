#pragma once

#include "TextRenderer.h"
#include "Textures.h"
#include "Types.h"

#include "je/Batch.h"


class Menu
{
public:
    Menu(je::Batch& batch, Textures& textures);

    void Start(double t, int maxLevel, const Scores& scores);
    Screens Update(double t, double dt);
    void Draw(double t);
    int SelectedLevel() const { return currentSelection_ + 1; }

private:
    je::Batch& batch_;
    Textures& textures_;
    TextRenderer textRenderer_;

    Scores scores_;
    double screenStartTime_{ 0 };
    int maxLevel_{ 0 };
    int currentSelection_{ 0 };
};

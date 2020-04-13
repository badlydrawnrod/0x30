#pragma once

#include "Constants.h"  // TODO: (re)move Screens.
#include "TextRenderer.h"
#include "Textures.h"

#include "je/Batch.h"


class Menu
{
public:
    Menu(je::Batch& batch, Textures& textures);

    void Start(double t, int maxLevel, const std::array<uint64_t, 10>& scores);
    Screens Update(double t, double dt);
    void Draw();
    int SelectedLevel() const { return currentSelection_ + 1; }

private:
    je::Batch& batch_;
    Textures& textures_;
    TextRenderer textRenderer_;

    std::array<uint64_t, 10> scores_;
    double screenStartTime_{ 0 };
    int maxLevel_{ 0 };
    int currentSelection_{ 0 };
};

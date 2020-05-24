#pragma once

#include "Input.h"
#include "Progress.h"
#include "TextRenderer.h"
#include "Textures.h"
#include "Types.h"

#include "je/Batch.h"


class Menu
{
public:
    Menu(input::Input& input, const Progress& progress, je::Batch& batch, Textures& textures);

    void Start(double t);
    Screens Update(double t, double dt);
    void Draw(double t);
    int SelectedLevel() const { return currentSelection_ + 1; }
    Mode SelectedMode() const { return mode_; }

private:
    input::Input& input_;
    const Progress& progress_;
    je::Batch& batch_;
    Textures& textures_;
    TextRenderer textRenderer_;

    double screenStartTime_{ 0 };
    int currentSelection_{ 0 };
    int firstVisibleLevel_{ 0 };
    Mode mode_{ Mode::ENDLESS };

    static constexpr int visibleLevels_{ 10 };
};

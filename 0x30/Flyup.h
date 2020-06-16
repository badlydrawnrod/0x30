#pragma once

#include "je/Batch.h"
#include "je/Textures.h"

class Flyup
{
public:
    Flyup(je::TextureRegion& texture, float x, float y, float duration);
    Flyup(const Flyup& other);
    Flyup& operator=(const Flyup& other);

    void Draw(je::Batch& batch);
    bool IsAlive() const;

private:
    je::TextureRegion texture_;
    float x_;
    float y_;
    double endTime_;
};
